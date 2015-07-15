#include "Camera.h"


Camera::Camera()
	: FovY(PI/4.0f), Aspect(1.0f), Near(0.01f), Far(1000.0f), ViewScale(1.0f),
	AttachedTo(0)
{
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	BodyUp = Up;

	UpdateProjMatrix();
}

Camera::~Camera()
{
}

XMFLOAT3 Camera::GetPosition()const
{
	return this->Position;
}
XMFLOAT3 Camera::GetRight()const
{
	return this->Right;
}
XMFLOAT3 Camera::GetUp()const
{
	return this->Up;
}
XMFLOAT3 Camera::GetLook()const
{
	return this->Look;
}
XMFLOAT3 Camera::GetBodyUp()const
{
	return this->BodyUp;
}

float Camera::GetViewScale()const
{
	return this->ViewScale;
}

float Camera::GetBoundingSphereRadius()const
{
	if (AttachedTo == 0)
		return CAMERA_SPHERE_RADIUS * ViewScale;
	return AttachedTo->GetBoundingSphereRadius();
}

XMMATRIX Camera::GetViewMatrix()const
{
	// calculate R, U, L axes scaled by ViewScale factor
	XMVECTOR R = XMLoadFloat3(&Right);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR L = XMLoadFloat3(&Look);

	XMVECTOR P = XMLoadFloat3(&Position);
	float tR = -XMVectorGetX(XMVector3Dot(P, R));
	float tU = -XMVectorGetX(XMVector3Dot(P, U));
	float tL = -XMVectorGetX(XMVector3Dot(P, L));

	return XMMATRIX(	Right.x,		Up.x,			Look.x,			0.0f,
						Right.y,		Up.y,			Look.y,			0.0f,
						Right.z,		Up.z,			Look.z,			0.0f,
						tR,				tU,				tL,				ViewScale	);

}

XMMATRIX Camera::GetProjMatrix()const
{
	return this->ProjMatrix;
}

void Camera::Orthonormalize()
{
	XMVECTOR R = XMLoadFloat3(&Right);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR L = XMLoadFloat3(&Look);

	// orthonormalize camera space axes
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));
	R = XMVector3Cross(U, L);
	XMStoreFloat3(&Right, R);
	XMStoreFloat3(&Up, U);
	XMStoreFloat3(&Look, L);

	if (AttachedTo)
		AttachedTo->SetOrientation(Right, Up, Look);
}

void Camera::UpdateProjMatrix()
{
	float h = tanf(FovY/2.0f);
	ProjMatrix = XMMATRIX(	1.0f/(Aspect*h),	0.0f,			0.0f,					0.0f,
							0.0f,				1.0f/h,			0.0f,					0.0f,
							0.0f,				0.0f,			Far/(Far-Near),			1.0f,
							0.0f,				0.0f,			Near*Far/(Near-Far),	0.0f	
						);
}


void Camera::SetPosition(XMFLOAT3 Position)
{
	this->Position = Position;
	if (AttachedTo)
		AttachedTo->SetPosition(Position);
}
void Camera::SetLens(float Near, float Far, float FovY)
{
	this->Near = Near;
	this->Far = Far;
	this->FovY = FovY;
	UpdateProjMatrix();
}
void Camera::SetAspect(float Aspect)
{
	this->Aspect = Aspect;
	UpdateProjMatrix();
}

void Camera::LookAtAndLevel(XMFLOAT3 Target)
{
	XMVECTOR P = XMLoadFloat3(&Position);
	XMVECTOR T = XMLoadFloat3(&Target);
	XMVECTOR L = XMVector3Normalize(T-P);
	XMStoreFloat3(&Look, L);
	Level();
}

void Camera::Level()
{
	XMVECTOR BU = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	XMVECTOR L = XMLoadFloat3(&Look);

	XMVECTOR R;
	XMVECTOR BUCrossL = XMVector3Cross(BU, L);
	if (XMVectorGetX(XMVector3Length(BUCrossL)) == 0.0f)
	{
		R = XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f));
	}
	else
	{
		R = XMVector3Normalize(BUCrossL);
	}
	
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&Look, L);
	XMStoreFloat3(&Right, R);
	XMStoreFloat3(&Up, U);
	XMStoreFloat3(&BodyUp, BU);

	if (AttachedTo)
		AttachedTo->SetOrientation(Right, Up, Look);
}

