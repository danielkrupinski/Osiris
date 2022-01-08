sampler texSampler;

float amount;

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float4 color = tex2D(texSampler, uv);
    color.rgb = lerp(color.rgb, dot(color.rgb, float3(0.2126f, 0.7152f, 0.0722f)), amount);
    return color;
}
