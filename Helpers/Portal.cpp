#include "Portal.h"


Portal::Portal()
	: PhysicalRadius(1.0f), IntendedPhysicalRadius(1.0f), MaxPhysicalRadius(std::numeric_limits<float>::infinity()),
	TextureRadiusRatio(1.0f)
{
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	Left = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

Portal::~Portal()
{
}


Portal& Portal::operator=(const Portal &rhs)
{
	memcpy(this, &rhs, sizeof(Portal));
	return *this;
}

void Portal::SetTextureRadiusRatio(float TextureRadiusRatio)
{
	this->TextureRadiusRatio = TextureRadiusRatio;
}

void Portal::SetPosition(XMFLOAT3 Position)
{
	this->Position = Position;
}

void Portal::SetNormalAndUp(XMFLOAT3 Normal, XMFLOAT3 Up)
{
	this->Normal = Normal;
	this->Up = Up;
	XMVECTOR N = XMLoadFloat3(&Normal);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMStoreFloat3(&(this->Left), XMVector3Cross(U, N));
}

/*
void Portal::SetPhysicalRadius(float PhysicalRadius)
{
	this->PhysicalRadius = PhysicalRadius;
}*/
void Portal::SetIntendedPhysicalRadius(float IntendedPhysicalRad)
{
	this->IntendedPhysicalRadius = max(IntendedPhysicalRad, PORTAL_MIN_PHYS_RADIUS);
	this->PhysicalRadius = min(this->IntendedPhysicalRadius, this->MaxPhysicalRadius);
}
void Portal::SetMaxPhysicalRadius(float MaxPhysicalRad)
{
	this->MaxPhysicalRadius = max(MaxPhysicalRad, PORTAL_MIN_PHYS_RADIUS);
	this->PhysicalRadius = min(this->IntendedPhysicalRadius, this->MaxPhysicalRadius);
}
/*
void Portal::SetTextureRadius(float TextureRadius)
{
	this->PhysicalRadius = TextureRadius / this->TextureRadiusRatio;
}*/
void Portal::SetIntendedTextureRadius(float IntendedTextureRad)
{
	this->IntendedPhysicalRadius = max(IntendedTextureRad/this->TextureRadiusRatio, PORTAL_MIN_PHYS_RADIUS);
	this->PhysicalRadius = min(this->IntendedPhysicalRadius, this->MaxPhysicalRadius);
}
void Portal::SetMaxTextureRadius(float MaxTextureRad)
{
	this->MaxPhysicalRadius = max(MaxTextureRad/this->TextureRadiusRatio, PORTAL_MIN_PHYS_RADIUS);
	this->PhysicalRadius = min(this->IntendedPhysicalRadius, this->MaxPhysicalRadius);
}

void Portal::RotateLeftAroundNormal(float Angle)
{
	XMVECTOR L = XMLoadFloat3(&Left);
	XMVECTOR U = XMLoadFloat3(&Up);

	float cos = cosf(Angle);
	float sin = sinf(Angle);

	XMStoreFloat3(&Left, cos*L - sin*U);
	XMStoreFloat3(&Up, sin*L + cos*U);
}




void Portal::Transform(const XMMATRIX &M)
{
	XMVECTOR L = XMLoadFloat3(&Left);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR N = XMLoadFloat3(&Normal);
	XMVECTOR P = XMLoadFloat3(&Position);

	L = XMVector3TransformNormal(L, M);
	PhysicalRadius *= XMVectorGetX(XMVector3Length(L));
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3TransformNormal(U, M));
	N = XMVector3Normalize(XMVector3TransformNormal(N, M));

	P = XMVector3TransformCoord(P, M);
	
	XMStoreFloat3(&Left, L);
	XMStoreFloat3(&Up, U);
	XMStoreFloat3(&Normal, N);
	XMStoreFloat3(&Position, P);
}

void Portal::Flip()
{
	Left = -Left;
	Normal = -Normal;
}

XMFLOAT3 Portal::GetLeft()const
{
	return this->Left;
}
XMFLOAT3 Portal::GetUp()const
{
	return this->Up;
}
XMFLOAT3 Portal::GetNormal()const
{
	return this->Normal;
}
XMFLOAT3 Portal::GetPosition()const
{
	return this->Position;
}

float Portal::GetPhysicalRadius()const
{
	return this->PhysicalRadius;
}
float Portal::GetIntendedPhysicalRadius()const
{
	return this->IntendedPhysicalRadius;
}
float Portal::GetMaxPhysicalRadius()const
{
	return this->MaxPhysicalRadius;
}

