	#include "SpherePath.h"



void SpherePath::MoveCameraAlongPathIterative(Camera &Cam, XMFLOAT3 Dir, float MoveDist,
									const Room &Level, const Portal &OrangePortal, const Portal &BluePortal)
{
	float XDist;
	float RedirectRatio;
	XMFLOAT3 RedirectDir;
	XMFLOAT3 X;

	bool RedirectNecessary;

	// primary path
	RedirectNecessary = MoveCameraAlongPath(Cam, Dir, MoveDist, Level, OrangePortal, BluePortal, &XDist, &RedirectRatio, &RedirectDir);

	// check if secondary path is necessary
	if (!RedirectNecessary)
		return;

	// calculate secondary path
	float MoveDist2 = (MoveDist - XDist) * RedirectRatio;
	RedirectNecessary = MoveCameraAlongPath(Cam, RedirectDir, MoveDist2, Level, OrangePortal, BluePortal, &XDist, &RedirectRatio, &RedirectDir);
	
	// check if tertiary path is necessary
	if (!RedirectNecessary)
		return;

	// calculate teriary path
	float MoveDist3 = (MoveDist2 - XDist) * RedirectRatio;
	MoveCameraAlongPath(Cam, RedirectDir, MoveDist3, Level, OrangePortal, BluePortal, &XDist, &RedirectRatio, &RedirectDir);
}







bool SpherePath::MoveCameraAlongPath(Camera &Cam, XMFLOAT3 Dir, float MoveDist,
										const Room &Level, const Portal &OrangePortal, const Portal &BluePortal,
										float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)
{
	// get some info about the path
	float SphereRadius = Cam.GetBoundingSphereRadius();
	XMFLOAT3 S = Cam.GetPosition();
	MoveDist *= Cam.GetViewScale();


	// is the camera currently clipping the portal?

	// SPHERE CLIPPING ORANGE PORTAL
	if (OrangePortal.DiscIntersectSphere(S, SphereRadius))
	{
		return MoveClippedCamera(Cam, Dir, MoveDist, Level, OrangePortal, BluePortal, XDist_ptr, RedirectRatio_ptr, RedirectDir_ptr);
	}
	
	// SPHERE CLIPPING BLUE PORTAL
	if (BluePortal.DiscIntersectSphere(S, SphereRadius))
	{
		return MoveClippedCamera(Cam, Dir, MoveDist, Level, BluePortal, OrangePortal, XDist_ptr, RedirectRatio_ptr, RedirectDir_ptr);
	}
	
	// SPHERE NOT CLIPPING A PORTAL

	// find room collision
	XMFLOAT3 X;
	float XDist;
	float RedirectRatio;
	XMFLOAT3 RedirectDir;
	XMFLOAT3 RoomT;
	XMFLOAT3 RoomTNormal;
	X = Level.SpherePathCollision(SphereRadius, S, Dir, MoveDist, &XDist, &RedirectRatio, &RedirectDir, &RoomT, &RoomTNormal);

	// if no room collision occurs, simply move the camera to the pathend and we're done
	if (XDist == MoveDist)
	{
		Cam.SetPosition(X);
		return false;
	}
		
	// check if the tangent point T is inside a portal disc
	XMFLOAT3 TtoPortalCenter;
	TtoPortalCenter = OrangePortal.GetPosition() - RoomT;
	if ( abs(XMFloat3Dot(TtoPortalCenter, OrangePortal.GetNormal())) < PORTALS_SAME_PLANE_THRESHOLD &&
			XMFloat3Length(TtoPortalCenter) < OrangePortal.GetPhysicalRadius() )				// T is inside orange disc
	{
		return MoveClippedCamera(Cam, Dir, MoveDist, Level, OrangePortal, BluePortal, XDist_ptr, RedirectRatio_ptr, RedirectDir_ptr);
	}
	TtoPortalCenter = BluePortal.GetPosition() - RoomT;
	if ( abs(XMFloat3Dot(TtoPortalCenter, BluePortal.GetNormal())) < PORTALS_SAME_PLANE_THRESHOLD &&
			XMFloat3Length(TtoPortalCenter) < BluePortal.GetPhysicalRadius() )					// T is inside blue disc
	{
		return MoveClippedCamera(Cam, Dir, MoveDist, Level, BluePortal, OrangePortal, XDist_ptr, RedirectRatio_ptr, RedirectDir_ptr);
	}



	// we've hit a wall but not a portal.  move the camera to the collision point and update redirect values
	Cam.SetPosition(X);

	*XDist_ptr = XDist;
	*RedirectRatio_ptr = RedirectRatio;
	*RedirectDir_ptr = RedirectDir;

	return (XDist < MoveDist && RedirectRatio != 0.0f);
}





