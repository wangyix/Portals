#include "Room.h"



// BOUNDARY ELEMENTS LIST
Room::BoundaryElementsList::BoundaryElementsList()
{
}
Room::BoundaryElementsList::~BoundaryElementsList()
{
	for (unsigned int i=0; i<ElementPointers.size(); ++i)
		delete ElementPointers[i];
}
void Room::BoundaryElementsList::AddEdge(const XMFLOAT2 &U, const XMFLOAT2 &V)
{
	ElementPointers.push_back(new BoundaryEdge(U, V));
}
void Room::BoundaryElementsList::AddVertex(const XMFLOAT2 &V)
{
	ElementPointers.push_back(new BoundaryVertex(V));
}
void Room::BoundaryElementsList::Delete(int Index)
{
	delete ElementPointers[Index];
	ElementPointers.erase(ElementPointers.begin() + Index);
}
Room::BoundaryElement* Room::BoundaryElementsList::operator[](const int Index)const
{
	return ElementPointers[Index];
}
unsigned int Room::BoundaryElementsList::size()const
{
	return ElementPointers.size();
}


// BOUNDARY EDGE stuff *************************************************************************

Room::BoundaryEdge::BoundaryEdge(XMFLOAT2 U, XMFLOAT2 V) :
U(U), V(V)
{
}

Room::BoundaryEdge::~BoundaryEdge()
{
}


// given a ray path for the disc, see if it will exit the polygon through this edge
bool Room::BoundaryEdge::RayPathExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, XMFLOAT2 *X_ptr, float *XDist_ptr,
											float *LeftRedCos_ptr, XMFLOAT2 *LeftRedDir_ptr,
											XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const
{
	// calculate shifted segment AB which is the DiscCenter boundary
	XMFLOAT2 UV = V-U;
	XMFLOAT2 UVNormal = XMFloat2Normalize(XMFloat2Left90(UV));
	XMFLOAT2 A = U + DiscRadius * UVNormal;
	XMFLOAT2 B = V + DiscRadius * UVNormal;

	float DirCrossSA = XMFloat2Cross(Dir, A-S);
	float DirCrossSB = XMFloat2Cross(Dir, B-S);
	if ( (DirCrossSA>0.0f || DirCrossSB<0.0f) || (DirCrossSA==0.0f && DirCrossSB==0.0f) )
		return false;

	XMFLOAT2 AB = B-A;

	// X = S+t*Dir = A+u*AB.
	float t = XMFloat2Cross(A-S, AB) / XMFloat2Cross(Dir, AB);
	if (t < -T_THRESHOLD)
		return false;

	// calculate everything except for X with unbumped t

	// calculate T info
	// X = A+u*AB, T = U+u*UV
	float u = XMFloat2Cross(A-S, Dir) / XMFloat2Cross(Dir, AB);
	*T_ptr = U + u*UV;
	*TNormal_ptr = UVNormal;

	// calculate X info
	*XDist_ptr = t;
	*LeftRedDir_ptr = XMFloat2Right90(UVNormal);
	*LeftRedCos_ptr = XMFloat2Dot(Dir, *LeftRedDir_ptr);

	// bump t, calculate X
	t -= T_BUMP;
	*X_ptr = S + t*Dir;

	return true;
}


void Room::BoundaryEdge::PrintInfo()const
{
	dprintf("Edge: (%f, %f)--(%f, %f)\n", U.x, U.y, V.x, V.y);
}




// BOUNDARY VERTEX STUFF *********************************************************************

Room::BoundaryVertex::BoundaryVertex(XMFLOAT2 V) :
V(V)
{
}

Room::BoundaryVertex::~BoundaryVertex()
{
}

