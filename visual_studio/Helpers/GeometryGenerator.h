#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include "d3dUtil.h"
#include "MathFunctions.h"

class GeometryGenerator
{
public:
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;
		XMFLOAT2 TexCoord;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};


	static void Tessellate(std::vector<XMFLOAT3> &Positions, std::vector<UINT> &Indices);
	static void GenerateSphere(MeshData &Mesh, float Radius, unsigned int Tessellations, 
													UINT *SphereIndexCount_ptr);
};

#endif