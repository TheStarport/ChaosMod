#pragma once

#include "Systems/DirectX/Drawing.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

enum class Movie
{
    OriginalIntro
};

class MoviePlayer final : public Singleton<MoviePlayer>
{
        struct MovieData
        {
                Movie movie;
                std::string path;
                std::vector<byte> audioBuffer;
                ma_audio_buffer audioBufferWrapper;
        };

        std::unordered_map<Movie, MovieData> movies;

        uint transparency = 0;

        int videoStream = -1;
        AVFormatContext* formatContext = nullptr;
        AVCodecParameters* codecParameters = nullptr;
        const AVCodec* codec = nullptr;
        AVCodecContext* codecContext = nullptr;
        SwsContext* swsCtx = nullptr;
        IDirect3DTexture9* texture = nullptr;

        size_t totalDecodedFrames = 0;

        TimeUtils::Stopwatch* stopwatch = nullptr;

        ma_sound miniAudioSound{};
        ma_engine engine{};
        std::optional<MovieData*> currentPlayingMovie;

    public:
        void StopMovie()
        {
            if (!currentPlayingMovie.has_value())
            {
                return;
            }

            ma_sound_stop(&miniAudioSound);
            ma_sound_uninit(&miniAudioSound);

            avcodec_close(codecContext);
            avformat_close_input(&formatContext);

            DrawingHelper::i()->ClearVideoTexture();

            currentPlayingMovie = std::nullopt;
        }

        void StartMovie(const Movie movie, const float transparencyPercent = 0.5f)
        {
            transparency = static_cast<uint>(255.0f * transparencyPercent);
            if (currentPlayingMovie.has_value())
            {
                StopMovie();
            }

            const auto data = movies.find(movie);
            if (data == movies.end())
            {
                return;
            }

            currentPlayingMovie = &data->second;

            texture = DrawingHelper::i()->GetVideoSurface();

            avformat_open_input(&formatContext, data->second.path.c_str(), nullptr, nullptr);
            avformat_find_stream_info(formatContext, nullptr);

            videoStream = -1;

            for (unsigned int i = 0; i < formatContext->nb_streams; i++)
            {
                if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    videoStream = i;
                }
            }

            codecParameters = formatContext->streams[videoStream]->codecpar;
            codec = avcodec_find_decoder(codecParameters->codec_id);
            codecContext = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(codecContext, codecParameters);
            avcodec_open2(codecContext, codec, nullptr);

            AVFrame* frameRgb = av_frame_alloc();

