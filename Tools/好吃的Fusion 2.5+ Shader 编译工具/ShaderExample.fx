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
sampler2D Tex0;

// Bled coefficient
float     fBlend;

PS_OUTPUT ps_main( in PS_INPUT In )
{
    PS_OUTPUT Out;
    Out.Color = tex2D(Tex0, In.Texture) * fBlend;
    return Out;
}

// Effect technique
technique tech_main
{
    pass P0
    {
        // shaders
        VertexShader = NULL;
        PixelShader  = compile ps_1_1 ps_main();
    }  
}