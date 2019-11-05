#pragma once
#include "d3d11.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include <vector>
#include <fstream>

class Mesh
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	std::vector<Vertex> vertsFromMesh;
	int indexCount;

	std::string inputfile;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;



	void Init(Vertex* vertices, int numVertices, unsigned int indices[], ID3D11Device* device);

public:
	Mesh(Vertex* vertices, int numVertices, unsigned int indices[], ID3D11Device* device);
	Mesh(const char* objFile, ID3D11Device* device);
	~Mesh();

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices);

	void CalculateObject(Vertex* verts, int numVerts, unsigned int* indices,std::string object);

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	std::vector<Vertex> GetVertsFromMesh();
};