bool Room::BoundaryVertex::RayPathExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, XMFLOAT2 *X_ptr, float *XDist_ptr,
									float *LeftRedCos_ptr, XMFLOAT2 *LeftRedDir_ptr,
									XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const
{
	XMFLOAT2 VS = S-V;
	/*
	// check if the circle around V is intersected by the ray line
	if (abs(XMFloat2Cross(VS, Dir)) >= DiscRadius)
		return false;
	*/
	
	// X = S + t*Dir
	// (X-V)dot(X-V)=rad^2
	// a = XMFloat2LengthSq(Dir) = 1
	float b_half = XMFloat2Dot(VS, Dir);
	float c = XMFloat2LengthSq(VS) - DiscRadius*DiscRadius;

	// check discriminant
	float discr_over_4 = b_half*b_half - c;
	if (discr_over_4 <= 0.0f)
		return false;

	// find t
	float t = -b_half - sqrtf(discr_over_4);
	if (t < -T_THRESHOLD)
		return false;

	XMFLOAT2 XUnbumped = S + t*Dir;

	// calculate T info
	*T_ptr = V;
	*TNormal_ptr = XMFloat2Normalize(XUnbumped - V);

	// calculate X info
	*XDist_ptr = t;
	*LeftRedDir_ptr = XMFloat2Right90(*TNormal_ptr);
	*LeftRedCos_ptr = XMFloat2Dot(Dir, *LeftRedDir_ptr);

	// bump t, calculate x
	t -= T_BUMP;
	*X_ptr = S + t*Dir;
	
	return true;
}

void Room::BoundaryVertex::PrintInfo()const
{
	dprintf("Vertex: (%f, %f)\n", V.x, V.y);
}


// ROOM STUFF ***************************************************************************************************
Room::Room()
	: FloorY(0.0f), CeilingY(0.0f), MinX(0.0f), MaxX(0.0f), MinZ(0.0f), MaxZ(0.0f), WallCount(0)
{
}

// setters
void Room::SetFloorAndCeiling(float FloorHeight, float CeilingHeight)
{
	FloorY = FloorHeight;
	CeilingY = CeilingHeight;
}

void Room::SetTopography(const std::vector<std::vector<XMFLOAT2>> &Polygons)
{
	BoundaryPolygons.resize(Polygons.size());
	WallCount = 0;
	for (unsigned int i=0; i<Polygons.size(); ++i)
	{
		// copy this polygon's vertices
		BoundaryPolygons[i] = Polygons[i];
		WallCount += Polygons[i].size();
		for (unsigned int j=0; j<Polygons[i].size(); ++j)
		{
			MinX = min(MinX, Polygons[i][j].x);
			MaxX = max(MaxX, Polygons[i][j].x);
			MinZ = min(MinZ, Polygons[i][j].y);
			MaxZ = max(MaxZ, Polygons[i][j].y);
		}
	}
}


// getters, prints
void Room::PrintBoundaries()
{
	dprintf("\n");
	for (unsigned int i=0; i<BoundaryPolygons.size(); ++i)
	{
		dprintf("BoundaryPolygon%d:\n", i);
		for (unsigned int j=0; j<BoundaryPolygons[i].size(); ++j)
		{
			dprintf("(%f, %f)\n", BoundaryPolygons[i][j].x, BoundaryPolygons[i][j].y);
		}
		dprintf("\n");
	}
}


XMFLOAT3 Room::SpherePathVirtualCollision(const XMMATRIX &Virtualize, const XMMATRIX &Unvirtualize, 
									float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)const
{
	// calculate virtual S, virtual Dir, virtual MoveDist 
	XMFLOAT3 Sv;
	XMFLOAT3 Dirv;
	XMVECTOR SVirtual = XMVector3TransformCoord(XMLoadFloat3(&S), Virtualize);
	XMVECTOR DirVirtual = XMVector3TransformNormal(XMLoadFloat3(&Dir), Virtualize);
	XMStoreFloat3(&Sv, SVirtual);
	XMStoreFloat3(&Dirv, DirVirtual);
	
	// scaling factor of the virtualization
	float DirvLength = XMFloat3Length(Dirv);

	// the only output that can be passed thru is RedirectRatio.  everything else needs to be unvirtualized
	XMFLOAT3 Xv;
	float XDistv;
	XMFLOAT3 RedirectDirv;
	XMFLOAT3 Tv;			// unused
	XMFLOAT3 TNormalv;		// unused
	Xv = SpherePathCollision(SphereRadius*DirvLength, Sv, Dirv/DirvLength, MoveDist*DirvLength, &XDistv, RedirectRatio_ptr, &RedirectDirv, &Tv, &TNormalv);

	// un-virtualize Xv
	XMFLOAT3 X;
	XMVECTOR XUnvirtual = XMVector3TransformCoord(XMLoadFloat3(&Xv), Unvirtualize);
	XMStoreFloat3(&X, XUnvirtual);
	// un-virtualize RedirectDirv
	XMVECTOR RedirectDirUnvirtual = XMVector3TransformNormal(XMLoadFloat3(&RedirectDirv), Unvirtualize);
	XMStoreFloat3(RedirectDir_ptr, RedirectDirUnvirtual);
	// unscale XDist
	*XDist_ptr = XDistv / DirvLength;	// XDist needs to be unscaled 
	
	return X;
}


