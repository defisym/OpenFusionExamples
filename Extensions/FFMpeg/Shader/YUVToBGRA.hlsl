struct PSInput
{
    float4 position: SV_Position;
    float3 color: COLOR0;
};

struct PSOutput
{
    float4 color: SV_Target0;
};

SamplerState samp : register(s0);

// Y plane
Texture2D<float> texY : register(t0);
// UV plane
Texture2D<float2> texUV : register(t1);

cbuffer PS_PIXELSIZE : register(b0)
{
    float fPixelWidth;
    float fPixelHeight;
}

static const float3x3 YUVtoRGBCoeffMatrix =
{
    1.164383f, 1.164383f, 1.164383f,
	0.000000f, -0.391762f, 2.017232f,
	1.596027f, -0.812968f, 0.000000f
};

float3 ConvertYUVtoRGB(float3 yuv)
{
	// Derived from https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
	// Section: Converting 8-bit YUV to RGB888

	// These values are calculated from (16 / 255) and (128 / 255)
    yuv -= float3(0.062745f, 0.501960f, 0.501960f);
    yuv = mul(yuv, YUVtoRGBCoeffMatrix);

    return saturate(yuv);
}


PSOutput Main(PSInput input)
{
    PSOutput output = (PSOutput)0;
    
    // may get green slide in float mode
    // 
    //float2 texCoord = input.position.xy * float2(fPixelWidth, fPixelHeight);
    //float y = texY.Sample(samp, texCoord);
    //float2 uv = texUV.Sample(samp, texCoord);        
    //output.color = float4(ConvertYUVtoRGB(float3(y, uv)), 1.0);
    
    uint2 texCoord = input.position.xy;
    float y = texY.Load(uint3(texCoord, 0));
    float2 uv = texUV.Load(uint3(texCoord / 2, 0));
    output.color = float4(ConvertYUVtoRGB(float3(y, uv)), 1.0);
    
    return output;
}