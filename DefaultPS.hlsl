Texture2D g_Tex : register(t0);
sampler samp : register(s0);

struct VOut {
    float4 position : SV_Position;
    float4 color : aCol;
    float2 texcoord : aTex;
};

float4 main(VOut input) : SV_Target {
    return g_Tex.Sample(samp, input.texcoord) *  input.color;
}