#pragma once

#include <ctime>
#include <d3dx9core.h>
#include <stdio.h>

#include "D3DTypes.hpp"

class DrawingHelper final : public Singleton<DrawingHelper>
{
        struct Vertex
        {
                float x, y, z, rhw;
                DWORD colour;
        };

        IDirect3DDevice9* pDevice = nullptr;

    public:
        void SetDevice(IDirect3DDevice9* device) { pDevice = device; }

        void String(int x, int y, D3DCOLOR colour, ID3DXFont* font, bool outlined, char* string, ...) const
        {
            va_list args;
            char cBuffer[256];

            va_start(args, string);
            vsprintf_s(cBuffer, string, args);
            va_end(args);

            RECT pRect;
            if (outlined)
            {
                pRect.left = x - 1;
                pRect.top = y;
                font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
                pRect.left = x + 1;
                pRect.top = y;
                font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
                pRect.left = x;
                pRect.top = y - 1;
                font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
                pRect.left = x;
                pRect.top = y + 1;
                font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
            }
            pRect.left = x;
            pRect.top = y;
            font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, colour);
        }

        static int StringWidth(ID3DXFont* font, char* string)
        {
            RECT pRect = RECT();
            font->DrawTextA(NULL, string, strlen(string), &pRect, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));
            return pRect.right - pRect.left;
        }

        void Line(int x, int y, int x2, int y2, D3DCOLOR colour) const
        {
            Vertex pVertex[2] = {
                { static_cast<float>(x),  static_cast<float>(y), 0.0f, 1.0f, colour},
                {static_cast<float>(x2), static_cast<float>(y2), 0.0f, 1.0f, colour}
            };
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &pVertex, sizeof(Vertex));
        }
        void FilledBox(int x, int y, int width, int height, D3DCOLOR colour) const
        {
            const Vertex pVertex[4] = {
                {        static_cast<float>(x), static_cast<float>(y) + height, 0.0f, 1.0f, colour},
                {        static_cast<float>(x),          static_cast<float>(y), 0.0f, 1.0f, colour},
                {static_cast<float>(x) + width, static_cast<float>(y) + height, 0.0f, 1.0f, colour},
                {static_cast<float>(x) + width,          static_cast<float>(y), 0.0f, 1.0f, colour}
            };
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(Vertex));
        }

        void FilledBoxOutlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR outlinecolor) const
        {
            BorderedBox(x, y, width, height, thickness, outlinecolor);
            FilledBox(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), colour);
        }

        void BorderedBox(int x, int y, int width, int height, int thickness, D3DCOLOR colour) const
        {
            FilledBox(x, y, width, thickness, colour);
            FilledBox(x, y, thickness, height, colour);
            FilledBox(x + width - thickness, y, thickness, height, colour);
            FilledBox(x, y + height - thickness, width, thickness, colour);
        }

        void BorderedBoxOutlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR outlinecolor) const
        {
            BorderedBox(x, y, width, height, thickness, outlinecolor);
            BorderedBox(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), thickness, colour);
            BorderedBox(x + (thickness * 2), y + (thickness * 2), width - (thickness * 4), height - (thickness * 4), thickness, outlinecolor);
        }

        void GradientBox(int x, int y, int width, int height, D3DCOLOR colour, D3DCOLOR color2, bool vertical) const
        {
            const Vertex pVertex[4] = {
                {        static_cast<float>(x),          static_cast<float>(y), 0.0f, 1.0f,                     colour},
                {static_cast<float>(x) + width,          static_cast<float>(y), 0.0f, 1.0f, vertical ? colour : color2},
                {        static_cast<float>(x), static_cast<float>(y) + height, 0.0f, 1.0f, vertical ? color2 : colour},
                {static_cast<float>(x) + width, static_cast<float>(y) + height, 0.0f, 1.0f,                     color2}
            };
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(Vertex));
        }

        void GradientBoxOutlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR color2, D3DCOLOR outlinecolor,
                                 bool vertical) const
        {
            BorderedBox(x, y, width, height, thickness, outlinecolor);
            GradientBox(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), colour, color2, vertical);
        }

        void Circle(int x, int y, int radius, int points, D3DCOLOR colour) const
        {
            Vertex* pVertex = new Vertex[points + 1];

            for (int i = 0; i <= points; i++)
            {
                pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y - radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
            }

            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points, pVertex, sizeof(Vertex));
            delete[] pVertex;
        }
        void FilledCircle(int x, int y, int radius, int points, D3DCOLOR colour) const
        {
            Vertex* pVertex = new Vertex[points + 1];
            for (int i = 0; i <= points; i++)
            {
                pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y + radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
            }
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, pVertex, sizeof(Vertex));
            delete[] pVertex;
        }
        int FrameRate()
        {
            static int iFps, iLastFps;
            static float flLastTickCount, flTickCount;
            flTickCount = clock() * 0.001f;
            iFps++;
            if ((flTickCount - flLastTickCount) >= 1.0f)
            {
                flLastTickCount = flTickCount;
                iLastFps = iFps;
                iFps = 0;
            }
            return iLastFps;
        }
};
