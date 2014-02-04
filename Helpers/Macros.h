#ifndef MACROS_H
#define MACROS_H


// used in main
#define ROOM_FILE_PATH "./RoomFiles/room.txt"
#define PORTAL_ITERATIONS 30

#define ORANGE_STENCIL_REF 10
#define BLUE_STENCIL_REF 100

#define CAMERA_MOVEMENT_SPEED 5.0f		// unscaled movement speed of camera, in m/s
#define CAMERA_MOVEMENT_SPRINT_MULTIPLIER 3.0f
#define CAMERA_ROLL_SPEED 60.0f			// speed at which camera rolls, in deg/s
#define PORTAL_ROTATE_SPEED 60.0f		// speed at which portal rolls, in deg/s
#define PORTAL_SIZE_CHANGE_SPEED 1.5f	// speed at which portal radius changes, in m/s



// keepout radius for the camera when it's not attached to any FirstPersonObject
// should not be 0: that allows camera to clip through room at corners
#define CAMERA_SPHERE_RADIUS 0.02f

// room, portal
#define T_THRESHOLD 0.01f	// X=S+t*Dir, no collision if t<-T_THRESHOLD. T_THRESHOLD should be nonnegative
#define T_BUMP 0.001f		// t -= T_BUMP before calculating X.  Slightly bumps the point of collision away from the boundary.

// portal
#define DISC_CONTAINS_THRESHOLD 0.01f	// used in DiscContainsPoint. returns true if point is within this value of disc plane
#define PORTAL_BOX_DEPTH 0.02f			// the depth of the portalbox used in place of the disc when camera is too close for disc to render
#define PORTAL_BOX_N_SIDES 16			// the portalbox will be an N-gon prism

#define ITERATIVE_THRESHOLD 0.00005f		// stop solving for t when accuracy of t reaches this threshold
#define SPHERE_INTERSECT_RING_THRESHOLD 0.001f	// used in SpherePathCollision when checking if a larger sphere already intersects the ring

#define PORTALS_SAME_PLANE_THRESHOLD 0.01f	// used in PortalRelocate to determine if 2 portals are in the same plane

#define PORTAL_MIN_PHYS_RADIUS 0.1f				// used in PortalRelocate to limit how small portals can be



#endif