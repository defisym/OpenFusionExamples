
// Pixel shader Input structure
struct PS_InPUT
{
    float4 Position   : POSITION;
    float2 Texture    : TEXCOORD0;
};

// Pixel shader output structure
struct PS_OUTPUT
{
    float4 Color   : COLOR0;
};

// Global variables
sampler2D Tex0;
sampler2D patternpic;

int mode;

float progress;
float4 texcolor;

PS_OUTPUT ps_maIn( in PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out,Pic;
	if(mode==1)
	{
		Pic.Color = tex2D(Tex0,In.Texture);
		Out.Color = tex2D(patternpic,In.Texture);
	}
	else
	{
		Pic.Color = tex2D(Tex0,In.Texture);
		Out.Color = tex2D(Tex0,In.Texture);
	}
	
	float a;
	float step=18;
	float RgbStep=0.01;
	float AlphaStep=1/step;	
	
	progress=progress-step*RgbStep;

	a=1;
	texcolor.a=1;
	if(Pic.Color.a==0)	
	{		
		a=0;
	}
	if(Pic.Color.a!=0)	
	{		
		for(int i=step;i>0;i--)
		{
			if(Out.Color.r<=(progress+i*RgbStep))
			{
				a=a-AlphaStep;
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

// Effect technique
technique tech_maIn
{
    pass P0
    {
        // shaders
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 ps_maIn();
    }  
}