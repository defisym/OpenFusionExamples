// Pixel shader Input structure
struct PS_InPUT {
    float4 Position   : POSITION;
    float2 Texture    : TEXCOORD0;
};

// Pixel shader output structure
struct PS_OUTPUT {
    float4 Color   : COLOR0;
};

// Global variables
sampler2D Tex0;
sampler2D patternpic;

int dir;
int mode;
float progress;
float transitionRange;
float4 texcolor;

// get 0.0 ~ 1.0
float Normalization(float input, float maximum){
	return input / maximum;
}

// input 0.0 ~ 1.0
float DeNormalization(float input, float maximum){
	return input * maximum;
}

PS_OUTPUT ps_maIn( in PS_InPUT In ){
    // Output pixel
	PS_OUTPUT Out;
	PS_OUTPUT Tex;
	PS_OUTPUT PatternTex;

	Tex.Color = tex2D(Tex0,In.Texture);

	float4 TexColor = texcolor;
	TexColor.a = Tex.Color.a;

	Out.Color = mode
					? Tex.Color
					: TexColor;
	PatternTex.Color = mode
					? tex2D(patternpic,In.Texture)
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

// Effect technique
technique tech_maIn {
    pass P0 {
        // shaders
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 ps_maIn();
    }  
}