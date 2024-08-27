struct VIn {
    float2 position : aPos;
    float color : aCol;
    float2 texcoord : aTex;
};

struct VOut {
    float4 position : SV_Position;
    float4 color : aCol;
    float2 texcoord : aTex;
};


cbuffer Matrices : register(b0) {
    float4x4 g_mOrthoProj;
};

static const float to1 = 1.0f / 255.0f;

VOut main(VIn input)  {
    VOut output;
    
    float2 pos = input.position;
    float4 res = mul(float4(pos, 0.0f, 1.0f), g_mOrthoProj);
    output.position = res;
    uint color = asuint(input.color);
    uint r = color >> 24;
    uint g = (color >> 16) & 0xff;
    uint b = (color >> 8) & 0xff;
    uint a = color & 0xff;
    output.color = float4(float(r) * to1, float(g) * to1, float(b) * to1, float(a) * to1);
    output.texcoord = input.texcoord;
    return output;
}