XMFLOAT3 Room::SpherePathCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr,
									XMFLOAT3 *T_ptr, XMFLOAT3 *TNormal_ptr)const
{
	// find path collision with floor or ceiling
	XMFLOAT3 FloorCeilingX;
	float FloorCeilingXDist;
	float FloorCeilingRedirectRatio;
	XMFLOAT3 FloorCeilingRedirectDir;
	XMFLOAT3 FloorCeilingT;
	XMFLOAT3 FloorCeilingTNormal;
	FloorCeilingX = SpherePathFloorCeilingCollision(SphereRadius, S, Dir, MoveDist, 
		&FloorCeilingXDist, &FloorCeilingRedirectRatio, &FloorCeilingRedirectDir, &FloorCeilingT, &FloorCeilingTNormal);

	// find path collision with walls
	float WallXDist;
	float WallRedirectRatio;
	XMFLOAT3 WallRedirectDir;
	XMFLOAT3 WallX;
	XMFLOAT3 WallT;
	XMFLOAT3 WallTNormal;
	WallX = SpherePathWallCollision(SphereRadius, S, Dir, MoveDist, 
			&WallXDist, &WallRedirectRatio, &WallRedirectDir, &WallT, &WallTNormal);

	// return the collision that's closer. if both equally close, then return the one that's more restrictive
	XMFLOAT3 X;
	if (FloorCeilingXDist < WallXDist || 
		(FloorCeilingXDist==WallXDist && FloorCeilingRedirectRatio < WallRedirectRatio))
	{
		X = FloorCeilingX;
		*XDist_ptr = FloorCeilingXDist;
		*RedirectRatio_ptr = FloorCeilingRedirectRatio;
		*RedirectDir_ptr = FloorCeilingRedirectDir;
		*T_ptr = FloorCeilingT;
		*TNormal_ptr = FloorCeilingTNormal;
	}
	else
	{
		X = WallX;
		*XDist_ptr = WallXDist;
		*RedirectRatio_ptr = WallRedirectRatio;
		*RedirectDir_ptr = WallRedirectDir;
		*T_ptr = WallT;
		*TNormal_ptr = WallTNormal;
	}
	return X;
}



XMFLOAT3 Room::SpherePathFloorCeilingCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr,
									XMFLOAT3 *T_ptr, XMFLOAT3 *TNormal_ptr)const
{
	// defaults
	*XDist_ptr = MoveDist;
	*RedirectRatio_ptr = 1.0f;
	*RedirectDir_ptr = Dir;

	// check if this path has movement in the Y direction
	if (Dir.y==0.0f)
		return S + MoveDist*Dir;


	// convert inputs to Y axis projections
	float StartY = S.y;

	// solve for t: X=S+t*Dir
	// X.y = StartY + t * Dir.y
	bool HeadingUp = (Dir.y > 0.0f);
	float t;
	if (HeadingUp)
		t = ((CeilingY-SphereRadius) - StartY) / Dir.y;
	else
		t = ((FloorY+SphereRadius) - StartY) / Dir.y;

	// check path exits floor/ceiling
	if (t < -T_THRESHOLD || t >= MoveDist)
		return S + MoveDist*Dir;

	// find XZ coordinate of X
	XMFLOAT2 XXZ = XMFLOAT2(S.x+t*Dir.x, S.z+t*Dir.z);

	// calculate T info
	*T_ptr = XMFLOAT3(XXZ.x, (HeadingUp ? CeilingY : FloorY), XXZ.y);
	*TNormal_ptr = XMFLOAT3(0.0f, (HeadingUp ? -1.0f : 1.0f), 0.0f);

	// calculate X info
	*XDist_ptr = t;
	if (Dir.x==0.0f && Dir.z==0.0f)
	{
		*RedirectRatio_ptr = 0.0f;
	}
	else
	{
		*RedirectDir_ptr = XMFloat3Normalize(XMFLOAT3(Dir.x, 0.0f, Dir.z));
		*RedirectRatio_ptr = XMFloat3Dot(*RedirectDir_ptr, Dir);
	}

	// bump t, calculate X
	t -= T_BUMP;
	return S + t*Dir;
}


