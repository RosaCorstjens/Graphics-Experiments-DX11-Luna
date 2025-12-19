cbuffer cbPerFrame
{
    float2 gLightPosition;

    float2 gScreenSize;
    float gAspectRatio;
    float gIntensity;

    float gBlend;
};

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
}; 

// Nonnumeric values cannot be added to a cbuffer.
// texture input for first pass
Texture2D gDiffTexture;
Texture2D gDepthTexture;

// tex input for second and third pass
Texture2D gShaftTexture;

SamplerState samp
{
    Filter = MIN_MAG_MIP_POINT;

	AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VertexIn
{
	float3 Pos     : POSITION0;
	float2 Tex     : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION0;
    float4 Tex     : TEXCOORD0;
};

VertexOut MaskVS(VertexIn vin)
{
    // declare v out to return
    VertexOut vout;

    // transform to CS
    vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

    vout.PosW = vin.Pos;

    // pass through the texture coord
    vout.Tex.xy = vin.Tex;                 // maybe: vin.Tex + halfpixelsize
    vout.Tex.zw = float2(0,0);

    return vout;
}
 
float4 MaskPS(VertexOut pin) : SV_Target
{
    // use depth as map: values near 1 (background) have more weight in shaft texture
    float depth = gDepthTexture.Sample(samp, pin.Tex.xy).r;

    // apply linear filtering on color texture
    float2 texelSize = float2(1.0f / gScreenSize.x, 1.0f / gScreenSize.y);

    float3 color = gDiffTexture.Sample(samp, pin.Tex.xy).rgb;

    return float4(color, depth);
}

#define DENSITY 0.3f//50 
#define MAX_WEIGTH 1.2f

#define SAMPLE_COUNT 100                        // the more the merrier
#define INV_SAMPLE_COUNT 0.01f

VertexOut BlurVS(VertexIn vin)
{
    // declare v out to return
    VertexOut vout;

    // transform to CS
    vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

    vout.PosW = vin.Pos;

    // pass through the texture coord
    vout.Tex.xy = vin.Tex; // maybe: vin.Tex + halfpixelsize
    vout.Tex.zw = vin.Tex * 2 - 1;

    return vout;
}

float4 BlurPS(VertexOut pin) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);

    // radial blur vector: from pixel to light
    float2 blurVector = (gLightPosition.xy - pin.Tex.zw); // [-1, 1]
    blurVector.y *= gAspectRatio;

    // calculate actual length for blur vector based on density and the total amount of samples
    blurVector *= (DENSITY * INV_SAMPLE_COUNT);

    // declare this coord as current coord and weight starting at 1
    float2 currTexCoord = pin.Tex.xy;
    float weight = MAX_WEIGTH;
    float4 currColor = float4(1, 1, 1, 1);

    // calculate texel size
    float2 texelSize = float2(1.0f / gScreenSize.x, 1.0f / gScreenSize.y);

    [unroll(SAMPLE_COUNT)]
    for (int i = 1; i < SAMPLE_COUNT+1; ++i)
    {
        // note: should stay higher than 0
        weight = MAX_WEIGTH - (MAX_WEIGTH / i);

        // sample the current color
        currColor = gShaftTexture.Sample(samp, currTexCoord);

        currColor *= weight;

        color += currColor;

        currTexCoord += blurVector;
    }

    color.rgba *= (INV_SAMPLE_COUNT * gIntensity);

    return float4(color.rgb, color.a); //gShaftTexture.Sample(samp, pin.Tex.xy).a);
}

VertexOut FinalMixVS(VertexIn vin)
{
    // declare v out to return
    VertexOut vout;

    // transform to CS
    vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

    // pass throught the world position
    vout.PosW = vin.Pos;

    // pass through the texture coord
    vout.Tex.xy = vin.Tex; 
    vout.Tex.zw = float2(0.0f, 0.0f);

    return vout;
}
 
#define SHAFT_TINT float3(1.0f, 0.9f, 0.7f);
#define BLEND 0.8f

float4 FinalMixPS(VertexOut pin) : SV_Target
{
    float4 shaft = gShaftTexture.Sample(samp, pin.Tex.xy).rgba;
    shaft.rgb *= SHAFT_TINT;
	float fadeShaft = shaft.a * BLEND;

    float3 originColor = gDiffTexture.Sample(samp, pin.Tex.xy).rgb;


    float4 returnColor = float4(originColor + (shaft.rgb * fadeShaft), 1);

    return returnColor;
    return float4(shaft.a, shaft.a, shaft.a, 1.0f);
}

technique11 Mask
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, MaskVS()));
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, MaskPS() ) );
    }
}

technique11 Blur
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, BlurVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, BlurPS()));
    }
}

technique11 FinalMix
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, FinalMixVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, FinalMixPS()));
    }
}