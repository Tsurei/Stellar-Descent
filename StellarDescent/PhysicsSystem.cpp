#include "PhysicsSystem.h"
#include <cmath>
#include "raylib.h"
#include "raymath.h"

bool PhysicsSystem::CheckLanding(const Rocket& rocket, Rectangle pad, float maxVerticalSpeed, float maxRotationDeg)
{
    // -------------------- ROCKET COLLISION BOX --------------------
    // Create a rectangle representing the rocket for collision detection
    // Shrink it slightly so the rocket can land on pad without triggering false crashes
    Rectangle rocketRect = {
        rocket.position.x - 5,  // shift left by half width
        rocket.position.y - 12, // shift up slightly less than half height
        10,                      // width
        24                       // height (slightly shorter than full rocket)
    };

    // -------------------- CHECK IF ROCKET IS ON PAD --------------------
    // Use Raylib's rectangle collision detection
    bool onPad = CheckCollisionRecs(rocketRect, pad);

    // -------------------- GET VERTICAL SPEED AND ROTATION --------------------
    // Only vertical speed matters for safe landing; horizontal speed is ignored
    float verticalSpeed = rocket.velocity.y;

    // Current rotation in degrees; 0 = upright
    float rotation = rocket.rotation;

    // -------------------- DEBUG DRAWING --------------------
    // Draw the landing pad in green and rocket collision box in red
    // Useful for visualizing landing collisions
    DrawRectangleLinesEx(pad, 2, GREEN);
    DrawRectangleLinesEx(rocketRect, 2, RED);

    // Display vertical speed and rotation on screen for debugging
    DrawText(TextFormat("Vert Speed: %.1f  Rot: %.1f", verticalSpeed, rotation), 20, 120, 20, WHITE);

    // -------------------- LANDING CONDITIONS --------------------
    // Check if vertical speed is slow enough for safe landing
    bool safeVertical = verticalSpeed <= maxVerticalSpeed;

    // Check if rocket is mostly upright within tolerance, 
    float normalizedRotation = std::fmod(rotation, 360.0f);
    if (normalizedRotation > 180.0f)
        normalizedRotation -= 360.0f;
    else if (normalizedRotation < -180.0f)
        normalizedRotation += 360.0f;
    bool safeRotation = std::fabs(normalizedRotation) <= maxRotationDeg;

    // Only return true if rocket is over pad AND meets speed & rotation requirements
    return onPad && safeVertical && safeRotation;
}
