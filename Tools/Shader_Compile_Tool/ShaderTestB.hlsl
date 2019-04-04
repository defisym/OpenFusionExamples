
Texture2D<float4> Texture0 : register(t0);
sampler TextureSampler0 : register(s0);

struct PS_INPUT
{
  float4 Tint : COLOR0;
  float2 texCoord : TEXCOORD0;
  //SV_POSITION is used in vortex shader
  float4 Position : SV_POSITION;
};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};

cbuffer PS_VARIABLES : register(b0)
{
	float     fBlend;
};

PS_OUTPUT ps_main(PS_INPUT In)
{
    PS_OUTPUT Out;
	Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * fBlend * In.Tint;
    return Out;
}
