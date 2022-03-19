float4x4 projectionMatrix;

struct VS_INPUT
{
    float2 pos : POSITION0;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(projectionMatrix, float4(input.pos, 0.0f, 1.0f));
    output.col = input.col;
    output.uv = input.uv;
    return output;
}
