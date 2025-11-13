#pragma once
#include "raylib.h"

/**
 * @brief Types of motion an obstacle can follow.
 *
 * STATIC      - no movement
 * HORIZONTAL  - moves left/right in a sine pattern
 * VERTICAL    - moves up/down in a sine pattern
 */
enum class ObstaclePattern {
    STATIC,
    HORIZONTAL,
    VERTICAL
};

/**
 * @brief Represents a moving and/or rotating obstacle in world space.
 *
 * Each obstacle has a base position, a movement pattern (static / sine),
 * and optional rotation. Collision is done using the Rectangle in world
 * coordinates after movement is applied.
 */
class MovingObstacle {
public:
    Rectangle rect;        // current rectangle used for drawing/collision
    Vector2  basePos;      // original center position (for sine offsets)
    ObstaclePattern pattern;

    float amplitude;       // how far it moves from basePos (pixels)
    float frequency;       // how fast it oscillates (cycles per second)
    float phase;           // phase offset so they don't all sync

    float rotation;        // current rotation angle (degrees)
    float angularVelocity; // rotation speed (deg/sec)

    MovingObstacle() = default;

    MovingObstacle(Rectangle r,
        ObstaclePattern p,
        float amp,
        float freq,
        float phaseOffset,
        float angVel);

    // Update position/rotation based on time
    void Update(float dt);

    // Draw the obstacle
    void Draw() const;

    // Axis-aligned collision (rotation is visual only)
    bool CheckCollision(const Rectangle& other) const;

    // Helper to check if this obstacle is near a given area (e.g. camera view)
    bool IsNear(const Rectangle& area) const;
};
