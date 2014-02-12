//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************

#include "d3dApp.h"
#include "Vertex.h"
#include "Effects.h"
#include "Room.h"
#include "Portal.h"
#include "Camera.h"
#include "RenderStates.h"
#include "SpherePath.h"
#include "Macros.h"



class PortalsApp : public D3DApp
{
public:
	PortalsApp(HINSTANCE hInstance);
	~PortalsApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:

	bool GetNextDataLine(std::ifstream &ifs, std::string &Line);

	void BuildRoomGeometryBuffers();
	void BuildPlayerGeometryBuffers();
	void BuildPortalGeometryBuffers();

	void DrawRoomBothPortals(bool PlaneClip, const XMMATRIX &World, const XMMATRIX &ViewProj, float ViewScale);
	void DrawPlayer(bool PlaneClip, const XMMATRIX &World, const XMMATRIX &ViewProj, float ViewScale);
	
	void DrawPortalBox(bool PlaneClip, const XMMATRIX &World, const XMMATRIX &ViewProj);
	void DrawPortalBoxClearDepth(const XMMATRIX &World, const XMMATRIX &ViewProj);

	void DrawRoomBothPortalsNoHoles(const XMMATRIX &World, const XMMATRIX &ViewProj, float ViewScale);


	void RenderPortalInsidesPlayerNoClip(const Portal &LookThruPortal, const XMMATRIX &Virtualize,
						const XMMATRIX &InitialWorldToVirtual, UINT InitialStencilRef, int Levels,
						const XMMATRIX &ViewProj, float ViewScale);

	
	void RenderPortalInsidesPlayerClip(const Portal &LookThruPortal, const XMMATRIX &Virtualize,
						const XMMATRIX &InitialWorldToVirtual, const XMMATRIX &InitialPlayerWorldToVirtual,
						UINT InitialStencilRef, int Levels,
						const XMMATRIX &ViewProj, float ViewScale);
	

private:
	
	DirectionalLight mDirLights[3];

	// added so we don't have to keep redeclaring them inside draw functions
	const UINT mStride;
	const UINT mOffset;
	ID3DX11EffectTechnique* mTech;
	D3DX11_TECHNIQUE_DESC mTechDesc;

	
	Camera *mCurrentCamera_ptr;

	Portal *mCurrentPortal_ptr;
	Portal *mCurrentOtherPortal_ptr;
	


	// MOUSE STUFF ****************************************
	POINT mLastMousePos;
	bool mRightButtonIsDown;


	// CAMERA STUFF ******************************************************************
	Camera mLeftCamera;
	Camera mRightCamera;

	XMMATRIX mLeftViewProj;
	float mLeftViewScale;
	XMMATRIX mRightViewProj;
	float mRightViewScale;

	
	// ROOM STUFF ***********************************************************************
	Room mRoom;

	// room geometry (walls, floor, ceiling)
	ID3D11Buffer* mRoomVB;
	ID3D11Buffer* mRoomIB;

	UINT mWallsIndexCount;
	UINT mWallsIBOffset;
	UINT mWallsVBOffset;
	Material mWallsMaterial;
	ID3D11ShaderResourceView* mWallsSRV;
	XMMATRIX mWallsTexTransform;

	UINT mFloorIndexCount;
	UINT mFloorIBOffset;
	UINT mFloorVBOffset;
	Material mFloorMaterial;
	ID3D11ShaderResourceView* mFloorSRV;
	XMMATRIX mFloorTexTransform;

	UINT mCeilingIndexCount;
	UINT mCeilingIBOffset;
	UINT mCeilingVBOffset;
	Material mCeilingMaterial;
	ID3D11ShaderResourceView* mCeilingSRV;
	XMMATRIX mCeilingTexTransform;

	// PORTAL STUFF **********************************************************************
	Portal mOrangePortal;
	ID3D11ShaderResourceView* mOrangePortalSRV;

	Portal mBluePortal;
	ID3D11ShaderResourceView* mBluePortalSRV;

	bool mPlayerIntersectOrangePortal;
	bool mPlayerIntersectBluePortal;


	// portal geometry (face, box)
	// use same buffers for both portals, only changing the world transformation and face texture
	ID3D11Buffer* mPortalVB;
	ID3D11Buffer* mPortalIB;

	UINT mPortalBoxIndexCount;


	// PLAYER STUFF *********************************************************************
	// player geometry
	FirstPersonObject mPlayer;

	ID3D11Buffer* mPlayerVB;
	ID3D11Buffer* mPlayerIB;
	UINT mPlayerIndexCount;

	Material mPlayerMaterial;
	ID3D11ShaderResourceView* mPlayerSRV;
	XMMATRIX mPlayerTexTransform;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	PortalsApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}


