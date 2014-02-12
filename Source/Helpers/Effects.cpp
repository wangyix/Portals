#include "Effects.h"

// Effect constructor
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &mFX));
}
// Effect destructor
Effect::~Effect()
{
	ReleaseCOM(mFX);
}


// Each specific effect is subclass of Effect

BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)	// call super constructor to create ID3DX11Effect object
{
	Light3Tech    = mFX->GetTechniqueByName("Light3");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");
	Light3TexClipTech= mFX->GetTechniqueByName("Light3TexClip");

	WireframeTech = mFX->GetTechniqueByName("Wireframe");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
		ViewScale		= mFX->GetVariableByName("gViewScale")->AsScalar();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
		ClipPlanePosition           = mFX->GetVariableByName("gClipPlanePosition")->AsVector();
		ClipPlaneNormal				= mFX->GetVariableByName("gClipPlaneNormal")->AsVector();
		ClipPlaneOffset				= mFX->GetVariableByName("gClipPlaneOffset")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat		          = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}




RoomPortalEffect::RoomPortalEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)	// call super constructor to create ID3DX11Effect object
{
	Light3TexPortalAClipTech = mFX->GetTechniqueByName("Light3TexPortalAClip");
	Light3TexPortalBClipTech = mFX->GetTechniqueByName("Light3TexPortalBClip");
	Light3TexPortalAPortalBTech    = mFX->GetTechniqueByName("Light3TexPortalAPortalB");
	Light3TexPortalAPortalBClipTech    = mFX->GetTechniqueByName("Light3TexPortalAPortalBClip");
	Light3TexPortalAPortalBNoHolesTech    = mFX->GetTechniqueByName("Light3TexPortalAPortalBNoHoles");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
		ViewScale		= mFX->GetVariableByName("gViewScale")->AsScalar();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
		ClipPlanePosition           = mFX->GetVariableByName("gClipPlanePosition")->AsVector();
		ClipPlaneNormal				= mFX->GetVariableByName("gClipPlaneNormal")->AsVector();
		ClipPlaneOffset			= mFX->GetVariableByName("gClipPlaneOffset")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat		          = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	// portal-related
	PortalA					= mFX->GetVariableByName("gPortalA")->AsMatrix();
	PortalATexRadRatio		= mFX->GetVariableByName("gPortalATexRadRatio")->AsScalar();
	PortalADiffuseMap		= mFX->GetVariableByName("gPortalADiffuseMap")->AsShaderResource();
	PortalB					= mFX->GetVariableByName("gPortalB")->AsMatrix();
	PortalBTexRadRatio		= mFX->GetVariableByName("gPortalBTexRadRatio")->AsScalar();
	PortalBDiffuseMap		= mFX->GetVariableByName("gPortalBDiffuseMap")->AsShaderResource();
}

RoomPortalEffect::~RoomPortalEffect()
{
}






PortalEffect::PortalEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)	// call super constructor to create ID3DX11Effect object
{
	PortalBoxTech    = mFX->GetTechniqueByName("PortalBox");
	PortalBoxClipTech    = mFX->GetTechniqueByName("PortalBoxClip");
	PortalBoxClearDepthTech = mFX->GetTechniqueByName("PortalBoxClearDepth");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
		ClipPlanePosition           = mFX->GetVariableByName("gClipPlanePosition")->AsVector();
		ClipPlaneNormal				= mFX->GetVariableByName("gClipPlaneNormal")->AsVector();
		ClipPlaneOffset			= mFX->GetVariableByName("gClipPlaneOffset")->AsScalar();
}

PortalEffect::~PortalEffect()
{
}






// Effects class used to instantiate/destroy all effect classes
BasicEffect* Effects::BasicFX = 0;
RoomPortalEffect* Effects::RoomPortalFX = 0;
PortalEffect* Effects::PortalFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"./FX/Basic.fxo");
	RoomPortalFX = new RoomPortalEffect(device, L"./FX/RoomPortal.fxo");
	PortalFX = new PortalEffect(device, L"./FX/Portal.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(RoomPortalFX);
	SafeDelete(PortalFX);
}