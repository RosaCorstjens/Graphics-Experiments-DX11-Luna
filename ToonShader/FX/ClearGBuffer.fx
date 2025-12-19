struct VertexIn
{
	float3 Pos    : POSITION;
};

struct VertexOut
{
    float4 Pos : POSITION;
};

struct PixelOut
{
    float4 Color : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Depth : SV_Target2;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
    vout.Pos = float4(vin.Pos, 1);
	return vout;
}
 
// Optionally color.a and normal.a can be used for other variables
PixelOut PS(VertexOut pin)
{
    PixelOut pout;

    // diff color, black
    pout.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // normal color, grey
    pout.Normal = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // depth color, white
    pout.Depth = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return pout;
}

technique11 ClearGBuffer
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