XMFLOAT3 Room::SpherePathWallCollision(float SphereRadius, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
									float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr,
									XMFLOAT3 *T_ptr, XMFLOAT3 *TNormal_ptr)const
{
	// defaults
	*XDist_ptr = MoveDist;
	*RedirectRatio_ptr = 1.0f;
	*RedirectDir_ptr = Dir;
	
	// see if this path has any movement in the XZ plane.  If not, no wall collision will happen
	if (Dir.x==0.0f && Dir.z==0.0f)
		return S + MoveDist*Dir;


	// convert inputs to XZ plane projections
	XMFLOAT2 StartXZ = XMFLOAT2(S.x, S.z);
	XMFLOAT2 DirXZ = XMFLOAT2(Dir.x, Dir.z);
	float DirXZRatio = XMFloat2Length(DirXZ);
	DirXZ = XMFloat2Normalize(DirXZ);
	float MoveDistXZ = MoveDist * DirXZRatio;

	// in the XZ plane, find the boundary elements that are close enough for the disc to exit at
	BoundaryElementsList DiscCenterBoundaryElements;
	
	float SumDist = MoveDistXZ + SphereRadius;
	for (unsigned int PolygonIndex=0; PolygonIndex<BoundaryPolygons.size(); ++PolygonIndex)
	{
		const std::vector<XMFLOAT2> &Vertices = BoundaryPolygons[PolygonIndex];
		for (unsigned int i=0; i<Vertices.size(); ++i)
		{
			// get edge at this index: UV
			XMFLOAT2 U = Vertices[i];
			XMFLOAT2 V = (i==Vertices.size()-1) ? Vertices[0] : Vertices[i+1];

			// can U be reached from S?
			if (SumDist >= XMFloat2Length(U-StartXZ))
			{
				//dprintf("	Vertex (%f, %f)\n",U.x,U.y);
				DiscCenterBoundaryElements.AddVertex(U);
			}

			// can UV be reached from S?
			XMFLOAT2 UVDir = XMFloat2Normalize(V-U);
			if (SumDist >= abs(XMFloat2Cross(StartXZ-U, UVDir)))	// S close enough to line UV
			{
				XMFLOAT2 US = StartXZ-U;
				XMFLOAT2 VS = StartXZ-V;
				if (XMFloat2Dot(VS, UVDir)<=SumDist			// S not too far to the side of U or V
					&& XMFloat2Dot(US, UVDir)>=-SumDist)
				{
					//dprintf("	Edge (%f, %f)--(%f, %f)\n", U.x,U.y,V.x,V.y);
					DiscCenterBoundaryElements.AddEdge(U, V);
				}
			}
		}
	}

	// find where the XZ disc of the sphere will exit the room in the XZ plane
	XMFLOAT2 XXZ;
	float XDistXZ;
	float RedirectRatioXZ;
	XMFLOAT2 RedirectDirXZ;
	XMFLOAT2 TXZ;
	XMFLOAT2 TNormalXZ;
	XXZ = FindFirstExit(SphereRadius, StartXZ, DirXZ, MoveDistXZ, DiscCenterBoundaryElements,
						&XDistXZ, &RedirectRatioXZ, &RedirectDirXZ, &TXZ, &TNormalXZ);

	// check if a wall collision even occurred.  if not, return
	if (XDistXZ==MoveDistXZ)
		return S + MoveDist*Dir;


	*XDist_ptr = XDistXZ / DirXZRatio;

	// find the y coordinate of X
	float XY = S.y + *XDist_ptr*Dir.y;

	// convert T info back to 3D
	*T_ptr = XMFLOAT3(TXZ.x, XY, TXZ.y);
	*TNormal_ptr = XMFLOAT3(TNormalXZ.x, 0.0f, TNormalXZ.y);

	// convert X info back to 3D
	// if no redirect is possible in the XZ plane, then we must redirect in the Y direction
	if (RedirectRatioXZ==0.0f)
	{
		if (Dir.y==0.0f)	// if we can't redirect along Y neither, then we have no redirect
		{
			*RedirectRatio_ptr = 0.0f;
		}
		else
		{
			*RedirectDir_ptr = XMFLOAT3(0.0f, (Dir.y>0.0f) ? 1.0f : -1.0f, 0.0f);
			*RedirectRatio_ptr = XMFloat3Dot(*RedirectDir_ptr, Dir);
		}
	}
	else
	{
		// normalize reconstructed 3D RedirectDir for good measure
		*RedirectDir_ptr = XMFloat3Normalize(XMFLOAT3(RedirectDirXZ.x * DirXZRatio, Dir.y, RedirectDirXZ.y * DirXZRatio));
		*RedirectRatio_ptr = XMFloat3Dot(*RedirectDir_ptr, Dir);
	}
	
	return XMFLOAT3(XXZ.x, XY, XXZ.y);	// note: XZ is the y coordinate of the unbumped X, while XXZ is the xa coordinates of the bumped X
}