PortalsApp::PortalsApp(HINSTANCE hInstance)
:	D3DApp(hInstance), mRoomVB(0), mRoomIB(0), mPlayerVB(0), mPlayerIB(0),
	mWallsIndexCount(0), mWallsIBOffset(0), mWallsVBOffset(0), 
	mFloorIndexCount(0), mFloorIBOffset(0), mFloorVBOffset(0), 
	mCeilingIndexCount(0), mCeilingIBOffset(0), mCeilingVBOffset(0), 
	mPlayerIntersectOrangePortal(false), mPlayerIntersectBluePortal(false), 
	mRightButtonIsDown(false),
	mStride(sizeof(Vertex::Basic32)), mOffset(0)
{
	mMainWndCaption = L"PortalsApp";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	// initialize fields and matrices etc

	// 3 directional lights
	mDirLights[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	// room values
	mWallsMaterial.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mWallsMaterial.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mWallsMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mWallsMaterial.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mWallsTexTransform = XMMatrixScaling(0.25f, 0.25f, 1.0f);

	mFloorMaterial.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mFloorMaterial.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mFloorMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mFloorMaterial.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mFloorTexTransform = XMMatrixScaling(0.25f, 0.25f, 1.0f);

	mCeilingMaterial.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mCeilingMaterial.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mCeilingMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mCeilingMaterial.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mCeilingTexTransform = XMMatrixScaling(0.25f, 0.25f, 1.0f);

	// player values
	mPlayerMaterial.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mPlayerMaterial.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mPlayerMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mPlayerMaterial.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mPlayerTexTransform = XMMatrixIdentity();

	// start out with control of left camera
	mCurrentCamera_ptr = &mLeftCamera;

	// start out with control of orange portal
	mCurrentPortal_ptr = &mOrangePortal;
	mCurrentOtherPortal_ptr = &mBluePortal;
}

PortalsApp::~PortalsApp()
{
	// release all resources
	ReleaseCOM(mRoomVB);
	ReleaseCOM(mRoomIB);
	ReleaseCOM(mWallsSRV);
	ReleaseCOM(mFloorSRV);
	ReleaseCOM(mCeilingSRV);

	ReleaseCOM(mPortalVB);
	ReleaseCOM(mPortalIB);
	ReleaseCOM(mOrangePortalSRV);
	ReleaseCOM(mBluePortalSRV);

	ReleaseCOM(mPlayerVB);
	ReleaseCOM(mPlayerIB);
	ReleaseCOM(mPlayerSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}


bool PortalsApp::GetNextDataLine(std::ifstream &ifs, std::string &Line)
{
	Line.clear();
	while (std::getline(ifs, Line))
	{
		// find first non-space character.  skip if #
		size_t StrBegin = Line.find_first_not_of(" \t");
		if (StrBegin!=std::string::npos && Line[StrBegin]!='#')
			break;
	}
	return (!ifs.eof());
}

bool PortalsApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	// read in room polygons from file
	
	std::ifstream ifs(ROOM_FILE_PATH);
	std::string Line;

	float a, b, c, d, e, f;

	// left camera start position
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &a, &b, &c);
	mLeftCamera.SetPosition(XMFLOAT3(a, b, c));

	// player 
	
	//radius
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f", &a);
	mPlayer.SetBoundingSphereRadius(a);

	// start position
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &a, &b, &c);
	mPlayer.SetPosition(XMFLOAT3(a, b, c));


	// orange portal 
	
	// radius
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f", &a);
	mOrangePortal.SetIntendedPhysicalRadius(a);

	// position
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &a, &b, &c);
	mOrangePortal.SetPosition(XMFLOAT3(a, b, c));

	// normal
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &a, &b, &c);
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &d, &e, &f);
	mOrangePortal.SetNormalAndUp(XMFLOAT3(a,b,c), XMFLOAT3(d,e,f));


	// blue portal 
	
	// radius
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f", &a);
	mBluePortal.SetIntendedPhysicalRadius(a);

	// position
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &a, &b, &c);
	mBluePortal.SetPosition(XMFLOAT3(a, b, c));

	// normal
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &a, &b, &c);
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f %f", &d, &e, &f);
	mBluePortal.SetNormalAndUp(XMFLOAT3(a,b,c), XMFLOAT3(d,e,f));
	

	// floor, ceiling heights
	GetNextDataLine(ifs, Line);
	sscanf_s(Line.c_str(), "%f %f", &a, &b);
	mRoom.SetFloorAndCeiling(a, b);


	// room polygons

	std::vector<std::vector<XMFLOAT2>> PolygonsList;
	unsigned int Polygons = 0;
	unsigned int Vertices;
	while(GetNextDataLine(ifs, Line))
	{
		PolygonsList.push_back(std::vector<XMFLOAT2>());
		
		// read vertex count for this polygon
		sscanf_s(Line.c_str(), "%d", &Vertices);

		// resize vertexlist for this polygon
		PolygonsList[Polygons].resize(Vertices);

		// read vertices for this polygon
		for (unsigned int i=0; i<Vertices; ++i)
		{
			GetNextDataLine(ifs, Line);
			sscanf_s(Line.c_str(), "%f %f", &a, &b);
			PolygonsList[Polygons][i] = XMFLOAT2(a, b);
		}
		++Polygons;
	}
	mRoom.SetTopography(PolygonsList);
	mRoom.PrintBoundaries();

	ifs.close();
	PolygonsList.clear();

	// set cameras' lenses
	mLeftCamera.SetLens(0.01f, 500.0f, PI/4.0f);
	mRightCamera.SetLens(0.01f, 500.0f, PI/4.0f);

	// attach right camera to player
	mRightCamera.AttachToObject(&mPlayer);


	// room
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/floor.dds", 0, 0, &mWallsSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/floor.dds", 0, 0, &mFloorSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/floor.dds", 0, 0, &mCeilingSRV, 0 ));

	// portals
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/orange_portal2.dds", 0, 0, &mOrangePortalSRV, 0 ));
	mOrangePortal.SetTextureRadiusRatio(1.22f);

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/blue_portal2.dds", 0, 0, &mBluePortalSRV, 0 ));
	mBluePortal.SetTextureRadiusRatio(1.22f);

	// player
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/stone.dds", 0, 0, &mPlayerSRV, 0 ));

	BuildRoomGeometryBuffers();
	BuildPortalGeometryBuffers();
	BuildPlayerGeometryBuffers();
	
	return true;
}


