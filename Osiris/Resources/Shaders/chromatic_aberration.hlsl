sampler texSampler;

float amount;

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float r = tex2D(texSampler, uv - amount).r;
    float g = tex2D(texSampler, uv).g;
    float b = tex2D(texSampler, uv + amount).b;

    return float4(r, g, b, 1.0f);
}
