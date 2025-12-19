#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;
    float gScreenWidth;
    float gScreenHeigth;
    float3 gPadding;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	Material gMaterial;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffTexture;

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
	float2 DiffTex : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 DiffTex : TEXCOORD0;
	float2 Tex1    : TEXCOORD1;
	float2 Tex2    : TEXCOORD2;
    float2 Tex3    : TEXCOORD3;
    float2 Tex4    : TEXCOORD4;
    float2 Tex5    : TEXCOORD5;
    float2 Tex6    : TEXCOORD6;
    float2 Tex7    : TEXCOORD7;
	float2 Tex8    : TEXCOORD8;
    float2 Tex9    : TEXCOORD9;
};

VertexOut HorizontalVS(VertexIn vin)
{
	VertexOut vout;

    float texelSize;

	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.DiffTex = vin.DiffTex;

    // determine float size of a texel for the specified width.
    texelSize = 1.0f / gScreenWidth;

    // Create UV coordinates for the pixel and its four horizontal neighbors on either side.
    vout.Tex1 = vin.DiffTex + float2(texelSize * -4.0f, 0.0f);
    vout.Tex2 = vin.DiffTex + float2(texelSize * -3.0f, 0.0f);
    vout.Tex3 = vin.DiffTex + float2(texelSize * -2.0f, 0.0f);
    vout.Tex4 = vin.DiffTex + float2(texelSize * -1.0f, 0.0f);
    vout.Tex5 = vin.DiffTex + float2(texelSize * 0.0f, 0.0f);
    vout.Tex6 = vin.DiffTex + float2(texelSize * 1.0f, 0.0f);
    vout.Tex7 = vin.DiffTex + float2(texelSize * 2.0f, 0.0f);
    vout.Tex8 = vin.DiffTex + float2(texelSize * 3.0f, 0.0f);
    vout.Tex9 = vin.DiffTex + float2(texelSize * 4.0f, 0.0f);

	return vout;
}

VertexOut VerticalVS(VertexIn vin)
{
    VertexOut vout;

    float texelSize;

	// Transform to world space space.
    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.DiffTex = vin.DiffTex;

    // determine float size of a texel for the specified width.
    texelSize = 1.0f / gScreenHeigth;

    // Create UV coordinates for the pixel and its four horizontal neighbors on either side.
    vout.Tex1 = vin.DiffTex + float2(0.0f, texelSize * -4.0f);
    vout.Tex2 = vin.DiffTex + float2(0.0f, texelSize * -3.0f);
    vout.Tex3 = vin.DiffTex + float2(0.0f, texelSize * -2.0f);
    vout.Tex4 = vin.DiffTex + float2(0.0f, texelSize * -1.0f);
    vout.Tex5 = vin.DiffTex + float2(0.0f, texelSize * 0.0f);
    vout.Tex6 = vin.DiffTex + float2(0.0f, texelSize * 1.0f);
    vout.Tex7 = vin.DiffTex + float2(0.0f, texelSize * 2.0f);
    vout.Tex8 = vin.DiffTex + float2(0.0f, texelSize * 3.0f);
    vout.Tex9 = vin.DiffTex + float2(0.0f, texelSize * 4.0f);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // Create the weights that each neighbor pixel will contribute to the blur.
    float weight0 = 1.0f;
    float weight1 = 0.9f;
    float weight2 = 0.55f;
    float weight3 = 0.18f;
    float weight4 = 0.1f;

    // Create a normalized value to average the weights out a bit.
    float normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

    // Normalize the weights
    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;

	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Sample the texture color
    float4 texColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //gDiffTexture.Sample(samAnisotropic, pin.DiffTex);

    // Add the nine horizontal pixels to the color by the specific weight of each.
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex1) * weight4;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex2) * weight3;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex3) * weight2;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex4) * weight1;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex5) * weight0;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex6) * weight1;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex7) * weight2;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex8) * weight3;
    texColor += gDiffTexture.Sample(samAnisotropic, pin.Tex9) * weight4;

    //clip(texColor.a - 0.1f);
    
    texColor.a = 1.0f;

    return texColor;
}

technique11 HorizontalBlur
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, HorizontalVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

technique11 VerticalBlur
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VerticalVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