float Portal::GetTextureRadius()const
{
	return this->PhysicalRadius * this->TextureRadiusRatio;
}
float Portal::GetTextureRadiusRatio()const
{
	return this->TextureRadiusRatio;
}
float Portal::GetIntendedTextureRadius()const
{
	return this->IntendedPhysicalRadius * this->TextureRadiusRatio;
}
float Portal::GetMaxTextureRadius()const
{
	return this->MaxPhysicalRadius * this->TextureRadiusRatio;
}


bool Portal::DiscContainsPoint(XMFLOAT3 Point)const
{
	// if Point is in plane of portal and within PhysicalRadius of portal center, return true
	XMVECTOR P = XMLoadFloat3(&Point);
	XMVECTOR C = XMLoadFloat3(&Position);
	if (XMVectorGetX(XMVector3Length(P-C)) <= PhysicalRadius)
	{
		XMVECTOR N = XMLoadFloat3(&Normal);
		return (abs(XMVectorGetX(XMVector3Dot(P-C, N))) <= DISC_CONTAINS_THRESHOLD);
	}
	return false;
}

// being tangent to portal plane does not count as intersection
bool Portal::DiscIntersectSphere(XMFLOAT3 Center, float Radius)const
{
	XMVECTOR D = XMLoadFloat3(&Center);
	XMVECTOR C = XMLoadFloat3(&Position);
	XMVECTOR N = XMLoadFloat3(&Normal);

	float DistToPortalPlane = XMVectorGetX(XMVector3Dot(D-C, N));
	if (DistToPortalPlane >= Radius)
		return false;

	// project sphere center onto portal plane
	XMVECTOR E = D - DistToPortalPlane * N;
	float SpherePlaneIntersectionRadius = sqrtf(Radius*Radius - DistToPortalPlane*DistToPortalPlane);

	// portal disc and SpherePlaneIntersectionDisc overlap if sum of their radii is bigger
	// than distance between their centers
	return ( PhysicalRadius + SpherePlaneIntersectionRadius > XMVectorGetX(XMVector3Length(E-C)) );
}