XMFLOAT2 Room::FindFirstExit(float DiscRadius, XMFLOAT2 S, XMFLOAT2 Dir, float MoveDist, 
							const BoundaryElementsList &DiscCenterBoundaryElements,
							float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT2 *RedirectDir_ptr,
							XMFLOAT2 *T_ptr, XMFLOAT2 *TNormal_ptr)const
{

	XMFLOAT2 ClosestX = S + MoveDist*Dir;
	float ClosestXDist = MoveDist;
	float ClosestLeftRedCos = 1.0f;
	XMFLOAT2 ClosestLeftRedDir = Dir;
	XMFLOAT2 ClosestT;
	XMFLOAT2 ClosestTNormal;

	//dprintf("\nFinding exit and redirect info for this intended path:\n");
	//dprintf("S=(%f, %f), Dir=(%f, %f), MoveDist=%f\n",S.x,S.y,Dir.x,Dir.y,MoveDist);

	// find the X and redirection from the closest and most restrictive boundary element
	float XDist;
	XMFLOAT2 X;
	float LeftRedCos;
	XMFLOAT2 LeftRedDir;
	XMFLOAT2 T;
	XMFLOAT2 TNormal;
	for (unsigned int i=0; i<DiscCenterBoundaryElements.size(); ++i)
	{
		if (!DiscCenterBoundaryElements[i]->RayPathExit(DiscRadius, S, Dir, &X, &XDist, &LeftRedCos, &LeftRedDir, &T, &TNormal))
			continue;
		
		//dprintf("	%d: ",i);
		//DiscCenterBoundaryElements[i]->PrintInfo();
		//dprintf("		Disc on path ray exits this element at X=(%f, %f), XDist=%f...\n",X.x,X.y,XDist);
		/*
		if (XDist == ClosestXDist)
		{
			//dprintf("			This exit is at same distance as current most restrictive...\n");
			if ( ClosestXDist < MoveDist && ((LeftRedCos<0.0f)!=(ClosestLeftRedCos<0.0f)) )
			{
				//dprintf("				It redirects to opposite side as the current one; new LeftRedCos=0\n");
				ClosestLeftRedCos = 0.0f;
			}
			else if(abs(LeftRedCos) < abs(ClosestLeftRedCos))
			{
				//dprintf("				It has a more restrictive redirect than the current one; new LeftRedCos=%f\n", XLeftRedCos);
				ClosestLeftRedCos = LeftRedCos;
				ClosestLeftRedDir = LeftRedDir;
			}
		}*/

		// CHANGE: don't check for opposing redirects anymore: it may think there's no redirect when wedged between 2 walls, but one of those walls
		// may have a portal.  We don't want this to erroneously think that no redirect is possible.
		
		if (XDist == ClosestXDist && abs(LeftRedCos) < abs(ClosestLeftRedCos))
		{
			ClosestLeftRedCos = LeftRedCos;
			ClosestLeftRedDir = LeftRedDir;
			ClosestT = T;
			ClosestTNormal = TNormal;
		}
		else if (XDist < ClosestXDist)
		{
			//dprintf("			This exit is closer than the current one; new XDist=%f, LeftRedCos=%f\n", XDist, XLeftRedCos);
			ClosestX = X;
			ClosestXDist = XDist;
			ClosestLeftRedCos = LeftRedCos;
			ClosestLeftRedDir = LeftRedDir;
			ClosestT = T;
			ClosestTNormal = TNormal;
		}
	}

	*XDist_ptr = ClosestXDist;
	*RedirectRatio_ptr = abs(ClosestLeftRedCos);
	*RedirectDir_ptr = ((ClosestLeftRedCos>0.0f) ? 1.0f : -1.0f) * ClosestLeftRedDir;
	*T_ptr = ClosestT;
	*TNormal_ptr = ClosestTNormal;
	return ClosestX;
}




