// Pixel shader Input structure
struct PS_INPUT {
	//Tint：对象属性中设定的RGBA
	float4 Tint : COLOR0;
	//输入的UV坐标
	//float2 Texture    : TEXCOORD0;
	float2 texCoord : TEXCOORD0;

	//SV_POSITION is used in vortex shader
	float4 Position : SV_POSITION;
};

// Pixel shader output structure
struct PS_OUTPUT {
    float4 Color : SV_TARGET;
};

//Sampler
Texture2D<float4> Texture0 : register(t0);
sampler TextureSampler0 : register(s0);

Texture2D<float4> patternpic : register(t1);
sampler patternpicSampler0 : register(s1);

cbuffer PS_VARIABLES : register(b0) {
	//和XML文件中次序相同
	float progress;
	int dir;
	float transitionRange;
	int mode;
	float4 texcolor;
};

cbuffer PS_PIXELSIZE:register(b1) {
	float fPixelWidth; 
	float fPixelHeight; 
}

// get 0.0 ~ 1.0
float Normalization(float input, float maximum){
	return input / maximum;
}

// input 0.0 ~ 1.0
float DeNormalization(float input, float maximum){
	return input * maximum;
}

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In) {
	// Output pixel
	PS_OUTPUT Out;
	PS_OUTPUT Tex;
	PS_OUTPUT PatternTex;

	Tex.Color = Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint;

	float4 TexColor = texcolor;
	TexColor.a = Tex.Color.a;

	Out.Color = mode
					? Tex.Color
					: TexColor;
	PatternTex.Color = mode
					? patternpic.Sample(patternpicSampler0,In.texCoord)
					: Tex.Color;

	// a == 0 transparent
	// a == 1 opaque

	// float alpha = max(0, min(1, 2 * progress - PatternTex.Color.r));

	if(Tex.Color.a != 0) {
		float4 f4 = PatternTex.Color * float4(0.299f, 0.587f, 0.114f, 1.0f);
		float grey = min(1.0, f4.r + f4.g + f4.b);

		float deNormalizedProgress = DeNormalization(progress, 1.0 + transitionRange);

		if(grey >= deNormalizedProgress) {
				Out.Color.a = 0;
		} else if(grey + transitionRange >= deNormalizedProgress) {
			float transitionAlpha = max(0, min(1.0, (deNormalizedProgress - grey) * (1.0 / transitionRange)));
			Out.Color.a = Out.Color.a * transitionAlpha;
		}

		if(dir) {
			Out.Color.a = 1.0 - Out.Color.a;
		}
	}

	return Out;
}

float4 deMulAlpha(float4 color){
	color.r = color.r / color.a;
	color.g = color.g / color.a;
	color.b = color.b / color.a;

	return color;
}

float4 mulAlpha(float4 color){
	color.r = color.r * color.a;
	color.g = color.g * color.a;
	color.b = color.b * color.a;

	return color;
}

//预乘
PS_OUTPUT ps_main_pm( in PS_INPUT In ) {
// Output pixel
	PS_OUTPUT Out;
	PS_OUTPUT Tex;
	PS_OUTPUT PatternTex;

	Tex.Color = deMulAlpha(Texture0.Sample(TextureSampler0, In.texCoord) * In.Tint);

	float4 TexColor = texcolor;
	TexColor.a = Tex.Color.a;

	Out.Color = mode
					? Tex.Color
					: TexColor;
	PatternTex.Color = mode
					? deMulAlpha(patternpic.Sample(patternpicSampler0,In.texCoord))
					: Tex.Color;

	// a == 0 transparent
	// a == 1 opaque

	// float alpha = max(0, min(1, 2 * progress - PatternTex.Color.r));

	if(Tex.Color.a != 0) {
		float4 f4 = PatternTex.Color * float4(0.299f, 0.587f, 0.114f, 1.0f);
		float grey = min(1.0, f4.r + f4.g + f4.b);

		float deNormalizedProgress = DeNormalization(progress, 1.0 + transitionRange);

		if(grey >= deNormalizedProgress) {
				Out.Color.a = 0;
		} else if(grey + transitionRange >= deNormalizedProgress) {
			float transitionAlpha = max(0, min(1.0, (deNormalizedProgress - grey) * (1.0 / transitionRange)));
			Out.Color.a = Out.Color.a * transitionAlpha;
		}

		if(dir) {
			Out.Color.a = 1.0 - Out.Color.a;
		}
	}
	
	Out.Color = mulAlpha(Out.Color);

	return Out;
}