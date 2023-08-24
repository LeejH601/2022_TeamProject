struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 velocity : VELOCITY;
	float lifetime : LIFETIME;
	int type : TYPE;
	float EmitTime : EMITTIME;
	uint TextureIndex :TEXTUREINDEX;
	uint2 SpriteTotalCoord : TEXTURECOORD;
	uint2 SpriteTotalCoord1 : TEXTURECOORD1;
	uint2 SpriteTotalCoord2 : TEXTURECOORD2;
	float2 size : SIZE;
	float emissive : EMISSIVE;
	uint rotateFlag : ROTATEFLAG;
	uint ScaleFlag : SCALEFLAG;
	float4 color : COLOR;
};



struct GS_PARTICLE_DRAW_OUTPUT
{
	float4 position : SV_Position;
	float3 positionW : POSITION;
	float4 color : COLOR;
	float2 uv : TEXTURE;
	float alpha : ALPHA;
	uint TextureIndex :TEXTUREINDEX;
	uint2 TextureCoord : TEXTURECOORD;
	float lifetime : LIFETIME;
	float EmitTime : EMITTIME; // 방출 시작 시간
	float emissive : EMISSIVE;
	uint ParticleType : PARTICLETYPE;
};

