#ifndef PORTAL_H
#define PORTAL_H

#include "d3dUtil.h"
#include "Macros.h"
#include "GeometryGenerator.h"
#include <limits>

class Portal
{
private:
	// physical attributes
	XMFLOAT3 Position;
	XMFLOAT3 Left;
	XMFLOAT3 Up;
	XMFLOAT3 Normal;

	float PhysicalRadius;			// the current physical radius (used for collision calculation) of the portal
	float IntendedPhysicalRadius;	// the size that it wants to be, but may not be due to proximity to wall edges/other portal
	float MaxPhysicalRadius;		// the largest the radius can be at the portals current location
								// NOTE: PhysicalRadius = min(IntendedPhysicalRadius, MaxPhysicalRadius)

	// texture attributes
	float TextureRadiusRatio;

public:
	Portal();
	~Portal();

	void SetTextureRadiusRatio(float TextureRadiusRatio);

	Portal& operator=(const Portal &rhs);
	void SetPosition(XMFLOAT3 Position);
	void SetNormalAndUp(XMFLOAT3 Normal, XMFLOAT3 Up);
	
	void SetIntendedPhysicalRadius(float IntendedPhysicalRad);
	void SetMaxPhysicalRadius(float MaxPhysicalRad);

	void SetIntendedTextureRadius(float IntendedTextureRad);
	void SetMaxTextureRadius(float MaxTextureRad);

	void RotateLeftAroundNormal(float Angle);

	void Transform(const XMMATRIX &M);
	void Flip();

	XMFLOAT3 GetLeft()const;
	XMFLOAT3 GetUp()const;
	XMFLOAT3 GetNormal()const;
	XMFLOAT3 GetPosition()const;

	float GetPhysicalRadius()const;
	float GetIntendedPhysicalRadius()const;
	float GetMaxPhysicalRadius()const;
	
	float GetTextureRadius()const;
	float GetTextureRadiusRatio()const;
	float GetIntendedTextureRadius()const;
	float GetMaxTextureRadius()const;

	bool DiscContainsPoint(XMFLOAT3 Point)const;
	bool DiscIntersectSphere(XMFLOAT3 Center, float Radius)const;

	void Orthonormalize();
	XMMATRIX GetBoxWorldMatrix()const;		// portal space to world space
	XMMATRIX GetPortalMatrix()const;	// world space to portal space
	XMMATRIX GetScaledPortalMatrix()const;	// world space to portal space, scaled by PhysicalRadius

	static XMMATRIX CalculateVirtualizationMatrix(const Portal &LookThru, const Portal &Other);

	static void BuildMeshData(GeometryGenerator::MeshData &PortalMesh, UINT *PortalBoxIndexCount_ptr);
	
	XMFLOAT3 SpherePathCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)const;
	
	bool PathCrossesPortal(XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist)const;

private:
	static bool FindLowestQuarticRootInInterval(const float coeffs[5], float m, float n, float *x_ptr);;
	static int FindQuadraticRootsInInterval(const float coeffs[3], float m, float n, float xs[2]);
	static int FindCubicRootsInInterval(const float coeffs[4], float m, float n, float xs[3]);
	static bool FindPolynomialRootInMonotonicInterval(int degree, const float *coeffs, float m, float n, float *x_ptr, bool *IsIncreasing_ptr);
	static float CalculatePolynomial(float t, int degree, const float *coeffs);
};

#endif