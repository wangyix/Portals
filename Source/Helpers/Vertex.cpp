#include "Vertex.h"

ID3D11InputLayout* InputLayouts::Basic32 = 0;


void InputLayouts::InitAll(ID3D11Device* device)
{
	const D3D11_INPUT_ELEMENT_DESC Basic32InputLayoutDesc[3] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3DX11_PASS_DESC passDesc;

	Effects::BasicFX->Light3Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(Basic32InputLayoutDesc, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &Basic32));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Basic32);
}

