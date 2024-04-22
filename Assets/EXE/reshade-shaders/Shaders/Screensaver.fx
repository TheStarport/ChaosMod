#include "ReShade.fxh"

static const float Ratio = 0.4;

uniform float xStartPos <source = "random"; min = 1; max = BUFFER_WIDTH * Ratio;>;
uniform float yStartPos <source = "random"; min = 1; max = BUFFER_HEIGHT * Ratio;>;

static const float2 Size = float2(BUFFER_WIDTH, BUFFER_HEIGHT) * Ratio;

// R = X Pos
// G = Y Pos
// B = X Speed
// A = Y Speed
texture BlockDataTex
{
	 Format = RGBA32F;
};

sampler BlockData
{
	Texture = BlockDataTex;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
};

texture LastBlockDataTex
{
	 Format = RGBA32F;
};

sampler LastBlockData
{
	Texture = LastBlockDataTex;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
};

uniform float FrameTime <source = "frametime";>;
#define delta (FrameTime * 0.001)

void UpdatePos(float4 p : SV_POSITION, float2 uv : TEXCOORD, out float4 data : SV_TARGET0)
{
    data = tex2Dfetch(LastBlockData, 0);

    // Not initalized yet
    if (data.z == 0)
    {
        data.x = xStartPos;
        data.y = yStartPos;
        data.z = 75f;
        data.w = 75.f;
    }

    if (data.x + Size.x >= BUFFER_WIDTH)
    {
        data.x = BUFFER_WIDTH - Size.x;
        data.z *= -1;
    }
    else if (data.x <= 0)
    {
        data.x = 0;
        data.z *= -1;
    }

    if (data.y + Size.y >= BUFFER_HEIGHT)
    {
        data.y = BUFFER_HEIGHT - Size.y;
        data.w *= -1;
    }
    else if (data.y <= 0)
    {
        data.y = 0;
        data.w *= -1;
    }

    data.x += data.z * delta;
    data.y += data.w * delta;
}

void SavePos(float4 p : SV_POSITION, float2 uv : TEXCOORD, out float4 data : SV_TARGET0)
{
    data = tex2Dfetch(BlockData, 0);
}

float4 PS_Screensaver(float4 vpos : SV_Position, float2 uv : TEXCOORD) : SV_Target
{
	float4 data = tex2Dfetch(BlockData, 0);

    float2 pos = vpos.xy + 1.0;
    float4 color = tex2Dlod(ReShade::BackBuffer, float4(uv, 0, 0));
    float2 areaCenter = Size / 2.0;

    float2 destPos = (pos.xy - data.xy - areaCenter) / 1 + data.xy + areaCenter;
    float2 destCoord = destPos * ReShade::PixelSize;
    float2 destBottomRight = data.xy + Size;

    float4 colorDest = tex2Dlod(ReShade::BackBuffer, float4(destCoord, 0, 0));
    if (pos.x >= data.x && pos.y >= data.y && pos.x <= destBottomRight.x && pos.y <= destBottomRight.y)
    {
        color.xyz = lerp(color.xyz, colorDest.xyz, 1.0);
    }
    else
    {
        color.xyz = float3(0, 0, 0);
    }

    return color;
}

technique Screensaver
{
    pass UpdatePosPass
    {
        VertexShader = PostProcessVS;
        PixelShader = UpdatePos;
        RenderTarget0 = BlockDataTex;
    }
    pass SavePosPass
    {
        VertexShader = PostProcessVS;
        PixelShader = SavePos;
        RenderTarget0 = LastBlockDataTex;
    }
    pass Render
    {
        VertexShader = PostProcessVS;
        PixelShader = PS_Screensaver;
    }
}
