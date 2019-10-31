#pragma once
#include "Vertex.h"
#include <climits>
class Collision
{
	//store the vertices locally
	Vertex* vertices;

	DirectX::XMFLOAT3 minCoord;
	DirectX::XMFLOAT3 maxCoord;
public:
	//Collision constructors -- you can either pass in the entire vertex array from the model, or pass in the mesh (and it will grab the vertices from there)
	Collision(Vertex* vertices);
	//check for collisions using this collider's AABB and another collider's AABB
	bool CheckCollision(Collision* other);

	//generates an AABB collision box based on the vertices given
	void GenAABB(Vertex* vertices, int size);

	//helper methods to get the coordinates of a min/max in space.

	DirectX::XMFLOAT3 GetMinCoord();
	DirectX::XMFLOAT3 GetMaxCoord();

	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetScale(DirectX::XMFLOAT3 scale);
};