void Room::PortalRelocate(XMFLOAT3 S, XMFLOAT3 Dir, Portal &ThisPortal, const Portal &OtherPortal)const
{
	// find out where this ray first intersects the room
	bool WallIntersect;

	float XDist = std::numeric_limits<float>::infinity();	// X = S+t*Dir
	XMFLOAT3 X;
	XMFLOAT3 XNormal;

	XMFLOAT2 XXZ;				// location of X in the XZ plane
	XMFLOAT2 XWallU, XWallV;	// 2D coords of the wall that X is on, if WallIntersect


	// first, find collision with floor/ceiling, if dir has a vertical component
	if (Dir.y != 0.0f)
	{
		WallIntersect = false;
		bool HeadingDown = (Dir.y < 0.0f);

		// calculate t
		if (HeadingDown)
			XDist = (FloorY - S.y) / Dir.y;
		else
			XDist = (CeilingY - S.y) / Dir.y;

		// calculate X, XNormal
		X = S + XDist*Dir;
		X.y = (HeadingDown ? FloorY : CeilingY);	// for good measure
		XNormal = XMFLOAT3(0.0f, (HeadingDown ? 1.0f : -1.0f), 0.0f);
		XXZ = XMFLOAT2(X.x, X.z);
	}

	// if ray has a horizontal component, it may hit a wall before the ceiling/floor plane
	if (Dir.x != 0.0f || Dir.z != 0.0f)
	{
		XMFLOAT2 StartXZ = XMFLOAT2(S.x, S.z);
		XMFLOAT2 DirXZ = XMFLOAT2(Dir.x, Dir.z);

		// find intersection between ray and walls, possibly ceiling/floor
		float t;
		for (unsigned int PolygonIndex=0; PolygonIndex<BoundaryPolygons.size(); ++PolygonIndex)
		{
			const std::vector<XMFLOAT2> &Vertices = BoundaryPolygons[PolygonIndex];
			for (unsigned int i=0; i<Vertices.size(); ++i)
			{
				// NOTE: DirXZ is not normalized

				// get vertices of this edge: UV
				XMFLOAT2 U = Vertices[i];
				XMFLOAT2 V = (i==Vertices.size()-1) ? Vertices[0] : Vertices[i+1];

				// intersect the ray with this edge
				float DirCrossSU = XMFloat2Cross(DirXZ, U-StartXZ);
				float DirCrossSV = XMFloat2Cross(DirXZ, V-StartXZ);
				if ( (DirCrossSU>0.0f || DirCrossSV<0.0f) || (DirCrossSU==0.0f && DirCrossSV==0.0f) )
					continue;

				XMFLOAT2 UV = V-U;

				// see if this edge results in a closer X
				// X = S+t*Dir = U+u*UV.
				t = XMFloat2Cross(U-StartXZ, UV) / XMFloat2Cross(DirXZ, UV);
				if (t < 0.0f || t >= XDist)
					continue;

				// replace current candidate for X
				
				// use A+u*AB to calculate X so it's guaranteed to be on AB even if u is slightly off
				float u = XMFloat2Cross(U-StartXZ, DirXZ) / XMFloat2Cross(DirXZ, UV);
				XXZ = U + u*UV;
				XMFLOAT2 XNormalXZ = XMFloat2Left90(XMFloat2Normalize(UV));

				XDist = t;
				X = XMFLOAT3(XXZ.x, S.y + t*Dir.y, XXZ.y);
				XNormal = XMFLOAT3(XNormalXZ.x, 0.0f, XNormalXZ.y);
				XWallU = U;
				XWallV = V;

				WallIntersect = true;
			}
		}//end for each polygon
	}//end if ray has horizontal component


	// we now have the location of X. now we need to calculate the radius the portal will have
	float MaxR = std::numeric_limits<float>::infinity();
	
	if (WallIntersect)
	{
		// check distance of X away from the 4 edges of the wall it's on
		float d;
		if ((d=XMFloat2Length(XXZ-XWallU)) < MaxR)
			MaxR = d;
		if ((d=XMFloat2Length(XXZ-XWallV)) < MaxR)
			MaxR = d;
		if ((d=CeilingY-X.y) < MaxR)
			MaxR = d;
		if ((d=X.y-FloorY) < MaxR)
			MaxR = d;
	}
	else
	{
		// check distance of X from all polygon edges
		float d;
		for (unsigned int PolygonIndex=0; PolygonIndex<BoundaryPolygons.size(); ++PolygonIndex)
		{
			const std::vector<XMFLOAT2> &Vertices = BoundaryPolygons[PolygonIndex];
			for (unsigned int i=0; i<Vertices.size(); ++i)
			{
				// get vertices of this edge: UV
				XMFLOAT2 U = Vertices[i];
				XMFLOAT2 V = (i==Vertices.size()-1) ? Vertices[0] : Vertices[i+1];

				XMFLOAT2 UV = V-U;

				// check if X is within bounds of UV
				if (XMFloat2Dot(XXZ-U, UV) < 0.0f)	// X is beyond U
					d = XMFloat2Length(XXZ-U);
				else if (XMFloat2Dot(XXZ-V, UV) > 0.0f)	// X is beyond V
					d = XMFloat2Length(XXZ-V);
				else
					d = abs(XMFloat2Cross(XMFloat2Normalize(UV), XXZ-U));

				if (d < MaxR)
					MaxR = d;
			}
		}// end for each polygon
	}

	// check distance of X away from the other portal, if they will be on the same plane
	if (XNormal==OtherPortal.GetNormal())
	{
		XMFLOAT3 XToOther = OtherPortal.GetPosition() - X;
		if (abs(XMFloat3Dot(XToOther, XNormal)) < PORTALS_SAME_PLANE_THRESHOLD)
		{
			float PortalsDist = XMFloat3Length(XToOther);
			// if X is inside the other portal, don't do anything
			if (PortalsDist < OtherPortal.GetTextureRadius())
				return;

			// check distance between X and the ring of the otherportal
			float d;
			if ((d=PortalsDist-OtherPortal.GetTextureRadius()) < MaxR)
				MaxR = d;
		}
	}

	// make sure the new radius is not too small
	if (MaxR < PORTAL_MIN_PHYS_RADIUS * ThisPortal.GetTextureRadiusRatio())
		return;


	// move ThisPortal to its new location
	ThisPortal.SetPosition(X);

	// set its orientation
	XMFLOAT3 Up;
	if (WallIntersect)
		Up = XMFLOAT3(0, 1, 0);
	else
		Up = XMFLOAT3(1, 0, 0);
	ThisPortal.SetNormalAndUp(XNormal, Up);
	
	// update its radius
	ThisPortal.SetMaxTextureRadius(MaxR);
}



