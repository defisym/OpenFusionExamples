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
sampler2D frontpic;
sampler2D backpic;

float fR;
float health;
float maxhealth;

float fPixelWidth; 
float fPixelHeight; 

PS_OUTPUT ps_maIn( in PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out;
	PS_OUTPUT front;
	PS_OUTPUT back;
	Out.Color = tex2D(Tex0,In.Texture);
	front.Color = tex2D(frontpic,In.Texture);
	back.Color = tex2D(backpic,In.Texture);
	
	float xr=fR*fPixelWidth;
	float yr=fR*fPixelHeight;
	
	float percent;
	percent=(health/maxhealth);

	
	float a=0;
	
	if(pow((In.Texture.y-0.5)/yr,2)+pow((In.Texture.x-0.5)/xr,2)<=1)
	{
		a=1;
		Out.Color=front.Color;
		if(In.Texture.y < (1-percent))
		{
			//a=0;
			Out.Color=back.Color;
		}
	}
	
	Out.Color.a=a;
    return Out;
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