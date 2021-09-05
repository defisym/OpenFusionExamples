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

Texture2D<float4> bkd : register(t1);
sampler bkdSampler : register(s1);

cbuffer PS_VARIABLES : register(b0)
{
	//和XML文件中次序相同
	int     fR;
	bool	BG;
	int		fA;
};

cbuffer PS_PIXELSIZE:register(b1){
	float fPixelWidth; 
	float fPixelHeight; 
}

float GetGauss(int x,float sigma){	
	float a=1/(sigma*sqrt(2*3.1415926));
	float index=-1.0*(x*x)/(2*sigma*sigma);
	float result=a*exp(index);

	return result;
}

// No need to process edge manually
// float Edge(float Input){
// 	if(Input>1.0){
// 		Input=1.0-(Input-1.0);
// 	}
// 	else if(Input<0.0){
// 		Input=0.0+(0.0-Input);
// 	}
// 	return Input;
// }

// float2 PositionEdge(float2 Input){
// 	Input.x=Edge(Input.x);
// 	Input.y=Edge(Input.y);
// 	return Input;
// }

// Sum_Temp.rgb+=(Texture0.Sample(Texture0Sampler, PositionEdge(In.texCoord+float2(0,y*fPixelHeight)+float2(x*fPixelWidth,0))) * In.Tint).rgb*weight[x+R];

#define MAX_R		1024
#define MAX_SIZE	2*MAX_R+1

//非预乘
PS_OUTPUT ps_main( in PS_INPUT In)
{
	// Output pixel
    PS_OUTPUT Out;
	
	//读取纹理
	Out.Color = Texture0.Sample(Texture0Sampler, In.texCoord) * In.Tint;

	//常规shader处理
		
	//1D		
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

	float weight[MAX_SIZE];
	float Normalization=0;

	for(int i=0;i<2*R+1;i++){
		weight[i]=GetGauss(abs(i-R),sigma);
		Normalization+=weight[i];
	}
	for(int j=0;j<2*R+1;j++){
		weight[j]=weight[j]/Normalization;
	}
	
	float4 Sum=float4(0,0,0,Out.Color.a);

	for(int y=-R;y<=R;y++){
		float4 Sum_Temp=float4(0,0,0,Out.Color.a);
		for(int x=-R;x<=R;x++){
			if(!BG){
				Sum_Temp.rgb+=(Texture0.Sample(Texture0Sampler, In.texCoord+float2(0,y*fPixelHeight)+float2(x*fPixelWidth,0)) * In.Tint).rgb*weight[x+R];
			}else{
				Sum_Temp.rgb+=(bkd.Sample(bkdSampler, In.texCoord+float2(0,y*fPixelHeight)+float2(x*fPixelWidth,0))).rgb*weight[x+R];
			}
		}
		Sum.rgb+=Sum_Temp.rgb*weight[y+R];
	}

	if(BG){
		float Alpha=1-fA/255.0;
		Out.Color.rgb=Out.Color.rgb*Alpha+Sum.rgb*(1-Alpha);
	}else{
		Out.Color.rgb = Sum.rgb;
	}

	Out.Color.rgb = Out.Color.rgb*Out.Color.a+(bkd.Sample(bkdSampler, In.texCoord)).rgb*(1-Out.Color.a);

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