#pragma once
#include "SimpleShader.h"
class Material
{
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, 
		ID3D11ShaderResourceView* textureSRV, ID3D11ShaderResourceView* specularSRV, ID3D11ShaderResourceView* normalMapSRV, 
		ID3D11SamplerState* samplerState);
	~Material();

	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();

	ID3D11ShaderResourceView* GetTextureSRV();
	ID3D11ShaderResourceView* GetSpecularSRV();
	ID3D11ShaderResourceView* GetNormalMapSRV();
	ID3D11SamplerState* GetSamplerState();
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11ShaderResourceView* specularSRV;
	ID3D11ShaderResourceView* normalMapSRV;
	ID3D11SamplerState* samplerState;
};