void Camera::RotateRight(float Angle)
{
	XMVECTOR R = XMLoadFloat3(&Right);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR L = XMLoadFloat3(&Look);
	XMVECTOR BU = XMLoadFloat3(&BodyUp);

	XMMATRIX M = XMMatrixRotationAxis(BU, Angle);

	XMStoreFloat3(&Right, XMVector3TransformNormal(R, M));
	XMStoreFloat3(&Up, XMVector3TransformNormal(U, M));
	XMStoreFloat3(&Look, XMVector3TransformNormal(L, M));

	if (AttachedTo)
		AttachedTo->SetOrientation(Right, Up, Look);
}
void Camera::RotateUp(float Angle)
{
	XMVECTOR BU = XMLoadFloat3(&BodyUp);
	XMVECTOR L = XMLoadFloat3(&Look);
	XMVECTOR U = XMLoadFloat3(&Up);
	
	float sin = sinf(Angle);
	float cos = cosf(Angle);

	XMVECTOR Lnew = cos*L + sin*U;
	XMVECTOR Unew = -sin*L + cos*U;
	if (XMVectorGetX(XMVector3Dot(Unew, BU)) < 0.0f)
	{
		Lnew = (XMVectorGetX(XMVector3Dot(Lnew, BU))>0.0f) ? BU : -BU;

		// works for looking straight up, but not down
		XMVECTOR R = XMLoadFloat3(&Right);
		Unew = XMVector3Cross(R, Lnew);
	}

	XMStoreFloat3(&Look, Lnew);
	XMStoreFloat3(&Up, Unew);

	if (AttachedTo)
		AttachedTo->SetOrientation(Right, Up, Look);
}

void Camera::RollRight(float Angle)
{
	// rotate R,U,L and BU around BL = R x BU
	XMVECTOR BU = XMLoadFloat3(&BodyUp);
	XMVECTOR R = XMLoadFloat3(&Right);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR L = XMLoadFloat3(&Look);

	// rotation matrix around R x BU
	XMMATRIX M = XMMatrixRotationAxis(XMVector3Cross(R, BU), -Angle);

	XMStoreFloat3(&BodyUp, XMVector3TransformNormal(BU, M));
	XMStoreFloat3(&Right, XMVector3TransformNormal(R, M));
	XMStoreFloat3(&Up, XMVector3TransformNormal(U, M));
	XMStoreFloat3(&Look, XMVector3TransformNormal(L, M));

	if (AttachedTo)
		AttachedTo->SetOrientation(Right, Up, Look);
}



XMFLOAT3 Camera::MoveForward(float Dist)
{
	XMVECTOR P = XMLoadFloat3(&Position);
	P += Dist*XMLoadFloat3(&Look);
	XMStoreFloat3(&Position, P);
	if (AttachedTo)
		AttachedTo->SetPosition(Position);
	return Position;
}

XMFLOAT3 Camera::MoveRight(float Dist)
{
	XMVECTOR P = XMLoadFloat3(&Position);
	P += Dist*XMLoadFloat3(&Right);
	XMStoreFloat3(&Position, P);
	if (AttachedTo)
		AttachedTo->SetPosition(Position);
	return Position;
}

XMFLOAT3 Camera::MoveUp(float Dist)
{
	XMVECTOR P = XMLoadFloat3(&Position);
	P += Dist*XMLoadFloat3(&BodyUp);
	XMStoreFloat3(&Position, P);
	if (AttachedTo)
		AttachedTo->SetPosition(Position);
	return Position;
}



