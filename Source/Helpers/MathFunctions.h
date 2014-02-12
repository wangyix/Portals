#ifndef XMFLOAT2OPERATORS_H
#define XMFLOAT2OPERATORS_H

#include "d3dUtil.h"
#define PI 3.14159265359f

XMFLOAT2 operator+(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs);
XMFLOAT2 operator-(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs);
XMFLOAT2 operator-(const XMFLOAT2 &rhs);
XMFLOAT2 operator*(float lhs, const XMFLOAT2 &rhs);
XMFLOAT2 operator*(const XMFLOAT2 &lhs, float rhs);
XMFLOAT2 operator/(const XMFLOAT2 &lhs, float rhs);
bool operator==(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs);
bool operator!=(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs);
float XMFloat2Dot(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs);
float XMFloat2Cross(const XMFLOAT2 &lhs, const XMFLOAT2 &rhs);
float XMFloat2Length(const XMFLOAT2 &v);
float XMFloat2LengthSq(const XMFLOAT2 &v);
XMFLOAT2 XMFloat2Normalize(const XMFLOAT2 &v);
XMFLOAT2 XMFloat2Left90(const XMFLOAT2 &v);
XMFLOAT2 XMFloat2Right90(const XMFLOAT2 &v);

XMFLOAT3 operator+(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs);
XMFLOAT3 operator-(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs);
XMFLOAT3 operator-(const XMFLOAT3 &rhs);
XMFLOAT3 operator*(float lhs, const XMFLOAT3 &rhs);
XMFLOAT3 operator*(const XMFLOAT3 &lhs, float rhs);
XMFLOAT3 operator/(const XMFLOAT3 &lhs, float rhs);
bool operator==(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs);
bool operator!=(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs);
float XMFloat3Dot(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs);
XMFLOAT3 XMFloat3Cross(const XMFLOAT3 &lhs, const XMFLOAT3 &rhs);
float XMFloat3Length(const XMFLOAT3 &v);
float XMFloat3LengthSq(const XMFLOAT3 &v);
XMFLOAT3 XMFloat3Normalize(const XMFLOAT3 &v);

class MathFunctions
{
public:
	static XMMATRIX InverseTranspose(const XMMATRIX &M);
};
#endif