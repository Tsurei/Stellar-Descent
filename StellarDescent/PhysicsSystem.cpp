#include "PhysicsSystem.h"
#include <cmath>
#include "raylib.h"
#include "raymath.h"

bool PhysicsSystem::CheckLanding(const Rocket& rocket,
    Rectangle /*pad*/,
    float maxVerticalSpeed,
    float maxRotationDeg)
{
    // -------------------- GET VERTICAL SPEED AND ROTATION --------------------
    float verticalSpeed = rocket.velocity.y;   // down is positive
    float rotation = rocket.rotation;     // degrees, 0 = upright

    // Normalize rotation to [-180, 180]
    float normalizedRotation = std::fmod(rotation, 360.0f);
    if (normalizedRotation > 180.0f)
        normalizedRotation -= 360.0f;
    else if (normalizedRotation < -180.0f)
        normalizedRotation += 360.0f;

    // -------------------- LANDING CONDITIONS --------------------
    bool safeVertical = (verticalSpeed <= maxVerticalSpeed);
    bool safeRotation = (std::fabs(normalizedRotation) <= maxRotationDeg);

    // Only care about speed + rotation here.
    // Being on the pad is checked in main.cpp.
    return safeVertical && safeRotation;
}
