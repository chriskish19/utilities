struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUT main(float3 pos : POSITION, float4 color : COLOR)
{
    VS_OUT output;
    output.Pos = float4(pos, 1.0f);
    output.Color = color;
    return output;
}



