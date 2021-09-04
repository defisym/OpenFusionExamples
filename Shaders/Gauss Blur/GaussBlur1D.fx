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
sampler2D bkd;

int fR;
bool	Dir;

float fPixelWidth; 
float fPixelHeight; 

// float samples[5][5]={{1/65,2/65,3/65,2/65,1/65},{2/65,3/65,4/65,3/65,2/65},{3/65,4/65,5/65,4/65,3/65},{2/65,3/65,4/65,3/65,2/65},{1/65,2/65,3/65,2/65,1/65}};
float samples[3][3]={{0.0625,0.125,0.0625},{0.125,0.25,0.125},{0.0625,0.125,0.0625}};

// PS_OUTPUT ps_maIn( in PS_InPUT In )
PS_OUTPUT ps_main( PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out,sum;
	Out.Color = tex2D(Tex0,In.Texture);	
	
	// float HeightBlur=fR*fPixelHeight;
	// float WidthBlur=fR*fPixelWidth;
	float HeightBlur=fPixelHeight;
	float WidthBlur=fPixelWidth;

	sum.Color=float4(0,0,0,Out.Color.a);

	for(int x=-1;x<=1;x++){
		for(int y=-1;y<=1;y++){
			sum.Color+=tex2D(bkd,In.Texture+float2(x*HeightBlur,y*WidthBlur))*samples[1+x][1+y];
		}
	}

	Out.Color.rgb = sum.Color.gbr;
	
	return Out;
}

// Effect technique
technique tech_main
{
    pass P0
    {
        // shaders
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 ps_main();
		// PixelShader  = compile ps_2_sw ps_main();
		// PixelShader  = compile ps_3_0 ps_main();
    }  
}