#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;
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
	float2 DiffTex     : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 DiffTex     : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
    vout.DiffTex = vin.DiffTex;

	return vout;
}

float4 PS(VertexOut pin, uniform int gLightCount, uniform bool gUseTexure, uniform bool gApplyToon) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye); 

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);

    if(gUseTexure)
	{
		// Sample texture with anisotropic wrap mode.
        texColor = gDiffTexture.Sample(samAnisotropic, pin.DiffTex);
        clip(texColor.a - 0.1f);
    }
	
	// Lighting
	float4 litColor = texColor;
	if(gLightCount > 0)
	{  
        if (gApplyToon)
        {
            // init output colors for lighting
            float4  ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
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
                float diffuseLevels = 3.0f;
                float diffuseScaleFactor = 1.0f / diffuseLevels;
                float diffuseFactor = max(0, dot(lightVec, pin.NormalW));

                if (diffuseFactor > 0.0f)
                {
                    diffuse += (floor(diffuseFactor * diffuseLevels) * diffuseScaleFactor) * gMaterial.Diffuse * gDirLights[i].Diffuse;

                    float3 viewVec = reflect(-lightVec, pin.NormalW);

                    spec += pow(max(dot(viewVec, toEye), 0.0f), gMaterial.Specular.w);
                 
                    //limit specular
                    specMask += (pow(abs(dot(viewVec, toEye)), gMaterial.Specular.w) > 0.8f) ? 1 : 0;
                }
            }

            litColor = texColor * (diffuse + ambient) + (spec * specMask);
        }
        else
        {
		    // Start with a sum of zero. 
            float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
            float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
            float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		    // Sum the light contribution from each light source.  
		    [unroll]
            for (int i = 0; i < gLightCount; ++i)
            {
                float4 A, D, S;
                ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
				A, D, S);

                ambient += A;
                diffuse += D;
                spec += S;
            }

		    // Modulate with late add.
            litColor = texColor * (ambient + diffuse) + spec;
        }
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false)));
    }
}

technique11 Light0Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false)));
    }
}

technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false)));
    }
}

technique11 ToonLightTex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true)));
    }
}
