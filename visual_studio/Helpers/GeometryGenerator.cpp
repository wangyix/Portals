#include "GeometryGenerator.h"

// Mesh only needs Position data.  
void GeometryGenerator::Tessellate(std::vector<XMFLOAT3> &Positions, std::vector<UINT> &Indices)
{
	unsigned OldIndicesCount = Indices.size();

	// for each triangle in the mesh, replace it with 4 triangles
	XMFLOAT3 A, B, C;
	UINT Ai, Bi, Ci;
	XMFLOAT3 AB, BC, CA;
	UINT ABi, BCi, CAi;
	for (unsigned int i=0; i<OldIndicesCount; i+=3)
	{
		// get triangle
		Ai = Indices[i];
		Bi = Indices[i+1];
		Ci = Indices[i+2];
		A = Positions[Ai];
		B = Positions[Bi];
		C = Positions[Ci];

		// find midpoints of each side
		int CurrentVerticesCount = Positions.size();
		ABi = CurrentVerticesCount;
		BCi = CurrentVerticesCount+1;
		CAi = CurrentVerticesCount+2;

		AB = 0.5f * (A + B);
		BC = 0.5f * (B + C);
		CA = 0.5f * (C + A);
		Positions.push_back(AB);
		Positions.push_back(BC);
		Positions.push_back(CA);
		

		// update original triagle A-B-C in the indices array to A-AB-CA
		Indices[i+1] = ABi;
		Indices[i+2] = CAi;

		// add the three other triangles to end of indices array
		Indices.push_back(ABi);
		Indices.push_back(Bi);
		Indices.push_back(BCi);

		Indices.push_back(CAi);
		Indices.push_back(BCi);
		Indices.push_back(Ci);

		Indices.push_back(CAi);
		Indices.push_back(ABi);
		Indices.push_back(BCi);
	}
}


void GeometryGenerator::GenerateSphere(MeshData &Mesh, float Radius, unsigned int Tessellations,
														UINT *SphereIndexCount_ptr)
{
	Mesh.Vertices.clear();
	Mesh.Indices.clear();

	// vertices list for an icosahedron
	const float X = 0.525731f; 
	const float Z = 0.850651f;

	std::vector<XMFLOAT3> Positions(12);
	Positions[0] = XMFLOAT3(-X, 0.0f, Z);
	Positions[1] = XMFLOAT3(X, 0.0f, Z);
	Positions[2] = XMFLOAT3(-X, 0.0f, -Z);
	Positions[3] = XMFLOAT3(X, 0.0f, -Z);
	Positions[4] = XMFLOAT3(0.0f, Z, X);
	Positions[5] = XMFLOAT3(0.0f, Z, -X);
	Positions[6] = XMFLOAT3(0.0f, -Z, X);
	Positions[7] = XMFLOAT3(0.0f, -Z, -X);
	Positions[8] = XMFLOAT3(Z, X, 0.0f);
	Positions[9] = XMFLOAT3(-Z, X, 0.0f);
	Positions[10] = XMFLOAT3(Z, -X, 0.0f);
	Positions[11] = XMFLOAT3(-Z, -X, 0.0f);

	// indices list for an icosahedron
	UINT IndicesArray[60] = 
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,    
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,    
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0, 
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7 
	};
	Mesh.Indices.resize(60);
	for (unsigned int i=0; i<60; ++i)
		Mesh.Indices[i] = IndicesArray[i];

	// tesselate the triangles in Positions vector
	for (unsigned int i=0; i<Tessellations; ++i)
		Tessellate(Positions, Mesh.Indices);


	// generate mesh vertices from Positions
	Mesh.Vertices.resize(Positions.size());
	for (unsigned int i=0; i<Positions.size(); ++i)
	{
		XMFLOAT3 PDir = XMFloat3Normalize(Positions[i]);

		Mesh.Vertices[i].Position = Radius * PDir;
		Mesh.Vertices[i].Normal = PDir;
		
		XMFLOAT2 TanXZ = XMFloat2Normalize(XMFloat2Left90(XMFLOAT2(PDir.x, PDir.z)));
		Mesh.Vertices[i].Tangent = XMFLOAT3(TanXZ.x, 0.0f, TanXZ.y);

		XMFLOAT2 PDirXZ = XMFLOAT2(PDir.x, PDir.z);
		float Theta = acosf(PDir.x / XMFloat2Length(PDirXZ));
		float Phi = acosf(PDir.y);
		Mesh.Vertices[i].TexCoord = XMFLOAT2(Theta/(2.0f*PI), Phi/PI);
	}

	*SphereIndexCount_ptr = Mesh.Indices.size();
}