XMFLOAT3 Portal::SpherePathCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)const
{
	// defaults
	*XDist_ptr = MoveDist;
	*RedirectRatio_ptr = 1.0f;
	*RedirectDir_ptr = Dir;

	// transform path to portal space
	XMFLOAT3 Sp;
	XMFLOAT3 Dirp;
	
	XMMATRIX M = GetPortalMatrix();
	XMStoreFloat3(&Sp, XMVector3TransformCoord(XMLoadFloat3(&S), M));
	XMStoreFloat3(&Dirp, XMVector3TransformNormal(XMLoadFloat3(&Dir), M));


	// NOTE: IN PORTAL SPACE, THE PORTAL IS IN THE XY-PLANE.

	// find t values where the sphere touches the ring
	// ray-torus intersection

	float R_sq = PhysicalRadius*PhysicalRadius;
	float r_sq = SphereRadius*SphereRadius;

	//float alpha = 1.0f;
	float beta = 2.0f * XMFloat3Dot(Sp, Dirp);
	float gamma = XMFloat3LengthSq(Sp) - r_sq - R_sq;

	// quartic equation coefficients
	// at^4+bt^3+ct^2+dt+e=0
	float coeffs[5];
	coeffs[0] = 1.0f;												// a
	coeffs[1] = 2.0f*beta;											// b
	coeffs[2] = beta*beta + 2.0f*gamma + 4.0f*R_sq*Dirp.z*Dirp.z;	// c
	coeffs[3] = 2.0f*beta*gamma + 8.0f*R_sq*Sp.z*Dirp.z;			// d
	coeffs[4] = gamma*gamma + 4.0f*R_sq*(Sp.z*Sp.z - r_sq);			// e

	// solve for point of collision of path with torus
	float t = MoveDist;
	FindLowestQuarticRootInInterval(coeffs, -T_THRESHOLD, MoveDist, &t);


	// spheres larger than the ring oftentimes slip thru the ring in the middle due to the large t errors
	// resulting from the sharp "pit" in the torus.  we will check for this
	if (SphereRadius > PhysicalRadius && Dirp.z != 0.0f)
	{
		// see if path ray intersects the disc of radius 2R, at height h = sqrt(r^2-R^2)
		// this height is the tip of the torus pit
		float h = sqrtf(r_sq - R_sq);

		// find where this path intersects with the plane at height +-h, depending on Dirp
		float t2;	// Sp.z + t2*Dirp.z = +- h
		if (Dirp.z  < 0.0f)	// path heading downwards
			t2 = (h - Sp.z) / Dirp.z;
		else		// path heading upwards
			t2 = (-h - Sp.z) / Dirp.z;

		// does a closer intersection occur?
		if (-T_THRESHOLD < t2 && t2 < t)
		{
			// find intersection point
			XMFLOAT3 Xp2 = Sp + t2*Dirp;

			// is the intersection point inside circle of radius 2R with origin as center?
			if (sqrtf(Xp2.x*Xp2.x + Xp2.y*Xp2.y) < 2.0f*PhysicalRadius)
			{
				// if intersection occurred, that means the sphere is erroneously passed thru the portal
				// teleport it to the portal-ring lock position
				*XDist_ptr = -T_THRESHOLD;	// set to this value so no other XDist can beat it
				*RedirectRatio_ptr = 0.0f;
			
				// relocate sphere to sphere-ring lock position, bumped back by T_BUMP
				if (Dirp.z < 0.0f)
					return Position + (h+T_BUMP)*Normal;//(h+T_BUMP)*Normal;
				else
					return Position - (h+T_BUMP)*Normal;//(h+T_BUMP)*Normal;
			}
		}
	}


	// if no intersection occurred, return
	if (t==MoveDist)
		return S + MoveDist*Dir;

	*XDist_ptr = t;

	// calculate collision point (X) in portal space
	XMFLOAT3 Xp = Sp + t*Dirp;	
	if (Xp.x==0.0f && Xp.y==0.0f)
	{
		*RedirectRatio_ptr = 0.0f;
	}
	else
	{
		// calculate redirect and redirect-ratio in portal space
		XMFLOAT3 XpDirLeft90 = XMFloat3Normalize(XMFLOAT3(Xp.y, -Xp.x, 0.0f));
		XMFLOAT3 Tp = PhysicalRadius * XMFloat3Normalize(XMFLOAT3(Xp.x, Xp.y, 0.0f));
		XMFLOAT3 XpTpDir = XMFloat3Normalize(Tp-Xp);
		XMFLOAT3 RedirectDirp = XMFloat3Cross(XpTpDir, XpDirLeft90);
		float RedirectRatiop = XMFloat3Dot(RedirectDirp, Dirp);
		if (RedirectRatiop < 0.0f)
		{
			RedirectDirp = -RedirectDirp;
			RedirectRatiop = -RedirectRatiop;
		}
		*RedirectRatio_ptr = RedirectRatiop;
		*RedirectDir_ptr = RedirectDirp.x*Left + RedirectDirp.y*Up + RedirectDirp.z*Normal;		// convert redirect to world space
	}

	// bump t, calculate X
	t -= T_BUMP;
	return S + t*Dir;
}


bool Portal::FindLowestQuarticRootInInterval(const float coeffs[5], float m, float n, float *x_ptr)
{
	// calculate f'(x) = a3*x^3+b3*x^2+c3*x+d3;
	float dcoeffs[4] = {4.0f*coeffs[0], 3.0f*coeffs[1], 2.0f*coeffs[2], coeffs[3]};

	// find critical points of this quartic that are in (m,n)
	float crits[3];
	int nCriticalPoints = FindCubicRootsInInterval(dcoeffs, m, n, crits);

	// generate array of endpoints of monotonic intervals inside [m,n)
	float IntervalEndpoints[5];
	IntervalEndpoints[0] = m;
	for (int i=0; i<nCriticalPoints; ++i)
		IntervalEndpoints[1+i] = crits[i];
	IntervalEndpoints[nCriticalPoints+1] = n;

	// find the first of these intervals [p,q) with f(p)>=0 and f(q)<0
	float p, q, x;
	for (int i=0; i<nCriticalPoints+1; ++i)
	{
		p = IntervalEndpoints[i];
		q = IntervalEndpoints[i+1];
		bool IsIncreasing;
		if (FindPolynomialRootInMonotonicInterval(4, coeffs, p, q, &x, &IsIncreasing))
		{
			if (!IsIncreasing)	// we're looking for places where the ray enters the torus
			{
				*x_ptr = x;
				return true;
			}
		}
	}
	return false;
}



