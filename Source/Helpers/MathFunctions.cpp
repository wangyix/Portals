#include "MathFunctions.h"

XMFLOAT2 operator+(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs)
{
	return XMFLOAT2(lhs.x+rhs.x, lhs.y+rhs.y);
}

XMFLOAT2 operator-(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs)
{
	return XMFLOAT2(lhs.x-rhs.x, lhs.y-rhs.y);
}

XMFLOAT2 operator-(const XMFLOAT2 &rhs)
{
	return XMFLOAT2(-rhs.x, -rhs.y);
}

XMFLOAT2 operator*(float lhs, const XMFLOAT2 &rhs)
{
	return XMFLOAT2(lhs*rhs.x, lhs*rhs.y);
}
XMFLOAT2 operator*(const XMFLOAT2 &lhs, float rhs)
{
	return XMFLOAT2(lhs.x*rhs, lhs.y*rhs);
}

XMFLOAT2 operator/(const XMFLOAT2 &lhs, float rhs)
{
	return XMFLOAT2(lhs.x/rhs, lhs.y/rhs);
}

bool operator==(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs)
{
	return (lhs.x==rhs.x && lhs.y==rhs.y);
}

bool operator!=(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs)
{
	return (lhs.x!=rhs.x || lhs.y!=rhs.y);
}

float XMFloat2Dot(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs)
{
	return lhs.x*rhs.x + lhs.y*rhs.y;
}

float XMFloat2Cross(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs)
{
	return lhs.x*rhs.y - lhs.y*rhs.x;
}

float XMFloat2Length(const XMFLOAT2 &v)
{
	return sqrtf(v.x*v.x + v.y*v.y);
}

float XMFloat2LengthSq(const XMFLOAT2 &v)
{
	return v.x*v.x + v.y*v.y;
}

XMFLOAT2 XMFloat2Normalize(const XMFLOAT2 &v)
{
	return v / XMFloat2Length(v);
}

XMFLOAT2 XMFloat2Left90(const XMFLOAT2 &v)
{
	return XMFLOAT2(-v.y, v.x);
}

XMFLOAT2 XMFloat2Right90(const XMFLOAT2 &v)
{
	return XMFLOAT2(v.y, -v.x);
}


XMFLOAT3 operator+(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return XMFLOAT3(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
}

XMFLOAT3 operator-(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return XMFLOAT3(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z);
}
XMFLOAT3 operator-(const XMFLOAT3 &rhs)
{
	return XMFLOAT3(-rhs.x, -rhs.y, -rhs.z);
}
XMFLOAT3 operator*(float lhs, const XMFLOAT3 &rhs)
{
	return XMFLOAT3(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z);
}

XMFLOAT3 operator*(const XMFLOAT3 &lhs, float rhs)
{
	return XMFLOAT3(lhs.x*rhs, lhs.y*rhs, lhs.z*rhs);
}

XMFLOAT3 operator/(const XMFLOAT3 &lhs, float rhs)
{
	return XMFLOAT3(lhs.x/rhs, lhs.y/rhs, lhs.z/rhs);
}

bool operator==(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return (lhs.x==rhs.x && (lhs.y==rhs.y && lhs.z==rhs.z));
}

bool operator!=(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return (lhs.x!=rhs.x || (lhs.y!=rhs.y || lhs.z!=rhs.z));
}

float XMFloat3Dot(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

XMFLOAT3 XMFloat3Cross(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return XMFLOAT3(lhs.y*rhs.z - lhs.z*rhs.y,
					lhs.z*rhs.x - lhs.x*rhs.z,
					lhs.x*rhs.y - lhs.y*rhs.x);
}

float XMFloat3Length(const XMFLOAT3 &v)
{
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

float XMFloat3LengthSq(const XMFLOAT3 &v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

XMFLOAT3 XMFloat3Normalize(const XMFLOAT3 &v)
{
	return v / XMFloat3Length(v);
}

XMMATRIX MathFunctions::InverseTranspose(const XMMATRIX &M)
{
	XMVECTOR Row4 = M.r[3];

	XMMATRIX Mrot = XMMATRIX(M.r[0], M.r[1], M.r[2], 
		XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));

	XMVECTOR Det;
	return XMMatrixTranspose(XMMatrixInverse(&Det, M));
}