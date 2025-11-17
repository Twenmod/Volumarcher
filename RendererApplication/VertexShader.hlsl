
struct MatrixBuffer
{
    float4x4 MVP;
};

cbuffer RootConstants : register(b0)
{
    MatrixBuffer mat;
};


struct VSOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
};

VSOut main(float3 pos : POSITION, float3 normal : NORMAL)
{
    VSOut res;
    res.pos = mul(mat.MVP,float4(pos, 1));
    res.normal = normal;
    return res;
}