// finds root(s) of cubic f(x)=ax^3+bx^2+cx+d=0
int Portal::FindCubicRootsInInterval(const float coeffs[4], float m, float n, float xs[3])
{
	// calculate f'(x) = a2*x^2 + b2*x + c2;
	float dcoeffs[3] = {3.0f*coeffs[0], 2.0f*coeffs[1], coeffs[2]};

	// find critical points of this cubic that are in (m,n)
	float crits[2];
	int nCriticalPoints = FindQuadraticRootsInInterval(dcoeffs, m, n, crits);
	
	// generate array of endpoints of monotonic intervals inside [m,n)
	float IntervalEndpoints[4];
	IntervalEndpoints[0] = m;
	for (int i=0; i<nCriticalPoints; ++i)
		IntervalEndpoints[1+i] = crits[i];
	IntervalEndpoints[nCriticalPoints+1] = n;
	
	// find the root in each of the monotonic intervals and add it to xs, if one exists
	int Roots = 0;
	float p, q, x;
	bool IsIncreasing;
	for (int i=0; i<nCriticalPoints+1; ++i)
	{
		p = IntervalEndpoints[i];
		q = IntervalEndpoints[i+1];
		if (FindPolynomialRootInMonotonicInterval(3, coeffs, p, q, &x, &IsIncreasing)) { xs[Roots++] = x; }
	}

	return Roots;
}


// finds root(s) of quadratic ax^2+bx+c=0
// x1 and/or x2 can be set to +infinity to indicate nonreal root
int Portal::FindQuadraticRootsInInterval(const float coeffs[3], float m, float n, float xs[2])
{
	float a = coeffs[0];
	float b = coeffs[1];
	float c = coeffs[2];
	
	int Roots = 0;
	float Discriminant_sq = b*b - 4*a*c;
	if (Discriminant_sq == 0.0f)
	{
		float x = -b / 2.0f / a;
		if (m < x && x < n) { xs[Roots++] = x; }
	}
	else if (Discriminant_sq > 0.0f)
	{
		float g = -b / 2.0f / a;
		float h = sqrtf(Discriminant_sq) / 2.0f / a;
		float x1 = g-h;
		float x2 = g+h;
		if (m < x1 && x1 < n) { xs[Roots++] = x1; }
		if (m < x2 && x2 < n) { xs[Roots++] = x2; }
	}
	return Roots;
}



bool Portal::FindPolynomialRootInMonotonicInterval(int degree, const float *coeffs, float m, float n, float *x_ptr, bool *IsIncreasing_ptr)
{
	float xmin = m;
	float xmax = n;
	float fxmin = CalculatePolynomial(xmin, degree, coeffs);
	float fxmax = CalculatePolynomial(xmax, degree, coeffs);

	// is this interval monotonically increasing or decreasing?
	*IsIncreasing_ptr = (fxmax > fxmin);

	// invariant: f(xmin)<=0, f(xmax)>0
	// negate f(x) from now on if necessary so that f(xmin)<=0
	float Negate = 1.0f;
	if (fxmin > 0.0f)
	{
		fxmin = -fxmin;
		fxmax = -fxmax;
		Negate = -1.0f;
	}
	// make sure f(xmax)>0
	if (fxmax <= 0.0f)
		return false;

	float fx;
	float x = m-ITERATIVE_THRESHOLD-1.0f;	// value to make sure the loop doesn't exit immediately
	float xprev;
	
	do
	{
		xprev = x;

		// calculate next guess
		x = xmin + fxmin/(fxmin-fxmax)*(xmax-xmin);
		fx = Negate * CalculatePolynomial(x, degree, coeffs);
		
		// invariant: f(xmin)<=0, f(xmax)>0
		if (fx <= 0.0f)
		{
			xmin = x;
			fxmin = fx;	
		}
		else
		{
			xmax = x;
			fxmax = fx;
		}
	}while(abs(x-xprev) > ITERATIVE_THRESHOLD);

	*x_ptr = x;
	return true;
}


// calculates f(t), where f is a polynomial in t with specified degree
// and coefficients, from highest order to lowest
float Portal::CalculatePolynomial(float t, int degree, const float *coeffs)
{
	float ft = coeffs[0];
	for (int i=0; i<degree; ++i)
		ft = ft*t + coeffs[i+1];
	return ft;
}




