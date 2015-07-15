
Requires installing DirectX End-User Runtimes (June 2010) before running.


This is a demonstration of a simple 3D engine with portal capabilities.  The
portals can be moved, resized, and rotated.  

In this demo, the player is a sphere.  When the program runs, there will be
two screens side by side: the left screen controls a free-roaming third-person
camera; the right screen controls the player (the sphere) from a first-person
perspective.

You are only able to see through portals in the left screen.  This is because
the right screen really only exists so the player can be easily moved around
the room or through portals.  The left screen is intended to watch the player
move in and out of portals to demonstrate the portal effects.

If you move through a portal that's bigger or smaller than the other portal,
you will perceive the room to be scaled in size according to the ratio of the
portal sizes.  Similarly, if the player moves through such a portal, the player
will appear smaller or bigger to the third-person camera.  Similarly, the
perceived orientation of the room can change when going through a portal that's
at a different orientation than the other portal.


CONTROLS ----------------------------------------------------------------------

Screen select:
    '1' - switch control over to left screen
    '2' - switch control over to right screen

Movement:
    Hold and drag left mouse to look around
    'w' - move forward
    's' - move back
    'a' - move left
    'd' - move right
    'Space' - ascend
    'Ctrl'  - descend
    Shift   - hold to sprint

Portal:
    'o' - switch control to orange portal
    'b' - switch control to blue portal
    
    Hold and drag right mouse to move portal to where you're aiming
    Left arrow - rotate portal CCW
    Right arrow - rotate portal CW
    Up arrow - increase portal size
    Down arrow - decrease portal size

    
CREATING A CUSTOM ROOM --------------------------------------------------------

The program generates the room at runtime from the "room.txt" file in the
RoomFiles directory.  The RoomFiles directory also includes a few subdirectories
with alternate "room.txt" files that can replace the one in the RoomFiles
directory.  The files specify the geometry of the room as well as the initial
3D locations of the third-person camera, the player, and the portals.

The room geometry is specified with a floor height, a ceiling height, and
2D polygons.  These polygons specify what the room looks like if you were above
its ceiling and looked straight down.  Typically, one polygon describes the
outer perimeter of the room while subsequent polygons describe pillars inside
the room.  They are distinguished by the winding order in which their vertices
are listed in the file: CCW-ordered polygons are ones that the player must stay
inside of (the outer perimeter); CW-ordered polygons are ones that the player
must stay outside of (pillars inside the room).


KNOWN BUGS --------------------------------------------------------------------

Sometimes the camera or player collides with the plane of the portal and cannot
pass through the portal.


CREDITS -----------------------------------------------------------------------

The code framework, shader lighting equations, and certain textures came from
the disc included with "Introduction to 3D Game Programming with DirectX 11" by
Frank D. Luna.  