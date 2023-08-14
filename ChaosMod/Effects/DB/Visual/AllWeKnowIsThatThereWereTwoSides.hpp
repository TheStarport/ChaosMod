#pragma once
#include "Effects/ActiveEffect.hpp"
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

class AllWeKnowIsThatThereWereTwoSides final : public ActiveEffect
{
        const char* fileName = "C:\\Users\\laz\\Downloads\\fl_intro_en_hd.wmv";

        int videoStream = -1;
        AVFormatContext* pFormatCtx = nullptr;
        AVCodecParameters* pCodecParameters = nullptr;
        const AVCodec* pCodec = nullptr;
        AVCodecContext* pCodecCtx = nullptr;
        AVFrame* pFrame = nullptr;
        SwsContext* swsCtx = nullptr;
        IDirect3DTexture9* texture = nullptr;

        inline static ma_stream_layout streamLayout;
        inline static std::vector<byte> audioBuffer;
        inline static ma_engine engine;
        inline static ma_audio_buffer miniAudioBuffer;
        inline static ma_sound miniAudioSound;

        size_t totalDecodedFrames = 0;

        Utils::Stopwatch* stopwatch;

        void FrameUpdate(float delta) override
        {
            long long e = stopwatch->ElapsedSeconds();
            auto ee = av_q2d(av_inv_q(pCodecCtx->framerate));

            auto currentFrame = static_cast<int>((double)stopwatch->ElapsedSeconds() / av_q2d(av_inv_q(pCodecCtx->framerate))) + 1;
            while (totalDecodedFrames < (currentFrame + 1))
            {
                static AVPacket packet;
                static auto sws_ctx = sws_getContext(pCodecCtx->width,
                                                     pCodecCtx->height,
                                                     pCodecCtx->pix_fmt,
                                                     pCodecCtx->width,
                                                     pCodecCtx->height,
                                                     AV_PIX_FMT_RGB24,
                                                     SWS_BILINEAR,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr);

                if (av_read_frame(pFormatCtx, &packet) < 0)
                {
                    return;
                }

                if (packet.stream_index == videoStream)
                {
                    int res = avcodec_send_packet(pCodecCtx, &packet);
                    if (res < 0)
                    {
                        av_packet_unref(&packet);
                        return;
                    }

                    res = avcodec_receive_frame(pCodecCtx, pFrame);

                    AVFrame* frame = av_frame_alloc();
                    frame->format = AV_PIX_FMT_RGB24;
                    frame->width = pFrame->width;
                    frame->height = pFrame->height;
                    av_frame_get_buffer(frame, 32);

                    sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, frame->height, frame->data, frame->linesize);

                    D3DLOCKED_RECT lockedRect;
                    if (HRESULT l; (l = texture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD)) == S_OK)
                    {
                        unsigned char* dest = static_cast<unsigned char*>(lockedRect.pBits);

                        for (int i = 0; i < pFrame->width * pFrame->height * 3; i += 3)
                        {
                            *dest++ = frame->data[0][i];
                            *dest++ = frame->data[0][i + 1];
                            *dest++ = frame->data[0][i + 2];
                            *dest++ = 126;
                        }

                        texture->UnlockRect(0);
                    }

                    av_frame_free(&frame);
                    totalDecodedFrames++;
                }