bool Portal::PathCrossesPortal(XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist)const
{
	// does not count as crossing if path heads into portal from wrong side (eg from behind)
	float DirDotN = XMFloat3Dot(Dir, Normal);
	if (DirDotN >= 0.0f)
		return false;

	float t = XMFloat3Dot(Position-S, Normal) / DirDotN;
	if (!(0.0f <= t && t < MoveDist))
		return false;

	XMFLOAT3 E = S + t*Dir;
	return (XMFloat3Length(E-Position) < PhysicalRadius);
}



void Portal::Orthonormalize()
{
	// orthonormalize
	XMVECTOR L = XMLoadFloat3(&Left);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR N = XMLoadFloat3(&Normal);
	N = XMVector3Normalize(N);
	L = XMVector3Normalize(XMVector3Cross(U, N));
	U = XMVector3Cross(N, L);
	XMStoreFloat3(&Left, L);
	XMStoreFloat3(&Up, U);
	XMStoreFloat3(&Normal, N);
}

XMMATRIX Portal::GetBoxWorldMatrix()const
{
	// scale portal box in x and y directions first to match physical portal size
	XMMATRIX Scale = XMMatrixScaling(PhysicalRadius, PhysicalRadius, 1.0f);
	XMMATRIX Affine = XMMATRIX(	Left.x,		Left.y,		Left.z,		0.0f,
								Up.x,		Up.y,		Up.z,		0.0f,
								Normal.x,	Normal.y,	Normal.z,	0.0f,
								Position.x,	Position.y,	Position.z,	1.0f	);

	return Scale * Affine;
}

XMMATRIX Portal::GetPortalMatrix()const
{
	XMVECTOR L = XMLoadFloat3(&Left);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR N = XMLoadFloat3(&Normal);

	XMVECTOR P = XMLoadFloat3(&Position);
	float tL = -XMVectorGetX(XMVector3Dot(P, L));
	float tU = -XMVectorGetX(XMVector3Dot(P, U));
	float tN = -XMVectorGetX(XMVector3Dot(P, N));

	// no scaling used.  portal size does not matter for this matrix
	return XMMATRIX(	Left.x,		Up.x,			Normal.x,		0.0f,
						Left.y,		Up.y,			Normal.y,		0.0f,
						Left.z,		Up.z,			Normal.z,		0.0f,
						tL,			tU,				tN,				1.0f	);
}

XMMATRIX Portal::GetScaledPortalMatrix()const
{
	XMVECTOR L = XMLoadFloat3(&Left);
	XMVECTOR U = XMLoadFloat3(&Up);
	XMVECTOR N = XMLoadFloat3(&Normal);

	XMVECTOR P = XMLoadFloat3(&Position);
	float tL = -XMVectorGetX(XMVector3Dot(P, L));
	float tU = -XMVectorGetX(XMVector3Dot(P, U));
	float tN = -XMVectorGetX(XMVector3Dot(P, N));

	// scaled by PhysicalRadius
	return XMMATRIX(	Left.x,		Up.x,			Normal.x,		0.0f,
						Left.y,		Up.y,			Normal.y,		0.0f,
						Left.z,		Up.z,			Normal.z,		0.0f,
						tL,			tU,				tN,				PhysicalRadius	);
}


// calculates transform to take a point from its current location to its virtual
// location when looking at it through a portal
XMMATRIX Portal::CalculateVirtualizationMatrix(const Portal &LookThru, const Portal &Other)
{
	XMVECTOR L = XMLoadFloat3(&(Other.Left));
	XMVECTOR U = XMLoadFloat3(&(Other.Up));
	XMVECTOR N = XMLoadFloat3(&(Other.Normal));
	XMVECTOR P = XMLoadFloat3(&(Other.Position));
	float tL = -XMVectorGetX(XMVector3Dot(P, L));
	float tU = -XMVectorGetX(XMVector3Dot(P, U));
	float tN = -XMVectorGetX(XMVector3Dot(P, N));

	XMMATRIX WorldToOtherPortalSpace = XMMATRIX(
			Other.Left.x,	Other.Up.x,		Other.Normal.x,		0.0f,
			Other.Left.y,	Other.Up.y,		Other.Normal.y,		0.0f,
			Other.Left.z,	Other.Up.z,		Other.Normal.z,		0.0f,
			tL,				tU,				tN,					1.0f
		);

	XMMATRIX LookThruPortalFlippedSpaceToWorld = 
						XMMATRIX(
							-LookThru.Left.x,		-LookThru.Left.y,		-LookThru.Left.z,		0.0f,
							LookThru.Up.x,			LookThru.Up.y,			LookThru.Up.z,			0.0f,
							-LookThru.Normal.x,		-LookThru.Normal.y,		-LookThru.Normal.z,		0.0f,
							LookThru.Position.x,	LookThru.Position.y,	LookThru.Position.z,	1.0f
						);

	float S = LookThru.PhysicalRadius / Other.PhysicalRadius;

	return WorldToOtherPortalSpace * XMMatrixScaling(S,S,S) * LookThruPortalFlippedSpaceToWorld;
}


