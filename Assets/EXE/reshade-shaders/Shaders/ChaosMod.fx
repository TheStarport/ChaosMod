#include "ReShade.fxh"
#include "ReshadeUI.fxh"

#include "DrawText.fxh"
#include "FXShaders/Common.fxh"
#include "FXShaders/Canvas.fxh"
#include "FXShaders/Digits.fxh"
#include "FXShaders/Math.fxh"

#undef fmod
#define fmod(x, y) x - y * floor(x/y)

#define MAX_LINE 5
#define MAX_PTS 5

#include "RadegastShaders.Depth.fxh"
#include "RadegastShaders.Transforms.fxh"
#include "RadegastShaders.BlendingModes.fxh"

uniform float Timer <source = "timer";>;

#define DECLARE_VARIABLE_TEX(name, format) \
texture name##Tex \
{ \
	Format = format; \
}; \
\
sampler name \
{ \
	Texture = name##Tex; \
	MinFilter = POINT; \
	MagFilter = POINT; \
	MipFilter = POINT; \
}

float hash(in float n)
{
    return frac(sin(n) * 43758.5453123);
}

float mod(float x, float y)
{
    return x - y * floor(x / y);
}

#include "Nightvision.reshade"
#include "CRT.reshade"
#include "Drunk.reshade"
#include "FilmGrain.reshade"
#include "Magnifier.reshade"
//#include "Rain.reshade"
#include "Pong.reshade"
#include "JPEG.reshade"
#include "LANoire.reshade"
#include "TurnTurtle.reshade"
#include "Vignette.reshade"