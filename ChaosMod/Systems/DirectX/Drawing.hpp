#pragma once

#include <ctime>
#include <d3dx9core.h>
#include <stdio.h>

#include "D3DTypes.hpp"

class DrawingHelper final : public Singleton<DrawingHelper>
{
        struct Vertex
        {
                FLOAT x, y, z, rhw;
                DWORD color;
        };

        LPDIRECT3DDEVICE9 device = nullptr;
        LPDIRECT3DVERTEXBUFFER9 vertexBuffer = nullptr; // Buffer to hold vertices
        LPDIRECT3DINDEXBUFFER9 indexBuffer = nullptr;   // Buffer to hold indices

        LPD3DXSPRITE sprite = nullptr;

        std::vector<std::function<void()>> awaitingCalls;

    public:
        enum class CircleType
        {
            Full,
            Half,
            Quarter
        };

        void Line(float x1, float y1, float x2, float y2, const float width, const bool antiAlias, const DWORD color)
        {
            const auto res = GetResolution();
            x1 *= res.x;
            x2 *= res.x;
            y1 *= res.y;
            y2 *= res.y;

            awaitingCalls.emplace_back(
                [this, x1, y1, x2, y2, width, antiAlias, color]
                {
                    ID3DXLine* line;

                    D3DXCreateLine(device, &line);
                    const D3DXVECTOR2 vec2[] = { D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2) };
                    line->SetWidth(width);
                    if (antiAlias)
                    {
                        line->SetAntialias(1);
                    }
                    line->Begin();
                    line->Draw(vec2, 2, color);
                    line->End();
                    line->Release();
                });
        }

        void Circle(float x, float y, const float radius, const int rotate, const CircleType type, const bool smoothing, const int resolution,
                    const DWORD color)
        {
            const auto res = GetResolution();
            x *= res.x;
            y *= res.y;

            awaitingCalls.emplace_back(
                [this, x, y, radius, rotate, type, smoothing, resolution, color]
                {
                    std::vector<Vertex> circle(resolution + 2);
                    const float angle = rotate * D3DX_PI / 180;
                    float pi = 0.0f;

                    if (type == CircleType::Full)
                    {
                        pi = D3DX_PI; // Full circle
                    }
                    if (type == CircleType::Half)
                    {
                        pi = D3DX_PI / 2; // 1/2 circle
                    }
                    if (type == CircleType::Quarter)
                    {
                        pi = D3DX_PI / 4; // 1/4 circle
                    }

                    for (int i = 0; i < resolution + 2; i++)
                    {
                        circle[i].x = x - radius * cos(i * (2 * pi / resolution));
                        circle[i].y = y - radius * sin(i * (2 * pi / resolution));
                        circle[i].z = 0;
                        circle[i].rhw = 1;
                        circle[i].color = color;
                    }

                    // Rotate matrix
                    const int res = resolution + 2;
                    for (int i = 0; i < res; i++)
                    {
                        circle[i].x = x + cos(angle) * (circle[i].x - x) - sin(angle) * (circle[i].y - y);
                        circle[i].y = y + sin(angle) * (circle[i].x - x) + cos(angle) * (circle[i].y - y);
                    }

                    device->CreateVertexBuffer(
                        (resolution + 2) * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vertexBuffer, nullptr);

                    VOID* pVertices;
                    vertexBuffer->Lock(0, (resolution + 2) * sizeof(Vertex), &pVertices, 0);
                    memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(Vertex));
                    vertexBuffer->Unlock();

                    device->SetTexture(0, nullptr);
                    device->SetPixelShader(nullptr);
                    if (smoothing)
                    {
                        device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
                        device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
                    }
                    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

                    device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));
                    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

                    device->DrawPrimitive(D3DPT_LINESTRIP, 0, resolution);
                    if (vertexBuffer != nullptr)
                    {
                        vertexBuffer->Release();
                    }
                });
        }

        void CircleFilled(float x, float y, const float rad, const float rotate, const CircleType type, const int resolution, const DWORD color)
        {
            const auto res = GetResolution();
            x *= res.x;
            y *= res.y;

            awaitingCalls.emplace_back(
                [this, x, y, rad, rotate, type, resolution, color]
                {
                    std::vector<Vertex> circle(resolution + 2);
                    const float angle = rotate * D3DX_PI / 180;
                    float pi;

                    if (type == CircleType::Full)
                    {
                        pi = D3DX_PI; // Full circle
                    }
                    if (type == CircleType::Half)
                    {
                        pi = D3DX_PI / 2; // 1/2 circle
                    }
                    if (type == CircleType::Quarter)
                    {
                        pi = D3DX_PI / 4; // 1/4 circle
                    }

                    circle[0].x = x;
                    circle[0].y = y;
                    circle[0].z = 0;
                    circle[0].rhw = 1;
                    circle[0].color = color;

                    for (int i = 1; i < resolution + 2; i++)
                    {
                        circle[i].x = x - rad * cos(pi * ((i - 1) / (resolution / 2.0f)));
                        circle[i].y = y - rad * sin(pi * ((i - 1) / (resolution / 2.0f)));
                        circle[i].z = 0;
                        circle[i].rhw = 1;
                        circle[i].color = color;
                    }

                    // Rotate matrix
                    const int res = resolution + 2;
                    for (int i = 0; i < res; i++)
                    {
                        circle[i].x = x + cos(angle) * (circle[i].x - x) - sin(angle) * (circle[i].y - y);
                        circle[i].y = y + sin(angle) * (circle[i].x - x) + cos(angle) * (circle[i].y - y);
                    }

                    device->CreateVertexBuffer(
                        (resolution + 2) * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vertexBuffer, nullptr);

                    VOID* pVertices;
                    vertexBuffer->Lock(0, (resolution + 2) * sizeof(Vertex), &pVertices, 0);
                    memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(Vertex));
                    vertexBuffer->Unlock();

                    device->SetTexture(0, nullptr);
                    device->SetPixelShader(nullptr);
                    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

                    device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));
                    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
                    device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
                    if (vertexBuffer != nullptr)
                    {
                        vertexBuffer->Release();
                    }
                });
        }

        void Box(const float x, const float y, const float w, const float h, const float lineWidth, const DWORD color)
        {
            if (lineWidth == 0.0f || lineWidth == 1.0f)
            {
                BoxFilled(x, y, w, 1, color);                     // Top
                BoxFilled(x, y + h - 1, w, 1, color);             // Bottom
                BoxFilled(x, y + 1, 1, h - 2 * 1, color);         // Left
                BoxFilled(x + w - 1, y + 1, 1, h - 2 * 1, color); // Right
            }
            else
            {
                BoxFilled(x, y, w, lineWidth, color);                                             // Top
                BoxFilled(x, y + h - lineWidth, w, lineWidth, color);                             // Bottom
                BoxFilled(x, y + lineWidth, lineWidth, h - 2 * lineWidth, color);                 // Left
                BoxFilled(x + w - lineWidth, y + lineWidth, lineWidth, h - 2 * lineWidth, color); // Right
            }
        }

        void BoxBordered(const float x, const float y, const float w, const float h, const float borderWidth, const DWORD color, const DWORD colorBorder)
        {
            BoxFilled(x, y, w, h, color);
            Box(x - borderWidth, y - borderWidth, w + 2 * borderWidth, h + borderWidth, borderWidth, colorBorder);
        }

        void BoxFilled(float x, float y, float w, float h, const DWORD color)
        {
            const auto res = GetResolution();
            x *= res.x;
            w *= res.x;
            y *= res.y;
            h *= res.y;

            awaitingCalls.emplace_back(
                [this, x, y, w, h, color]
                {
                    Vertex v[4];

                    v[0].color = v[1].color = v[2].color = v[3].color = color;

                    v[0].z = v[1].z = v[2].z = v[3].z = 0;
                    v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 0;

                    v[0].x = x;
                    v[0].y = y;
                    v[1].x = x + w;
                    v[1].y = y;
                    v[2].x = x + w;
                    v[2].y = y + h;
                    v[3].x = x;
                    v[3].y = y + h;

                    const unsigned short indexes[] = { 0, 1, 3, 1, 2, 3 };

                    device->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vertexBuffer, nullptr);
                    device->CreateIndexBuffer(2 * sizeof(short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr);

                    VOID* pVertices;
                    vertexBuffer->Lock(0, sizeof(v), &pVertices, 0);
                    memcpy(pVertices, v, sizeof(v));
                    vertexBuffer->Unlock();

                    VOID* pIndex;
                    indexBuffer->Lock(0, sizeof(indexes), &pIndex, 0);
                    memcpy(pIndex, indexes, sizeof(indexes));
                    indexBuffer->Unlock();

                    device->SetTexture(0, nullptr);
                    device->SetPixelShader(nullptr);
                    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

                    device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));
                    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
                    device->SetIndices(indexBuffer);

                    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

                    vertexBuffer->Release();
                    indexBuffer->Release();
                });
        }

        void BoxRounded(const float x, const float y, const float w, const float h, const float radius, const bool smoothing, const DWORD color,
                        const DWORD bcolor)
        {
            BoxFilled(x + radius, y + radius, w - 2 * radius - 1, h - 2 * radius - 1, color); // Center rect.
            BoxFilled(x + radius, y + 1, w - 2 * radius - 1, radius - 1, color);              // Top rect.
            BoxFilled(x + radius, y + h - radius - 1, w - 2 * radius - 1, radius, color);     // Bottom rect.
            BoxFilled(x + 1, y + radius, radius - 1, h - 2 * radius - 1, color);              // Left rect.
            BoxFilled(x + w - radius - 1, y + radius, radius, h - 2 * radius - 1, color);     // Right rect.

            // Smoothing method
            if (smoothing)
            {
                CircleFilled(x + radius, y + radius, radius - 1, 0, CircleType::Quarter, 16, color);                   // Top-left corner
                CircleFilled(x + w - radius - 1, y + radius, radius - 1, 90, CircleType::Quarter, 16, color);          // Top-right corner
                CircleFilled(x + w - radius - 1, y + h - radius - 1, radius - 1, 180, CircleType::Quarter, 16, color); // Bottom-right corner
                CircleFilled(x + radius, y + h - radius - 1, radius - 1, 270, CircleType::Quarter, 16, color);         // Bottom-left corner

                Circle(x + radius + 1, y + radius + 1, radius, 0, CircleType::Quarter, true, 16, bcolor);           // Top-left corner
                Circle(x + w - radius - 2, y + radius + 1, radius, 90, CircleType::Quarter, true, 16, bcolor);      // Top-right corner
                Circle(x + w - radius - 2, y + h - radius - 2, radius, 180, CircleType::Quarter, true, 16, bcolor); // Bottom-right corner
                Circle(x + radius + 1, y + h - radius - 2, radius, 270, CircleType::Quarter, true, 16, bcolor);     // Bottom-left corner

                Line(x + radius, y + 1, x + w - radius - 1, y + 1, 1, false, bcolor);         // Top line
                Line(x + radius, y + h - 2, x + w - radius - 1, y + h - 2, 1, false, bcolor); // Bottom line
                Line(x + 1, y + radius, x + 1, y + h - radius - 1, 1, false, bcolor);         // Left line
                Line(x + w - 2, y + radius, x + w - 2, y + h - radius - 1, 1, false, bcolor); // Right line
            }
            else
            {
                CircleFilled(x + radius, y + radius, radius, 0, CircleType::Quarter, 16, color);                   // Top-left corner
                CircleFilled(x + w - radius - 1, y + radius, radius, 90, CircleType::Quarter, 16, color);          // Top-right corner
                CircleFilled(x + w - radius - 1, y + h - radius - 1, radius, 180, CircleType::Quarter, 16, color); // Bottom-right corner
                CircleFilled(x + radius, y + h - radius - 1, radius, 270, CircleType::Quarter, 16, color);         // Bottom-left corner
            }
        }

        void Sprite(const LPDIRECT3DTEXTURE9 tex, float x, float y, const float resolution, const float scale, const float rotation)
        {
            const auto res = GetResolution();
            x *= res.x;
            y *= res.y;

            awaitingCalls.emplace_back(
                [this, tex, x, y, resolution, scale, rotation]
                {
                    const float screenPosX = x;
                    const float screenPosY = y;

                    // Texture being used is 64x64:
                    const auto spriteCentre = D3DXVECTOR2(resolution / 2, resolution / 2);

                    // Screen position of the sprite
                    const auto trans = D3DXVECTOR2(screenPosX, screenPosY);

                    // Build our matrix to rotate, scale and position our sprite
                    D3DXMATRIX mat;

                    const D3DXVECTOR2 scaling(scale, scale);

                    // out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
                    D3DXMatrixTransformation2D(&mat, nullptr, 0.0, &scaling, &spriteCentre, rotation, &trans);

                    // device->SetRenderState(D3DRS_ZENABLE, false);
                    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
                    device->SetPixelShader(nullptr);
                    sprite->Begin(NULL);
                    sprite->SetTransform(&mat); // Tell the sprite about the matrix
                    sprite->Draw(tex, nullptr, nullptr, nullptr, 0xFFFFFFFF);
                    sprite->End();
                });
        }

        void CaptureSurface(float x, float y, float w, float h)
        {
            const auto res = GetResolution();
            x *= res.x;
            w *= res.x;
            y *= res.y;
            h *= res.y;

            awaitingCalls.emplace_back(
                [this, x, y, w, h]
                {
                    IDirect3DSurface9* backBuffer;
                    device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

                    LPDIRECT3DTEXTURE9 textureMap;
                    D3DXCreateTexture(
                        device, static_cast<UINT>(w), static_cast<UINT>(h), D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &textureMap);

                    LPDIRECT3DSURFACE9 pTexSurface;
                    textureMap->GetSurfaceLevel(0, &pTexSurface);

                    const RECT clippingRegion = { static_cast<LONG>(x), static_cast<LONG>(y), static_cast<LONG>(x + w), static_cast<LONG>(y + h) };
                    device->StretchRect(backBuffer, &clippingRegion, pTexSurface, nullptr, D3DTEXF_NONE);

                    device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                    const auto spriteCentre = D3DXVECTOR2(w, h);
                    const auto trans = D3DXVECTOR2(x, y);

                    D3DXMATRIX mat;
                    const D3DXVECTOR2 scaling(1.0f, 1.0f);

                    D3DXMatrixTransformation2D(&mat, nullptr, 0.0, &scaling, &spriteCentre, 0.0f, &trans);

                    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
                    device->SetPixelShader(nullptr);

                    if (!sprite)
                    {
                        D3DXCreateSprite(device, &sprite);
                    }

                    sprite->Begin(NULL);
                    sprite->SetTransform(&mat); // Tell the sprite about the matrix
                    sprite->Draw(textureMap, nullptr, nullptr, nullptr, 0xFFFFFFFF);
                    sprite->End();

                    pTexSurface->Release();
                    textureMap->Release();
                    backBuffer->Release();
                });
        }

        void SetDevice(IDirect3DDevice9* dev) { device = dev; }

        [[nodiscard]]
        glm::vec2 GetResolution() const
        {
            D3DDISPLAYMODE mode;
            device->GetDisplayMode(0, &mode);

            return { static_cast<float>(mode.Width), static_cast<float>(mode.Height) };
        }

        void Draw()
        {
            for (auto& call : awaitingCalls)
            {
                call();
            }

            awaitingCalls.clear();
        }
};
