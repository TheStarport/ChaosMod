#include "ReShade.fxh"

static const int colors = 256;

float4 PS_Mexico(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target
{
    float3 ink = float3(1.0, 0.65, 0.0);
    float3 c11 = tex2D(ReShade::BackBuffer, uv).xyz;
    float lct = floor(colors * length(c11)) / colors;
    return float4(lct * ink, 1);
}

technique Mexico
{
    pass Mexico0
    {
         VertexShader = PostProcessVS;
         PixelShader  = PS_Mexico;
    }
}
