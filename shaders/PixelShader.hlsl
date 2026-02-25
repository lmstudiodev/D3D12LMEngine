#include "RootSignature.hlsl"

struct VertexOut
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 textCoord : TEXCOORD;
};

[RootSignature(ROOTSIG)]
float4 main(VertexOut Input) : SV_TARGET
{
    return float4(Input.color, 1.0f);
}