void Camera::Transform(const XMMATRIX &M)
{
	XMVECTOR R = XMLoadFloat3(&Right);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR L = XMLoadFloat3(&Look);
	XMVECTOR BU = XMLoadFloat3(&BodyUp);
	XMVECTOR P = XMLoadFloat3(&Position);

	R = XMVector3Normalize(XMVector3TransformNormal(R, M));
	U = XMVector3Normalize(XMVector3TransformNormal(U, M));
	L = XMVector3TransformNormal(L, M);
	MultiplyViewScale(XMVectorGetX(XMVector3Length(L)));
	L = XMVector3Normalize(L);
	BU = XMVector3TransformNormal(BU, M);

	P = XMVector3TransformCoord(P, M);
	
	XMStoreFloat3(&Right, R);
	XMStoreFloat3(&Up, U);
	XMStoreFloat3(&Look, L);
	XMStoreFloat3(&BodyUp, BU);
	XMStoreFloat3(&Position, P);

	if (AttachedTo)
	{
		AttachedTo->SetPosition(Position);
		AttachedTo->SetOrientation(Right, Up, Look);
	}
}

float Camera::MultiplyViewScale(float multiplier)
{
	ViewScale *= multiplier;
	// scale the radius of the attached object by multiplier as well, if there is one.
	if (AttachedTo)
		AttachedTo->SetBoundingSphereRadius(AttachedTo->GetBoundingSphereRadius() * multiplier);
	return ViewScale;
}



void Camera::AttachToObject(FirstPersonObject *Object)
{
	this->AttachedTo = Object;
	
	this->Position = Object->GetPosition();
	this->Right = Object->GetRight();
	this->Up = Object->GetUp();
	this->Look = Object->GetLook();
}

FirstPersonObject* Camera::DetachFromObject()
{
	FirstPersonObject* Object = this->AttachedTo;
	this->AttachedTo = 0;
	return Object;
}

float Camera::SurfaceVisibilityFactor(XMFLOAT3 SurfacePoint, XMFLOAT3 SurfaceNormal)const
{
	XMVECTOR P = XMLoadFloat3(&SurfacePoint);
	XMVECTOR N = XMLoadFloat3(&SurfaceNormal);
	XMVECTOR E = XMLoadFloat3(&Position);
	
	return XMVectorGetX(XMVector3Dot(N, XMVector3Normalize(E-P)));
}


XMFLOAT3 Camera::SelfVirtualCollision(const XMMATRIX &Virtualize, float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)const
{
	// defaults
	*XDist_ptr = MoveDist;
	*RedirectRatio_ptr = 1.0f;
	*RedirectDir_ptr = Dir;

	// if this camera has no attached object, it cannot collide with itself
	if (!AttachedTo)
		return S + MoveDist*Dir;

	// calculate virtual S, virtual Dir, virtual MoveDist 
	XMFLOAT3 Sv;
	XMFLOAT3 Dirv;
	XMVECTOR SVirtual = XMVector3TransformCoord(XMLoadFloat3(&S), Virtualize);
	XMVECTOR DirVirtual = XMVector3TransformNormal(XMLoadFloat3(&Dir), Virtualize);
	XMStoreFloat3(&Sv, SVirtual);
	XMStoreFloat3(&Dirv, DirVirtual);
	// NOTE: do not normalize Dirv and do not scale MoveDistv

	// the virtual sphereradius is scaled by DirvLength as well
	float SphereRadiusv = SphereRadius * XMFloat3Length(Dirv);

	// find collision
	XMFLOAT3 g = S-Sv;
	XMFLOAT3 h = Dir-Dirv;
	float rsum = SphereRadius+SphereRadiusv;
	// quadratic coefficients	
	float a = XMFloat3LengthSq(h);
	float b_half = XMFloat3Dot(g, h);
	float c = XMFloat3LengthSq(g) - rsum*rsum;

	float discr_over_4 = b_half*b_half - a*c;		// discriminant/2
	if (discr_over_4 <= 0.0f)
		return S + MoveDist*Dir;

	// find smaller root for t
	float t = (-b_half - sqrtf(discr_over_4)) / a;
	if (t < -T_THRESHOLD || t > MoveDist)
		return S + MoveDist*Dir;

	// collision occurs
	*RedirectRatio_ptr = 0.0f;	// no redirects for self-collision
	*XDist_ptr = t;

	// bump t, calculate X
	t -= T_BUMP;
	return S + t*Dir;
}




