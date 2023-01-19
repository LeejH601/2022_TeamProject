struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

float4 PS_Bound(VS_OUTPUT input) : SV_TARGET
{
	return(float4(1.0f, 0.0f, 0.0f, 1.0f));
}