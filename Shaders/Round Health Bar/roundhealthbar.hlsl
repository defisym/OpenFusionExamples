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

Texture2D<float4> frontpic : register(t1);
sampler frontpicSampler0 : register(s1);

Texture2D<float4> backpic : register(t2);
sampler backpicSampler0 : register(s2);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	float fR;
	float	health;
	float maxhealth;	
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
	PS_OUTPUT front;
	PS_OUTPUT back;
	
	//读取纹理
	Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
	front.Color = frontpic.Sample(frontpicSampler0, In.texCoord) * In.Tint;
	back.Color = backpic.Sample(backpicSampler0, In.texCoord) * In.Tint;
		
	//常规shader处理
	float xr=fR*fPixelWidth;
	float yr=fR*fPixelHeight;
	
	float percent;
	percent=(health/maxhealth);

	
	float a=0;
	
	if(pow((In.texCoord.y-0.5)/yr,2)+pow((In.texCoord.x-0.5)/xr,2)<=1)
	{
		a=1;
		Out.Color=front.Color;
		if(In.texCoord.y < (1-percent))
		{
			//a=0;
			Out.Color=back.Color;
		}
	}
	
	Out.Color.a=a;
	
    return Out;
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In )
{
	// Output pixel
    PS_OUTPUT Out;
	PS_OUTPUT front;
	PS_OUTPUT back;
	
	//读取纹理
	Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
	front.Color = frontpic.Sample(frontpicSampler0, In.texCoord) * In.Tint;
	back.Color = backpic.Sample(backpicSampler0, In.texCoord) * In.Tint;
	
	//除以不透明度
	Out.Color.r=Out.Color.r/Out.Color.a;
	Out.Color.g=Out.Color.g/Out.Color.a;
	Out.Color.b=Out.Color.b/Out.Color.a;
	
	front.Color.r=front.Color.r/front.Color.a;
	front.Color.g=front.Color.g/front.Color.a;
	front.Color.b=front.Color.b/front.Color.a;
	
	back.Color.r=back.Color.r/back.Color.a;
	back.Color.g=back.Color.g/back.Color.a;
	back.Color.b=back.Color.b/back.Color.a;
	
	//常规shader处理
	float xr=fR*fPixelWidth;
	float yr=fR*fPixelHeight;
	
	float percent;
	percent=(health/maxhealth);

	
	float a=0;
	
	if(pow((In.texCoord.y-0.5)/yr,2)+pow((In.texCoord.x-0.5)/xr,2)<=1)
	{
		a=1;
		Out.Color=front.Color;
		if(In.texCoord.y < (1-percent))
		{
			//a=0;
			Out.Color=back.Color;
		}
	}
	
	Out.Color.a=a;
	
	//乘以不透明度
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;	
		
    return Out;
}