void Portal::BuildMeshData(GeometryGenerator::MeshData &PortalMesh,	UINT *PortalBoxIndexCount_ptr)
{
	PortalMesh.Vertices.resize(PORTAL_BOX_N_SIDES*2 + 1);
	PortalMesh.Indices.resize(PORTAL_BOX_N_SIDES*9);

	// NOTE: For vertices, only the position matters

	// calculate distance away from center of each N-gon vertex
	float RadiansPerSlice = 2.0f * PI/PORTAL_BOX_N_SIDES;
	float R = 1.0f / cosf(RadiansPerSlice / 2.0f);

	//PORTAL_BOX_N_SIDES
	GeometryGenerator::Vertex Vert;
	Vert.Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Vert.Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Vert.TexCoord = XMFLOAT2(0.0f, 0.0f);

	// N-gon front and back N points
	for (int i=0; i<PORTAL_BOX_N_SIDES; ++i)
	{
		float Theta = (float)i * RadiansPerSlice;
		Vert.Position = XMFLOAT3(R*cosf(Theta), R*sinf(Theta), 0.0f);
		PortalMesh.Vertices[i] = Vert;

		Vert.Position.z = -PORTAL_BOX_DEPTH;
		PortalMesh.Vertices[PORTAL_BOX_N_SIDES+i] = Vert;
	}
	// center point on back face
	const int BACK_CENTER_INDEX = 2*PORTAL_BOX_N_SIDES;
	Vert.Position = XMFLOAT3(0.0f, 0.0f, -PORTAL_BOX_DEPTH);
	PortalMesh.Vertices[BACK_CENTER_INDEX] = Vert;
	

	// portalbox back face indices
	int IndicesCount = 0;
	for (int i=0; i<PORTAL_BOX_N_SIDES-1; ++i)
	{
		PortalMesh.Indices[IndicesCount] = BACK_CENTER_INDEX;
		PortalMesh.Indices[IndicesCount+1] = PORTAL_BOX_N_SIDES+i;
		PortalMesh.Indices[IndicesCount+2] = PORTAL_BOX_N_SIDES+i+1;
		IndicesCount += 3;
	}
	PortalMesh.Indices[IndicesCount] = BACK_CENTER_INDEX;
	PortalMesh.Indices[IndicesCount+1] = BACK_CENTER_INDEX-1;
	PortalMesh.Indices[IndicesCount+2] = PORTAL_BOX_N_SIDES;
	IndicesCount += 3;


	// portalbox side rectangles
	for (int i=0; i<PORTAL_BOX_N_SIDES-1; ++i)
	{
		PortalMesh.Indices[IndicesCount] = i;
		PortalMesh.Indices[IndicesCount+1] = i+1;
		PortalMesh.Indices[IndicesCount+2] = PORTAL_BOX_N_SIDES+i+1;

		PortalMesh.Indices[IndicesCount+3] = i;
		PortalMesh.Indices[IndicesCount+4] = PORTAL_BOX_N_SIDES+i+1;
		PortalMesh.Indices[IndicesCount+5] = PORTAL_BOX_N_SIDES+i;

		IndicesCount += 6;
	}
	PortalMesh.Indices[IndicesCount] = PORTAL_BOX_N_SIDES-1;
	PortalMesh.Indices[IndicesCount+1] = 0;
	PortalMesh.Indices[IndicesCount+2] = PORTAL_BOX_N_SIDES;

	PortalMesh.Indices[IndicesCount+3] = PORTAL_BOX_N_SIDES-1;
	PortalMesh.Indices[IndicesCount+4] = PORTAL_BOX_N_SIDES;
	PortalMesh.Indices[IndicesCount+5] = BACK_CENTER_INDEX-1;

	IndicesCount += 6;


	*PortalBoxIndexCount_ptr = IndicesCount;
}

