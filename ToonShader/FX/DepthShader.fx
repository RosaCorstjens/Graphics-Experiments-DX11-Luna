#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
}; 

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
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float4 DepthPos : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
    // Store position in second variable for depth calculations (needed due to an offset)
    vout.DepthPos = vout.PosH;

	return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{ 
    // Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate
    float depthValue = pin.DepthPos.z / pin.DepthPos.w;

    // declare color to return
    float4 color = float4(depthValue, depthValue, depthValue, 1.0f);

    return color;
}

technique11 Depth
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
