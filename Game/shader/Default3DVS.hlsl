struct VS_IN {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normals : NORMAL;
};

struct VS_OUT {
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normals : NORMAL;
    float fograte : FOG;
};

cbuffer CameraMatrix : register(b0) {
    float4x4 camMat;
    float4x4 View;
    float4x4 Projection;
    float2 Fog;
    float2 ExpFog;
    float Density;
}

cbuffer Normals : register(b1) {
    float4x4 model;
    float3x3 normMat;
}

cbuffer InstancedDraw : register(b2) {
    float3 offsetFromPos;
    float3 offsetFromInstance;
    int3 max_per_side;
}

static const float density = 0.01f;
static const float grad = 2.3;

VS_OUT main(VS_IN input, uint id : SV_InstanceID) {
    VS_OUT ret;
    float3 curpos = input.position;
    //curpos += float3((id % 40) * 30.0, ((id / 40) % 40) * 10.0, ((id / 1600) % 40) * 20.0);
    float4 CameraPos = mul(mul(float4(curpos, 1.0f), model), View);
    
    ret.position = mul(mul(float4(curpos, 1.0f), model), camMat);
    ret.texcoord = input.texcoord;
    ret.color = input.color;
    ret.normals = normalize(mul(input.normals, (float3x3) model));
    ret.texcoord.y = 1.0f - ret.texcoord.y;
    ret.fograte = saturate((length(CameraPos) - Fog.x) / (Fog.y - Fog.x));
    //ret.fograte = pow(1.0 / abs(grad), (density * CameraPos.z));
    
    return ret;
}