
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

int levelNum;
int samplerSize;

float fPixelWidth; 
float fPixelHeight; 

float normalize(float f, float div) {
    return f - fmod(f, div);
}

float3 normalize_3(float3 f3, float div){
    f3.r = normalize(f3.r, div);
    f3.g = normalize(f3.g, div);
    f3.b = normalize(f3.b, div);

    return f3;
}

float2 updateSize(float2 f2) {
    float divX = 1.0 / ((1.0 / fPixelWidth) / samplerSize);
    float divY = 1.0 / ((1.0 / fPixelHeight) / samplerSize);

    f2.x = normalize(f2.x, divX);
    f2.y = normalize(f2.y, divY);

    return f2;
}

PS_OUTPUT ps_main( in PS_INPUT In )
{
    // Output pixel
    PS_OUTPUT Out;

    Out.Color = tex2D(Tex0, updateSize(In.Texture));

    float div = 1.0 / levelNum;

    Out.Color.rgb = normalize_3(Out.Color.rgb, div);

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