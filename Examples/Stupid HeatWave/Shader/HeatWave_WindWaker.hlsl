// Pixel shader Input structure
struct PS_INPUT
{
  float4 Tint : COLOR0;
  float2 Texture    : TEXCOORD0;
  //float2 texCoord : TEXCOORD0;
  
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

Texture2D<float4> BackGround0 : register(t1);
sampler BackGroundSampler0 : register(s1);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	int    offset;
	int	side;
};

cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In)
{
	// Output pixel
    PS_OUTPUT Out,Src;
	
	//读取纹理
	Src.Color = Texture0.Sample(TextureSampler0, In.Texture) * In.Tint;
	
	//常规shader处理
	float a=Src.Color.a;
	
	if(((In.Texture.x<=offset*fPixelWidth)||((In.Texture.x+offset*fPixelWidth)>=1))||((In.Texture.y<=offset*fPixelHeight)||((In.Texture.y+offset*fPixelHeight)>=1)))
		{
			a=0;
		}
	
	int sidem=(side==0)?-1:1;
	
	In.Texture.x=In.Texture.x-sidem*offset*fPixelWidth;
	In.Texture.y=In.Texture.y-sidem*offset*fPixelHeight;
	
	Out.Color = BackGround0.Sample(BackGroundSampler0, In.Texture) * In.Tint;

	Out.Color.a=a;
	
    return Out;
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In )
{
	// Output pixel
    PS_OUTPUT Out,Src;
	
	//读取纹理
	Src.Color = Texture0.Sample(TextureSampler0, In.Texture) * In.Tint;	
	
	//除以不透明度
	Src.Color.r=Src.Color.r/Src.Color.a;
	Src.Color.g=Src.Color.g/Src.Color.a;
	Src.Color.b=Src.Color.b/Src.Color.a;
	
	//常规shader处理
	float a=Src.Color.a;
	
	if(((In.Texture.x<=offset*fPixelWidth)||((In.Texture.x+offset*fPixelWidth)>=1))||((In.Texture.y<=offset*fPixelHeight)||((In.Texture.y+offset*fPixelHeight)>=1)))
		{
			a=0;
		}
	
	int sidem=(side==0)?-1:1;
	
	In.Texture.x=In.Texture.x-sidem*offset*fPixelWidth;
	In.Texture.y=In.Texture.y-sidem*offset*fPixelHeight;
	
	Out.Color = BackGround0.Sample(BackGroundSampler0, In.Texture) * In.Tint;
	
	Out.Color.r=Out.Color.r/Out.Color.a;
	Out.Color.g=Out.Color.g/Out.Color.a;
	Out.Color.b=Out.Color.b/Out.Color.a;

	Out.Color.a=a;
	
	//乘以不透明度
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;
	
    return Out;
}