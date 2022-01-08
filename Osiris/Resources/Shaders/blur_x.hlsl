sampler texSampler;

float texelWidth;

static const float offsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
static const float weights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float4 color = tex2D(texSampler, uv);
    color.rgb *= weights[0];

    for (int i = 1; i < 3; ++i) {
        color.rgb += tex2D(texSampler, float2(uv.x - texelWidth * offsets[i], uv.y)).rgb * weights[i];
        color.rgb += tex2D(texSampler, float2(uv.x + texelWidth * offsets[i], uv.y)).rgb * weights[i];
    }

    return color;
}