void PortalsApp::OnResize()
{
	D3DApp::OnResize();

	mLeftCamera.SetAspect(AspectRatio());
	mRightCamera.SetAspect(AspectRatio());
}

void PortalsApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	
	SetCapture(mhMainWnd);

	if ((btnState & MK_RBUTTON) != 0)
		mRightButtonIsDown = true;
}

void PortalsApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) == 0)
		mRightButtonIsDown = false;

	ReleaseCapture();
}

void PortalsApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCurrentCamera_ptr->RotateUp(-dy);
		mCurrentCamera_ptr->RotateRight(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}


void PortalsApp::UpdateScene(float dt)
{

	// see if the portals can be modified (e.g. do they intersect the player or the camera?)
	bool PortalsCanBeModified = (!mPlayerIntersectOrangePortal && !mPlayerIntersectBluePortal);
	if (PortalsCanBeModified)
	{
		// check if one of the two portals intersect with leftcamera
		if (mOrangePortal.DiscIntersectSphere(mLeftCamera.GetPosition(), mLeftCamera.GetBoundingSphereRadius()+0.001f))
			PortalsCanBeModified = false;
	}
	if (PortalsCanBeModified)
	{
		// check if one of the two portals intersect with leftcamera
		if (mBluePortal.DiscIntersectSphere(mLeftCamera.GetPosition(), mLeftCamera.GetBoundingSphereRadius()+0.001f))
			PortalsCanBeModified = false;
	}


	// switch which portal to control
	if (GetAsyncKeyState('O') & 0x8000)
	{
		mCurrentPortal_ptr = &mOrangePortal;
		mCurrentOtherPortal_ptr = &mBluePortal;
	}
	else if (GetAsyncKeyState('B') & 0x8000)
	{
		mCurrentPortal_ptr = &mBluePortal;
		mCurrentOtherPortal_ptr = &mOrangePortal;
	}


	// switch which camera to control
	if (GetAsyncKeyState('1') & 0x8000)
		mCurrentCamera_ptr = &mLeftCamera;
	else if (GetAsyncKeyState('2') & 0x8000)
		mCurrentCamera_ptr = &mRightCamera;

	// orthonormalize camera axes
	mCurrentCamera_ptr->Orthonormalize();


	// move camera
	float ForwardSteps = 0;
	float RightSteps = 0;
	float UpSteps = 0;
	if (GetAsyncKeyState('W') & 0x8000)
		ForwardSteps += 1.0f;
	if (GetAsyncKeyState('S') & 0x8000)
		ForwardSteps -= 1.0f;
	if (GetAsyncKeyState('A') & 0x8000)
		RightSteps -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000)
		RightSteps += 1.0f;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		UpSteps += 1.0f;
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		UpSteps -= 1.0f;
	
	XMFLOAT3 Dir = 		ForwardSteps*mCurrentCamera_ptr->GetLook() +
						RightSteps*mCurrentCamera_ptr->GetRight() +
						UpSteps*mCurrentCamera_ptr->GetBodyUp();

	if (XMFloat3LengthSq(Dir)!=0.0f)
	{
		float speed = CAMERA_MOVEMENT_SPEED;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			speed *= CAMERA_MOVEMENT_SPRINT_MULTIPLIER;

		Dir = XMFloat3Normalize(Dir);
		SpherePath::MoveCameraAlongPathIterative(*mCurrentCamera_ptr, Dir, speed*dt,
												mRoom, mOrangePortal, mBluePortal);
	}


	// level camera
	if (GetAsyncKeyState('L') & 0x8000)
		mCurrentCamera_ptr->Level();
	else
	{
		// roll camera
		float RightRollUnits = 0.0f;
		if (GetAsyncKeyState('Q') & 0x8000)
			RightRollUnits -= 1.0f;
		if (GetAsyncKeyState('E') & 0x8000)
			RightRollUnits += 1.0f;

		if (RightRollUnits != 0.0f)
			mCurrentCamera_ptr->RollRight(RightRollUnits * CAMERA_ROLL_SPEED/180.0f*PI * dt);
	}


	// rotate/resize/move current portal
	if (PortalsCanBeModified)
	{
		// move
		if (mRightButtonIsDown)
		{
			mRoom.PortalRelocate(mCurrentCamera_ptr->GetPosition(), mCurrentCamera_ptr->GetLook(), 
				*mCurrentPortal_ptr, *mCurrentOtherPortal_ptr);
		}

		// rotate
		float PortalLeftRotateUnits = 0.0f;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			PortalLeftRotateUnits += 1.0f;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			PortalLeftRotateUnits -= 1.0f;
		if (PortalLeftRotateUnits != 0.0f)
		{
			mCurrentPortal_ptr->RotateLeftAroundNormal(PortalLeftRotateUnits * PORTAL_ROTATE_SPEED/180.0f*PI * dt);
			mCurrentPortal_ptr->Orthonormalize();
		}
	
		// resize
		float PortalSizeIncreaseUnits = 0;
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			PortalSizeIncreaseUnits += 1.0f;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			PortalSizeIncreaseUnits -= 1.0f;
		if (PortalSizeIncreaseUnits != 0.0f)
		{
			float NewRadius = mCurrentPortal_ptr->GetPhysicalRadius() + (PortalSizeIncreaseUnits * PORTAL_SIZE_CHANGE_SPEED * dt);
			mCurrentPortal_ptr->SetIntendedPhysicalRadius(NewRadius);
		}
	}
	
	
	// Update whether or not the player clips either portal at their new locations.

	// when seeing if the player intersects either portal, add a small buffer to the BoundingSphereRadius so it's less likely to go
	// into the no-intersect case, which is susceptible to z-fighting with far-away players and discs when the player is very close to the disc
	mPlayerIntersectOrangePortal = mOrangePortal.DiscIntersectSphere(mPlayer.GetPosition(), mPlayer.GetBoundingSphereRadius()+0.01f);
	mPlayerIntersectBluePortal = mBluePortal.DiscIntersectSphere(mPlayer.GetPosition(), mPlayer.GetBoundingSphereRadius()+0.01f);


	// update camera ViewProj matrices and ViewScale values
	XMMATRIX View, Proj;

	View = mLeftCamera.GetViewMatrix();
	Proj = mLeftCamera.GetProjMatrix();
	mLeftViewProj = View * Proj;
	mLeftViewScale = mLeftCamera.GetViewScale();

	View = mRightCamera.GetViewMatrix();
	Proj = mRightCamera.GetProjMatrix();
	mRightViewProj = View * Proj;
	mRightViewScale = mRightCamera.GetViewScale();
}