bool Camera::FrustumContainsSegment2D(const XMFLOAT2 &LDir, const XMFLOAT2 &RDir, 
								const XMFLOAT2 &A, const XMFLOAT2 &B) // L->R is CW (left to right)
{
	//dprintf("		Segment of intersection: (%f, %f)--(%f, %f)\n", A.x,A.y,B.x,B.y);
	//dprintf("		L ray direction: (%f, %f)\n", LDir.x, LDir.y);
	//dprintf("		R ray direction: (%f, %f)\n", RDir.x, RDir.y);

	// check if either endpoint is inside both rays
	if ( (XMFloat2Cross(RDir, A) > 0.0f && XMFloat2Cross(A, LDir) > 0.0f) ||
		 (XMFloat2Cross(RDir, B) > 0.0f && XMFloat2Cross(B, LDir) > 0.0f) )
	{
		return true;
	}

	// both endpoints are outside the rays.  for this segment to intersect the area between the rays,
	// it must intersect both rays.  So we'll check L for ray-segment intersection
	
	// if A and B are on the same side of L, no intersection occurs
	float LDirCrossA = XMFloat2Cross(LDir, A);
	float LDirCrossB = XMFloat2Cross(LDir, B);
	if ( (LDirCrossA <= 0.0f && LDirCrossB <= 0.0f) || (LDirCrossA >= 0.0f && LDirCrossB >= 0.0f) )
		return false;


	// point of intersection is X = t*LDir = A + u*(B-A)
	// solve for t
	float t = XMFloat2Cross(A, B) / XMFloat2Cross(LDir, B-A);
	return (t > 0);
}



