#ifndef SPHEREPATH_H
#define SPHEREPATH_H

#include "d3dUtil.h"
#include "Camera.h"
#include "Room.h"
#include "Portal.h"
#include "FirstPersonObject.h"

class SpherePath
{
public:
	static void MoveCameraAlongPathIterative(Camera &Cam, XMFLOAT3 Dir, float MoveDist,
									const Room &Level, const Portal &OrangePortal, const Portal &BluePortal);

	/*
	static XMFLOAT3 SpherePathNoSelfClipFindEnd(const FirstPersonObject &Player, XMFLOAT3 S, XMFLOAT3 Dir, float MoveDist,
											const Room &Level, const Portal &OrangePortal, const Portal &BluePortal);
	*/

private:

	// returns whether or not a redirect is necessary
	static bool MoveCameraAlongPath(Camera &Cam, XMFLOAT3 Dir, float MoveDist,
										const Room &Level, const Portal &OrangePortal, const Portal &BluePortal,
										float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr);

	static void UpdateClosestCollision(XMFLOAT3 *ClosestX_ptr, float *ClosestXDist_ptr,
									float *ClosestRedirectRatio_ptr, XMFLOAT3 *ClosestRedirectDir_ptr,
									const XMFLOAT3 &X, float XDist, float RedirectRatio, const XMFLOAT3 &RedirectDir);


	// computes collision for a camera that's already clipping a portal that moves
	// against the portal normal (heading into portal)
	static bool MoveClippedCamera(Camera &Cam, XMFLOAT3 Dir, float MoveDist,
										const Room &Level, const Portal &ClipPortal, const Portal &OtherPortal,
										float *XDist_ptr, float *RedirectRatio_ptr, XMFLOAT3 *RedirectDir_ptr);
};

#endif