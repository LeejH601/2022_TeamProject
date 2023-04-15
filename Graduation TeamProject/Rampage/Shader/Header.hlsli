struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 f4Scene : SV_TARGET0; //Swap Chain Back Buffer

	float4 f4Color : SV_TARGET1;
	float4 f4Normal : SV_TARGET2;
	float4 f4PositoinW : SV_TARGET3;
	float4 f4Illumination : SV_TARGET4;
	float4 f4Trail : SV_TARGET5;
	float4 f4CameraNormal : SV_TARGET6;
};