void Room::BuildMeshData(GeometryGenerator::MeshData &RoomMesh, 
						UINT *WallsIndexCount_ptr, UINT *WallsIBOffset_ptr, UINT *WallsVBOffset_ptr,
						UINT *FloorIndexCount_ptr, UINT *FloorIBOffset_ptr, UINT *FloorVBOffset_ptr,
						UINT *CeilingIndexCount_ptr, UINT *CeilingIBOffset_ptr, UINT *CeilingVBOffset_ptr)const
{
	RoomMesh.Vertices.clear();
	RoomMesh.Indices.clear();


	UINT WallsVertexCount = 0;
	UINT WallsIndexCount = 0;
	GeometryGenerator::Vertex Vert;
	for (unsigned int P=0; P<BoundaryPolygons.size(); ++P)
	{
		const std::vector<XMFLOAT2> &PolygonVertices = BoundaryPolygons[P];
		for (unsigned int i=0; i<PolygonVertices.size(); ++i)
		{
			// get this vertex and the next one
			XMFLOAT2 U = PolygonVertices[i];
			XMFLOAT2 V = PolygonVertices[i==PolygonVertices.size()-1 ? 0 : i+1];
			XMFLOAT2 UVDir = XMFloat2Normalize(V-U);
			XMFLOAT2 UVNormal = XMFloat2Left90(UVDir);
			float WallWidth = XMFloat2Length(V-U);
			float WallHeight = CeilingY - FloorY;

			Vert.Normal = XMFLOAT3(UVNormal.x, 0.0f, UVNormal.y);
			Vert.Tangent = XMFLOAT3(UVDir.x, 0.0f, UVDir.y);
			
			// vertices are added in CW order, starting from bottom left
			// textures are tiled to 1x1 squares on the wall

			// bottom left
			Vert.Position = XMFLOAT3(V.x, FloorY, V.y);
			Vert.TexCoord = XMFLOAT2(0.0f, WallHeight);
			RoomMesh.Vertices.push_back(Vert);

			// top left
			Vert.Position = XMFLOAT3(V.x, CeilingY, V.y);
			Vert.TexCoord = XMFLOAT2(0.0f, 0.0f);
			RoomMesh.Vertices.push_back(Vert);
			
			// top right
			Vert.Position = XMFLOAT3(U.x, CeilingY, U.y);
			Vert.TexCoord = XMFLOAT2(WallWidth, 0.0f);
			RoomMesh.Vertices.push_back(Vert);

			// bottom right
			Vert.Position = XMFLOAT3(U.x, FloorY, U.y);
			Vert.TexCoord = XMFLOAT2(WallWidth, WallHeight);
			RoomMesh.Vertices.push_back(Vert);


			// add to indices

			// upper left triangle
			RoomMesh.Indices.push_back(WallsVertexCount);
			RoomMesh.Indices.push_back(WallsVertexCount + 1);
			RoomMesh.Indices.push_back(WallsVertexCount + 2);
			// bottom right triangle
			RoomMesh.Indices.push_back(WallsVertexCount);
			RoomMesh.Indices.push_back(WallsVertexCount + 2);
			RoomMesh.Indices.push_back(WallsVertexCount + 3);

			WallsVertexCount += 4;
			WallsIndexCount += 6; 
		}
	}
	*WallsIndexCount_ptr = WallsIndexCount;
	*WallsIBOffset_ptr = 0;
	*WallsVBOffset_ptr = 0;

	//dprintf("Xminmax [%f %f] Zminmax [%f %f]\n",MinX,MaxX,MinZ,MaxZ);

	float FloorWidth = MaxX - MinX;
	float FloorHeight = MaxZ - MinZ;

	// add floor offsets
	*FloorIndexCount_ptr = 6;
	*FloorIBOffset_ptr = WallsIndexCount;
	*FloorVBOffset_ptr = WallsVertexCount;

	// add floor vertices
	Vert.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Vert.Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	// bottom left
	Vert.Position = XMFLOAT3(MinX, FloorY, MinZ);
	Vert.TexCoord = XMFLOAT2(0.0f, FloorHeight);
	RoomMesh.Vertices.push_back(Vert);

	// top left
	Vert.Position = XMFLOAT3(MinX, FloorY, MaxZ);
	Vert.TexCoord = XMFLOAT2(0.0f, 0.0f);
	RoomMesh.Vertices.push_back(Vert);

	// top right
	Vert.Position = XMFLOAT3(MaxX, FloorY, MaxZ);
	Vert.TexCoord = XMFLOAT2(FloorWidth, 0.0f);
	RoomMesh.Vertices.push_back(Vert);

	// bottom right
	Vert.Position = XMFLOAT3(MaxX, FloorY, MinZ);
	Vert.TexCoord = XMFLOAT2(FloorWidth, FloorHeight);
	RoomMesh.Vertices.push_back(Vert);

	// add floor indices
	RoomMesh.Indices.push_back(0);
	RoomMesh.Indices.push_back(1);
	RoomMesh.Indices.push_back(2);
	RoomMesh.Indices.push_back(0);
	RoomMesh.Indices.push_back(2);
	RoomMesh.Indices.push_back(3);


	// add ceiling offsets
	*CeilingIndexCount_ptr = 6;
	*CeilingIBOffset_ptr = WallsIndexCount + 6;
	*CeilingVBOffset_ptr = WallsVertexCount + 4;

	// add ceiling vertices
	Vert.Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	Vert.Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

	// bottom left
	Vert.Position = XMFLOAT3(MinX, CeilingY, MinZ);
	Vert.TexCoord = XMFLOAT2(0.0f, FloorHeight);
	RoomMesh.Vertices.push_back(Vert);

	// bottom right
	Vert.Position = XMFLOAT3(MaxX, CeilingY, MinZ);
	Vert.TexCoord = XMFLOAT2(FloorWidth, FloorHeight);
	RoomMesh.Vertices.push_back(Vert);

	// top right
	Vert.Position = XMFLOAT3(MaxX, CeilingY, MaxZ);
	Vert.TexCoord = XMFLOAT2(FloorWidth, 0.0f);
	RoomMesh.Vertices.push_back(Vert);

	// top left
	Vert.Position = XMFLOAT3(MinX, CeilingY, MaxZ);
	Vert.TexCoord = XMFLOAT2(0.0f, 0.0f);
	RoomMesh.Vertices.push_back(Vert);

	// add ceiling indices
	RoomMesh.Indices.push_back(0);
	RoomMesh.Indices.push_back(2);
	RoomMesh.Indices.push_back(3);
	RoomMesh.Indices.push_back(0);
	RoomMesh.Indices.push_back(1);
	RoomMesh.Indices.push_back(2);
}