#pragma region DRAW_FUNCTIONS

void PortalsApp::DrawRoomBothPortals(bool PlaneClip, const XMMATRIX &World, const XMMATRIX &ViewProj, float ViewScale)
{	
	XMMATRIX WorldInvTranspose, WorldViewProj;

	mTech = PlaneClip ? Effects::RoomPortalFX->Light3TexPortalAPortalBClipTech : 
						Effects::RoomPortalFX->Light3TexPortalAPortalBTech;
	mTech->GetDesc(&mTechDesc);
	for (UINT p=0; p < mTechDesc.Passes; ++p)
	{
		ID3DX11EffectPass* Pass = mTech->GetPassByIndex(p);
		// set room buffers
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mRoomVB, &mStride, &mOffset);
		md3dImmediateContext->IASetIndexBuffer(mRoomIB, DXGI_FORMAT_R32_UINT, 0);

		WorldInvTranspose = MathFunctions::InverseTranspose(World);
		WorldViewProj = World * ViewProj;

		Effects::RoomPortalFX->SetWorld(World);
		Effects::RoomPortalFX->SetWorldInvTranspose(WorldInvTranspose);
		Effects::RoomPortalFX->SetWorldViewProj(WorldViewProj);
			Effects::RoomPortalFX->SetViewScale(ViewScale);
		
		// portal-related settings are set once before drawing anything

		// draw the walls
		Effects::RoomPortalFX->SetTexTransform(mWallsTexTransform);
		Effects::RoomPortalFX->SetMaterial(mWallsMaterial);
		Effects::RoomPortalFX->SetDiffuseMap(mWallsSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mWallsIndexCount, mWallsIBOffset, mWallsVBOffset);

		// draw floor
		Effects::RoomPortalFX->SetTexTransform(mFloorTexTransform);
		Effects::RoomPortalFX->SetMaterial(mFloorMaterial);
		Effects::RoomPortalFX->SetDiffuseMap(mFloorSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mFloorIndexCount, mFloorIBOffset, mFloorVBOffset);


		// draw ceiling
		Effects::RoomPortalFX->SetTexTransform(mCeilingTexTransform);
		Effects::RoomPortalFX->SetMaterial(mCeilingMaterial);
		Effects::RoomPortalFX->SetDiffuseMap(mCeilingSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mCeilingIndexCount, mCeilingIBOffset, mCeilingVBOffset);
	}
}

void PortalsApp::DrawPlayer(bool PlaneClip, const XMMATRIX &World, const XMMATRIX &ViewProj, float ViewScale)
{
	XMMATRIX WorldInvTranspose, WorldViewProj;

	mTech = PlaneClip ? Effects::BasicFX->Light3TexClipTech : Effects::BasicFX->Light3TexTech;
	mTech->GetDesc(&mTechDesc);
	for (UINT p=0; p < mTechDesc.Passes; ++p)
	{
		ID3DX11EffectPass* Pass = mTech->GetPassByIndex(p);
		// set room buffers
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mPlayerVB, &mStride, &mOffset);
		md3dImmediateContext->IASetIndexBuffer(mPlayerIB, DXGI_FORMAT_R32_UINT, 0);

		// draw the player
		WorldInvTranspose = MathFunctions::InverseTranspose(World);
		WorldViewProj = World * ViewProj;

		Effects::BasicFX->SetWorld(World);
		Effects::BasicFX->SetWorldInvTranspose(WorldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(WorldViewProj);
			Effects::BasicFX->SetViewScale(ViewScale);
		Effects::BasicFX->SetTexTransform(mPlayerTexTransform);
		Effects::BasicFX->SetMaterial(mPlayerMaterial);
		Effects::BasicFX->SetDiffuseMap(mPlayerSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mPlayerIndexCount, 0, 0);
	}
}



void PortalsApp::DrawPortalBox(bool PlaneClip, const XMMATRIX &World, const XMMATRIX &ViewProj)
{
	XMMATRIX WorldViewProj;

	mTech = PlaneClip ? Effects::PortalFX->PortalBoxClipTech : Effects::PortalFX->PortalBoxTech;
	mTech->GetDesc(&mTechDesc);
	for (UINT p=0; p < mTechDesc.Passes; ++p)
	{
		ID3DX11EffectPass* Pass = mTech->GetPassByIndex(p);
		// set portal buffers
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mPortalVB, &mStride, &mOffset);
		md3dImmediateContext->IASetIndexBuffer(mPortalIB, DXGI_FORMAT_R32_UINT, 0);

		// draw portal box
		WorldViewProj = World * ViewProj;
		Effects::PortalFX->SetWorld(World);
		Effects::PortalFX->SetWorldViewProj(WorldViewProj);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mPortalBoxIndexCount, 0, 0);
	}
}

