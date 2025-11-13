#pragma once
#include "raylib.h"

/**
 * @brief Types of motion an obstacle can follow.
 */
enum class ObstaclePattern {
    STATIC,
    HORIZONTAL,
    VERTICAL
};

/**
 * @brief Moving/rotating obstacle used as debris or platforms.
 *
 * rect.x, rect.y are always the TOP-LEFT of the obstacle in world space.
 * basePos is the rest (un-offset) center used by the sine motion.
 */
class MovingObstacle {
public:
    Rectangle rect;        // Top-left + size, kept in sync with motion
    Vector2  basePos;      // Rest center position for sine motion

    ObstaclePattern pattern;

    float amplitude;       // sine wave amplitude in pixels
    float frequency;       // sine frequency (multiplier on rotation-time)
    float phase;           // phase offset

    float rotation;        // degrees
    float angularVelocity; // deg/sec

    MovingObstacle(Rectangle r,
        ObstaclePattern p,
        float amp,
        float freq,
        float phaseOffset,
        float angVel);

    void Update(float dt);
    void Draw() const;

    // Pixel-perfect(ish) OBB vs OBB collision against the rocket.
    bool CheckCollisionOBB(Vector2 otherCenter,
        Vector2 otherHalfExtents,
        float otherRotationDeg) const;

    // Broad-phase check against camera / view rectangle
    bool IsNear(const Rectangle& area) const;
};
