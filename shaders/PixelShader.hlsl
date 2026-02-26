#include "RootSignature.hlsl"

float3 color : register(b0);
Texture2D<float4> textures[] : register(t0);
sampler textureSampler : register(s0);

struct VertexOut
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 textCoord : TEXCOORD;
};

[RootSignature(ROOTSIG)]
float4 main(VertexOut Input) : SV_TARGET
{
    float4 texel = textures[0].Sample(textureSampler, Input.textCoord);
    
    return float4(texel.rgb, 1.0f);
}