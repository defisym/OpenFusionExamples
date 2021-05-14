
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

float fR;

float fPixelWidth; 
float fPixelHeight; 

PS_OUTPUT ps_maIn( in PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out;
	Out.Color = tex2D(Tex0,In.Texture);
	
	float a=0;
	float xr=fR*fPixelWidth;
	float xr2=1-xr;
	float yr=fR*fPixelHeight;
	float yr2=1-yr;
	
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