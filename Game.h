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
	Mesh* enemyMesh;
	Mesh* playerMesh;

	Entity* player;
	Entity* enemy;
	Entity* playerL;
	Entity* enemyL;

	std::vector<Entity*> entities;
	std::vector<Entity*> enemies;
	std::vector<Entity*> enemies2;
	std::vector<Entity*> lasers;
	std::vector<Entity*> enemyLasers;

	float timer = 3.0f;
	float timer2 = 4.0f;

	Camera* camera;

	Material* giraffeMaterial;
	Material* fabricMaterial;
	Material* rustMaterial;
	Material* rockMaterial;

	Material* enemyMaterial;
	Material* playerMaterial;

	ID3D11ShaderResourceView* giraffeTextureSRV;
	ID3D11ShaderResourceView* fabricTextureSRV;
	ID3D11ShaderResourceView* rustTextureSRV;
	ID3D11ShaderResourceView* rustSpecularSRV;
	ID3D11ShaderResourceView* rockTextureSRV;
	ID3D11ShaderResourceView* rockNormalMapSRV;

	ID3D11ShaderResourceView* enemyDiffuse1;
	ID3D11ShaderResourceView* enemyDiffuse2;
	ID3D11ShaderResourceView* enemySpec;
	ID3D11ShaderResourceView* enemyNormal;
	ID3D11ShaderResourceView* playerDiffuse;
	ID3D11ShaderResourceView* playerSpec;
	ID3D11ShaderResourceView* playerNormal;


	ID3D11SamplerState* samplerState;

	DirectionalLight light;
	DirectionalLight redDirLight;
	PointLight greenPointLight;
	PointLight whitePointLight;

	bool isAlive;
	int score = 0;
	int hiScore = 0;
};