bool Camera::FrustumContainsDisc(XMFLOAT3 DiscCenter, XMFLOAT3 DiscNormal, float DiscRadius)const
{
	// if not, check for intersections with 4 frustum planes
	XMVECTOR C = XMLoadFloat3(&DiscCenter);
	XMVECTOR N = XMLoadFloat3(&DiscNormal);
	XMMATRIX View = GetViewMatrix();
	C = ViewScale * XMVector3TransformCoord(C, View);	// convert disccenter and discnormal to view space
	N = XMVector3TransformNormal(N, View);
	float HalfFovY = FovY / 2.0f;
	float HalfFovX = atanf(tanf(HalfFovY)*Aspect);


	// left plane
	XMVECTOR LeftNP = XMLoadFloat3(&XMFLOAT3(cosf(HalfFovX), 0.0f, sinf(HalfFovX)));
	float DiscCenterDistToLeftPlane = XMVectorGetX(XMVector3Dot(C, LeftNP));
	float DiscRadiusTowardsLeftPlane = DiscRadius * XMVectorGetX(XMVector3Length(XMVector3Cross(N, LeftNP)));
	if (DiscCenterDistToLeftPlane <= -DiscRadiusTowardsLeftPlane)
	{
		//dprintf("disc wrong side of left plane\n");
		return false;
	}

	// right plane
	XMVECTOR RightNP = XMLoadFloat3(&XMFLOAT3(-cosf(HalfFovX), 0.0f, sinf(HalfFovX)));
	float DiscCenterDistToRightPlane = XMVectorGetX(XMVector3Dot(C, RightNP));
	float DiscRadiusTowardsRightPlane = DiscRadius * XMVectorGetX(XMVector3Length(XMVector3Cross(N, RightNP)));
	if (DiscCenterDistToRightPlane <= -DiscRadiusTowardsRightPlane)
	{
		//dprintf("disc wrong side of right plane\n");
		return false;
	}

	// bottom plane
	XMVECTOR BottomNP = XMLoadFloat3(&XMFLOAT3(0.0f, cosf(HalfFovY), sinf(HalfFovY)));
	float DiscCenterDistToBottomPlane = XMVectorGetX(XMVector3Dot(C, BottomNP));
	float DiscRadiusTowardsBottomPlane = DiscRadius * XMVectorGetX(XMVector3Length(XMVector3Cross(N, BottomNP)));
	if (DiscCenterDistToBottomPlane <= -DiscRadiusTowardsBottomPlane)
	{
		//dprintf("disc wrong side of bottom plane\n");
		return false;
	}

	// top plane
	XMVECTOR TopNP = XMLoadFloat3(&XMFLOAT3(0.0f, -cosf(HalfFovY), sinf(HalfFovY)));
	float DiscCenterDistToTopPlane = XMVectorGetX(XMVector3Dot(C, TopNP));
	float DiscRadiusTowardsTopPlane = DiscRadius * XMVectorGetX(XMVector3Length(XMVector3Cross(N, TopNP)));
	if (DiscCenterDistToTopPlane <= -DiscRadiusTowardsTopPlane)
	{
		//dprintf("disc wrong side of top plane\n");
		return false;
	}


	// now check if the disc is completely inside all planes, ie completely inside frustum
	if ( (DiscCenterDistToLeftPlane >= DiscRadiusTowardsLeftPlane &&
			DiscCenterDistToRightPlane >= DiscRadiusTowardsRightPlane) &&
			(DiscCenterDistToBottomPlane >= DiscRadiusTowardsBottomPlane &&
			DiscCenterDistToTopPlane >= DiscRadiusTowardsTopPlane ) )
	{
		//dprintf("disc is completely inside all 4 planes\n");
		return true;
	}


	// at this point, the disc must intersect the frustum somewhere.

	// for each plane that the disc intersects, find the line segment of intersection with that plane
	// for this disc to be partially in the frustum, at least one of these line segments of intersection
	// has to be inside or partially inside the frustum rays of that plane
	


	XMVECTOR G,H,Q1,Q2;
	XMFLOAT2 A, B;
	float a;

	// frustum ray directions
	float Tan_HalfFovY = tanf(HalfFovY);
	XMVECTOR TopLeftRay = XMLoadFloat3(&XMFLOAT3(-Aspect*Tan_HalfFovY, Tan_HalfFovY, 1.0f));


	// check intersection in left and right frustum planes

	
	// calculate 2D top and bottom rays in the ZY plane. 
	XMMATRIX M = XMMatrixRotationY(HalfFovX);	// rotate CW around Y axis to YZ plane
	XMVECTOR TopRayYZ = XMVector3TransformNormal(TopLeftRay, M);
	XMFLOAT2 TopRay = XMFLOAT2(XMVectorGetZ(TopRayYZ), XMVectorGetY(TopRayYZ));
	XMFLOAT2 BottomRay = XMFLOAT2(TopRay.x, -TopRay.y);

	// Left Plane
	if (abs(DiscCenterDistToLeftPlane) < DiscRadiusTowardsLeftPlane)
	{
		//dprintf("Left plane intersected\n");

		// find disc intersection segment endpoints with the left plane
		XMVECTOR LeftM = XMVector3Cross(LeftNP, N);
		a = DiscCenterDistToLeftPlane / XMVectorGetX(XMVector3Length(LeftM));
		LeftM = XMVector3Normalize(LeftM);
		G = C + a*(XMVector3Cross(LeftM, N));
		H = sqrtf(DiscRadius*DiscRadius - a*a) * LeftM;
		Q1 = G - H;
		Q2 = G + H;

		// transform Q1 and Q2 to ZY plane
		Q1 = XMVector3TransformCoord(Q1, M);
		Q2 = XMVector3TransformCoord(Q2, M);

		A = XMFLOAT2(XMVectorGetZ(Q1), XMVectorGetY(Q1));
		B = XMFLOAT2(XMVectorGetZ(Q2), XMVectorGetY(Q2));

		if (FrustumContainsSegment2D(TopRay, BottomRay, A, B))
		{
			//dprintf("			Left plane 2D frustum intersects disc.\n");
			return true;
		}
	}

	// Right Plane
	if (abs(DiscCenterDistToRightPlane) < DiscRadiusTowardsRightPlane)
	{
		//dprintf("Right plane intersected\n");


		// find disc intersection segment endpoints with the Right plane
		XMVECTOR RightM = XMVector3Cross(RightNP, N);
		a = DiscCenterDistToRightPlane / XMVectorGetX(XMVector3Length(RightM));
		RightM = XMVector3Normalize(RightM);
		G = C + a*(XMVector3Cross(RightM, N));
		H = sqrtf(DiscRadius*DiscRadius - a*a) * RightM;
		Q1 = G - H;
		Q2 = G + H;

		// transform Q1 and Q2 to ZY plane, now rotating CCW around Y.
		M = XMMatrixRotationY(-HalfFovX);
		Q1 = XMVector3TransformCoord(Q1, M);
		Q2 = XMVector3TransformCoord(Q2, M);

		A = XMFLOAT2(XMVectorGetZ(Q1), XMVectorGetY(Q1));
		B = XMFLOAT2(XMVectorGetZ(Q2), XMVectorGetY(Q2));

		if (FrustumContainsSegment2D(TopRay, BottomRay, A, B))
		{
			//dprintf("			Right plane 2D frustum intersects disc.\n");
			return true;
		}
	}


	// Check intersection in the 

	// calculate 2D top and bottom rays in the ZY plane. 
	M = XMMatrixRotationX(HalfFovY);	// rotate CW around X axis to XZ plane
	XMVECTOR LeftRayXZ = XMVector3TransformNormal(TopLeftRay, M);
	XMFLOAT2 LeftRay = XMFLOAT2(XMVectorGetX(LeftRayXZ), XMVectorGetZ(LeftRayXZ));
	XMFLOAT2 RightRay = XMFLOAT2(-LeftRay.x, LeftRay.y);


	// top plane
	if (abs(DiscCenterDistToTopPlane) < DiscRadiusTowardsTopPlane)
	{
		//dprintf("Top plane intersected\n");

		// find disc intersection segment endpoints with the Top plane
		XMVECTOR TopM = XMVector3Cross(TopNP, N);
		a = DiscCenterDistToTopPlane / XMVectorGetX(XMVector3Length(TopM));
		TopM = XMVector3Normalize(TopM);
		G = C + a*(XMVector3Cross(TopM, N));
		H = sqrtf(DiscRadius*DiscRadius - a*a) * TopM;
		Q1 = G - H;
		Q2 = G + H;

		// transform Q1 and Q2 to XZ plane
		Q1 = XMVector3TransformCoord(Q1, M);
		Q2 = XMVector3TransformCoord(Q2, M);

		A = XMFLOAT2(XMVectorGetX(Q1), XMVectorGetZ(Q1));
		B = XMFLOAT2(XMVectorGetX(Q2), XMVectorGetZ(Q2));

		if (FrustumContainsSegment2D(LeftRay, RightRay, A, B))
		{
			//dprintf("			Top plane 2D frustum intersects disc.\n");
			return true;
		}
	}

	// bottom plane
	if (abs(DiscCenterDistToBottomPlane) < DiscRadiusTowardsBottomPlane)
	{
		//dprintf("Bottom plane intersected\n");


		// find disc intersection segment endpoints with the Bottom plane
		XMVECTOR BottomM = XMVector3Cross(BottomNP, N);
		a = DiscCenterDistToBottomPlane / XMVectorGetX(XMVector3Length(BottomM));
		BottomM = XMVector3Normalize(BottomM);
		G = C + a*(XMVector3Cross(BottomM, N));
		H = sqrtf(DiscRadius*DiscRadius - a*a) * BottomM;
		Q1 = G - H;
		Q2 = G + H;

		// transform Q1 and Q2 to ZY plane, now rotating CCW around X.
		M = XMMatrixRotationX(-HalfFovY);
		Q1 = XMVector3TransformCoord(Q1, M);
		Q2 = XMVector3TransformCoord(Q2, M);

		A = XMFLOAT2(XMVectorGetX(Q1), XMVectorGetZ(Q1));
		B = XMFLOAT2(XMVectorGetX(Q2), XMVectorGetZ(Q2));

		if (FrustumContainsSegment2D(LeftRay, RightRay, A, B))
		{
			//dprintf("			Bottom plane 2D frustum intersects disc.\n");
			return true;
		}
	}

	//dprintf("Disc intersects one of the 4 planes, but not any of the 2D frustums\n");
	return false;
}