void PortalsApp::DrawPortalBoxClearDepth(const XMMATRIX &World, const XMMATRIX &ViewProj)
{
	XMMATRIX WorldViewProj;

	mTech = Effects::PortalFX->PortalBoxClearDepthTech;
	mTech->GetDesc(&mTechDesc);
	for (UINT p=0; p < mTechDesc.Passes; ++p)
	{
		ID3DX11EffectPass* Pass = mTech->GetPassByIndex(p);
		// set portal buffers
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mPortalVB, &mStride, &mOffset);
		md3dImmediateContext->IASetIndexBuffer(mPortalIB, DXGI_FORMAT_R32_UINT, 0);

		// draw portal box
		WorldViewProj = World * ViewProj;
		Effects::PortalFX->SetWorld(World);
		Effects::PortalFX->SetWorldViewProj(WorldViewProj);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mPortalBoxIndexCount, 0, 0);
	}
}


void PortalsApp::DrawRoomBothPortalsNoHoles(const XMMATRIX &World, const XMMATRIX &ViewProj, float ViewScale)
{	
	XMMATRIX WorldInvTranspose, WorldViewProj;

	mTech = Effects::RoomPortalFX->Light3TexPortalAPortalBNoHolesTech;
	mTech->GetDesc(&mTechDesc);
	for (UINT p=0; p < mTechDesc.Passes; ++p)
	{
		ID3DX11EffectPass* Pass = mTech->GetPassByIndex(p);
		// set room buffers
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mRoomVB, &mStride, &mOffset);
		md3dImmediateContext->IASetIndexBuffer(mRoomIB, DXGI_FORMAT_R32_UINT, 0);

		WorldInvTranspose = MathFunctions::InverseTranspose(World);
		WorldViewProj = World * ViewProj;

		Effects::RoomPortalFX->SetWorld(World);
		Effects::RoomPortalFX->SetWorldInvTranspose(WorldInvTranspose);
		Effects::RoomPortalFX->SetWorldViewProj(WorldViewProj);
			Effects::RoomPortalFX->SetViewScale(ViewScale);
		
		// portal-related settings are set once before drawing anything

		// draw the walls
		Effects::RoomPortalFX->SetTexTransform(mWallsTexTransform);
		Effects::RoomPortalFX->SetMaterial(mWallsMaterial);
		Effects::RoomPortalFX->SetDiffuseMap(mWallsSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mWallsIndexCount, mWallsIBOffset, mWallsVBOffset);

		// draw floor
		Effects::RoomPortalFX->SetTexTransform(mFloorTexTransform);
		Effects::RoomPortalFX->SetMaterial(mFloorMaterial);
		Effects::RoomPortalFX->SetDiffuseMap(mFloorSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mFloorIndexCount, mFloorIBOffset, mFloorVBOffset);


		// draw ceiling
		Effects::RoomPortalFX->SetTexTransform(mCeilingTexTransform);
		Effects::RoomPortalFX->SetMaterial(mCeilingMaterial);
		Effects::RoomPortalFX->SetDiffuseMap(mCeilingSRV);

		Pass->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mCeilingIndexCount, mCeilingIBOffset, mCeilingVBOffset);
	}
}

#pragma endregion DRAW_FUNCTIONS



void PortalsApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Fog));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	
	// calculate how many iterations the world should be virtualized inside each portal
	int OrangePortalIterations = 1;
	int BluePortalIterations = 1;

	bool OrangePortalInFrustum = mLeftCamera.FrustumContainsDisc(mOrangePortal.GetPosition(), mOrangePortal.GetNormal(), 
															mOrangePortal.GetPhysicalRadius());
	bool BluePortalInFrustum = mLeftCamera.FrustumContainsDisc(mBluePortal.GetPosition(), mBluePortal.GetNormal(), 
															mBluePortal.GetPhysicalRadius());
	float OrangePortalVisibility = mLeftCamera.SurfaceVisibilityFactor(mOrangePortal.GetPosition(), mOrangePortal.GetNormal());
	float BluePortalVisibility  = mLeftCamera.SurfaceVisibilityFactor(mBluePortal.GetPosition(), mBluePortal.GetNormal());
	
	bool CanSeeOrange = OrangePortalInFrustum && (OrangePortalVisibility >= 0.0f);
	bool CanSeeBlue = BluePortalInFrustum && (BluePortalVisibility >= 0.0f);

	if (CanSeeOrange && CanSeeBlue)
	{
		OrangePortalIterations = PORTAL_ITERATIONS / 2;
		BluePortalIterations = PORTAL_ITERATIONS - OrangePortalIterations;
	}
	else if (CanSeeOrange)
		OrangePortalIterations = PORTAL_ITERATIONS-1;
	else if (CanSeeBlue)
		BluePortalIterations = PORTAL_ITERATIONS-1;


	


	// set camera-independent per-frame variables (these don't change when rendering either viewport)

	// portal-related
	Effects::RoomPortalFX->SetPortalA(mOrangePortal.GetScaledPortalMatrix());
	Effects::RoomPortalFX->SetPortalATexRadRatio(mOrangePortal.GetTextureRadiusRatio());
	Effects::RoomPortalFX->SetPortalADiffuseMap(mOrangePortalSRV);
	Effects::RoomPortalFX->SetPortalB(mBluePortal.GetScaledPortalMatrix());
	Effects::RoomPortalFX->SetPortalBTexRadRatio(mBluePortal.GetTextureRadiusRatio());
	Effects::RoomPortalFX->SetPortalBDiffuseMap(mBluePortalSRV);



	// RENDER TO LEFT VIEWPORT *************************************************************************************************************
	md3dImmediateContext->RSSetViewports(1, &mScreenViewports[0]);


	// set per-frame variables (none of these should change for this whole frame)
	
	// eyepos
	Effects::RoomPortalFX->SetEyePosW(mLeftCamera.GetPosition());
	Effects::BasicFX->SetEyePosW(mLeftCamera.GetPosition());


	// render room and both portals
	
	Effects::RoomPortalFX->SetDirLights(mDirLights);

	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	DrawRoomBothPortals(false, XMMatrixIdentity(), mLeftViewProj, mLeftViewScale);



	// ***************************************************************************************************************************************************
	// ***************************************************************************************************************************************************
	// rest of rendering steps depends on whether or not the player intersects the portal


	if (mPlayerIntersectOrangePortal || mPlayerIntersectBluePortal)
	{
		Portal *ThisPortal_ptr;
		int ThisPortalIterations;
		UINT ThisPortalStencilRef;

		Portal *OtherPortal_ptr;
		int OtherPortalIterations;
		UINT OtherPortalStencilRef;
		
		if (mPlayerIntersectOrangePortal)
		{
			ThisPortal_ptr = &mOrangePortal;
			ThisPortalIterations = OrangePortalIterations;
			ThisPortalStencilRef = ORANGE_STENCIL_REF;
			OtherPortal_ptr = &mBluePortal;
			OtherPortalIterations = BluePortalIterations;
			OtherPortalStencilRef = BLUE_STENCIL_REF;
		}
		else
		{
			ThisPortal_ptr = &mBluePortal;
			ThisPortalIterations = BluePortalIterations;
			ThisPortalStencilRef = BLUE_STENCIL_REF;
			OtherPortal_ptr = &mOrangePortal;
			OtherPortalIterations = OrangePortalIterations;
			OtherPortalStencilRef =  ORANGE_STENCIL_REF;
		}


		// virtualization matrices
		XMMATRIX ThisVirtualize = Portal::CalculateVirtualizationMatrix(*ThisPortal_ptr, *OtherPortal_ptr);
		XMMATRIX OtherVirtualize = Portal::CalculateVirtualizationMatrix(*OtherPortal_ptr, *ThisPortal_ptr);

		Effects::BasicFX->SetDirLights(mDirLights);
		

		// render portion of player outside of ThisPortal

		Effects::BasicFX->SetClipPlanePosition(ThisPortal_ptr->GetPosition());
		Effects::BasicFX->SetClipPlaneNormal(ThisPortal_ptr->GetNormal());
		Effects::BasicFX->SetClipPlaneOffset(-0.01f);

		//md3dImmediateContext->OMSetDepthStencilState(0, 0);
		DrawPlayer(true, mPlayer.GetWorldMatrix(), mLeftViewProj, mLeftViewScale);


		// render portion of player outside of OtherPortal

		Effects::BasicFX->SetClipPlanePosition(OtherPortal_ptr->GetPosition());
		Effects::BasicFX->SetClipPlaneNormal(OtherPortal_ptr->GetNormal());
		//Effects::BasicFX->SetClipPlaneOffset(-0.01f);

		//md3dImmediateContext->OMSetDepthStencilState(0, 0);
		DrawPlayer(true, mPlayer.GetWorldMatrix() * OtherVirtualize, mLeftViewProj, mLeftViewScale);

		
		// render portal boxes to stencil, and to cover up the hole

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilSetToDSS, ORANGE_STENCIL_REF);
		DrawPortalBox(false, mOrangePortal.GetBoxWorldMatrix(), mLeftViewProj);

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilSetToDSS, BLUE_STENCIL_REF);
		DrawPortalBox(false, mBluePortal.GetBoxWorldMatrix(), mLeftViewProj);


		// render portal insides

		RenderPortalInsidesPlayerClip(*ThisPortal_ptr, ThisVirtualize, XMMatrixIdentity(), XMMatrixIdentity(), ThisPortalStencilRef,
									ThisPortalIterations, mLeftViewProj, mLeftViewScale);


		RenderPortalInsidesPlayerClip(*OtherPortal_ptr, OtherVirtualize, XMMatrixIdentity(), OtherVirtualize, OtherPortalStencilRef,
									OtherPortalIterations, mLeftViewProj, mLeftViewScale);

	}
	// ***************************************************************************************************************************************************
	// ***************************************************************************************************************************************************
	else
	{
		// render player

		Effects::BasicFX->SetDirLights(mDirLights);
		
		//md3dImmediateContext->OMSetDepthStencilState(0, 0);
		DrawPlayer(false, mPlayer.GetWorldMatrix(), mLeftViewProj, mLeftViewScale);

		
		// render portal boxes to stencil, and to cover up the hole

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilSetToDSS, ORANGE_STENCIL_REF);
		DrawPortalBox(false, mOrangePortal.GetBoxWorldMatrix(), mLeftViewProj);

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilSetToDSS, BLUE_STENCIL_REF);
		DrawPortalBox(false, mBluePortal.GetBoxWorldMatrix(), mLeftViewProj);
		
		
		// render orange portal insides

		XMMATRIX OrangeVirtualize = Portal::CalculateVirtualizationMatrix(mOrangePortal, mBluePortal);
		
		RenderPortalInsidesPlayerNoClip(mOrangePortal, OrangeVirtualize, XMMatrixIdentity(), ORANGE_STENCIL_REF,
									OrangePortalIterations, mLeftViewProj, mLeftViewScale);


		// render blue portal insides

		XMMATRIX BlueVirtualize = Portal::CalculateVirtualizationMatrix(mBluePortal, mOrangePortal);
		
		RenderPortalInsidesPlayerNoClip(mBluePortal, BlueVirtualize, XMMatrixIdentity(), BLUE_STENCIL_REF,
									BluePortalIterations, mLeftViewProj, mLeftViewScale);
		
		
	}
	// ***************************************************************************************************************************************************
	// ***************************************************************************************************************************************************
	






	// RENDER TO RIGHT VIEWPORT *********************************************************************************************************
	md3dImmediateContext->RSSetViewports(1, &mScreenViewports[1]);

	// draw room with both portals, but without their holes

	Effects::RoomPortalFX->SetEyePosW(mRightCamera.GetPosition());	
	Effects::RoomPortalFX->SetDirLights(mDirLights);

	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	DrawRoomBothPortalsNoHoles(XMMatrixIdentity(), mRightViewProj, mRightViewScale);




	HR(mSwapChain->Present(0, 0));
}

