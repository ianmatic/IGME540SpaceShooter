#include "Material.h"

Material::Material(SimpleVertexShader* vertexShader ,SimplePixelShader* pixelShader, 
	ID3D11ShaderResourceView* textureSRV, ID3D11ShaderResourceView* specularSRV, ID3D11ShaderResourceView* normalMapSRV, 
	ID3D11SamplerState* samplerState)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->textureSRV = textureSRV;
	this->specularSRV = specularSRV;
	this->normalMapSRV = normalMapSRV;
	this->samplerState = samplerState;
}

Material::~Material()
{
	// nothing to delete
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView* Material::GetTextureSRV()
{
	return textureSRV;
}

ID3D11ShaderResourceView* Material::GetSpecularSRV()
{
	return specularSRV;
}

ID3D11ShaderResourceView* Material::GetNormalMapSRV()
{
	return normalMapSRV;
}

ID3D11SamplerState* Material::GetSamplerState()
{
	return samplerState;
}
