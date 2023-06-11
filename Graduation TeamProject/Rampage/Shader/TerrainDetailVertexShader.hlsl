SamplerState gSamplerState : register(s0);


struct VS_DETAIL_OUTPUT
{
	float4 position : SV_POSITION;
	float2 size : SIZE;
	uint nInstanceID : SV_InstanceID;
};


VS_DETAIL_OUTPUT VS_Detail(uint nVertexID : SV_VertexID, uint nInstanceID : SV_InstanceID)
{
	VS_DETAIL_OUTPUT output;
	output.position = float4(0, 0, 0, 1);
	output.size = float2(5.0f, 5.0f);
	output.nInstanceID = nInstanceID;

	return output;
}