                av_packet_unref(&packet);
            }
        }

        void Begin() override
        {
            texture = DrawingHelper::i()->GetVideoSurface();

            int res = avformat_open_input(&pFormatCtx, fileName, nullptr, nullptr);
            res = avformat_find_stream_info(pFormatCtx, nullptr);

            videoStream = -1;

            for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
            {
                if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    videoStream = i;
                }
            }

            pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
            pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
            pCodecCtx = avcodec_alloc_context3(pCodec);
            res = avcodec_parameters_to_context(pCodecCtx, pCodecParameters);
            res = avcodec_open2(pCodecCtx, pCodec, nullptr);

            pFrame = av_frame_alloc();
            AVFrame* pFrameRGB = av_frame_alloc();

            const int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

            const uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

            res = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

            stopwatch->Reset();
            stopwatch->Start();
            ma_sound_start(&miniAudioSound);
        }

        void End() override
        {
            ma_sound_stop(&miniAudioSound);
            ma_sound_seek_to_pcm_frame(&miniAudioSound, 0);

            // Set the texture to transparent
            D3DLOCKED_RECT lockedRect;
            HRESULT l;
            if ((l = texture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD)) == S_OK)
            {
                auto* dest = static_cast<unsigned char*>(lockedRect.pBits);

                for (int i = 0; i < pFrame->width * pFrame->height * 3; i += 3)
                {
                    *dest++ = 0;
                    *dest++ = 0;
                    *dest++ = 0;
                    *dest++ = 0;
                }

                texture->UnlockRect(0);
            }

            avcodec_close(pCodecCtx);
            avformat_close_input(&pFormatCtx);
        }

        void Init() override
        {

            // open video
            int res = avformat_open_input(&pFormatCtx, fileName, nullptr, nullptr);

            // get video info
            res = avformat_find_stream_info(pFormatCtx, nullptr);
            int audioStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

            const AVStream* audioStream = pFormatCtx->streams[audioStreamIndex];

            /* find decoder for the stream */
            const AVCodec* decoder = avcodec_find_decoder(audioStream->codecpar->codec_id);

            /* Allocate a codec context for the decoder */
            AVCodecContext* decoderContext = avcodec_alloc_context3(decoder);

            /* Copy codec parameters from input stream to output codec context */
            avcodec_parameters_to_context(decoderContext, audioStream->codecpar);

            /* Init the decoders */
            avcodec_open2(decoderContext, decoder, nullptr);

            AVFrame* frame = av_frame_alloc();
            AVPacket* packet = av_packet_alloc();

            // Read the packets in a loop
            while (av_read_frame(pFormatCtx, packet) >= 0)
            {
                if (packet->stream_index != audioStreamIndex)
                {
                    av_packet_unref(packet);
                    continue;
                }

                int ret = 0;

                // submit the packet to the decoder
                ret = avcodec_send_packet(decoderContext, packet);
                if (ret < 0)
                {
                    av_packet_unref(packet);
                    continue;
                }

                // get all the available frames from the decoder
                while (ret >= 0)
                {
                    ret = avcodec_receive_frame(decoderContext, frame);
                    if (ret < 0)
                    {
                        // those two return values are special and mean there is no output
                        // frame available, but there were no errors during decoding
                        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                        {
                            // TODO: Handle
                        }
                        av_frame_unref(frame);
                        continue;
                    }

                    // Time to write our raw audio
                    const size_t rawLineSize = frame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format));
                    const size_t curSize = audioBuffer.size();
                    audioBuffer.resize(curSize + rawLineSize);
                    std::memcpy(audioBuffer.data() + curSize, frame->extended_data[0], rawLineSize);

                    av_frame_unref(frame);
                }

                av_packet_unref(packet);
            }

            ma_format format;
            switch (decoderContext->sample_fmt)
            {
                case AV_SAMPLE_FMT_U8:
                    format = ma_format_u8;
                    streamLayout = ma_stream_layout_interleaved;
                    break;

                case AV_SAMPLE_FMT_S16:
                    format = ma_format_s16;
                    streamLayout = ma_stream_layout_interleaved;
                    break;

                case AV_SAMPLE_FMT_S32:
                    format = ma_format_s32;
                    streamLayout = ma_stream_layout_interleaved;
                    break;

                case AV_SAMPLE_FMT_FLT:
                    format = ma_format_f32;
                    streamLayout = ma_stream_layout_interleaved;
                    break;

                case AV_SAMPLE_FMT_U8P:
                    format = ma_format_u8;
                    streamLayout = ma_stream_layout_deinterleaved;
                    break;

                case AV_SAMPLE_FMT_S16P:
                    format = ma_format_s16;
                    streamLayout = ma_stream_layout_deinterleaved;
                    break;

                case AV_SAMPLE_FMT_S32P:
                    format = ma_format_s32;
                    streamLayout = ma_stream_layout_deinterleaved;
                    break;

                case AV_SAMPLE_FMT_FLTP:
                    format = ma_format_f32;
                    streamLayout = ma_stream_layout_deinterleaved;
                    break;

                default:
                    format = ma_format_unknown;
                    streamLayout = ma_stream_layout_deinterleaved;
                    break;
            }

            ma_engine_init(nullptr, &engine);
            ma_engine_set_volume(&engine, 0.4f);

            auto audioBufferConfig = ma_audio_buffer_config_init(format,
                                                                 audioStream->codecpar->ch_layout.nb_channels,
                                                                 audioBuffer.size() / audioStream->codecpar->ch_layout.nb_channels,
                                                                 audioBuffer.data(),
                                                                 nullptr);
            audioBufferConfig.sampleRate = decoderContext->sample_rate / 2;
            ma_audio_buffer_init(&audioBufferConfig, &miniAudioBuffer);
            ma_sound_init_from_data_source(&engine, &miniAudioBuffer, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &miniAudioSound);

            avformat_close_input(&pFormatCtx);
        }

    public:
        AllWeKnowIsThatThereWereTwoSides() { stopwatch = new Utils::Stopwatch(); }

        ~AllWeKnowIsThatThereWereTwoSides() override
        {
            if (engine.pDevice)
            {
                ma_sound_stop(&miniAudioSound);
                ma_audio_buffer_uninit(&miniAudioBuffer);
                ma_engine_uninit(&engine);
            }
        }
        DefEffectInfoFixed("All We Know...", 223.0f, EffectType::Visual);
};
