sampler2D img;

float4 dotColor;

int rowSpace;
int colSpace;

int radius;
float threshold;

float fPixelWidth, fPixelHeight;

float4 ps_main(in float2 In : TEXCOORD0) : COLOR0 {
	float4 src = tex2D(img,In);
	float srcAlpha = src.a;
	
	float4 f4 = src * float4(0.299f, 0.587f, 0.114f, 1.0f);
	float grey = min(1.0, f4.r + f4.g + f4.b);

	// float alpha = srcAlpha * grey;
	float alpha = grey;
	dotColor.a = 0.0;

	if(srcAlpha != 0){
		float curX = (float)(In.x / fPixelWidth);
		float curY = (float)(In.y / fPixelHeight);

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

			dotColor.a = alpha * alphaCoef;

			// dotColor = src * (1.0 - dotColor.a) + dotColor * dotColor.a;
			dotColor = dotColor * (1.0 - dotColor.a) + src * dotColor.a;

			dotColor.a = srcAlpha;
		}
	}

	return dotColor;
}

technique tech_main {
	pass P0 { PixelShader = compile ps_2_0 ps_main(); }
}