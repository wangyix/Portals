#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	// Depth stencil states
	static ID3D11DepthStencilState* StencilSetToDSS;
	static ID3D11DepthStencilState* StencilEqualDSS;
	static ID3D11DepthStencilState* StencilLessEqualDSS;
	static ID3D11DepthStencilState* StencilEqualIncrementDSS;
	static ID3D11DepthStencilState* StencilEqualDecrementDSS;
	static ID3D11DepthStencilState* StencilEqualDepthAlwaysDSS;
	static ID3D11DepthStencilState* StencilLessEqualSetToDSS;	// used for rendering player after next portal disc has been rendered
};

#endif