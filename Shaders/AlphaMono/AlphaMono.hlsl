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
Texture2D<float4> Tex0 : register(t0);
sampler Tex0Sampler : register(s0);

Texture2D<float4> bkd : register(t1);
sampler bkdSampler : register(s1);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	int fA;
  bool BG;
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
  PS_OUTPUT Mono;
	
	//读取纹理
  	if(!BG){
        Out.Color = Tex0.Sample(Tex0Sampler, In.texCoord) * In.Tint;
    }else{
        Out.Color = bkd.Sample(bkdSampler, In.texCoord);
        Out.Color.a = (Tex0.Sample(Tex0Sampler, In.texCoord) * In.Tint).a;
    }	
	
	//常规shader处理
  float4 f4 = Out.Color * float4(0.299f, 0.587f, 0.114f, 1.0f);
	float f = f4.r + f4.g + f4.b;
  Mono.Color.rgb=f;
  
  float Alpha=1-fA/255.0;
  Out.Color.rgb=Mono.Color.rgb*Alpha+Out.Color.rgb*(1-Alpha);

	return Out;
}