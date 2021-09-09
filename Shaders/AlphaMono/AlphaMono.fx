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
sampler2D BKD;

int fA;
bool BG;

float fPixelWidth; 
float fPixelHeight; 

PS_OUTPUT ps_maIn( in PS_InPUT In )
{
    // Output pixel
    PS_OUTPUT Out;
	PS_OUTPUT Mono;
	
	if(!BG){
        Out.Color = tex2D(Tex0,In.Texture);
    }else{
        Out.Color = tex2D(BKD,In.Texture);
        Out.Color.a = tex2D(Tex0,In.Texture).a;
    }

    float4 f4 = Out.Color * float4(0.299f, 0.587f, 0.114f, 1.0f);
	float f = f4.r + f4.g + f4.b;
    Mono.Color.rgb=f;
	
	float Alpha=1-fA/255.0;
    Out.Color.rgb=Mono.Color.rgb*Alpha+Out.Color.rgb*(1-Alpha);
    
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