void PortalsApp::RenderPortalInsidesPlayerNoClip(const Portal &LookThruPortal, const XMMATRIX &Virtualize,
						const XMMATRIX &InitialWorldToVirtual, UINT InitialStencilRef, int Levels,
						const XMMATRIX &ViewProj, float ViewScale)
{
	DirectionalLight VirtualDirLights[3];
	memcpy(VirtualDirLights, mDirLights, 3*sizeof(DirectionalLight));
	XMMATRIX WorldToVirtual = InitialWorldToVirtual;
	Portal CurrentPortal = LookThruPortal;
	UINT StencilRef = InitialStencilRef;

	for (int i=0; i<Levels; ++i)
	{
		// computer worldtovirtual transform for this realm
		WorldToVirtual *= Virtualize;

		// computer dir lights for this realm
		for (unsigned int i=0; i<3; ++i)
		{
			XMVECTOR D;
			D = XMLoadFloat3(&(VirtualDirLights[i].Direction));
			D = XMVector3Normalize(XMVector3TransformNormal(D, Virtualize));
			XMStoreFloat3(&(VirtualDirLights[i].Direction), D);
		}

		// everything in this realm uses the same set of virtual lights
		Effects::RoomPortalFX->SetDirLights(VirtualDirLights);
		Effects::BasicFX->SetDirLights(VirtualDirLights);

		// everything in this realm must be clipped using the plane of the CurrentPortal
		Effects::RoomPortalFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::RoomPortalFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::RoomPortalFX->SetClipPlaneOffset(0.0f);
		Effects::BasicFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::BasicFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::BasicFX->SetClipPlaneOffset(0.0f);
		Effects::PortalFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::PortalFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::PortalFX->SetClipPlaneOffset(0.0f);


		// render current portal box to clear depth of this portal's insides
	
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDepthAlwaysDSS, StencilRef);
		DrawPortalBoxClearDepth(CurrentPortal.GetBoxWorldMatrix(), ViewProj);
	

		// render virtual room with both portals, even though we only need CurrentPortal

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDSS, StencilRef);
		DrawRoomBothPortals(true, WorldToVirtual, ViewProj, ViewScale);

	
		// render virtual player
	
		//md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDSS, StencilRef);
		DrawPlayer(true, mPlayer.GetWorldMatrix() * WorldToVirtual, ViewProj, ViewScale);

	
		// calculate next portal
		CurrentPortal.Transform(Virtualize);

	
		// render next portal box to increment stencil, and to cover up hole

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualIncrementDSS, StencilRef);
		DrawPortalBox(true, CurrentPortal.GetBoxWorldMatrix(), ViewProj);


		++StencilRef;		
	}
}




