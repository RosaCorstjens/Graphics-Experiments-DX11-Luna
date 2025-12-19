#include "LightHelper.fx"

cbuffer cbPerFrame
{
    float3 gEyePosW;
    float gScreenWidth;
    float gScreenHeigth;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4x4 gWorldViewProj;
}; 

Texture2D gDepthTexture;
Texture2D gNormalTexture;
Texture2D gDiffuseTexture;

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samClamp
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = CLAMP;
    AddressV = CLAMP;

};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 UV : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex1 : TEXCOORD1;
    float2 Tex2 : TEXCOORD2;
    float2 Tex3 : TEXCOORD3;
    float2 Tex4 : TEXCOORD4;
    float2 Tex5 : TEXCOORD5;
    float2 Tex6 : TEXCOORD6;
    float2 Tex7 : TEXCOORD7;
    float2 Tex8 : TEXCOORD8;
    float2 Tex9 : TEXCOORD9;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

	// Transform to world space space.
    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
    // determine float size of a texel for the specified width.
    float2 texelSize = float2(1.0f / gScreenWidth, 1.0f / gScreenHeigth);
   
    // Create UV coordinates for the pixel and its four horizontal neighbors on either side.
    vout.Tex1 = vin.UV + float2(texelSize.x * 0.0f, texelSize.y * 0.0f);            // center
    vout.Tex2 = vin.UV + float2(texelSize.x * -1.0f, texelSize.y * -1.0f);          // left top
    vout.Tex3 = vin.UV + float2(texelSize.x * 1.0f, texelSize.y * 1.0f);            // right bottom
    vout.Tex4 = vin.UV + float2(texelSize.x * 1.0f, texelSize.y * -1.0f);           // right top
    vout.Tex5 = vin.UV + float2(texelSize.x * -1.0f, texelSize.y * 1.0f);           // left bottom
    vout.Tex6 = vin.UV + float2(texelSize.x * -1.0f, texelSize.y * 0.0f);           // left mid
    vout.Tex7 = vin.UV + float2(texelSize.x * 1.0f, texelSize.y * 0.0f);            // right mid
    vout.Tex8 = vin.UV + float2(texelSize.x * 0.0f, texelSize.y * -1.0f);           // mid top
    vout.Tex9 = vin.UV + float2(texelSize.x * 0.0f, texelSize.y * 1.0f);            // mid bottom

    return vout;
}

float4 determineEdge(VertexOut pin, Texture2D tex, float threshold)
{
    float4 midleft = tex.Sample(samClamp, pin.Tex6);
    float4 midright = tex.Sample(samClamp, pin.Tex7);
    float4 dx = (midleft - midright) / 2;
    
    float4 topmid = tex.Sample(samClamp, pin.Tex8);
    float4 botttommid = tex.Sample(samClamp, pin.Tex9);
    float4 dy = (topmid - botttommid) / 2;

    float4 magnitude = ((dx * dx) + (dy * dy)) - threshold;

    float returnvalue = dot(magnitude, float4(1, 1, 1, 1));

    return step(returnvalue, 0);
}

float4 PS(VertexOut pin) : SV_TARGET
{
    // determine edge based on normal
    float4 normalColor = determineEdge(pin, gNormalTexture, 0.01);

    // determine edge based on depth
    float4 depthColor = determineEdge(pin, gDepthTexture, 0.00001);
   
    // determine edge based on diffuse color
    float4 diffColor = determineEdge(pin, gDiffuseTexture, 0.002);

    return clamp((normalColor * depthColor * diffColor), 0.1f, 1);
}

technique11 EdgeEnhancementStalker
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}