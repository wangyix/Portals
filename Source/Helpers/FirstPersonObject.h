#ifndef FIRSTPERSONOBJECT_H
#define FIRSTPERSONOBJECT_H

#include "d3dUtil.h"

class FirstPersonObject
{
private:
	float BoundingSphereRadius;

	XMFLOAT3 Position;
	XMFLOAT3 Right;		// X
	XMFLOAT3 Up;		// Y
	XMFLOAT3 Look;		// Z

public:
	FirstPersonObject();
	~FirstPersonObject();

	float GetBoundingSphereRadius();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetLook();

	void SetBoundingSphereRadius(float BoundingSphereRadius);
	void SetPosition(const XMFLOAT3 &Position);
	void SetOrientation(const XMFLOAT3 &Right, const XMFLOAT3 &Up, const XMFLOAT3 &Look);

	XMMATRIX GetWorldMatrix()const;
};

#endif