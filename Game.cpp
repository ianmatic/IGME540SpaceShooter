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

	redMesh = 0;
	blueMesh = 0;
	greenMesh = 0;
	sphereMesh = 0;
	coneMesh = 0;
	torusMesh = 0;
	cubeMesh = 0;

	giraffeMaterial = 0;
	fabricMaterial = 0;
	rustMaterial = 0;
	rockMaterial = 0;

	giraffeTextureSRV = 0;
	rustTextureSRV = 0;
	rustSpecularSRV = 0;
	rockTextureSRV = 0;
	rockNormalMapSRV = 0;
	fabricTextureSRV = 0;
	samplerState = 0;

	light = DirectionalLight();
	redDirLight = DirectionalLight();
	greenPointLight = PointLight();
	whitePointLight = PointLight();

	camera = new Camera();
	camera->CalculateProjectionMatrix(width, height);

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

	delete redMesh;
	delete greenMesh;
	delete blueMesh;
	delete sphereMesh;
	delete coneMesh;
	delete cubeMesh;
	delete torusMesh;

	delete giraffeMaterial;
	delete fabricMaterial;
	delete rustMaterial;
	delete rockMaterial;

	giraffeTextureSRV->Release();
	fabricTextureSRV->Release();
	rustTextureSRV->Release();
	rustSpecularSRV->Release();
	rockTextureSRV->Release();
	rockNormalMapSRV->Release();
	samplerState->Release();

	delete camera;
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

	// Load the texture
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/giraffe.jpg",
		0,	// don't need reference to texture
		&giraffeTextureSRV
	);

	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/fabric.jpg",
		0,	// don't need reference to texture
		&fabricTextureSRV
	);

	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/rust.png",
		0,	// don't need reference to texture
		&rustTextureSRV
	);
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/rustSpecular.png",
		0,	// don't need reference to texture
		&rustSpecularSRV
	);

	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/rock.jpg",
		0,	// don't need reference to texture
		&rockTextureSRV
	);
	CreateWICTextureFromFile(device,
		context,
		L"../../assets/textures/rockNormals.jpg",
		0,	// don't need reference to texture
		&rockNormalMapSRV
	);

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

	giraffeMaterial = new Material(vertexShader, pixelShader, giraffeTextureSRV, 0, 0, samplerState);
	fabricMaterial = new Material(vertexShader, pixelShader, fabricTextureSRV, 0, 0, samplerState);
	// specular
	rustMaterial = new Material(vertexShaderSpecularMap, pixelShaderSpecularMap, rustTextureSRV, rustSpecularSRV, 0, samplerState);
	// normalMap
	rockMaterial = new Material(vertexShaderNormalMap, pixelShaderNormalMap, rockTextureSRV, 0, rockNormalMapSRV, samplerState);
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

	redMesh = new Mesh(redVertices, 3, redIndices, device);


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

	greenMesh = new Mesh(greenVertices, 9, greenIndices, device);


	Vertex blueVertices[] =
	{
		{ XMFLOAT3(-2.0f, -2.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(-2.0f, +0.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
		{ XMFLOAT3(+0.0f, -2.0f, +0.0f), XMFLOAT2(0,0), XMFLOAT3(0,0,-1), XMFLOAT3(0,0,0) },
	};

	unsigned int blueIndices[] = { 0, 1, 2, 0, 2, 3 };

	blueMesh = new Mesh(blueVertices, 6, blueIndices, device);

	//Load in from files
	coneMesh = new Mesh("../../assets/models/cone.obj", device);
	torusMesh = new Mesh("../../assets/models/torus.obj", device);
	sphereMesh = new Mesh("../../assets/models/sphere.obj", device);
	cubeMesh = new Mesh("../../assets/models/cube.obj", device);

	entities.push_back(new Entity(sphereMesh, fabricMaterial));
	entities.push_back(new Entity(coneMesh, giraffeMaterial));
	entities.push_back(new Entity(torusMesh, rockMaterial));
	entities.push_back(new Entity(cubeMesh, rustMaterial));

	player = new Entity(cubeMesh, fabricMaterial);
	enemy = new Entity(torusMesh, rustMaterial);
	playerL = new Entity(sphereMesh, fabricMaterial);
	enemyL = new Entity(sphereMesh, rustMaterial);

	player->SetPosition(XMFLOAT3(0, 0, 5));
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
	camera->Update(deltaTime);

	for (int i = 0; i < entities.size(); i++) {

		//do various transformations on entities
		switch (i)
		{
		case 0:
			//entities[i]->Translate(XMVectorSet(cos(totalTime) / 2, 0, 0, 0) * deltaTime * 2);
			break;
		case 1:
			entities[i]->SetPosition(XMFLOAT3(-2.5, 1.5, 0));
			entities[i]->Rotate(XMVectorSet(0, 0, 1, 0) * deltaTime);
			break;
		case 2:
			entities[i]->Rotate(XMVectorSet(1, 0, 0, 0) * deltaTime);
			entities[i]->SetPosition(XMFLOAT3(3, 0, 0));
			break;
		case 3:
			entities[i]->SetPosition(XMFLOAT3(0, 0, -2));
			entities[i]->Rotate(XMVectorSet(0, 0.5f, 0, 0) * deltaTime);
		default:
			break;
		}
	}

	if (GetAsyncKeyState('A') & 0x8000) {
		player->SetPosition(XMFLOAT3(player->GetPosition().x - 0.1f, player->GetPosition().y, player->GetPosition().z));
		if (player->GetPosition().x <= -10)
		{
			player->SetPosition(XMFLOAT3(-10, 0, player->GetPosition().z));
		}
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		player->SetPosition(XMFLOAT3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z + 0.1f));
		if (player->GetPosition().z >= 10)
		{
			player->SetPosition(XMFLOAT3(player->GetPosition().x, 0, 10));
		}
	}

	else if (GetAsyncKeyState('D') & 0x8000) {
		player->SetPosition(XMFLOAT3(player->GetPosition().x + 0.1f, player->GetPosition().y, player->GetPosition().z));
		if (player->GetPosition().x >= 10)
		{
			player->SetPosition(XMFLOAT3(10, 0, player->GetPosition().z));
		}
	}
	else if (GetAsyncKeyState('W') & 0x8000) {
		player->SetPosition(XMFLOAT3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 0.1f));
		if (player->GetPosition().z <= -10)
		{
			player->SetPosition(XMFLOAT3(player->GetPosition().x, 0, -10));
		}
	}

	playerL->SetPosition(player->GetPosition());

	if (GetAsyncKeyState('P') & 0x8000) {
		lasers.push_back(playerL);
	}

	for (int i = 0; i < lasers.size(); i++)
	{
		lasers[i]->SetPosition(XMFLOAT3(lasers[i]->GetPosition().x, lasers[i]->GetPosition().y, lasers[i]->GetPosition().z - 0.1f));
		if (lasers[i]->GetPosition().z <= -50.0f && i < lasers.size())
		{

			// reduce size of array and move all 
			// elements on space ahead 
			int n = lasers.size() - 1;
			for (int j = i; j < n; j++)
				lasers[j] = lasers[j + 1];
		}
	}

	timer -= 0.1f;
	if (timer <= 0)
	{
		timer = 10.0f;
		enemies.push_back(enemy);
		for (int i = 0; i < enemies.size(); i++)
		{
			enemyL->SetPosition(enemies[i]->GetPosition());
			enemyLasers.push_back(enemyL);
		}

	}

	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->SetPosition(XMFLOAT3(enemies[i]->GetPosition().x + 0.1f, enemies[i]->GetPosition().y, enemies[i]->GetPosition().z));
		if (enemies[i]->GetPosition().x >= 50.0f && i < enemies.size())
		{

			// reduce size of array and move all 
			// elements on space ahead 
			int n = enemies.size() - 1;
			for (int j = i; j < n; j++)
				enemies[j] = enemies[j + 1];
		}
	}

	for (int i = 0; i < enemyLasers.size(); i++)
	{
		enemyLasers[i]->SetPosition(XMFLOAT3(enemyLasers[i]->GetPosition().x, enemyLasers[i]->GetPosition().y, enemyLasers[i]->GetPosition().z + 0.1f));
		if (enemyLasers[i]->GetPosition().z >= 50.0f && i < enemyLasers.size())
		{

			// reduce size of array and move all 
			// elements on space ahead 
			int n = enemyLasers.size() - 1;
			for (int j = i; j < n; j++)
				enemyLasers[j] = enemyLasers[j + 1];
		}
	}

	greenPointLight.position.x += cos(totalTime) * deltaTime;


	// Quit if the escape key is pressed
	//if (GetAsyncKeyState(VK_ESCAPE))
	//	Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
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

		player->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix());
		player->GetMaterial()->GetVertexShader()->CopyAllBufferData();


		// Set the vertex and pixel shaders to use for the next Draw() command
		//  - These don't technically need to be set every frame...YET
		//  - Once you start applying different shaders to different objects,
		//    you'll need to swap the current shaders before each draw
		player->GetMaterial()->GetVertexShader()->SetShader();

		// Send data to pixel shader
		player->GetMaterial()->GetPixelShader()->SetData("light", //name of variable in shader
			&light, sizeof(DirectionalLight));
		player->GetMaterial()->GetPixelShader()->SetData("secondLight", //name of variable in shader
			&redDirLight, sizeof(DirectionalLight));
		player->GetMaterial()->GetPixelShader()->SetData("pointLight", //name of variable in shader
			&greenPointLight, sizeof(PointLight));
		player->GetMaterial()->GetPixelShader()->SetData("secondPointLight", //name of variable in shader
			&whitePointLight, sizeof(PointLight));
		player->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", //name of variable in shader
			camera->GetPosition());

		player->GetMaterial()->GetPixelShader()->CopyAllBufferData();
		player->GetMaterial()->GetPixelShader()->SetShader();

		for (int i = 0; i < lasers.size(); i++)
		{
			lasers[i]->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix());
			lasers[i]->GetMaterial()->GetVertexShader()->CopyAllBufferData();


			// Set the vertex and pixel shaders to use for the next Draw() command
			//  - These don't technically need to be set every frame...YET
			//  - Once you start applying different shaders to different objects,
			//    you'll need to swap the current shaders before each draw
			lasers[i]->GetMaterial()->GetVertexShader()->SetShader();

			// Send data to pixel shader
			lasers[i]->GetMaterial()->GetPixelShader()->SetData("light", //name of variable in shader
				&light, sizeof(DirectionalLight));
			lasers[i]->GetMaterial()->GetPixelShader()->SetData("secondLight", //name of variable in shader
				&redDirLight, sizeof(DirectionalLight));
			lasers[i]->GetMaterial()->GetPixelShader()->SetData("pointLight", //name of variable in shader
				&greenPointLight, sizeof(PointLight));
			lasers[i]->GetMaterial()->GetPixelShader()->SetData("secondPointLight", //name of variable in shader
				&whitePointLight, sizeof(PointLight));
			lasers[i]->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", //name of variable in shader
				camera->GetPosition());

			lasers[i]->GetMaterial()->GetPixelShader()->CopyAllBufferData();
			lasers[i]->GetMaterial()->GetPixelShader()->SetShader();
		}

		for (int i = 0; i < enemyLasers.size(); i++)
		{
			enemyLasers[i]->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix());
			enemyLasers[i]->GetMaterial()->GetVertexShader()->CopyAllBufferData();


			// Set the vertex and pixel shaders to use for the next Draw() command
			//  - These don't technically need to be set every frame...YET
			//  - Once you start applying different shaders to different objects,
			//    you'll need to swap the current shaders before each draw
			enemyLasers[i]->GetMaterial()->GetVertexShader()->SetShader();

			// Send data to pixel shader
			enemyLasers[i]->GetMaterial()->GetPixelShader()->SetData("light", //name of variable in shader
				&light, sizeof(DirectionalLight));
			enemyLasers[i]->GetMaterial()->GetPixelShader()->SetData("secondLight", //name of variable in shader
				&redDirLight, sizeof(DirectionalLight));
			enemyLasers[i]->GetMaterial()->GetPixelShader()->SetData("pointLight", //name of variable in shader
				&greenPointLight, sizeof(PointLight));
			enemyLasers[i]->GetMaterial()->GetPixelShader()->SetData("secondPointLight", //name of variable in shader
				&whitePointLight, sizeof(PointLight));
			enemyLasers[i]->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", //name of variable in shader
				camera->GetPosition());

			enemyLasers[i]->GetMaterial()->GetPixelShader()->CopyAllBufferData();
			enemyLasers[i]->GetMaterial()->GetPixelShader()->SetShader();
		}

		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix());
			enemies[i]->GetMaterial()->GetVertexShader()->CopyAllBufferData();


			// Set the vertex and pixel shaders to use for the next Draw() command
			//  - These don't technically need to be set every frame...YET
			//  - Once you start applying different shaders to different objects,
			//    you'll need to swap the current shaders before each draw
			enemies[i]->GetMaterial()->GetVertexShader()->SetShader();

			// Send data to pixel shader
			enemies[i]->GetMaterial()->GetPixelShader()->SetData("light", //name of variable in shader
				&light, sizeof(DirectionalLight));
			enemies[i]->GetMaterial()->GetPixelShader()->SetData("secondLight", //name of variable in shader
				&redDirLight, sizeof(DirectionalLight));
			enemies[i]->GetMaterial()->GetPixelShader()->SetData("pointLight", //name of variable in shader
				&greenPointLight, sizeof(PointLight));
			enemies[i]->GetMaterial()->GetPixelShader()->SetData("secondPointLight", //name of variable in shader
				&whitePointLight, sizeof(PointLight));
			enemies[i]->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", //name of variable in shader
				camera->GetPosition());

			enemies[i]->GetMaterial()->GetPixelShader()->CopyAllBufferData();
			enemies[i]->GetMaterial()->GetPixelShader()->SetShader();
		}

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(3, 0, 0);

	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain effect,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
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
	if (buttonState & 0x0002) {
		float xDiff = (float)prevMousePos.x - x;
		float yDiff = (float)prevMousePos.y - y;
		camera->Rotate(xDiff / 500, yDiff / 500);
	}

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