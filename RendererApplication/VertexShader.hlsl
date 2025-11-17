
struct MatrixBuffer
{
    float4x4 MVP;
};

cbuffer RootConstants : register(b0)
{
    MatrixBuffer mat;
};


float4 main( float3 pos : POSITION ) : SV_POSITION
{
    return mul(mat.MVP,float4(pos, 1));
}