/*------------------.
| :: Description :: |
'-------------------/

Flipbook Animation PS (version 1.2.4)

Copyright:
This code © 2018-2023 Jakub Maksymilian Fober

License:
This work is licensed under the Creative Commons
Attribution-ShareAlike 4.0 International License.
To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/4.0/
*/

/*--------------.
| :: Commons :: |
'--------------*/

#include "ReShade.fxh"
#include "ReShadeUI.fxh"

/*-------------.
| :: Macros :: |
'-------------*/

#ifndef FLIPBOOK_PIXELATED
	#define FLIPBOOK_PIXELATED 0
#endif
// Texture path
#ifndef FLIPBOOK_TEXTURE
	#define FLIPBOOK_TEXTURE "VibeCat.png" // Texture file name
#endif
// Texture width
#ifndef FLIPBOOK_SIZE_X
	#define FLIPBOOK_SIZE_X 4096 // Texture horizontal resolution
#endif
// Texture height
#ifndef FLIPBOOK_SIZE_Y
	#define FLIPBOOK_SIZE_Y 2304 // Texture vertical resolution
#endif

uniform int3 Size
<	__UNIFORM_INPUT_INT3
	ui_label = "X frames, Y frames, FPS";
	ui_tooltip = "Adjust flipbook texture dimensions and framerate\n"
		"To change texture resolution and name,\n"
		"add following preprocessor definition:\n"
		"  FLIPBOOK_TEXTURE 'name.png'\n"
		"  FLIPBOOK_SIZE_X [ResolutionX]\n"
		"  FLIPBOOK_SIZE_Y [ResolutionY]";
	ui_min = 1; ui_max = 32;
	ui_category = "Texture dimensions";
> = int3(32, 32, 30);

uniform float3 Position
<	__UNIFORM_SLIDER_FLOAT3
	ui_label = "X position, Y position, Scale";
	ui_tooltip = "Adjust flipbook texture size and position";
	ui_min = float3(0f, 0f, 0.1); ui_max = float3(1f, 1f, 1f);
	ui_category = "Position on screen";
> = float3(0f, 0f, 1f);

// Get time in milliseconds from start
uniform float timer < source = "timer"; >;

texture FlipbookTex
< source = FLIPBOOK_TEXTURE; >
{
	Width  = FLIPBOOK_SIZE_X;
	Height = FLIPBOOK_SIZE_Y;
};
sampler FlipbookSampler
{
	Texture = FlipbookTex;
	AddressU = REPEAT;
	AddressV = REPEAT;
#if FLIPBOOK_PIXELATED==1
	MagFilter = POINT;
	MinFilter = POINT;
	MipFilter = POINT;
#endif
};

float Mask(float2 Coord)
{
	Coord = abs(Coord*2f-1f);
	float2 Pixel = fwidth(Coord);
	float2 Borders = 1f-smoothstep(1f-Pixel, 1f+Pixel, Coord);
	return min(Borders.x, Borders.y);
}

float3 FlipbookPS(
	float4 vois : SV_Position,
	float2 texcoord : TexCoord
) : SV_Target
{
	float ScreenAspect = ReShade::AspectRatio;
	// Screen aspect divided by animation frame aspect
	float AspectDifference = (ScreenAspect * float(Size.x * FLIPBOOK_SIZE_Y )) / float(Size.y * FLIPBOOK_SIZE_X);

	// Scale coordinates
	float2 scale = 1f / Position.z;
	scale.y -= 0.1f;

	float2 scaledCoord = texcoord * scale;

	// Adjust aspect ratio
	if(AspectDifference > 1f)
	{
		scaledCoord.x *= AspectDifference;
		scale.x *= AspectDifference;
	}
	else if(AspectDifference < 1f)
	{
		scaledCoord.y /= AspectDifference;
		scale.y /= AspectDifference;
	}

	// Offset coordinates
	scaledCoord += (1f - scale) * float2(Position.x, 1f - Position.y);
	scaledCoord.x += 0.01f;
	scaledCoord.y -= 0.02f;

	float mask = Mask(scaledCoord);
	// Frame time in milliseconds
	float frameRateInMs = 1000f / Size.z;
	float2 animCoord = scaledCoord / Size.xy;
	// Sample UVs for horizontal and vertical frames
	animCoord.x += frac(floor(timer / frameRateInMs) / Size.x);
	animCoord.y += frac(floor(timer / (frameRateInMs * Size.x)) / Size.y);

	// Sample display image
	float3 color = tex2D(ReShade::BackBuffer, texcoord).rgb;
	// Sample flipbook texture
	float4 animTex = tex2D(FlipbookSampler, animCoord);

	float3 resultingColor = lerp(color, animTex.rgb, animTex.a * mask);

    // Reject the color green from our Greenscreen'ed image
    if (scaledCoord.y < 0.015f || scaledCoord.y > 0.985f || scaledCoord.x > 0.8f || (resultingColor.g > .7f && resultingColor.r < .6f && resultingColor.b < .6f))
    {
    	resultingColor = color;
	}

    return resultingColor;
}

/*-------------.
| :: Output :: |
'-------------*/

technique Flipbook
<
	ui_tooltip =
		"Flipbook animation FX:\n"
		"======================\n"
		"To change texture resolution and name,\n"
		"add following preprocessor definition:\n"
		"  flipbook 'name.png'\n"
		"  FLIPBOOK_SIZE_X [ResolutionX]\n"
		"  FLIPBOOK_SIZE_Y [ResolutionY]"
		"\n"
		"This effect © 2018-2023 Jakub Maksymilian Fober\n"
		"Licensed under CC BY-SA 4.0";
>
{
	pass
	{
		VertexShader = PostProcessVS;
		PixelShader = FlipbookPS;
	}
}
