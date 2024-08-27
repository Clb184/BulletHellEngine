Texture2D g_Tex : register(t0);
sampler samp : register(s0);


struct VS_OUT {
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normals : NORMAL;
    float fograte : FOG;
};

struct light {
    float3 pos;
    float4 color;
};

cbuffer Lights : register(b0) {
    float4 GlobalLightRot;
    float4 Ambient;
    float4 FogColor;
    float4 dum1;
}
static const float3 rot = float3(0.2f, 0.1f, 0.8f);
static const float4 ambient = float4(0.11f, 0.0f, 0.1f, 0.0f);
static const float4 fog = float4(0.5f, 0.5f, 0.5f, 1.0f);

float4 main(VS_OUT input) : SV_TARGET {
    float4 finalcolor = g_Tex.Sample(samp, input.texcoord);
    float3 norm = normalize(input.normals);
    float4 factor = max(dot(norm, -normalize(GlobalLightRot.xyz)), 0.0f);
    factor.a = 1.0f;
    finalcolor *= factor;
    finalcolor += Ambient;
    finalcolor.rgb = lerp(finalcolor.rgb, FogColor.rgb, input.fograte);
    return saturate(finalcolor);
}