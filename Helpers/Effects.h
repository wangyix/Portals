#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"
#include "Light.h"

// Effect class, all specific FX classes are subclasses of this
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};


// BassicEffect class
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetViewScale(float f)							{ ViewScale->SetFloat(f); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlanePosition(const XMFLOAT3& v)		{ ClipPlanePosition->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlaneNormal(const XMFLOAT3& v)			{ ClipPlaneNormal->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlaneOffset(float f)					{ ClipPlaneOffset->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light3Tech;
	ID3DX11EffectTechnique* Light3TexTech;
	ID3DX11EffectTechnique* Light3TexClipTech;
	ID3DX11EffectTechnique* WireframeTech;


	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
		ID3DX11EffectScalarVariable* ViewScale;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
		ID3DX11EffectVectorVariable* ClipPlanePosition;
		ID3DX11EffectVectorVariable* ClipPlaneNormal;
		ID3DX11EffectScalarVariable* ClipPlaneOffset;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;			// don't call this "Material": that's a class name
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};







// RoomPortalEffect class
class RoomPortalEffect : public Effect
{
public:
	RoomPortalEffect(ID3D11Device* device, const std::wstring& filename);
	~RoomPortalEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetViewScale(float f)							{ ViewScale->SetFloat(f); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlanePosition(const XMFLOAT3& v)		{ ClipPlanePosition->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlaneNormal(const XMFLOAT3& v)			{ ClipPlaneNormal->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlaneOffset(float f)					{ ClipPlaneOffset->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	// portal related
	void SetPortalA(CXMMATRIX M)								{ PortalA->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetPortalATexRadRatio(float f)							{ PortalATexRadRatio->SetFloat(f); }
	void SetPortalADiffuseMap(ID3D11ShaderResourceView* tex)	{ PortalADiffuseMap->SetResource(tex); }
	void SetPortalB(CXMMATRIX M)								{ PortalB->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetPortalBTexRadRatio(float f)							{ PortalBTexRadRatio->SetFloat(f); }
	void SetPortalBDiffuseMap(ID3D11ShaderResourceView* tex)	{ PortalBDiffuseMap->SetResource(tex); }


	ID3DX11EffectTechnique* Light3TexPortalAClipTech;
	ID3DX11EffectTechnique* Light3TexPortalBClipTech;
	ID3DX11EffectTechnique* Light3TexPortalAPortalBTech;
	ID3DX11EffectTechnique* Light3TexPortalAPortalBClipTech;
	ID3DX11EffectTechnique* Light3TexPortalAPortalBNoHolesTech;


	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
		ID3DX11EffectScalarVariable* ViewScale;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
		ID3DX11EffectVectorVariable* ClipPlanePosition;
		ID3DX11EffectVectorVariable* ClipPlaneNormal;
		ID3DX11EffectScalarVariable* ClipPlaneOffset;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;			// don't call this "Material": that's a class name
	ID3DX11EffectShaderResourceVariable* DiffuseMap;

	// portal-related
	ID3DX11EffectMatrixVariable* PortalA;
	ID3DX11EffectScalarVariable* PortalATexRadRatio;
	ID3DX11EffectShaderResourceVariable* PortalADiffuseMap;
	ID3DX11EffectMatrixVariable* PortalB;
	ID3DX11EffectScalarVariable* PortalBTexRadRatio;
	ID3DX11EffectShaderResourceVariable* PortalBDiffuseMap;
};




// PortalEffect class
class PortalEffect : public Effect
{
public:
	PortalEffect(ID3D11Device* device, const std::wstring& filename);
	~PortalEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetClipPlanePosition(const XMFLOAT3& v)		{ ClipPlanePosition->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlaneNormal(const XMFLOAT3& v)			{ ClipPlaneNormal->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		void SetClipPlaneOffset(float f)					{ ClipPlaneOffset->SetFloat(f); }

	ID3DX11EffectTechnique* PortalBoxTech;
	ID3DX11EffectTechnique* PortalBoxClipTech;
	ID3DX11EffectTechnique* PortalBoxClearDepthTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
		ID3DX11EffectVectorVariable* ClipPlanePosition;
		ID3DX11EffectVectorVariable* ClipPlaneNormal;
		ID3DX11EffectScalarVariable* ClipPlaneOffset;
};




// class used for intializing/destroying all effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();
	
	static BasicEffect* BasicFX;
	static RoomPortalEffect* RoomPortalFX;
	static PortalEffect* PortalFX;
};

#endif



