#include "RootSignature.hlsl"

struct ModelViewProjection
{
    matrix MVP;
};

struct Correction
{
    float aspectRatio;
    float zoom;
    float sinAngle;
    float cosAngle;
};

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

Correction correction : register(b1);
ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b2);

[RootSignature(ROOTSIG)]
VertexOut main(VertexIn input)
{
    VertexOut vo;
    
    //Z rotation
    //float3 v;
    //v.x = (input.pos.x * correction.cosAngle) - (input.pos.y * correction.sinAngle); // Model
    //v.y = (input.pos.x * correction.sinAngle) + (input.pos.y * correction.cosAngle);
    //v.z = input.pos.z;
    //v *= correction.zoom; // View
    //v.x *= correction.aspectRatio; // Projection
    
    //OUT.Position = mul(ModelViewProjectionCB.MVP, float4(input.pos, 1.0f));
    
    vo.pos = mul(ModelViewProjectionCB.MVP, float4(input.pos, 1.0f)); //float4(v, 1.0f);
    vo.color = input.color;
    vo.textCoord = input.textCoord;
    
    return vo;
}