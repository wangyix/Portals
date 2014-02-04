#ifndef LIGHT_H
#define LIGHT_H

struct DirectionalLight
{
	// struct constructor: zeros everything
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float pad;			// allows arrays of lights
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;	// cos exponent in w component
	XMFLOAT4 Reflect;
};

#endif