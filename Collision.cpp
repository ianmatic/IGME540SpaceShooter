#include "Collision.h"
Collision::Collision(std::vector<Vertex> vertices)
{
	minCoord = DirectX::XMFLOAT3();
	maxCoord = DirectX::XMFLOAT3();
	this->vertices = vertices;
}

void Collision::SetPosition(DirectX::XMFLOAT3 pos)
{
	float halfWidthX = (maxCoord.x - minCoord.x) / 2;
	float halfWidthY = (maxCoord.y - minCoord.y) / 2;
	float halfWidthZ = (maxCoord.z - minCoord.z) / 2;

	minCoord.x = pos.x - halfWidthX;
	minCoord.y = pos.y - halfWidthY;
	minCoord.z = pos.z - halfWidthZ;
	maxCoord.x = pos.x + halfWidthX;
	maxCoord.y = pos.y + halfWidthY;
	maxCoord.z = pos.z + halfWidthZ;
}

//will be done for next milestone
void Collision::SetScale(DirectX::XMFLOAT3 scale)
{
	float halfWidthX = (maxCoord.x - minCoord.x) / 2;
	float halfWidthY = (maxCoord.y - minCoord.y) / 2;
	float halfWidthZ = (maxCoord.z - minCoord.z) / 2;

	DirectX::XMFLOAT3 center = {maxCoord.x - halfWidthX, maxCoord.y - halfWidthY, maxCoord.z = halfWidthZ};

	halfWidthX *= scale.x;
	halfWidthY *= scale.y;
	halfWidthZ *= scale.z;

	maxCoord = { center.x + halfWidthX, center.y + halfWidthY, center.z + halfWidthZ };
	minCoord = { center.x - halfWidthX, center.y - halfWidthY, center.z - halfWidthZ };
	//minCoord.x 
	
}

void Collision::GenAABB(std::vector<Vertex> vertices)
{
	DirectX::XMFLOAT3 minX = { (float)INT_MAX, 0, 0 };
	DirectX::XMFLOAT3 minY = { 0, (float)INT_MAX, 0 };
	DirectX::XMFLOAT3 minZ = { 0, 0, (float)INT_MAX };

	DirectX::XMFLOAT3 maxX = { (float)INT_MIN, 0, 0 };
	DirectX::XMFLOAT3 maxY = { 0, (float)INT_MIN, 0 };
	DirectX::XMFLOAT3 maxZ = { 0, 0, (float)INT_MIN };

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].Position.x < minX.x)
		{
			minX = vertices[i].Position;
		}
		if (vertices[i].Position.y < minY.y)
		{
			minY = vertices[i].Position;
		}
		if (vertices[i].Position.z < minZ.z)
		{
			minZ = vertices[i].Position;
		}

		if (vertices[i].Position.x > maxX.x)
		{
			maxX = vertices[i].Position;
		}
		if (vertices[i].Position.y > maxY.y)
		{
			maxY = vertices[i].Position;
		}
		if (vertices[i].Position.z > maxZ.z)
		{
			maxZ = vertices[i].Position;
		}
	}

	//this gives us the bottom left and top right corners.
	minCoord = { minX.x, minY.y, minZ.z };
	maxCoord = { maxX.x, maxY.y, maxZ.z };
}

bool Collision::CheckCollision(Collision* other)
{
	DirectX::XMFLOAT3 otherMin = other->GetMinCoord();
	DirectX::XMFLOAT3 otherMax = other->GetMaxCoord();

	if (otherMin.x > this->minCoord.x && otherMin.x < this->maxCoord.x)
	{
		if (otherMin.z > this->minCoord.z && otherMin.z < this->maxCoord.z)
		{
			return true;
		}
	}

	if (otherMax.x > this->minCoord.x && otherMax.x < this->maxCoord.x)
	{
		if (otherMax.z > this->minCoord.z && otherMax.z < this->maxCoord.z)
		{
			return true;
		}
	}
	return false;
}

DirectX::XMFLOAT3 Collision::GetMinCoord()
{
	return this->minCoord;
}

DirectX::XMFLOAT3 Collision::GetMaxCoord()
{
	return this->maxCoord;
}