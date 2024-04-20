#include "ReShade.fxh"

static const float2 Source = float2(0, 0);
static const float2 Dest = float2(0, 0);
static const float Ratio = 0.4;
static const float2 Size = float2(BUFFER_WIDTH, BUFFER_HEIGHT) * Ratio;
static const float Opacity = 1.0;
static const float Zoom = 1.0;
static const bool EnableOutline = true;
static const float OutlineWidth = 2.0;
static const float OutlineOpacity = 1.0;

float4 PS_PictureInPicture(float4 vpos : SV_Position, float2 texcoord : TEXCOORD) : SV_Target {
    float2 pos = texcoord / ReShade::PixelSize;
    float4 color = tex2Dlod(ReShade::BackBuffer, float4(texcoord, 0, 0));
    float2 areaCenter = Size;

    float2 destPos = (pos.xy - Dest - 1);

    float2 destCoord = destPos * ReShade::PixelSize * (1 / Ratio);

    float2 destTopLeft = Dest;
    float2 destBottomRight = Dest + Size;

    float4 colorDest = tex2Dlod(ReShade::BackBuffer, float4(destCoord, 0, 0));

    if (EnableOutline)
	{
        if (pos.x >= (destTopLeft.x - OutlineWidth) && pos.y >= (destTopLeft.y - OutlineWidth)
            && pos.x <= (destBottomRight.x + OutlineWidth) && pos.y <= (destBottomRight.y + OutlineWidth))
        {
            color.xyz = lerp(color.xyz, float3(1.0, 1.0, 1.0), OutlineOpacity);
        }
    }

    if (pos.x >= destTopLeft.x && pos.y >= destTopLeft.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y)
	{
        color.xyz = lerp(color.xyz, colorDest.xyz, Opacity);
    }

    return color;
}

//techniques
technique PictureInPicture {
    pass {
        VertexShader = PostProcessVS;
        PixelShader = PS_PictureInPicture;
    }
}
