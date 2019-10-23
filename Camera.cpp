#include "Camera.h"

void Camera::SetProjectionMatrix(DirectX::XMFLOAT4X4 value)
{
	projectionMat = value;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMat;
}

void Camera::SetViewMatrix(DirectX::XMFLOAT4X4 value)
{
	viewMat = value;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMat;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return cameraPos;
}

Camera::Camera()
{
	rotationQuat = DirectX::XMFLOAT4(0, 0, 0, 0);
	cameraPos = DirectX::XMFLOAT3(0, 0, -5);
	cameraDir = DirectX::XMFLOAT3(0, 0, 1);
	xRot = 0;
	yRot = 0;
}

Camera::~Camera()
{
	// nothing to delete
}

void Camera::Update(float deltaTime)
{

	// calculate rotation and store it
	DirectX::XMStoreFloat4(&rotationQuat, DirectX::XMQuaternionRotationRollPitchYaw(xRot, yRot, 0));

	// calculate direction and store it
	DirectX::XMStoreFloat3(&cameraDir,
		DirectX::XMVectorAdd(
			DirectX::XMLoadFloat3(&cameraPos),
			DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1, 0), DirectX::XMLoadFloat4(&rotationQuat))));

	// handle keyboard input
	HandleInput(deltaTime, rotationQuat);

	// calculate view matrix and store it
	DirectX::XMStoreFloat4x4(&viewMat,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&cameraPos), 
		DirectX::XMLoadFloat3(&cameraDir), DirectX::XMVectorSet(0, 1, 0, 0))
		));
}

void Camera::Rotate(float x, float y)
{
	xRot += y;
	yRot += x;
}

void Camera::CalculateProjectionMatrix(unsigned int width, unsigned int height)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMat, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::HandleInput(float deltaTime, DirectX::XMFLOAT4 rotationQuat)
{
	// get vectors for calculations
	DirectX::XMVECTOR tempCameraPos = DirectX::XMLoadFloat3(&cameraPos);
	DirectX::XMVECTOR tempRotationQuat = DirectX::XMLoadFloat4(&rotationQuat);
	bool dirty = false;

	float speed = 1 * deltaTime;

	if (GetAsyncKeyState(VK_SHIFT)) {
		speed *= 3;
	}
	if (GetAsyncKeyState(VK_CONTROL)) {
		speed *= 0.5;
	}

	// Relative Movement
	// Use scale for vector and scalar multiplication
	/*if (GetAsyncKeyState('W') && 0x8000) {
		tempCameraPos = DirectX::XMVectorAdd(tempCameraPos, 
			DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, speed, 0), tempRotationQuat));
		dirty = true;
	}
	if (GetAsyncKeyState('S') && 0x8000) {
		tempCameraPos = DirectX::XMVectorAdd(tempCameraPos, 
			DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, -speed, 0), tempRotationQuat));
		dirty = true;
	}
	if (GetAsyncKeyState('A') && 0x8000) {
		tempCameraPos = DirectX::XMVectorAdd(tempCameraPos, 
			DirectX::XMVector3Rotate(DirectX::XMVectorSet(-speed, 0, 0, 0), tempRotationQuat));
		dirty = true;
	}
	if (GetAsyncKeyState('D') && 0x8000) {
		tempCameraPos = DirectX::XMVectorAdd(tempCameraPos, 
			DirectX::XMVector3Rotate(DirectX::XMVectorSet(speed, 0, 0, 0), tempRotationQuat));
		dirty = true;
	}*/

	// Apply relative movement
	if (dirty) {
		DirectX::XMStoreFloat3(&cameraPos, tempCameraPos);
	}

	// Absolute movement
	if (GetAsyncKeyState(VK_SPACE) && 0x8000) {
		cameraPos.y += 1 * deltaTime;
	}
	if (GetAsyncKeyState('X') && 0x8000) {
		cameraPos.y -= 1 * deltaTime;
	}
}
