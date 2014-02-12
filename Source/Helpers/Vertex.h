#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"
#include "Effects.h"

namespace Vertex
{
	struct Basic32
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};
}


class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Basic32;
};

#endif