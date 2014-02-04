#include "RenderStates.h"

ID3D11DepthStencilState* RenderStates::StencilSetToDSS = 0;
ID3D11DepthStencilState* RenderStates::StencilEqualDSS = 0;
ID3D11DepthStencilState* RenderStates::StencilLessEqualDSS = 0;
ID3D11DepthStencilState* RenderStates::StencilEqualIncrementDSS = 0;
ID3D11DepthStencilState* RenderStates::StencilEqualDecrementDSS = 0;
ID3D11DepthStencilState* RenderStates::StencilEqualDepthAlwaysDSS = 0;
ID3D11DepthStencilState* RenderStates::StencilLessEqualSetToDSS = 0;


void RenderStates::InitAll(ID3D11Device* device)
{

	D3D11_DEPTH_STENCIL_DESC StencilSetToDesc;

	StencilSetToDesc.DepthEnable = true;
	StencilSetToDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilSetToDesc.DepthFunc = D3D11_COMPARISON_LESS;
	StencilSetToDesc.StencilEnable = true;
	StencilSetToDesc.StencilReadMask  = 0xff;
    StencilSetToDesc.StencilWriteMask = 0xff;

	StencilSetToDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	StencilSetToDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilSetToDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilSetToDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	// backfaces are culled, these don't matter
	StencilSetToDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilSetToDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilSetToDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilSetToDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilSetToDesc, &StencilSetToDSS));



	D3D11_DEPTH_STENCIL_DESC StencilEqualDesc;

	StencilEqualDesc.DepthEnable = true;
	StencilEqualDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilEqualDesc.DepthFunc = D3D11_COMPARISON_LESS;
	StencilEqualDesc.StencilEnable = true;
	StencilEqualDesc.StencilReadMask  = 0xff;
    StencilEqualDesc.StencilWriteMask = 0xff;

	StencilEqualDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// backfaces are culled, these don't matter
	StencilEqualDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilEqualDesc, &StencilEqualDSS));



	D3D11_DEPTH_STENCIL_DESC StencilLessEqualDesc;

	StencilLessEqualDesc.DepthEnable = true;
	StencilLessEqualDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilLessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS;
	StencilLessEqualDesc.StencilEnable = true;
	StencilLessEqualDesc.StencilReadMask  = 0xff;
    StencilLessEqualDesc.StencilWriteMask = 0xff;

	StencilLessEqualDesc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;
	StencilLessEqualDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// backfaces are culled, these don't matter
	StencilLessEqualDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilLessEqualDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilLessEqualDesc, &StencilLessEqualDSS));


	D3D11_DEPTH_STENCIL_DESC StencilEqualIncrementDesc;

	StencilEqualIncrementDesc.DepthEnable = true;
	StencilEqualIncrementDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilEqualIncrementDesc.DepthFunc = D3D11_COMPARISON_LESS;
	StencilEqualIncrementDesc.StencilEnable = true;
	StencilEqualIncrementDesc.StencilReadMask  = 0xff;
    StencilEqualIncrementDesc.StencilWriteMask = 0xff;

	StencilEqualIncrementDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualIncrementDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualIncrementDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualIncrementDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;

	// backfaces are culled, these don't matter
	StencilEqualIncrementDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualIncrementDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualIncrementDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualIncrementDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilEqualIncrementDesc, &StencilEqualIncrementDSS));


	D3D11_DEPTH_STENCIL_DESC StencilEqualDecrementDesc;

	StencilEqualDecrementDesc.DepthEnable = true;
	StencilEqualDecrementDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilEqualDecrementDesc.DepthFunc = D3D11_COMPARISON_LESS;
	StencilEqualDecrementDesc.StencilEnable = true;
	StencilEqualDecrementDesc.StencilReadMask  = 0xff;
    StencilEqualDecrementDesc.StencilWriteMask = 0xff;

	StencilEqualDecrementDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualDecrementDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDecrementDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDecrementDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;

	// backfaces are culled, these don't matter
	StencilEqualDecrementDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualDecrementDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDecrementDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDecrementDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilEqualDecrementDesc, &StencilEqualDecrementDSS));



	D3D11_DEPTH_STENCIL_DESC StencilEqualDepthAlwaysDesc;

	StencilEqualDepthAlwaysDesc.DepthEnable = true;
	StencilEqualDepthAlwaysDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilEqualDepthAlwaysDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	StencilEqualDepthAlwaysDesc.StencilEnable = true;
	StencilEqualDepthAlwaysDesc.StencilReadMask  = 0xff;
    StencilEqualDepthAlwaysDesc.StencilWriteMask = 0xff;

	StencilEqualDepthAlwaysDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualDepthAlwaysDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDepthAlwaysDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDepthAlwaysDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// backfaces are culled, these don't matter
	StencilEqualDepthAlwaysDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilEqualDepthAlwaysDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDepthAlwaysDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilEqualDepthAlwaysDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilEqualDepthAlwaysDesc, &StencilEqualDepthAlwaysDSS));


	D3D11_DEPTH_STENCIL_DESC StencilLessEqualSetToDesc;

	StencilLessEqualSetToDesc.DepthEnable = true;
	StencilLessEqualSetToDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	StencilLessEqualSetToDesc.DepthFunc = D3D11_COMPARISON_LESS;
	StencilLessEqualSetToDesc.StencilEnable = true;
	StencilLessEqualSetToDesc.StencilReadMask  = 0xff;
    StencilLessEqualSetToDesc.StencilWriteMask = 0xff;

	StencilLessEqualSetToDesc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;
	StencilLessEqualSetToDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualSetToDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualSetToDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	// backfaces are culled, these don't matter
	StencilLessEqualSetToDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	StencilLessEqualSetToDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualSetToDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	StencilLessEqualSetToDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&StencilLessEqualSetToDesc, &StencilLessEqualSetToDSS));

}

void RenderStates::DestroyAll()
{
	ReleaseCOM(StencilSetToDSS);
	ReleaseCOM(StencilEqualDSS);
	ReleaseCOM(StencilLessEqualDSS);
	ReleaseCOM(StencilEqualIncrementDSS);
	ReleaseCOM(StencilEqualDecrementDSS);
	ReleaseCOM(StencilEqualDepthAlwaysDSS);
	ReleaseCOM(StencilLessEqualSetToDSS);
}