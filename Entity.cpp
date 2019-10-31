#include "Entity.h"
#include< cstdio>

void Entity::CalculateWorldMatrix()
{
	//get the world matrix and convert it to a matrix
	DirectX::XMMATRIX tempWorldMat = DirectX::XMLoadFloat4x4(&worldMat);

	// Apply Transformations
	DirectX::XMMATRIX tempScaleMat = DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scaleVec));
	DirectX::XMMATRIX tempRotationMat = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&rotationQuat));
	DirectX::XMMATRIX tempTranslationMat = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&positionVec));

	//Calculate World Matrix
	tempWorldMat = tempScaleMat * tempRotationMat * tempTranslationMat;

	//Store world matrix as float4x4
	XMStoreFloat4x4(&worldMat, XMMatrixTranspose(tempWorldMat));
}

Entity::Entity(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;
	worldMat = DirectX::XMFLOAT4X4(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);
	positionVec = DirectX::XMFLOAT3(0,0,0);
	scaleVec = DirectX::XMFLOAT3(1,1,1);
	rotationQuat = DirectX::XMFLOAT4(0,0,0,0);
	coll = 0;
}

Entity::~Entity()
{
	// Nothing to delete
	delete coll;
}

Mesh* Entity::GetMesh()
{
	return mesh;
}

void Entity::SetWorldMatrix(DirectX::XMFLOAT4X4 value)
{
	worldMat = value;
}

DirectX::XMFLOAT4X4 Entity::GetWorldMatrix()
{
	CalculateWorldMatrix();
	return worldMat;
}

void Entity::SetPosition(DirectX::XMFLOAT3 value)
{
	positionVec = value;
}

DirectX::XMFLOAT3 Entity::GetPosition()
{
	return positionVec;
}

void Entity::SetScale(DirectX::XMFLOAT3 value)
{
	scaleVec = value;
}

DirectX::XMFLOAT3 Entity::GetScale()
{
	return scaleVec;
}

void Entity::SetRotation(DirectX::XMFLOAT4 value)
{
	rotationQuat = value;
}

DirectX::XMFLOAT4 Entity::GetRotation()
{
	return rotationQuat;
}

Material* Entity::GetMaterial()
{
	return material;
}

void Entity::PrepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix)
{
	// Set vertex shader data for materials
	// Call GetWorldMatrix so that calculation is also performed
	material->GetVertexShader()->SetMatrix4x4("world", GetWorldMatrix());
	material->GetVertexShader()->SetMatrix4x4("view", viewMatrix);
	material->GetVertexShader()->SetMatrix4x4("projection", projectionMatrix);

	// Set pixel shader data for textures
	material->GetPixelShader()->SetShaderResourceView("diffuseTexture", material->GetTextureSRV());
	if (material->GetSpecularSRV()) {
		material->GetPixelShader()->SetShaderResourceView("specularTexture", material->GetSpecularSRV());
	}
	if (material->GetNormalMapSRV()) {
		material->GetPixelShader()->SetShaderResourceView("normalMap", material->GetNormalMapSRV());
	}

	material->GetPixelShader()->SetSamplerState("basicSampler", material->GetSamplerState());
}

void Entity::Translate(DirectX::XMVECTOR position)
{
	//create an XMVECTOR from float3 for math
	DirectX::XMVECTOR tempTranslationVector = DirectX::XMLoadFloat3(&positionVec);

	//do the math
	tempTranslationVector = DirectX::XMVectorAdd(tempTranslationVector, position);

	//convert the XMVECTOR then store it
	DirectX::XMStoreFloat3(&positionVec, tempTranslationVector);
}

void Entity::Scale(DirectX::XMVECTOR scale)
{
	//create an XMVECTOR from float3 for math
	DirectX::XMVECTOR tempScaleVector = DirectX::XMLoadFloat3(&scaleVec);

	//do the math
	tempScaleVector = DirectX::XMVectorMultiply(tempScaleVector, scale);

	//convert the XMVECTOR then store it
	DirectX::XMStoreFloat3(&scaleVec, tempScaleVector);
}

void Entity::Rotate(DirectX::XMVECTOR rotation)
{
	//create an XMVECTOR from float4 for math
	DirectX::XMVECTOR tempRotationQuat = DirectX::XMLoadFloat4(&rotationQuat);

	//do the math
	//tempRotationQuat = DirectX::XMQuaternionMultiply(tempRotationQuat, rotation);
	tempRotationQuat = DirectX::XMVectorAdd(tempRotationQuat, rotation);

	//convert the XMVECTOR then store it
	DirectX::XMStoreFloat4(&rotationQuat, tempRotationQuat);
}

void Entity::AttachCollider()
{
	Vertex* verts = mesh->GetVertsFromMesh();
	coll = new Collision(verts);
	coll->GenAABB(verts, mesh->GetIndexCount());
}

Collision* Entity::GetCollision()
{
	return coll;
}