void PortalsApp::RenderPortalInsidesPlayerClip(const Portal &LookThruPortal, const XMMATRIX &Virtualize,
						const XMMATRIX &InitialWorldToVirtual, const XMMATRIX &InitialPlayerWorldToVirtual,
						UINT InitialStencilRef, int Levels,
						const XMMATRIX &ViewProj, float ViewScale)
{
	DirectionalLight VirtualDirLights[3];
	memcpy(VirtualDirLights, mDirLights, 3*sizeof(DirectionalLight));
	XMMATRIX WorldToVirtual = InitialWorldToVirtual;
	XMMATRIX OldPlayerWorldToVirtual;
	XMMATRIX PlayerWorldToVirtual = InitialPlayerWorldToVirtual;
	Portal CurrentPortal = LookThruPortal;
	UINT StencilRef = InitialStencilRef;

	for (int i=0; i<Levels; ++i)
	{
		// computer worldtovirtual transform for this realm
		WorldToVirtual *= Virtualize;
		OldPlayerWorldToVirtual = PlayerWorldToVirtual;
		PlayerWorldToVirtual *= Virtualize;

		// computer dir lights for this realm
		for (unsigned int i=0; i<3; ++i)
		{
			XMVECTOR D;
			D = XMLoadFloat3(&(VirtualDirLights[i].Direction));
			D = XMVector3Normalize(XMVector3TransformNormal(D, Virtualize));
			XMStoreFloat3(&(VirtualDirLights[i].Direction), D);
		}

		// everything in this realm uses the same set of virtual lights
		Effects::RoomPortalFX->SetDirLights(VirtualDirLights);
		Effects::BasicFX->SetDirLights(VirtualDirLights);

		// everything in this realm must be clipped using the plane of the CurrentPortal
		Effects::RoomPortalFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::RoomPortalFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::RoomPortalFX->SetClipPlaneOffset(0.0f);
		Effects::BasicFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::BasicFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::BasicFX->SetClipPlaneOffset(0.0f);
		Effects::PortalFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::PortalFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::PortalFX->SetClipPlaneOffset(0.0f);


		// render current portal box to clear depth of this portal's insides
	
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDepthAlwaysDSS, StencilRef);
		DrawPortalBoxClearDepth(CurrentPortal.GetBoxWorldMatrix(), ViewProj);
	

		// render virtual room with both portals, even though we only need CurrentPortal

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDSS, StencilRef);
		DrawRoomBothPortals(true, WorldToVirtual, ViewProj, ViewScale);

	
		// render portion of previous virtual player that's in this realm
	
		Effects::BasicFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::BasicFX->SetClipPlaneNormal(-CurrentPortal.GetNormal());
		Effects::BasicFX->SetClipPlaneOffset(-0.01f);

		//md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDSS, StencilRef);
		DrawPlayer(true, mPlayer.GetWorldMatrix() * OldPlayerWorldToVirtual, ViewProj, ViewScale);


		// calculate next portal
		CurrentPortal.Transform(Virtualize);


		// render portion of current virtual player that's in this realm

		Effects::BasicFX->SetClipPlanePosition(CurrentPortal.GetPosition());
		Effects::BasicFX->SetClipPlaneNormal(CurrentPortal.GetNormal());
		//Effects::BasicFX->SetClipPlaneOffset(-0.01f);

		//md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualDSS, StencilRef);
		DrawPlayer(true, mPlayer.GetWorldMatrix() * PlayerWorldToVirtual, ViewProj, ViewScale);

	
		// render next portal box to increment stencil, and to cover up hole

		md3dImmediateContext->OMSetDepthStencilState(RenderStates::StencilEqualIncrementDSS, StencilRef);
		DrawPortalBox(true, CurrentPortal.GetBoxWorldMatrix(), ViewProj);


		++StencilRef;		
	}
}



void PortalsApp::BuildRoomGeometryBuffers()
{
	// get mesh data from room
	GeometryGenerator::MeshData RoomMesh;
	mRoom.BuildMeshData(RoomMesh,
					&mWallsIndexCount, &mWallsIBOffset, &mWallsVBOffset,
					&mFloorIndexCount, &mFloorIBOffset, &mFloorVBOffset, 
					&mCeilingIndexCount, &mCeilingIBOffset, &mCeilingVBOffset);

	// copy info over to list of Basic32 vertices
	std::vector<Vertex::Basic32> Basic32Vertices(RoomMesh.Vertices.size());
	for (unsigned int i=0; i<Basic32Vertices.size(); ++i)
	{
		Basic32Vertices[i].normal = RoomMesh.Vertices[i].Normal;
		Basic32Vertices[i].pos = RoomMesh.Vertices[i].Position;
		Basic32Vertices[i].tex = RoomMesh.Vertices[i].TexCoord;
	}

	// create vertex buffer
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * RoomMesh.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &Basic32Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mRoomVB));

	// create index buffer
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * RoomMesh.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &RoomMesh.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mRoomIB));
}

void PortalsApp::BuildPlayerGeometryBuffers()
{
	GeometryGenerator::MeshData PlayerMesh;
	GeometryGenerator::GenerateSphere(PlayerMesh, 1.0f, 3, &mPlayerIndexCount);

	// copy info over to list of Basic32 vertices
	std::vector<Vertex::Basic32> Basic32Vertices(PlayerMesh.Vertices.size());
	for (unsigned int i=0; i<Basic32Vertices.size(); ++i)
	{
		Basic32Vertices[i].normal = PlayerMesh.Vertices[i].Normal;
		Basic32Vertices[i].pos = PlayerMesh.Vertices[i].Position;
		Basic32Vertices[i].tex = PlayerMesh.Vertices[i].TexCoord;
	}

	// create vertex buffer
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * PlayerMesh.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &Basic32Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mPlayerVB));

	// create index buffer
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * PlayerMesh.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &PlayerMesh.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mPlayerIB));
}

void PortalsApp::BuildPortalGeometryBuffers()
{
	GeometryGenerator::MeshData PortalMesh;
	Portal::BuildMeshData(PortalMesh, &mPortalBoxIndexCount);

	// copy info over to list of Basic32 vertices
	std::vector<Vertex::Basic32> Basic32Vertices(PortalMesh.Vertices.size());
	for (unsigned int i=0; i<Basic32Vertices.size(); ++i)
	{
		// only Positions are generated by Portal::BuildMeshData. all else is 0s
		Basic32Vertices[i].normal = PortalMesh.Vertices[i].Normal;
		Basic32Vertices[i].pos = PortalMesh.Vertices[i].Position;
		Basic32Vertices[i].tex = PortalMesh.Vertices[i].TexCoord;
	}

	// create vertex buffer
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * PortalMesh.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &Basic32Vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mPortalVB));

	// create index buffer
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * PortalMesh.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &PortalMesh.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mPortalIB));
}