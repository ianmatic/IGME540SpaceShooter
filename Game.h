#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "WICTextureLoader.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	SimpleVertexShader* vertexShaderNormalMap;
	SimplePixelShader* pixelShaderNormalMap;

	SimpleVertexShader* vertexShaderSpecularMap;
	SimplePixelShader* pixelShaderSpecularMap;

	// Matrices handled by Camera and Entities

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	Mesh* redMesh;
	Mesh* greenMesh;
	Mesh* blueMesh;
	Mesh* sphereMesh;
	Mesh* coneMesh;
	Mesh* torusMesh;
	Mesh* cubeMesh;

	std::vector<Entity*> entities;

	Camera* camera;

	Material* giraffeMaterial;
	Material* fabricMaterial;
	Material* rustMaterial;
	Material* rockMaterial;
	ID3D11ShaderResourceView* giraffeTextureSRV;
	ID3D11ShaderResourceView* fabricTextureSRV;
	ID3D11ShaderResourceView* rustTextureSRV;
	ID3D11ShaderResourceView* rustSpecularSRV;
	ID3D11ShaderResourceView* rockTextureSRV;
	ID3D11ShaderResourceView* rockNormalMapSRV;
	ID3D11SamplerState* samplerState;

	DirectionalLight light;
	DirectionalLight redDirLight;
	PointLight greenPointLight;
	PointLight whitePointLight;
};

