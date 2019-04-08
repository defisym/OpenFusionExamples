// Pixel shader Input structure
struct PS_INPUT
{
  //Tint：对象属性中设定的RGBA
  float4 Tint : COLOR0;
  //输入的UV坐标
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
	float     fBlend;
};

cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In)
{
	// Output pixel
    PS_OUTPUT Out;
	
	//读取纹理
	Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
	
	//常规shader处理
	Out.Color = Out.Color * fBlend;
	
    return Out;
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In )
{
	// Output pixel
    PS_OUTPUT Out;
	
	//读取纹理
	Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
	
	//除以不透明度
	Out.Color.r=Out.Color.r/Out.Color.a;
	Out.Color.g=Out.Color.g/Out.Color.a;
	Out.Color.b=Out.Color.b/Out.Color.a;
	
	//常规shader处理
	Out.Color = Out.Color * fBlend;
	
	//乘以不透明度
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;
	
    return Out;
}