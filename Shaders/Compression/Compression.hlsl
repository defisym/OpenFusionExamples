// Pixel shader Input structure
struct PS_INPUT
{
  float4 Tint : COLOR0;
  //float2 Texture    : TEXCOORD0;
  float2 texCoord : TEXCOORD0;
  
  //SV_POSITION is used in vortex shader
  float4 Position : SV_POSITION;
};

// Pixel shader output structure
struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};

//Sampler
Texture2D<float4> Texture0 : register(t0);
sampler TextureSampler0 : register(s0);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	int levelNum;
	int samplerSize;
};

cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

float normalize(float f, float div) {
    return f - fmod(f, div);
}

float3 normalize_3(float3 f3, float div){
    f3.r = normalize(f3.r, div);
    f3.g = normalize(f3.g, div);
    f3.b = normalize(f3.b, div);

    return f3;
}

float2 updateSize(float2 f2) {
    float divX = 1.0 / ((1.0 / fPixelWidth) / samplerSize);
    float divY = 1.0 / ((1.0 / fPixelHeight) / samplerSize);

    f2.x = normalize(f2.x, divX);
    f2.y = normalize(f2.y, divY);

    return f2;
}

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In)
{
    PS_OUTPUT Out;
	
	Out.Color = Texture0.Sample(TextureSampler0, updateSize(In.texCoord)) * In.Tint;

	float div = 1.0 / levelNum;

    Out.Color.rgb = normalize_3(Out.Color.rgb, div);
	
    return Out;
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In )
{
    PS_OUTPUT Out;
	Out.Color = Texture0.Sample(TextureSampler0, updateSize(In.texCoord)) * In.Tint;
	
	//除以不透明度
	Out.Color.r=Out.Color.r/Out.Color.a;
	Out.Color.g=Out.Color.g/Out.Color.a;
	Out.Color.b=Out.Color.b/Out.Color.a;
	
	//常规shader处理
	float div = 1.0 / levelNum;

    Out.Color.rgb = normalize_3(Out.Color.rgb, div);
	
	//乘以不透明度
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;
	
    return Out;
}