void SpherePath::UpdateClosestCollision(XMFLOAT3 *ClosestX_ptr, float *ClosestXDist_ptr,
									float *ClosestRedirectRatio_ptr, XMFLOAT3 *ClosestRedirectDir_ptr,
									const XMFLOAT3 &X, float XDist, float RedirectRatio, const XMFLOAT3 &RedirectDir)
{
	if (XDist == *ClosestXDist_ptr && RedirectRatio < *ClosestRedirectRatio_ptr)
	{
		*ClosestRedirectRatio_ptr = RedirectRatio;
		*ClosestRedirectDir_ptr = RedirectDir;
	}
	else if (XDist < *ClosestXDist_ptr)
	{
		*ClosestX_ptr = X;
		*ClosestXDist_ptr = XDist;
		*ClosestRedirectRatio_ptr= RedirectRatio;
		*ClosestRedirectDir_ptr = RedirectDir;
	}
}






// returns whether or not a redirect is possible afterwards

bool SpherePath::MoveClippedCamera(Camera &Cam, XMFLOAT3 Dir, float MoveDist,
										const Room &Level, const Portal &ClipPortal, const Portal &OtherPortal,
										float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr)
{
	// fetch some necessary values about path
	float SphereRadius = Cam.GetBoundingSphereRadius();
	XMFLOAT3 S = Cam.GetPosition();
	

	// default values
	XMFLOAT3 ClosestX = S + MoveDist*Dir;
	float ClosestXDist = MoveDist;
	float ClosestRedirectRatio = 1.0f;
	XMFLOAT3 ClosestRedirectDir = Dir;

	// values returned by collisions
	XMFLOAT3 X;
	float XDist;
	float RedirectRatio;
	XMFLOAT3 RedirectDir;


	// check path for collision with Portal ring, virtual/real room, and virtual self

	// collision with the ClipPortal ring
	X = ClipPortal.SpherePathCollision(SphereRadius, S, Dir, MoveDist, &XDist, &RedirectRatio, &RedirectDir);
	UpdateClosestCollision(&ClosestX, &ClosestXDist, &ClosestRedirectRatio, &ClosestRedirectDir, X, XDist, RedirectRatio, RedirectDir);


	XMMATRIX Virtualize = Portal::CalculateVirtualizationMatrix(OtherPortal, ClipPortal);


	// check if this path is heading into or away from the portal
	if (XMFloat3Dot(Dir, ClipPortal.GetNormal()) < 0.0f)	// heading into
	{
		// collision with the virtual room on the other side of ClipPortal
		XMMATRIX Unvirtualize = Portal::CalculateVirtualizationMatrix(ClipPortal, OtherPortal);
		X = Level.SpherePathVirtualCollision(Virtualize, Unvirtualize, SphereRadius, S, Dir, MoveDist, &XDist, &RedirectRatio, &RedirectDir);
	}
	else		// heading out of
	{
		// collision with room
		XMFLOAT3 T, TNormal;
		X = Level.SpherePathCollision(SphereRadius, S, Dir, MoveDist, &XDist, &RedirectRatio, &RedirectDir, &T, &TNormal);
	}
	UpdateClosestCollision(&ClosestX, &ClosestXDist, &ClosestRedirectRatio, &ClosestRedirectDir, X, XDist, RedirectRatio, RedirectDir);



	// collision with virtual self
	X = Cam.SelfVirtualCollision(Virtualize, SphereRadius, S, Dir, MoveDist, &XDist, &RedirectRatio, &RedirectDir);
	UpdateClosestCollision(&ClosestX, &ClosestXDist, &ClosestRedirectRatio, &ClosestRedirectDir, X, XDist, RedirectRatio, RedirectDir);



	// move camera to the end of this path
	Cam.SetPosition(ClosestX);
	
	// if the path from S to ClosestX goes thru the clipportal, transform the camera
	if (ClipPortal.PathCrossesPortal(S, Dir, ClosestXDist))
	{
		Cam.Transform(Virtualize);
	}



	*XDist_ptr = ClosestXDist;
	*RedirectRatio_ptr = ClosestRedirectRatio;
	*RedirectDir_ptr = ClosestRedirectDir;

	return (ClosestXDist < MoveDist && ClosestRedirectRatio != 0.0f);
}