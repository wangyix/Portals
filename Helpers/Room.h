#ifndef ROOM_H
#define ROOM_H

#include "d3dUtil.h"
#include "Macros.h"
#include <vector>
#include <limits>
#include "MathFunctions.h"
#include "GeometryGenerator.h"
#include "Portal.h"

class Room
{
private:
	class BoundaryElement
	{
	public:
		// must exist so delete can be called on pointers of derived classes
		// pure destructor, but must have a body
		virtual ~BoundaryElement() {}

		virtual void PrintInfo()const = 0;
		virtual bool RayPathExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, XMFLOAT2 *X_ptr, float *XDist_ptr,
									float *LeftRedCos_ptr, XMFLOAT2 *LeftRedDir_ptr, 
									XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const = 0;
	};

	class BoundaryElementsList
	{
	public:
		BoundaryElementsList();
		~BoundaryElementsList();

		void AddEdge(const XMFLOAT2 &U, const XMFLOAT2 &V);
		void AddVertex(const XMFLOAT2 &V);
		void Delete(int Index);
		BoundaryElement* operator[](const int Index)const;
		unsigned int size()const;
	private:
		std::vector<BoundaryElement*> ElementPointers;
	};


	class BoundaryEdge : public BoundaryElement
	{
	private:
		XMFLOAT2 U;
		XMFLOAT2 V;
	public:
		BoundaryEdge(XMFLOAT2 U, XMFLOAT2 V);
		~BoundaryEdge();

		void PrintInfo()const;
		bool RayPathExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, XMFLOAT2 *X_ptr, float *XDist_ptr,
									float *LeftRedCos_ptr, XMFLOAT2 *LeftRedDir_ptr,
									XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const;
	};
	
	class BoundaryVertex : public BoundaryElement
	{
	private:
		XMFLOAT2 V;
	public:
		BoundaryVertex(XMFLOAT2 V);
		~BoundaryVertex();

		void PrintInfo()const;
		bool RayPathExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, XMFLOAT2 *X_ptr, float *XDist_ptr,
									float *LeftRedCos_ptr, XMFLOAT2 *LeftRedDir_ptr,
									XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const;
	};


public:
	Room();

	void SetFloorAndCeiling(float FloorHeight, float CeilingHeight);
	void SetTopography(const std::vector<std::vector<XMFLOAT2>> &PhysicalBoundariesVerticesList);
	void PrintBoundaries();
	
	void BuildMeshData(GeometryGenerator::MeshData &RoomMesh, 
						UINT *WallsIndexCount_ptr, UINT *WallsIBOffset_ptr, UINT *WallsVBOffset_ptr,
						UINT *FloorIndexCount_ptr, UINT *FloorIBOffset_ptr, UINT *FloorVBOffset_ptr,
						UINT *CeilingIndexCount_ptr, UINT *CeilingIBOffset_ptr, UINT *CeilingVBOffset_ptr)const;

	XMFLOAT3 SpherePathCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr,
									XMFLOAT3 *T_ptr, XMFLOAT3 *TNormal_ptr)const;

	XMFLOAT3 SpherePathVirtualCollision(const XMMATRIX &Virtualize, const XMMATRIX &Unvirtualize,
									float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)const;


	void PortalRelocate(XMFLOAT3 S, XMFLOAT3 Dir, Portal &ThisPortal, const Portal &OtherPortal)const;

private:
	XMFLOAT2 FindFirstExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, float MoveDist, 
							const BoundaryElementsList &DiscCenterBoundaryElements,
							float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT2 *RedirectDir_ptr,
							XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const;


	XMFLOAT3 SpherePathWallCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr,
									XMFLOAT3 *T_ptr, XMFLOAT3 *TNormal_ptr)const;

	XMFLOAT3 SpherePathFloorCeilingCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr,
									XMFLOAT3 *T_ptr, XMFLOAT3 *TNormal_ptr)const;


private:
	float MinX;
	float MaxX;
	float MinZ;
	float MaxZ;
	int WallCount;

	float FloorY;				// height of floor
	float CeilingY;				// height of ceiling
	
	// a vector of vertexlists, each representing a boundary polygon in the room
	std::vector<std::vector<XMFLOAT2>> BoundaryPolygons;
};

#endif


