static const float Ratio = 0.25;
static const float Size = BUFFER_WIDTH * Ratio;

#include "ReShade.fxh"

float4 PS_PortraitMode(float4 vpos : SV_Position, float2 uv : TEXCOORD) : SV_Target
{
    float2 pos = vpos.xy + 1.0;
    float4 color = tex2Dlod(ReShade::BackBuffer, float4(uv, 0, 0));

    if (pos.x < Size || pos.x > BUFFER_WIDTH - Size)
    {
        color.xyz = float3(0, 0, 0);
    }

    return color;
}

technique PortraitMode
{
    pass
    {
        VertexShader = PostProcessVS;
        PixelShader = PS_PortraitMode;
    }
}
