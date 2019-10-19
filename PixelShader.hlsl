
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
};

struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

struct PointLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 position;
};

// Holds data passed in from c++
cbuffer externalData : register(b0) {
	DirectionalLight light; // must match string passed through pixelShader->SetData
	DirectionalLight secondLight;
	PointLight pointLight;
	PointLight secondPointLight;
	float3 cameraPosition;
};

// Helper method that computes directional lights
float4 ComputeDirectionalLight(DirectionalLight light, float3 normal, float3 toCamera, float4 surfaceColor) {
	// Negate and normalize the direction
	float3 dirToLight = normalize(-light.direction);

	// Calculate the light amount
	float lightAmount = saturate(dot(normal, dirToLight));

	// Specular Light (Blinn Phong)
	float shininessExponent = 128.0f;
	float3 halfwayDir = normalize(dirToLight + toCamera);
	float NdotH = dot(normal, halfwayDir);	// get cosine between normal and halfway vector
	float specularAmount = saturate(pow(NdotH, shininessExponent));

	float4 result = light.diffuseColor * lightAmount;
	result += light.ambientColor;

	// Diffuse with surface texture
	result *= surfaceColor;

	// Add specular shining
	result += specularAmount;

	return result;
}

// Helper method that computes point lights
float4 ComputePointLight(PointLight light, float3 normal, float3 worldPos, float3 toCamera, float4 surfaceColor) {
	// Diffuse calculation
	float3 directionToLight = normalize(light.position - worldPos);
	float NdotL = saturate(dot(normal, directionToLight));

	// Specular Light (Blinn Phong)
	float shininessExponent = 128.0f;
	float3 halfwayDir = normalize(directionToLight + normalize(cameraPosition - worldPos));
	float NdotH = saturate(dot(normal, halfwayDir));	// get cosine between normal and halfway vector
	float specularAmount = saturate(pow(NdotH, shininessExponent));

	// Final color for directional light
	float4 result = (light.diffuseColor * NdotL) + light.ambientColor;

	// Diffuse with surface texture
	result *= surfaceColor;

	// Add specular shining
	result += specularAmount;

	return result;
}

// Passed in from c++
Texture2D diffuseTexture	: register(t0);
SamplerState basicSampler	: register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Calculate material color with texture
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	float3 toCamera = normalize(cameraPosition - input.worldPos);

	// normalize, as interpolation in vertShader could result in non unit vectors
	input.normal = normalize(input.normal);

	//compute the various lights and return them
	return
		ComputeDirectionalLight(light, input.normal, input.worldPos, surfaceColor) +
		ComputeDirectionalLight(secondLight, input.normal, input.worldPos, surfaceColor) +
		ComputePointLight(pointLight, input.normal, input.worldPos, toCamera, surfaceColor) + 
		ComputePointLight(secondPointLight, input.normal, input.worldPos, toCamera, surfaceColor);
}