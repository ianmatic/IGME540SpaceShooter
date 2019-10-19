#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include "DXCore.h"
class Camera
{
	DirectX::XMFLOAT4X4 projectionMat;
	DirectX::XMFLOAT4X4 viewMat;
	DirectX::XMFLOAT4 rotationQuat;

	DirectX::XMFLOAT3 cameraPos;
	DirectX::XMFLOAT3 cameraDir;
	float xRot;
	float yRot;

	void HandleInput(float deltaTime, DirectX::XMFLOAT4 rotationQuat);
public:
	Camera();
	~Camera();

	// Setters/Getters
	void SetProjectionMatrix(DirectX::XMFLOAT4X4 value);
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void SetViewMatrix(DirectX::XMFLOAT4X4 value);
	DirectX::XMFLOAT4X4 GetViewMatrix();

	DirectX::XMFLOAT3 GetPosition();

	// Misc. Camera Logic Methods
	void Update(float deltaTime);

	void Rotate(float x, float y);

	void CalculateProjectionMatrix(unsigned int width, unsigned int height);
};

