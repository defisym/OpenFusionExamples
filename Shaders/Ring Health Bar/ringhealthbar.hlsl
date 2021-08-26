// Pixel shader Input structure
struct PS_InPUT
{    
	float4 Tint : COLOR0;
    float2 Texture    : TEXCOORD0;
};

// Pixel shader output structure
struct PS_OUTPUT
{
    float4 Color   : SV_TARGET;
};

// Global variables
//sampler2D Tex0;
Texture2D<float4> Tex0:register(t0);
sampler Tex0Sampler:register(s0);

cbuffer PS_VARIABLES:register(b0)
{
	float fR;
	float fD;	
	float angle;
}


cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

PS_OUTPUT ps_main( in PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out;	
	Out.Color=Tex0.Sample(Tex0Sampler,In.Texture)*In.Tint;
	
	float fR2;
	
	float deg2rad = 0.017453;
	//pi/180
	float xcenter=0.5*fPixelHeight;
	float ycenter=0.5*fPixelWidth;
	float xr=fR*fPixelWidth;
	float yr=fR*fPixelHeight;
	
	fR2=fR-fD;
	float xr2=fR2*fPixelWidth;
	float yr2=fR2*fPixelHeight;
	
	float a=0;
	//float alpha=1;
	float alpha=Out.Color.a;
	
	if((pow((In.Texture.y-0.5)/yr,2)+pow((In.Texture.x-0.5)/xr,2)<=1)&&(pow((In.Texture.y-0.5)/yr2,2)+pow((In.Texture.x-0.5)/xr2,2)>=1))
	{
		a=Out.Color.a;
		if(angle < 0)
		{
			a=0;
		}			
		else if(angle <= 90)
		{
            if((In.Texture.x < 0.5)||(In.Texture.y > 0.5))
			{
				a=0;
			}
			else if (In.Texture.y < 0.5 && abs(0.5 -In.Texture.y) >= abs(0.5 -In.Texture. x) * tan((90-angle) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }
		else if(angle <= 180)
		{
            if(In.Texture.x < 0.5)
			{
				a=0;
			}
			else if((In.Texture.x > 0.5)&&(In.Texture.y < 0.5))
			{
				a=alpha;
			}
			else if (In.Texture.y > 0.5 && abs(0.5 -In.Texture.y) <= abs(0.5 -In.Texture. x) * tan((angle-90) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }
		else if(angle <= 270)
		{
            if((In.Texture.x < 0.5)&&(In.Texture.y < 0.5))
			{
				a=0;
			}
			if(In.Texture.x > 0.5)
			{
				a=alpha;
			}			
			else if (In.Texture.y > 0.5 && abs(0.5 -In.Texture.y) >= abs(0.5 -In.Texture. x) * tan((270-angle) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }
		else if(angle <= 360)
		{           
			if((In.Texture.x > 0.5)||(In.Texture.y > 0.5))
			{
				a=alpha;
			}
			else if (In.Texture.y < 0.5 && abs(0.5 -In.Texture.y) <= abs(0.5 -In.Texture. x) * tan((angle-270) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }      
	}
	
	Out.Color.a=a;
    return Out;
}

PS_OUTPUT ps_main_pm( in PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out;
	Out.Color=Tex0.Sample(Tex0Sampler,In.Texture)*In.Tint;

	Out.Color.r=Out.Color.r/Out.Color.a;
	Out.Color.g=Out.Color.g/Out.Color.a;
	Out.Color.b=Out.Color.b/Out.Color.a;
	
	float fR2;
	
	float deg2rad = 0.017453;
	//pi/180
	float xcenter=0.5*fPixelHeight;
	float ycenter=0.5*fPixelWidth;
	float xr=fR*fPixelWidth;
	float yr=fR*fPixelHeight;
	
	fR2=fR-fD;
	float xr2=fR2*fPixelWidth;
	float yr2=fR2*fPixelHeight;
	
	float a=0;
	//float alpha=1;
	float alpha=Out.Color.a;
	
	if((pow((In.Texture.y-0.5)/yr,2)+pow((In.Texture.x-0.5)/xr,2)<=1)&&(pow((In.Texture.y-0.5)/yr2,2)+pow((In.Texture.x-0.5)/xr2,2)>=1))
	{
		a=Out.Color.a;
		if(angle < 0)
		{
			a=0;
		}			
		else if(angle <= 90)
		{
            if((In.Texture.x < 0.5)||(In.Texture.y > 0.5))
			{
				a=0;
			}
			else if (In.Texture.y < 0.5 && abs(0.5 -In.Texture.y) >= abs(0.5 -In.Texture. x) * tan((90-angle) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }
		else if(angle <= 180)
		{
            if(In.Texture.x < 0.5)
			{
				a=0;
			}
			else if((In.Texture.x > 0.5)&&(In.Texture.y < 0.5))
			{
				a=alpha;
			}
			else if (In.Texture.y > 0.5 && abs(0.5 -In.Texture.y) <= abs(0.5 -In.Texture. x) * tan((angle-90) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }
		else if(angle <= 270)
		{
            if((In.Texture.x < 0.5)&&(In.Texture.y < 0.5))
			{
				a=0;
			}
			if(In.Texture.x > 0.5)
			{
				a=alpha;
			}			
			else if (In.Texture.y > 0.5 && abs(0.5 -In.Texture.y) >= abs(0.5 -In.Texture. x) * tan((270-angle) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }
		else if(angle <= 360)
		{           
			if((In.Texture.x > 0.5)||(In.Texture.y > 0.5))
			{
				a=alpha;
			}
			else if (In.Texture.y < 0.5 && abs(0.5 -In.Texture.y) <= abs(0.5 -In.Texture. x) * tan((angle-270) * deg2rad))
			{
				a=alpha;
			}
			else
			{
				a=0;
			}
        }      
	}
	
	Out.Color.a=a;
	
	Out.Color.r=Out.Color.r*Out.Color.a;
	Out.Color.g=Out.Color.g*Out.Color.a;
	Out.Color.b=Out.Color.b*Out.Color.a;
	
    return Out;
}