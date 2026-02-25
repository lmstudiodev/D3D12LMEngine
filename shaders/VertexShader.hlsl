#include "RootSignature.hlsl"

struct VertexIn
{
    float3 pos : Position;
    float3 color : Color;
    float2 textCoord : TexCoord;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 textCoord : TEXCOORD;
};

[RootSignature(ROOTSIG)]
VertexOut main(VertexIn input)
{
    VertexOut vo;
    
    vo.pos = float4(input.pos.xyz, 1.0f);
    vo.color = input.color;
    vo.textCoord = input.textCoord;
    
    return vo;
}