            const int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);

            void* buffer = av_malloc(numBytes * sizeof(uint8_t));

            av_image_fill_arrays(
                frameRgb->data, frameRgb->linesize, static_cast<uint8_t*>(buffer), AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);

            ma_sound_init_from_data_source(&engine, &data->second.audioBufferWrapper, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &miniAudioSound);

            stopwatch->Reset();
            stopwatch->Start();
            ma_sound_start(&miniAudioSound);
        }

        void FrameUpdate()
        {
            if (!currentPlayingMovie.has_value())
            {
                return;
            }

            const auto currentFrame = static_cast<int>((double)stopwatch->ElapsedSeconds() / av_q2d(av_inv_q(codecContext->framerate))) + 1;
            AVFrame* frame = av_frame_alloc();
            while (totalDecodedFrames < (currentFrame + 1))
            {
                static AVPacket packet;
                static auto sws_ctx = sws_getContext(codecContext->width,
                                                     codecContext->height,
                                                     codecContext->pix_fmt,
                                                     codecContext->width,
                                                     codecContext->height,
                                                     AV_PIX_FMT_RGB24,
                                                     SWS_BILINEAR,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr);

                if (av_read_frame(formatContext, &packet) < 0)
                {
                    return;
                }

                if (packet.stream_index == videoStream)
                {
                    int res = avcodec_send_packet(codecContext, &packet);
                    if (res < 0)
                    {
                        av_packet_unref(&packet);
                        return;
                    }

                    res = avcodec_receive_frame(codecContext, frame);

                    AVFrame* swapFrame = av_frame_alloc();
                    swapFrame->format = AV_PIX_FMT_RGB24;
                    swapFrame->width = frame->width;
                    swapFrame->height = frame->height;
                    av_frame_get_buffer(swapFrame, 32);

                    sws_scale(sws_ctx, frame->data, frame->linesize, 0, swapFrame->height, swapFrame->data, swapFrame->linesize);

                    D3DLOCKED_RECT lockedRect;
                    if (HRESULT l; (l = texture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD)) == S_OK)
                    {
                        auto* dest = static_cast<unsigned char*>(lockedRect.pBits);

                        for (int i = 0; i < swapFrame->width * swapFrame->height * 3; i += 3)
                        {
                            *dest++ = swapFrame->data[0][i + 2];
                            *dest++ = swapFrame->data[0][i + 1];
                            *dest++ = swapFrame->data[0][i];
                            *dest++ = transparency;
                        }

                        texture->UnlockRect(0);
                    }

                    av_frame_free(&swapFrame);
                    totalDecodedFrames++;
                }

                av_packet_unref(&packet);
            }

            av_frame_free(&frame);
        }

        MoviePlayer()
        {
            stopwatch = new TimeUtils::Stopwatch();

            ma_engine_init(nullptr, &engine);
            ma_engine_set_volume(&engine, 0.4f);
        }
        ~MoviePlayer()
        {
            delete stopwatch;

            if (currentPlayingMovie.has_value())
            {
                StopMovie();
            }

            ma_engine_uninit(&engine);
        }

        void RegisterMovie(const Movie movie, const std::string& path)
        {
            movies[movie] = { movie, path };
            auto& data = movies[movie];

            // open video
            avformat_open_input(&formatContext, path.c_str(), nullptr, nullptr);

            // get video info
            avformat_find_stream_info(formatContext, nullptr);
            const int audioStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

            const AVStream* audioStream = formatContext->streams[audioStreamIndex];

            /* find decoder for the stream */
            const AVCodec* decoder = avcodec_find_decoder(audioStream->codecpar->codec_id);

            /* Allocate a codec context for the decoder */
            AVCodecContext* decoderContext = avcodec_alloc_context3(decoder);

            /* Copy codec parameters from input stream to output codec context */
            avcodec_parameters_to_context(decoderContext, audioStream->codecpar);

            /* Init the decoders */
            avcodec_open2(decoderContext, decoder, nullptr);

            AVFrame* tempFrame = av_frame_alloc();
            AVPacket* packet = av_packet_alloc();

            // Read the packets in a loop
            while (av_read_frame(formatContext, packet) >= 0)
            {
                if (packet->stream_index != audioStreamIndex)
                {
                    av_packet_unref(packet);
                    continue;
                }

                // submit the packet to the decoder
                int ret = avcodec_send_packet(decoderContext, packet);
                if (ret < 0)
                {
                    av_packet_unref(packet);
                    continue;
                }

                // get all the available frames from the decoder
                while (ret >= 0)
                {
                    ret = avcodec_receive_frame(decoderContext, tempFrame);
                    if (ret < 0)
                    {
                        av_frame_unref(tempFrame);
                        continue;
                    }

                    // Time to write our raw audio
                    const size_t rawLineSize = tempFrame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(tempFrame->format));
                    const size_t curSize = data.audioBuffer.size();
                    data.audioBuffer.resize(curSize + rawLineSize);
                    std::memcpy(data.audioBuffer.data() + curSize, tempFrame->extended_data[0], rawLineSize);

                    av_frame_unref(tempFrame);
                }

                av_packet_unref(packet);
            }

            ma_format format;
            switch (decoderContext->sample_fmt)
            {
                case AV_SAMPLE_FMT_U8: format = ma_format_u8; break;

                case AV_SAMPLE_FMT_S16: format = ma_format_s16; break;

                case AV_SAMPLE_FMT_S32: format = ma_format_s32; break;

                case AV_SAMPLE_FMT_FLT: format = ma_format_f32; break;

                case AV_SAMPLE_FMT_U8P: format = ma_format_u8; break;

                case AV_SAMPLE_FMT_S16P: format = ma_format_s16; break;

                case AV_SAMPLE_FMT_S32P: format = ma_format_s32; break;

                case AV_SAMPLE_FMT_FLTP: format = ma_format_f32; break;

                default: format = ma_format_unknown; break;
            }

            auto audioBufferConfig = ma_audio_buffer_config_init(format,
                                                                 audioStream->codecpar->ch_layout.nb_channels,
                                                                 data.audioBuffer.size() / audioStream->codecpar->ch_layout.nb_channels,
                                                                 data.audioBuffer.data(),
                                                                 nullptr);

            audioBufferConfig.sampleRate = decoderContext->sample_rate / 2;
            ma_audio_buffer_init(&audioBufferConfig, &data.audioBufferWrapper);

            avformat_close_input(&formatContext);
        }
};
