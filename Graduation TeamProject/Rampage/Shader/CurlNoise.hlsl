


#define RAMP(r) r >= 1.0f ? 1.0f : (r <= -1.0f ? -1.0f : 15.0f/8.0f * r - 10.0f / 8.0f * pow(r,3) + 3.0f / 8.0f * pow(r,5) )

float4 mod289(float4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float mod289(float x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x) {
	return mod289(((x * 34.0) + 1.0) * x);
}

float permute(float x) {
	return mod289(((x * 34.0) + 1.0) * x);
}

float4 taylorInvSqrt(float4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

float taylorInvSqrt(float r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

float4 LessThan(float4 lhs, float4 rhs) {
	float4 bVec;

	bVec.x = lhs.x < rhs.x ? 1 : 0;
	bVec.y = lhs.y < rhs.y ? 1 : 0;
	bVec.z = lhs.z < rhs.z ? 1 : 0;
	bVec.w = lhs.w < rhs.w ? 1 : 0;

	return bVec;
}

float4 grad4(float j, float4 ip)
{
	const float4 ones = float4(1.0, 1.0, 1.0, -1.0);
	float4 p, s;

	p.xyz = floor(frac(float4(j,j,j,j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);

	s = float4(LessThan(p, float4(0.0,0.0,0.0,0.0)));
	p.xyz = p.xyz + (s.xyz * 2.0 - 1.0) * s.www;

	return p;
}

// (sqrt(5) - 1)/4 = F4, used once below
#define F4 0.309016994374947451


float snoise(float4 v, float FrequencyValue = 1.0f, float AmplitudeValue = 1.0f)
{
	const float4  C = float4(0.138196601125011,  // (5 - sqrt(5))/20  G4
		0.276393202250021,  // 2 * G4
		0.414589803375032,  // 3 * G4
		-0.447213595499958); // -1 + 4 * G4

	// First corner
	float4 i = floor(v + dot(v, float4(F4, F4, F4, F4)));
	float4 x0 = v - i + dot(i, C.xxxx) * FrequencyValue;

	// Other corners

	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
	float4 i0;
	float3 isX = step(x0.yzw, x0.xxx);
	float3 isYZ = step(x0.zww, x0.yyz);
	//  i0.x = dot( isX, vec3( 1.0 ) );
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;
	//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;

	// i0 now contains the unique values 0,1,2,3 in each channel
	float4 i3 = clamp(i0, 0.0, 1.0);
	float4 i2 = clamp(i0 - 1.0, 0.0, 1.0);
	float4 i1 = clamp(i0 - 2.0, 0.0, 1.0);

	//  x0 = x0 - 0.0 + 0.0 * C.xxxx
	//  x1 = x0 - i1  + 1.0 * C.xxxx
	//  x2 = x0 - i2  + 2.0 * C.xxxx
	//  x3 = x0 - i3  + 3.0 * C.xxxx
	//  x4 = x0 - 1.0 + 4.0 * C.xxxx
	float4 x1 = x0 - i1 + C.xxxx;
	float4 x2 = x0 - i2 + C.yyyy;
	float4 x3 = x0 - i3 + C.zzzz;
	float4 x4 = x0 + C.wwww;

	// Permutations
	i = mod289(i);
	float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
	float4 j1 = permute(permute(permute(permute(
		i.w + float4(i1.w, i2.w, i3.w, 1.0))
		+ i.z + float4(i1.z, i2.z, i3.z, 1.0))
		+ i.y + float4(i1.y, i2.y, i3.y, 1.0))
		+ i.x + float4(i1.x, i2.x, i3.x, 1.0));

	// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
	// 7*7*6 = 294, which is close to the ring size 17*17 = 289.
	float4 ip = float4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

	float4 p0 = grad4(j0, ip);
	float4 p1 = grad4(j1.x, ip);
	float4 p2 = grad4(j1.y, ip);
	float4 p3 = grad4(j1.z, ip);
	float4 p4 = grad4(j1.w, ip);

	// Normalise gradients
	float4 norm = taylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylorInvSqrt(dot(p4, p4));

	// Mix contributions from the five corners
	float3 m0 = max(0.6 - float3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
	float2 m1 = max(0.6 - float2(dot(x3, x3), dot(x4, x4)), 0.0);
	m0 = m0 * m0;
	m1 = m1 * m1;
	return 49.0 * (dot(m0 * m0, float3(dot(p0, x0), dot(p1, x1), dot(p2, x2)))
		+ dot(m1 * m1, float2(dot(p3, x3), dot(p4, x4)))) * AmplitudeValue;
}

float smoothstep(float edge0, float edge1, float x)
{
	// Scale, bias and saturate x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}

//// Uniform data
//// From previous state
//uniform sampler2D position_sampler_2D;
//uniform sampler2D velocity_sampler_2D;
//// From current state
//uniform sampler2D acceleration_sampler_2D;
//uniform float dt; // Time step
//uniform float time; // Global time 
//// Properties of the particle system
//uniform float field_speed;
//uniform float noise_strength;
//uniform float progression_rate;
//uniform float length_scale;
//uniform vec3  field_main_direction;
//// Blocking sphere
//uniform vec3 sphere_position;
//uniform float sphere_radius;

cbuffer cbNoiseParameters : register(b2)
{
	float gnDt : packoffset(c0.x);
	float gnTime : packoffset(c0.y);
	float gnFieldSpeed : packoffset(c0.z);
	float gnNoiseStrength : packoffset(c0.w);
	float3 gnFieldMainDirection : packoffset(c1);
	float gnProgressionRate : packoffset(c1.w);
	float3 gnSpherePosition : packoffset(c2);
	float gnLengthScale : packoffset(c2.w);
	float gnSphereRadius : packoffset(c3);
};


inline float3 potential(float3 p)
{
	float L;      // Length scale as described by Bridson
	float speed;  // field speed
	float alpha;  // Alpha as described by Bridson
	float beta;   // amount of curl noise compared to the constant field
	float3  n;      // Normal of closest surface
	float3  pot;    // Output potential

	L = gnLengthScale;
	speed = gnFieldSpeed;
	beta = gnNoiseStrength;

	// Start with an empty field
	pot = float3(0, 0, 0);
	// Add Noise in each direction
	float progression_constant = 2;
	pot += L * beta * speed * float3(
		snoise(float4(p.x, p.y, p.z, gnTime * gnProgressionRate * progression_constant) / L),
		snoise(float4(p.x, p.y + 43, p.z, gnTime * gnProgressionRate * progression_constant) / L),
		snoise(float4(p.x, p.y, p.z + 43, gnTime * gnProgressionRate * progression_constant) / L));

	// External directional field
	// Rotational potential gives a constant velocity field
	float3 p_parallel = dot(gnFieldMainDirection, p) * gnFieldMainDirection;
	float3 p_orthogonal = p - p_parallel;
	float3 pot_directional = cross(p_orthogonal, gnFieldMainDirection);

	// Add the rotational potential
	pot += (1 - beta) * speed * pot_directional;

	// Affect the field by a sphere
	// The closer to the sphere, the less of the original potential
	// and the more of a tangental potential.
	// The variable d_0 determines the distance to the sphere when the
	// particles start to become affected.
	/*float d_0 = L * 0.5;
	alpha = abs((smoothstep(gnSphereRadius, gnSphereRadius + d_0, length(p - gnSpherePosition))));
	n = normalize(p);
	pot = (alpha)*pot + (1 - (alpha)) * n * dot(n, pot);*/

	return pot;
}


inline float4 CalculrateCulrNoise(float3 p) 
{
	// Step length for approximating derivatives
	float epsilon = 0.0001;
	float3 pot = potential(p);

	// Partial derivatives of different components of the potential
	float dp3_dy = (pot.z - potential(float3(p.x, p.y + epsilon, p.z))).z / epsilon;
	float dp2_dz = (pot.y - potential(float3(p.x, p.y, p.z + epsilon))).y / epsilon;
	float dp1_dz = (pot.x - potential(float3(p.x, p.y, p.z + epsilon))).x / epsilon;
	float dp3_dx = (pot.z - potential(float3(p.x + epsilon, p.y, p.z))).z / epsilon;
	float dp2_dx = (pot.y - potential(float3(p.x + epsilon, p.y, p.z))).y / epsilon;
	float dp1_dy = (pot.x - potential(float3(p.x, p.y + epsilon, p.z))).x / epsilon;

	// vel = nabla x potential
	// Since this vecotor field has only a vector potential component
	// it is divergent free and hence contains no sources
	float3 vel = float3(dp3_dy - dp2_dz, dp1_dz - dp3_dx, dp2_dx - dp1_dy);
	return float4(vel.x, vel.y, vel.z, 1);
}

