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

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	float     fR;
	int		Mono;
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
	Out.Color = Texture0.Sample(TextureSampler0, In.Texture) * In.Tint;
	
	//常规shader处理
	
	if(Mono){
		float4 f4 = Out.Color * float4(0.299f, 0.587f, 0.114f, 1.0f);
		float f = f4.r + f4.g + f4.b;
		Out.Color.rgb = f;
	}
	
	float width=1.0/fPixelWidth;
	float height=1.0/fPixelHeight;
	
	float R=min(fR,min(width/2,height/2));
		
	float xr=R*fPixelWidth;
	float yr=R*fPixelHeight;
	
	float xr2=1-xr;
	float yr2=1-yr;
	
	float a=0;
	
	if((In.Texture.x>=xr)&&(In.Texture.x<=xr2)||(In.Texture.y>=yr)&&(In.Texture.y<=yr2))
	{
		a=1;	
	}

	//--------------
	else if(pow((In.Texture.y-yr)/yr,2)+pow((In.Texture.x-xr)/xr,2)<=1)
		{
			a =1;
		}		
	
 	else if(pow((In.Texture.y-yr2)/yr,2)+pow((In.Texture.x-xr)/xr,2)<=1)
		{
			a =1;
		}		
	//--------------
	else if(pow((In.Texture.y-yr)/yr,2)+pow((In.Texture.x-xr2)/xr,2)<=1)
		{
			a =1;
		}
	else if(pow((In.Texture.y-yr2)/yr,2)+pow((In.Texture.x-xr2)/xr,2)<=1)
		{
			a =1;			
		}
	

	Out.Color.a*=a;
	
    return Out;
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In )
{
	// Output pixel
    PS_OUTPUT Out;
	
	//读取纹理
	Out.Color = Texture0.Sample(TextureSampler0, In.Texture) * In.Tint;
	
	//除以不透明度
	Out.Color.r=Out.Color.r/Out.Color.a;
	Out.Color.g=Out.Color.g/Out.Color.a;
	Out.Color.b=Out.Color.b/Out.Color.a;
	
	//常规shader处理
	
	if(Mono){
		float4 f4 = Out.Color * float4(0.299f, 0.587f, 0.114f, 1.0f);
		float f = f4.r + f4.g + f4.b;
		Out.Color.rgb = f;
	}
	
	float width=1.0/fPixelWidth;
	float height=1.0/fPixelHeight;
	
	float R=min(fR,min(width/2,height/2));
		
	float xr=R*fPixelWidth;
	float yr=R*fPixelHeight;
	
	float xr2=1-xr;
	float yr2=1-yr;
	
	float a=0;
	
	if((In.Texture.x>=xr)&&(In.Texture.x<=xr2)||(In.Texture.y>=yr)&&(In.Texture.y<=yr2))
	{
		a=1;	
	}

	//--------------
	else if(pow((In.Texture.y-yr)/yr,2)+pow((In.Texture.x-xr)/xr,2)<=1)
		{
			a =1;
		}		
	
 	else if(pow((In.Texture.y-yr2)/yr,2)+pow((In.Texture.x-xr)/xr,2)<=1)
		{
			a =1;
		}		
	//--------------
	else if(pow((In.Texture.y-yr)/yr,2)+pow((In.Texture.x-xr2)/xr,2)<=1)
		{
			a =1;
		}
	else if(pow((In.Texture.y-yr2)/yr,2)+pow((In.Texture.x-xr2)/xr,2)<=1)
		{
			a =1;			
		}
	

	Out.Color.a*=a;
	
	//乘以不透明度
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;
	
    return Out;
}