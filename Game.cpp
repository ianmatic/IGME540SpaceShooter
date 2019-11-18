#include "Game.h"
#include "Vertex.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

	vertexShaderNormalMap = 0;
	pixelShaderNormalMap = 0;

	vertexShaderSpecularMap = 0;
	pixelShaderSpecularMap = 0;

	prevMousePos = { 0,0 };

	sphereMesh = 0;
	enemyMesh = 0;
	playerMesh = 0;
	isAlive = true;

	fabricMaterial = 0;

	playerMaterial = 0;
	enemyMaterial = 0;

	enemyDiffuse1 = 0;
	enemyDiffuse2 = 0;
	enemySpec = 0;
	enemyNormal = 0;
	playerDiffuse = 0;
	playerSpec = 0;


	fabricTextureSRV = 0;
	samplerState = 0;

	light = DirectionalLight();
	redDirLight = DirectionalLight();
	greenPointLight = PointLight();
	whitePointLight = PointLight();

	camera = new Camera();
	camera->CalculateProjectionMatrix(width, height);
	score = 0;
	hiScore = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;

	delete vertexShaderNormalMap;
	delete pixelShaderNormalMap;

	delete vertexShaderSpecularMap;
	delete pixelShaderSpecularMap;


	for (int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}

	delete sphereMesh;
	delete playerMesh;
	delete enemyMesh;

	delete fabricMaterial;

	delete enemyMaterial;
	delete playerMaterial;
	
	score = 0;
	hiScore = 0;

	fabricTextureSRV->Release();

	enemyDiffuse1->Release();
	enemyDiffuse2->Release();
	enemySpec->Release();
	enemyNormal->Release();
	playerDiffuse->Release();
	playerSpec->Release();

	samplerState->Release();

	delete camera;

	// spritebatch stuff
	delete spriteBatch;
	delete spriteFont;

	// particle stuff
	particleTexture->Release();
	particleBlendState->Release();
	particleDepthState->Release();
	delete particleVS;
	delete particlePS;

	for (int i = 0; i < emitters.size(); i++) {
		delete emitters[i];
	}
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	spriteBatch = new SpriteBatch(context);
	spriteFont = new SpriteFont(device, L"Fonts/Arial.spritefont");

	// Blue light
	light.ambientColor = XMFLOAT4(0.0, 0.0, 0.2f, 0);
	light.diffuseColor = XMFLOAT4(0, 0.0, 1, 1);
	light.direction = XMFLOAT3(1, 0, 0);

	// Red Light
	redDirLight.ambientColor = XMFLOAT4(0.2f, 0, 0, 0);
	redDirLight.diffuseColor = XMFLOAT4(1, 0.0, 0, 1);
	redDirLight.direction = XMFLOAT3(-1, 0, 0);

	// Green Light
	greenPointLight.ambientColor = XMFLOAT4(0, 0.0f, 0, 1);
	greenPointLight.diffuseColor = XMFLOAT4(1, 0.6f, 0, 1);
	greenPointLight.position = XMFLOAT3(0, -3, 0);

	// White Light
	whitePointLight.ambientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	whitePointLight.diffuseColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1);
	whitePointLight.position = XMFLOAT3(-3, 1, 1);

	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);


	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Still respect pixel shader output alpha
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");
	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	vertexShaderNormalMap = new SimpleVertexShader(device, context);
	vertexShaderNormalMap->LoadShaderFile(L"VertexShaderNormalMap.cso");
	pixelShaderNormalMap = new SimplePixelShader(device, context);
	pixelShaderNormalMap->LoadShaderFile(L"PixelShaderNormalMap.cso");

	vertexShaderSpecularMap = new SimpleVertexShader(device, context);
	vertexShaderSpecularMap->LoadShaderFile(L"VertexShaderSpecularMap.cso");
	pixelShaderSpecularMap = new SimplePixelShader(device, context);
	pixelShaderSpecularMap->LoadShaderFile(L"PixelShaderSpecularMap.cso");

	particleVS = new SimpleVertexShader(device, context);
	particleVS->LoadShaderFile(L"ParticleVS.cso");

	particlePS = new SimplePixelShader(device, context);
	particlePS->LoadShaderFile(L"ParticlePS.cso");

	// Load the texture

	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/fabric.jpg",
		0,	// don't need reference to texture
		&fabricTextureSRV
	);


	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/ufo_diffuse.png",
		0,	// don't need reference to texture
		&enemyDiffuse1
	);
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/ufo_diffuse_glow.png",
		0,	// don't need reference to texture
		&enemyDiffuse2
	);
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/ufo_normal.png",
		0,	// don't need reference to texture
		&enemyNormal
	);
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/ufo_spec.png",
		0,	// don't need reference to texture
		&enemySpec
	);

	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/mat.png",
		0,	// don't need reference to texture
		&playerDiffuse
	);
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/int.png",
		0,	// don't need reference to texture
		&playerSpec
	);

	// Particle setup ====================
	CreateWICTextureFromFile(device, context, L"../../assets/textures/particle.jpg", 0, &particleTexture);

	// Create Sampler State
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap UVs outside 0-1 range in U direction
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap UVs outside 0-1 range in V direction
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap UVs outside 0-1 range in W direction
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC; // D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Trillinear filtering
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX; // Must be larger than 0 
	device->CreateSamplerState(&sampDesc, &samplerState);

	fabricMaterial = new Material(vertexShader, pixelShader, fabricTextureSRV, 0, 0, samplerState);

	enemyMaterial = new Material(vertexShaderSpecularMap, pixelShaderSpecularMap, enemyDiffuse1, enemySpec, 0, samplerState);
	playerMaterial = new Material(vertexShaderSpecularMap, pixelShaderSpecularMap, playerDiffuse, playerSpec, 0, samplerState);
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Not needed anymore (Keeping in case I'm dumb/wrong)
	// Camera calculates View and Projection matrices
	// Entities calculate world matrices
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex redVertices[] =
	{
		{ XMFLOAT3(-2.0f, +0.0f, +0.0f), XMFLOAT2(0,0),  XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(-1.0f, +2.0f, +0.0f), XMFLOAT2(0,0),  XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT2(0,0),  XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int redIndices[] = { 0, 1, 2 };


	Vertex greenVertices[] =
	{
		{ XMFLOAT3(+0.0f, +2.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+3.0f, +2.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+3.0f, +0.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+3.0f, -1.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
	};

	// make 3 triangles using the 5 vertices -> so 9 indices
	unsigned int greenIndices[] = {
		0, 1, 2,
		0, 2, 3,
		2, 4, 3 };



	Vertex blueVertices[] =
	{
		{ XMFLOAT3(-2.0f, -2.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(-2.0f, +0.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+0.0f, -2.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
	};

	unsigned int blueIndices[] = { 0, 1, 2, 0, 2, 3 };

	enemyMesh = new Mesh("../../assets/models/enemy.obj", device);
	sphereMesh = new Mesh("../../assets/models/sphere.obj", device);
	playerMesh = new Mesh("../../assets/models/f.obj", device);
	
	//Change models later

	player = new Entity(playerMesh, playerMaterial);
	player->AttachCollider();
	player->SetPosition(XMFLOAT3(0, 0, -1));
	//player->SetRotation(XMFLOAT4(0,-1.55,0,0));
	player->SetScale(XMFLOAT3(0.2, 0.2, 0.2));
	player->GetCollision()->SetPosition(player->GetPosition());
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	camera->CalculateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	entities.clear();

	// Update emitters
	for (int i = 0; i < emitters.size(); i++) {
		emitters[i]->Update(deltaTime);

		// remove expired emitters
		if (emitters[i]->GetTotalTime() < 0.0f) {
			delete emitters[i];
			emitters.erase(emitters.begin() + i);
			--i;
			continue;
		}
	}

	camera->Update(deltaTime);
	if (isAlive)
	{
		float playerSpeed = 5.0f;
		if (GetAsyncKeyState('A') & 0x8000) {
			player->SetPosition(XMFLOAT3(player->GetPosition().x - (playerSpeed * deltaTime), player->GetPosition().y, player->GetPosition().z));
			player->GetCollision()->SetPosition(player->GetPosition());
			if (player->GetPosition().x <= -8)
			{
				player->SetPosition(XMFLOAT3(-8, 0, player->GetPosition().z));
				player->GetCollision()->SetPosition(XMFLOAT3(-8, 0, player->GetPosition().z));
			}
		}
		else if (GetAsyncKeyState('D') & 0x8000) {
			player->SetPosition(XMFLOAT3(player->GetPosition().x + (playerSpeed * deltaTime), player->GetPosition().y, player->GetPosition().z));
			player->GetCollision()->SetPosition(player->GetPosition());

			if (player->GetPosition().x >= 8)
			{
				player->SetPosition(XMFLOAT3(8, 0, player->GetPosition().z));
				player->GetCollision()->SetPosition(player->GetPosition());
			}
		}


		if (GetAsyncKeyState('W') & 0x8000) {
			player->SetPosition(XMFLOAT3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z + (playerSpeed * deltaTime)));
			player->GetCollision()->SetPosition(player->GetPosition());

			if (player->GetPosition().z >= 8)
			{
				player->SetPosition(XMFLOAT3(player->GetPosition().x, 0, 8));
				player->GetCollision()->SetPosition(XMFLOAT3(player->GetPosition().x, 0, 8));
			}
		}
		else if (GetAsyncKeyState('S') & 0x8000) {
			player->SetPosition(XMFLOAT3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - (playerSpeed * deltaTime)));
			player->GetCollision()->SetPosition(player->GetPosition());

			if (player->GetPosition().z <= -2)
			{
				player->SetPosition(XMFLOAT3(player->GetPosition().x, 0, -2));
				player->GetCollision()->SetPosition(player->GetPosition());
			}
		}

		if (GetAsyncKeyState('P') & 0x43) {
			playerL = new Entity(sphereMesh, fabricMaterial);
			playerL->SetScale(XMFLOAT3(0.4, 0.4, 0.4));
			playerL->SetPosition(player->GetPosition());
			playerL->AttachCollider();
			playerL->GetCollision()->SetPosition(player->GetPosition());
			playerL->GetCollision()->SetScale(playerL->GetScale());
			lasers.push_back(playerL);
		}

		float laserSpeed = 7.5f;
		int i = 0;
		for (int i = 0; i < lasers.size(); i++)
		{
			lasers[i]->SetPosition(XMFLOAT3(lasers[i]->GetPosition().x, lasers[i]->GetPosition().y, lasers[i]->GetPosition().z + (laserSpeed * deltaTime)));
			lasers[i]->GetCollision()->SetPosition(lasers[i]->GetPosition());
			if (lasers[i]->GetPosition().z >= 30.0f && i < lasers.size())
			{
				delete lasers[i];
				lasers.erase(lasers.begin() + i);
				i--;
				continue;
			}
		}
		for (int i = 0; i < lasers.size(); i++) {
			bool markContinue = false;
			for (int j = 0; j < enemies.size(); j++)
			{
				// laser hits enemy
				if (enemies[j]->GetCollision()->CheckCollision(lasers[i]->GetCollision()) && i < lasers.size() && j < enemies.size())
				{
					// create particle effect
					emitters.push_back(new Emitter(
						110,							// Max particles
						20,								// Particles per second
						1,								// Particle lifetime
						1.25f,							// Start size
						0.75f,							// End size
						XMFLOAT4(1, 0.1f, 0.1f, 0.7f),	// Start color
						XMFLOAT4(1, 0.6f, 0.1f, 0.0f),		// End color
						XMFLOAT3(0, 0, 0),				// Start velocity
						XMFLOAT3(0.2f, 0.2f, 0.2f),		// Velocity randomness range
						XMFLOAT3(
							enemies[j]->GetPosition().x, 
							enemies[j]->GetPosition().y, 
							enemies[j]->GetPosition().z),				// Emitter position
						XMFLOAT3(0.1f, 0.1f, 0.1f),		// Position randomness range
						XMFLOAT4(-2, 2, -2, 2),			// Random rotation ranges (startMin, startMax, endMin, endMax)
						XMFLOAT3(0, 0, 0),				// Constant acceleration
						device,
						particleVS,
						particlePS,
						particleTexture));


					delete enemies[j];
					enemies.erase(enemies.begin() + j);
					--j;
					score+=10;
					if (score >= hiScore)
					{
						hiScore = score;
					}
					markContinue = true;
					continue;
				}
			}
			if (markContinue) {
				delete lasers[i];
				lasers.erase(lasers.begin() + i);
				--i;
				continue;
			}
		}
		for (int i = 0; i < lasers.size(); i++) {
			bool markContinue = false;
			for (int j = 0; j < enemies2.size(); j++)
			{
				if (enemies2[j]->GetCollision()->CheckCollision(lasers[i]->GetCollision()) && i < lasers.size() && j < enemies2.size())
				{
					// create particle effect
					emitters.push_back(new Emitter(
						110,							// Max particles
						20,								// Particles per second
						1,								// Particle lifetime
						1.25f,							// Start size
						0.75f,							// End size
						XMFLOAT4(1, 0.1f, 0.1f, 0.7f),	// Start color
						XMFLOAT4(1, 0.6f, 0.1f, 0.0f),		// End color
						XMFLOAT3(0, 0, 0),				// Start velocity
						XMFLOAT3(0.2f, 0.2f, 0.2f),		// Velocity randomness range
						XMFLOAT3(
							enemies[j]->GetPosition().x,
							enemies[j]->GetPosition().y,
							enemies[j]->GetPosition().z),				// Emitter position
						XMFLOAT3(0.1f, 0.1f, 0.1f),		// Position randomness range
						XMFLOAT4(-2, 2, -2, 2),			// Random rotation ranges (startMin, startMax, endMin, endMax)
						XMFLOAT3(0, 0, 0),				// Constant acceleration
						device,
						particleVS,
						particlePS,
						particleTexture));
					delete enemies2[j];
					enemies2.erase(enemies2.begin() + j);
					j--;
					score += 20;
					if (score >= hiScore)
					{
						hiScore = score;
					}
					markContinue = true;
					continue;
				}
			}

			if (markContinue) {
				delete lasers[i];
				lasers.erase(lasers.begin() + i);
				i--;
				continue;
			}
		}

		timer -= 1.0f * deltaTime;

		if (timer <= 0.0f)
		{

			enemy = new Entity(enemyMesh, enemyMaterial);
			enemy->SetPosition(XMFLOAT3(-20, 0, 10));
			enemy->SetScale(XMFLOAT3(0.02, 0.02, 0.02));
			enemy->AttachCollider();
			enemy->GetCollision()->SetPosition(enemy->GetPosition());
			enemy->GetCollision()->SetScale(enemy->GetScale());
			enemies.push_back(enemy);
			for (int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i]->GetPosition().x >= -7.0f || enemies[i]->GetPosition().x <= 7.0f)
				{
					enemyL = new Entity(sphereMesh, enemyMaterial);
					enemyL->SetScale(XMFLOAT3(0.5, 0.5, 0.5));
					enemyL->SetPosition(enemies[i]->GetPosition());
					enemyL->AttachCollider();
					enemyL->GetCollision()->SetPosition(enemies[i]->GetPosition());
					enemyL->GetCollision()->SetScale(enemies[i]->GetScale());
					enemyLasers.push_back(enemyL);
				}
			}

			timer = 3.0f;
		}


		timer2 -= 2.0f * deltaTime;

		if (timer2 <= 0.0f)
		{
			enemy = new Entity(enemyMesh, enemyMaterial);
			enemy->SetPosition(XMFLOAT3(20, 0, 15));
			enemy->SetScale(XMFLOAT3(0.02, 0.02, 0.02));
			enemy->AttachCollider();
			enemy->GetCollision()->SetPosition(enemy->GetPosition());
			enemy->GetCollision()->SetScale(enemy->GetScale());
			enemies2.push_back(enemy);
			for (int i = 0; i < enemies.size(); i++)
			{
				enemyL = new Entity(sphereMesh, enemyMaterial);
				enemyL->SetScale(XMFLOAT3(0.5, 0.5, 0.5));
				enemyL->SetPosition(enemies2[i]->GetPosition());
				enemyL->AttachCollider();
				enemyL->GetCollision()->SetPosition(enemies2[i]->GetPosition());
				enemyL->GetCollision()->SetScale(enemies2[i]->GetScale());
				enemyLasers.push_back(enemyL);
			}

			timer2 = 4.0f;
		}


		float enemySpeed = 3.0f;
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->SetPosition(XMFLOAT3(enemies[i]->GetPosition().x + (enemySpeed * 1.2f * deltaTime), enemies[i]->GetPosition().y, enemies[i]->GetPosition().z));
			enemies[i]->GetCollision()->SetPosition(enemies[i]->GetPosition());
			if (enemies[i]->GetPosition().x >= 30.0f && i < enemies.size())
			{
				delete enemies[i];
				enemies.erase(enemies.begin() + i);
				i--;
				continue;
			}
		}

		for (int i = 0; i < enemies2.size(); i++)
		{
			enemies2[i]->SetPosition(XMFLOAT3(enemies2[i]->GetPosition().x - (enemySpeed * deltaTime), enemies2[i]->GetPosition().y, enemies2[i]->GetPosition().z));
			enemies2[i]->GetCollision()->SetPosition(enemies2[i]->GetPosition());
			if (enemies2[i]->GetPosition().x <= -30.0f && i < enemies2.size())
			{
				delete enemies2[i];
				enemies2.erase(enemies2.begin() + i);
				i--;
				continue;
			}
		}

		for (int i = 0; i < enemyLasers.size(); i++)
		{
			enemyLasers[i]->SetPosition(XMFLOAT3(enemyLasers[i]->GetPosition().x, enemyLasers[i]->GetPosition().y, enemyLasers[i]->GetPosition().z - (enemySpeed * deltaTime)));
			enemyLasers[i]->GetCollision()->SetPosition(enemyLasers[i]->GetPosition());
			if (enemyLasers[i]->GetPosition().z <= -3.0f && i < enemyLasers.size())
			{
				delete enemyLasers[i];
				enemyLasers.erase(enemyLasers.begin() + i);
				i--;
				continue;
			}
		}
		for (int i = 0; i < enemyLasers.size(); i++)
		{
			if (player->GetCollision()->CheckCollision(enemyLasers[i]->GetCollision()) && i < enemyLasers.size())
			{
				delete enemyLasers[i];
				delete player;
				enemyLasers.erase(enemyLasers.begin() + i);
				i = enemyLasers.size();
				isAlive = false;
				break;
			}
		}

		greenPointLight.position.x += cos(totalTime) * deltaTime;


		

		// add all entities to entities for drawing
		entities.push_back(player);
		entities.insert(entities.end(), enemies.begin(), enemies.end());
		entities.insert(entities.end(), enemies2.begin(), enemies2.end());
		entities.insert(entities.end(), lasers.begin(), lasers.end());
		entities.insert(entities.end(), enemyLasers.begin(), enemyLasers.end());
	}
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
	if (!isAlive)
	{
		entities.clear();
		Quit();
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	if (isAlive)
	{
		// Background color (Cornflower Blue in this case) for clearing
		const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

		// Clear the render target and depth buffer (erases what's on the screen)
		//  - Do this ONCE PER FRAME
		//  - At the beginning of Draw (before drawing *anything*)
		context->ClearRenderTargetView(backBufferRTV, color);
		context->ClearDepthStencilView(
			depthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0);

		// Send data to shader variables
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.
		for (int i = 0; i < entities.size(); i++) {

			// Setup Vertex and Pixel Shaders
			entities[i]->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix());

			// Once you've set all of the data you care to change for
			// the next draw call, you need to actually send it to the GPU
			//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
			entities[i]->GetMaterial()->GetVertexShader()->CopyAllBufferData();


			// Set the vertex and pixel shaders to use for the next Draw() command
			//  - These don't technically need to be set every frame...YET
			//  - Once you start applying different shaders to different objects,
			//    you'll need to swap the current shaders before each draw
			entities[i]->GetMaterial()->GetVertexShader()->SetShader();

			// Send data to pixel shader
			entities[i]->GetMaterial()->GetPixelShader()->SetData("light", //name of variable in shader
				&light, sizeof(DirectionalLight));
			entities[i]->GetMaterial()->GetPixelShader()->SetData("secondLight", //name of variable in shader
				&redDirLight, sizeof(DirectionalLight));
			entities[i]->GetMaterial()->GetPixelShader()->SetData("pointLight", //name of variable in shader
				&greenPointLight, sizeof(PointLight));
			entities[i]->GetMaterial()->GetPixelShader()->SetData("secondPointLight", //name of variable in shader
				&whitePointLight, sizeof(PointLight));
			entities[i]->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", //name of variable in shader
				camera->GetPosition());

			entities[i]->GetMaterial()->GetPixelShader()->CopyAllBufferData();
			entities[i]->GetMaterial()->GetPixelShader()->SetShader();

			// Set buffers in the input assembler
			//  - Do this ONCE PER OBJECT you're drawing, since each object might
			//    have different geometry.
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			ID3D11Buffer* vertexBuffer = entities[i]->GetMesh()->GetVertexBuffer();
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(entities[i]->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(entities[i]->GetMesh()->GetIndexCount(), 0, 0);
		}


		//// Particle drawing =============
		{

			// Particle states
			float blend[4] = { 1,1,1,1 };
			context->OMSetBlendState(particleBlendState, blend, 0xffffffff);	// Additive blending
			context->OMSetDepthStencilState(particleDepthState, 0);				// No depth WRITING

			// No wireframe debug
			particlePS->SetInt("debugWireframe", 0);
			particlePS->CopyAllBufferData();

			// Draw the emitters
			for (int i = 0; i < emitters.size(); i++) {
				emitters[i]->Draw(context, camera);
			}

			// Reset to default states for next frame
			context->OMSetBlendState(0, blend, 0xffffffff);
			context->OMSetDepthStencilState(0, 0);
			context->RSSetState(0);

		}





		// Draw some arbitrary text
		RECT imageRect = { 10, 10, 110, 110 };
		RECT normalMapRect = { 120, 10, 220, 110 };
		RECT fontSheetRect = { 230, 10, 320, 110 };

		ID3D11ShaderResourceView* fontSheet;
		spriteFont->GetSpriteSheet(&fontSheet);

		// We'll use a sprite batch to draw some 2D images
		spriteBatch->Begin();

		// Draw some arbitrary text
		spriteFont->DrawString(
			spriteBatch,
			L"WASD - Move\n P - Shoot",
			XMFLOAT2(10, 600));

		std::wstring dynamicText = L"Score: " + std::to_wstring(score) + L"\nHigh Score:" + std::to_wstring(hiScore);
		spriteFont->DrawString(
			spriteBatch,
			dynamicText.c_str(),
			XMFLOAT2(10, 40));

		spriteBatch->End();

		fontSheet->Release();

		// Reset any states that may be changed by sprite batch!
		float blendFactor[4] = { 1,1,1,1 };
		context->OMSetBlendState(0, blendFactor, 0xFFFFFFFF);
		context->RSSetState(0);
		context->OMSetDepthStencilState(0, 0);

		// Present the back buffer to the user
		//  - Puts the final frame we're drawing into the window so the user can see it
		//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
		swapChain->Present(0, 0);

		// Due to the usage of a more sophisticated swap chain effect,
		// the render target must be re-bound after every call to Present()
		context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	}
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// right mouse down
	// uncomment for camera angle testing
	//if (buttonState & 0x0002) {
	//	float xDiff = (float)prevMousePos.x - x;
	//	float yDiff = (float)prevMousePos.y - y;
	//	camera->Rotate(xDiff / 500, yDiff / 500);
	//}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion