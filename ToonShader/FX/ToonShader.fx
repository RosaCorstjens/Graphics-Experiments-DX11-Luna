#include "LightHelper.fx"

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	Material gMaterial;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffTexture;
Texture2D gEdgeTexture;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 DiffTex     : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 DiffTex     : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.DiffTex = vin.DiffTex;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);

	// Sample texture with anisotropic wrap mode.
    texColor = gDiffTexture.Sample(samAnisotropic, pin.DiffTex);
    clip(texColor.a - 0.1f);
    
    // check for adding line color
    float lineColor = 1;
    lineColor = gEdgeTexture.Sample(samAnisotropic, pin.DiffTex).r;

    if (lineColor < 0.9)
    {
        texColor.rgb = lineColor;
    }
    
	// Common to take alpha from diffuse material and texture.
    texColor.a = gMaterial.Diffuse.a * texColor.a;

    return texColor;
}

technique11 Toon
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

