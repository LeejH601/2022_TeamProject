
struct VS_OUT
{
	float4 Position : SV_POSITION;
	uint index : INDEX;
};

VS_OUT SwordTrail_VS(uint nVertexID : SV_VertexID)
{
	VS_OUT output;

	output.Position = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.index = nVertexID ;

	return output;
}