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
sampler Texture0Sampler : register(s0);

cbuffer PS_VARIABLES : register(b0) {
	//和XML文件中次序相同
	float4 dotColor;

	uint rowSpace;
	uint colSpace;

	uint radius;
	float threshold;
};

cbuffer PS_PIXELSIZE : register(b1) {
	float fPixelWidth;
	float fPixelHeight;
}

//非预乘
PS_OUTPUT ps_main(in PS_INPUT In) {
	// Output pixel	
	PS_OUTPUT outPut;

	//读取纹理
	// Alpha in object properties is ignored
	float4 src = Texture0.Sample(Texture0Sampler, In.texCoord) * In.Tint;

	//常规shader处理
	float srcAlpha = src.a;
	
	float4 f4 = src * float4(0.299f, 0.587f, 0.114f, 1.0f);
	float grey = min(1.0, f4.r + f4.g + f4.b);

	// float alpha = srcAlpha * grey;
	float alpha = grey;
	float4 outColor = dotColor;
	
	outColor.a = 0.0;

	if(srcAlpha != 0){
		float curX = (float)(In.texCoord.x / fPixelWidth);
		float curY = (float)(In.texCoord.y / fPixelHeight);

		float centerX = (int)(curX / colSpace) * colSpace + (float)(colSpace / 2);
		float centerY = (int)(curY / rowSpace) * rowSpace + (float)(rowSpace / 2);

		// float centerX = curX - fmod(curX, colSpace) + (float)(colSpace / 2);
		// float centerY = curY - fmod(curY, rowSpace) + (float)(rowSpace / 2);

		float dis = pow(curX - centerX, 2) + pow(curY - centerY, 2);
		float rad = pow(radius, 2);
		
		if(dis <= rad){
			float percent = dis / rad;
			float alphaCoef = percent > threshold
								? 1.0 - ((percent - threshold) / (1.0 - threshold))
								: 1.0;

			outColor.a = alpha * alphaCoef;

			// outColor = src * (1.0 - outColor.a) + outColor * outColor.a;
			outColor = outColor * (1.0 - outColor.a) + src * outColor.a;

			outColor.a = srcAlpha;
		}
	}

	outPut.Color = outColor;
	
	return outPut;
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
	PS_OUTPUT outPut;

	//读取纹理
	// Alpha in object properties is ignored
	float4 src = deMulAlpha(Texture0.Sample(Texture0Sampler, In.texCoord) * In.Tint);

	//常规shader处理
	float srcAlpha = src.a;
	
	float4 f4 = src * float4(0.299f, 0.587f, 0.114f, 1.0f);
	float grey = min(1.0, f4.r + f4.g + f4.b);

	// float alpha = srcAlpha * grey;
	float alpha = grey;
	float4 outColor = dotColor;

	outColor.a = 0.0;

	if(srcAlpha != 0){
		float curX = (float)(In.texCoord.x / fPixelWidth);
		float curY = (float)(In.texCoord.y / fPixelHeight);

		float centerX = (int)(curX / colSpace) * colSpace + (float)(colSpace / 2);
		float centerY = (int)(curY / rowSpace) * rowSpace + (float)(rowSpace / 2);

		// float centerX = curX - fmod(curX, colSpace) + (float)(colSpace / 2);
		// float centerY = curY - fmod(curY, rowSpace) + (float)(rowSpace / 2);

		float dis = pow(curX - centerX, 2) + pow(curY - centerY, 2);
		float rad = pow(radius, 2);
		
		if(dis <= rad){
			float percent = dis / rad;
			float alphaCoef = percent > threshold
								? 1.0 - ((percent - threshold) / (1.0 - threshold))
								: 1.0;

			outColor.a = alpha * alphaCoef;

			// outColor = src * (1.0 - outColor.a) + outColor * outColor.a;
			outColor = outColor * (1.0 - outColor.a) + src * outColor.a;

			outColor.a = srcAlpha;
		}
	}

	outPut.Color = outColor;
	outPut.Color = mulAlpha(outPut.Color);	

	return outPut;
}