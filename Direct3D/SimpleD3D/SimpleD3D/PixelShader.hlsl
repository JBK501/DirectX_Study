Texture2D shaderTexture;
SamplerState simpleSampler;

float4 main(float4 position : SV_POSITION, float2 tex : TEXCOORD0) : SV_TARGET
{
	//return shaderTexture.Sample(simpleSampler, tex);
	float4 result = shaderTexture.Sample(simpleSampler, tex);
	
	// È¸»öÁ¶
	/*float grayscale = dot(result, float3(0.3, 0.59, 0.11));
	result[0] = grayscale;
	result[1] = grayscale;
	result[2] = grayscale;*/

	return result;
}