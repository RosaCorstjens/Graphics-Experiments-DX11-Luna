#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;
    float gDiffuseLevels;
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
    float4 DepthPos : POSITION1;
};

struct PixelOut
{
    float4 DiffuseColor : SV_Target0;
    float4 NormalColor : SV_Target1;
    float4 DepthColor : SV_Target2;
    float4 ToonColor : SV_Target3;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.DepthPos = vout.PosH;

    vout.DiffTex = vin.DiffTex;

	return vout;
}
 
PixelOut PS(VertexOut pin, uniform int gLightCount)
{
    PixelOut pout;

    pin.NormalW = normalize(pin.NormalW);

    // diffuse color
    pout.DiffuseColor = gDiffTexture.Sample(samAnisotropic, pin.DiffTex);
    clip(pout.DiffuseColor.a - 0.1f);

    // normal color
    pout.NormalColor.rgb = 0.5f * pin.NormalW + 1.0f;
    pout.NormalColor.a = 1.0f;

    // depth color
    float depthValue = pin.DepthPos.z / pin.DepthPos.w;
    pout.DepthColor = float4(depthValue, depthValue, depthValue, 1.0f);

    // toon color, including lighting
    float3 toEye = gEyePosW - pin.PosW;
    float distToEye = length(toEye);
    toEye /= distToEye;

    // init output colors for lighting
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float spec = 0;
    float specMask = 0;

    [unroll]
    for (int i = 0; i < gLightCount; ++i)
    {
        float3 lightVec = -gDirLights[i].Direction;

        // add ambient term
        ambient += gMaterial.Ambient * gDirLights[i].Ambient;

        // add the diffuse and specular term if the surface is in line of site of light
        float diffuseScaleFactor = 1.0f / gDiffuseLevels;
        float diffuseFactor = max(0, dot(lightVec, pin.NormalW));

        if (diffuseFactor > 0.0f)
        {
            diffuse += (floor(diffuseFactor * gDiffuseLevels) * diffuseScaleFactor) * gMaterial.Diffuse * gDirLights[i].Diffuse;

            float3 viewVec = reflect(-lightVec, pin.NormalW);

            spec += pow(max(dot(viewVec, toEye), 0.0f), gMaterial.Specular.w);
                 
            //limit specular
            specMask += (pow(abs(dot(viewVec, toEye)), gMaterial.Specular.w) > 0.9f) ? 1 : 0;
        }
    }

    pout.ToonColor = pout.DiffuseColor * (diffuse + ambient) + (spec * specMask);

    return pout;
}

technique11 MultipleRenderTargets
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1) ) );
    }
}
