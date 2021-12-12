// Pixel shader input structure
struct PS_INPUT
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
sampler2D Source;
sampler2D Tex0 : register(s1);

int side;
int offset;

float fPixelWidth; 
float fPixelHeight; 

PS_OUTPUT ps_main( in PS_INPUT In )
{
    // Output pixel
    PS_OUTPUT Out,Src;
	
	float a;	
	Src.Color = tex2D(Source, In.Texture.xy);	
	a=Src.Color.a;
	
	
	if(((In.Texture.x<=offset*fPixelWidth)||((In.Texture.x+offset*fPixelWidth)>=1))||((In.Texture.y<=offset*fPixelHeight)||((In.Texture.y+offset*fPixelHeight)>=1)))
		{
			a=0;
		}
	
	side=(side==0)?-1:1;
	
	In.Texture.x=In.Texture.x-side*offset*fPixelWidth;
	In.Texture.y=In.Texture.y-side*offset*fPixelHeight;
	
	Out.Color = tex2D(Tex0, In.Texture.xy);			
	Out.Color.a=a;
	
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
    }  
}