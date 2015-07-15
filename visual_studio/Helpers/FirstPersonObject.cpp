#include "FirstPersonObject.h"

FirstPersonObject::FirstPersonObject()
	: BoundingSphereRadius(1.0f)
{
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

FirstPersonObject::~FirstPersonObject()
{
}

float FirstPersonObject::GetBoundingSphereRadius()
{
	return this->BoundingSphereRadius;
}
XMFLOAT3 FirstPersonObject::GetPosition()
{
	return this->Position;
}
XMFLOAT3 FirstPersonObject::GetRight()
{
	return this->Right;
}
XMFLOAT3 FirstPersonObject::GetUp()
{
	return this->Up;
}
XMFLOAT3 FirstPersonObject::GetLook()
{
	return this->Look;
}

void FirstPersonObject::SetBoundingSphereRadius(float BoundingSphereRadius)
{
	this->BoundingSphereRadius = BoundingSphereRadius;
}

void FirstPersonObject::SetPosition(const XMFLOAT3 &Position)
{
	this->Position = Position;
}

void FirstPersonObject::SetOrientation(const XMFLOAT3 &Right, const XMFLOAT3 &Up, const XMFLOAT3 &Look)
{
	this->Right = Right;
	this->Up = Up;
	this->Look = Look;
}

XMMATRIX FirstPersonObject::GetWorldMatrix()const
{
	// assume this object has bounding sphere radius of 1 in object space
	return XMMatrixScaling(BoundingSphereRadius, BoundingSphereRadius, BoundingSphereRadius)
			* XMMATRIX(	Right.x,	Right.y,	Right.z,	0.0f,
						Up.x,		Up.y,		Up.z,		0.0f,
						Look.x,		Look.y,		Look.z,		0.0f,
						Position.x,	Position.y,	Position.z,	1.0f	);
}