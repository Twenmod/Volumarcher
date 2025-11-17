struct Input
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
};

static const float3 SUN_DIR = normalize(float3(0.4, -1, 0.4));
static const float3 SUN_LIGHT = float3(0.996, 0.9, 0.8) * 20;

static const float3 BACKGROUND_COLOR_UP = float3(0.467, 0.529, 0.671);
static const float3 BACKGROUND_COLOR_DOWN = float3(0.694, 0.596, 0.467) * 0.5;

float4 main(Input input) : SV_TARGET
{
    float NdotL = saturate(dot(input.normal, -SUN_DIR)); 

    float3 diffuse = SUN_LIGHT * NdotL;

    float h = saturate((input.normal.y * 0.5) + 0.55);
    float3 ambient = lerp(BACKGROUND_COLOR_DOWN, BACKGROUND_COLOR_UP, h);

    float3 color = diffuse + ambient;

    return float4(color, 1.0);
}