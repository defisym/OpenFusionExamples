// Pixel shader Input structure
struct PS_INPUT
{
  //Tint：对象属性中设定的RGBA
  float4 Tint : COLOR0;
  //输入的UV坐标
  //float2 Texture    : TEXCOORD0;
  float2 texCoord : TEXCOORD0;
  
  //SV_POSITION is used in vortex shader
  float4 Position : SV_POSITION;
};

// Pixel shader output structure
struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};

//Sampler
Texture2D<float4> Texture0 : register(t0);
sampler Texture0Sampler : register(s0);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	int     fR;
};

cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

// float GetGauss(int x,float sigma){	
// 	float a=1/(sigma*sqrt(2*3.1415926));
// 	float index=-1.0*(x*x)/(2*sigma*sigma);
// 	float result=a*exp(index);

// 	return result;
// }

float GetGauss2D(int x,int y,float sigma){	
	float a=1/(sigma*sigma*2*3.1415926);
	float index=-1.0*(x*x+y*y)/(2*sigma*sigma);
	float result=a*exp(index);

	return result;
}

#define MAX_SIZE 63

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In)
{
	// Output pixel
    PS_OUTPUT Out;
	
	//读取纹理
	Out.Color = Texture0.Sample(Texture0Sampler, In.texCoord) * In.Tint;

	//常规shader处理	
	
	//2D
	int R=1;

	if(2*fR+1>MAX_SIZE){
		R=(MAX_SIZE-1)/2;
	}
	else{
		R=fR;
	}

	//int r = ( int ) ( sigma * 3 + 0.5 ); 
	//完整模板的逻辑尺寸：( 2 * r + 1 ) * ( 2 * r + 1 );
	float sigma =(R-0.5)/3;

	float weight[MAX_SIZE][MAX_SIZE];
	float Normalization=0;

	for(int i=0;i<2*R+1;i++){
		for(int j=0;j<2*R+1;j++){
			weight[i][j]=GetGauss2D(abs(i-R),abs(j-R),sigma);
			Normalization+=weight[i][j];
		}
	}
	for(int k=0;k<2*R+1;k++){
		for(int l=0;l<2*R+1;l++){
			weight[k][l]=weight[k][l]/Normalization;
		}
	}
	
	float4 Sum=float4(0,0,0,Out.Color.a);

	for(int x=-R;x<=R;x++){
		for(int y=-R;y<=R;y++){
			Sum.rgb+=(Texture0.Sample(Texture0Sampler, In.texCoord+float2(x*fPixelWidth,y*fPixelHeight)) * In.Tint).rgb*weight[x+R][y+R];
		}
	}

	Out.Color.rgb = Sum.rgb;
	
    return Out;
}

// //预乘
// PS_OUTPUT ps_main_pm( in PS_INPUT In )
// {
// 	// Output pixel
//     PS_OUTPUT Out;
	
// 	//读取纹理
// 	Out.Color = Texture0.Sample(Texture0Sampler, In.texCoord) * In.Tint;
	
// 	//除以不透明度
// 	Out.Color.r=Out.Color.r/Out.Color.a;
// 	Out.Color.g=Out.Color.g/Out.Color.a;
// 	Out.Color.b=Out.Color.b/Out.Color.a;
	
// 	//常规shader处理
// 	Out.Color = Out.Color;
	
// 	//乘以不透明度
// 	Out.Color.r=Out.Color.r*Out.Color.a;
// 	Out.Color.g=Out.Color.g*Out.Color.a;
// 	Out.Color.b=Out.Color.b*Out.Color.a;
	
//     return Out;
// }