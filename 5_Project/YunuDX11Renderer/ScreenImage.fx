//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"

Texture2D imageTexture;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
    float3 Pos    : POSITION;
    float3 Tex     : TEXCOORD;
};

struct VertexOut
{
    float4 Pos    : SV_POSITION;
    float3 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.Pos = float4(vin.Pos, 1);
    vout.Tex = vin.Tex;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 texColor = float4(1, 1, 1, 1);
    texColor = imageTexture.Sample(samAnisotropic, pin.Tex);
    return texColor;
}

technique11 imageTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
