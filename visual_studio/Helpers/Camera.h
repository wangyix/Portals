#ifndef CAMERA_H
#define CAMERA_H

#include "d3dUtil.h"
#include "Macros.h"
#include "Room.h"
#include "FirstPersonObject.h"


class Camera
{
private:
	XMFLOAT3 Position;
	XMFLOAT3 Right;		// X	// R,U,L,BU are always length 1.  View scaling is done using the ViewScale value
	XMFLOAT3 Up;		// Y
	XMFLOAT3 Look;		// Z
	XMFLOAT3 BodyUp;
	float ViewScale;

	float Near;
	float Far;
	float FovY;
	float Aspect;

	FirstPersonObject *AttachedTo;	// an object that will move/rotate with this camera

	XMMATRIX ProjMatrix;	// cached projection matrix

public:
	Camera();
	~Camera();

	XMFLOAT3 GetPosition()const;
	XMFLOAT3 GetRight()const;
	XMFLOAT3 GetUp()const;
	XMFLOAT3 GetLook()const;
	XMFLOAT3 GetBodyUp()const;

	float GetViewScale()const;
	float GetBoundingSphereRadius()const;

	XMMATRIX GetViewMatrix()const;
	XMMATRIX GetProjMatrix()const;
	void Orthonormalize();

	void SetPosition(XMFLOAT3 Position);
	void SetLens(float Near, float Far, float FovY);
	void SetAspect(float Aspect);

	void Level();
	void LookAtAndLevel(XMFLOAT3 Target);
	void RotateRight(float Angle);
	void RotateUp(float Angle);
	void RollRight(float Angle);

	XMFLOAT3 MoveForward(float Dist);
	XMFLOAT3 MoveRight(float Dist);
	XMFLOAT3 MoveUp(float Dist);

	void Transform(const XMMATRIX &M);
	float MultiplyViewScale(float multiplier);

	void AttachToObject(FirstPersonObject *Object);
	FirstPersonObject* DetachFromObject();

	float SurfaceVisibilityFactor(XMFLOAT3 SurfacePoint, XMFLOAT3 SurfaceNormal)const;
	bool FrustumContainsDisc(XMFLOAT3 DiscCenter, XMFLOAT3 DiscNormal, float DiscRadius)const;

	XMFLOAT3 SelfVirtualCollision(const XMMATRIX &Virtualize, float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)const;

private:
	void UpdateProjMatrix();
	static bool FrustumContainsSegment2D(const XMFLOAT2 &LDir, const XMFLOAT2 &RDir, 
											const XMFLOAT2 &A, const XMFLOAT2 &B);

};

#endif