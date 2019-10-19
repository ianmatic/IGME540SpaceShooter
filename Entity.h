#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Lights.h"
#include <DirectXMath.h>
class Entity
{
	DirectX::XMFLOAT4X4 worldMat;
	DirectX::XMFLOAT3 positionVec;
	DirectX::XMFLOAT3 scaleVec;
	DirectX::XMFLOAT4 rotationQuat;
	Mesh* mesh;
	Material* material;

	void CalculateWorldMatrix();
public:
	Entity(Mesh* mesh, Material* material);
	~Entity();

	Mesh* GetMesh();

	// Setters/Getter
	void SetWorldMatrix(DirectX::XMFLOAT4X4 value);
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	void SetPosition(DirectX::XMFLOAT3 value);
	DirectX::XMFLOAT3 GetPosition();

	void SetScale(DirectX::XMFLOAT3 value);
	DirectX::XMFLOAT3 GetScale();

	void SetRotation(DirectX::XMFLOAT4 value);
	DirectX::XMFLOAT4 GetRotation();

	Material* GetMaterial();

	// Shading
	void PrepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

	// Transformations
	void Translate(DirectX::XMVECTOR position);
	void Scale(DirectX::XMVECTOR scale);
	void Rotate(DirectX::XMVECTOR rotation);
};

