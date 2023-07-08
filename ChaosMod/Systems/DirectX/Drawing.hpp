#pragma once

#include <ctime>
#include <d3dx9core.h>
#include <stdio.h>

#include "D3DTypes.hpp"

class DXDrawing
{
        struct SD3DVertex
        {
                float x, y, z, rhw;
                DWORD colour;
        };

        IDirect3DDevice9* pDevice;

    public:
        DXDrawing(IDirect3DDevice9* device) { pDevice = device; }
        ~DXDrawing() {}

        void String(int x, int y, D3DCOLOR colour, ID3DXFont* font, bool outlined, char* string, ...)
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
        int StringWidth(ID3DXFont* font, char* string)
        {
            RECT pRect = RECT();
            font->DrawTextA(NULL, string, strlen(string), &pRect, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));
            return pRect.right - pRect.left;
        }
        void Line(int x, int y, int x2, int y2, D3DCOLOR colour)
        {
            SD3DVertex pVertex[2] = {
                { static_cast<float>(x),  static_cast<float>(y), 0.0f, 1.0f, colour},
                {static_cast<float>(x2), static_cast<float>(y2), 0.0f, 1.0f, colour}
            };
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &pVertex, sizeof(SD3DVertex));
        }
        void FilledBox(int x, int y, int width, int height, D3DCOLOR colour)
        {
            SD3DVertex pVertex[4] = {
                {        static_cast<float>(x), static_cast<float>(y) + height, 0.0f, 1.0f, colour},
                {        static_cast<float>(x),          static_cast<float>(y), 0.0f, 1.0f, colour},
                {static_cast<float>(x) + width, static_cast<float>(y) + height, 0.0f, 1.0f, colour},
                {static_cast<float>(x) + width,          static_cast<float>(y), 0.0f, 1.0f, colour}
            };
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(SD3DVertex));
        }
        void FilledBoxOutlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR outlinecolor)
        {
            this->BorderedBox(x, y, width, height, thickness, outlinecolor);
            this->FilledBox(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), colour);
        }
        void BorderedBox(int x, int y, int width, int height, int thickness, D3DCOLOR colour)
        {
            this->FilledBox(x, y, width, thickness, colour);
            this->FilledBox(x, y, thickness, height, colour);
            this->FilledBox(x + width - thickness, y, thickness, height, colour);
            this->FilledBox(x, y + height - thickness, width, thickness, colour);
        }
        void BorderedBoxOutlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR outlinecolor)
        {
            this->BorderedBox(x, y, width, height, thickness, outlinecolor);
            this->BorderedBox(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), thickness, colour);
            this->BorderedBox(x + (thickness * 2), y + (thickness * 2), width - (thickness * 4), height - (thickness * 4), thickness, outlinecolor);
        }
        void GradientBox(int x, int y, int width, int height, D3DCOLOR colour, D3DCOLOR color2, bool vertical)
        {
            SD3DVertex pVertex[4] = {
                {        static_cast<float>(x),          static_cast<float>(y), 0.0f, 1.0f,                     colour},
                {static_cast<float>(x) + width,          static_cast<float>(y), 0.0f, 1.0f, vertical ? colour : color2},
                {        static_cast<float>(x), static_cast<float>(y) + height, 0.0f, 1.0f, vertical ? color2 : colour},
                {static_cast<float>(x) + width, static_cast<float>(y) + height, 0.0f, 1.0f,                     color2}
            };
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(SD3DVertex));
        }
        void GradientBoxOutlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR color2, D3DCOLOR outlinecolor, bool vertical)
        {
            this->BorderedBox(x, y, width, height, thickness, outlinecolor);
            this->GradientBox(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), colour, color2, vertical);
        }
        void Circle(int x, int y, int radius, int points, D3DCOLOR colour)
        {
            SD3DVertex* pVertex = new SD3DVertex[points + 1];
            for (int i = 0; i <= points; i++)
            {
                pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y - radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
            }
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points, pVertex, sizeof(SD3DVertex));
            delete[] pVertex;
        }
        void FilledCircle(int x, int y, int radius, int points, D3DCOLOR colour)
        {
            SD3DVertex* pVertex = new SD3DVertex[points + 1];
            for (int i = 0; i <= points; i++)
            {
                pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y + radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
            }
            pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, pVertex, sizeof(SD3DVertex));
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
