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

Texture2D<float4> patternpic : register(t1);
sampler patternpicSampler0 : register(s1);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	float     progress;
	int mode;
	float4 texcolor;
};

cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In)
{
	// Output pixel
    PS_OUTPUT Out,Pic;	
	
	//常规shader处理
	if(mode==1)
	{
		Pic.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
		Out.Color = patternpic.Sample(patternpicSampler0,In.texCoord);		
	}
	else
	{
		Pic.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
		Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
	}
	
	float a;
	float step=18;
	float RgbStep=0.01;
	float AlphaStep=1/step;	
	
	float progressl=progress-step*RgbStep;
	
	a=0;
	//texcolor.a=1;

	if(Pic.Color.a!=0)	
	{
		for(int i=step;i>0;i--)
		{
			if(Out.Color.r<=(progressl+i*RgbStep))
			{
				a=a+AlphaStep;
			}				
		}
	}

	Out.Color.rgb=texcolor.rgb;		
	Out.Color.a=a;
	Pic.Color.a=a;
	
	if(mode==1)
	{
		return Pic;
	}
	else
	{
		return Out;
	}
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In )
{
	// Output pixel
    PS_OUTPUT Out,Pic;
	
	//读取纹理
	Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
		
	if(mode==1)
	{
		Pic.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
		Out.Color = patternpic.Sample(patternpicSampler0,In.texCoord);
		//除以不透明度
		Out.Color.r=Out.Color.r/Out.Color.a;
		Out.Color.g=Out.Color.g/Out.Color.a;
		Out.Color.b=Out.Color.b/Out.Color.a;
		//除以不透明度
		Pic.Color.r=Pic.Color.r/Pic.Color.a;
		Pic.Color.g=Pic.Color.g/Pic.Color.a;
		Pic.Color.b=Pic.Color.b/Pic.Color.a;
		
	}
	else
	{
		Pic.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
		Out.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;
		//除以不透明度
		Out.Color.r=Out.Color.r/Out.Color.a;
		Out.Color.g=Out.Color.g/Out.Color.a;
		Out.Color.b=Out.Color.b/Out.Color.a;
		//除以不透明度
		Pic.Color.r=Pic.Color.r/Pic.Color.a;
		Pic.Color.g=Pic.Color.g/Pic.Color.a;
		Pic.Color.b=Pic.Color.b/Pic.Color.a;
	}
	
	float a;
	float step=18;
	float RgbStep=0.01;
	float AlphaStep=1/step;	
	
	float progressl=progress-step*RgbStep;
	
	a=0;
	//texcolor.a=1;

	if(Pic.Color.a!=0)	
	{
		for(int i=step;i>0;i--)
		{
			if(Out.Color.r<=(progressl+i*RgbStep))
			{
				a=a+AlphaStep;
			}				
		}
	}

	Out.Color.rgb=texcolor.rgb;		
	Out.Color.a=a;
	Pic.Color.a=a;
	
	//乘以不透明度
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;
	//乘以不透明度
	Pic.Color.r=Pic.Color.r*Pic.Color.a;
	Pic.Color.g=Pic.Color.g*Pic.Color.a;
	Pic.Color.b=Pic.Color.b*Pic.Color.a;
	
	if(mode==1)
	{
		return Pic;
	}
	else
	{
		return Out;
	}
}