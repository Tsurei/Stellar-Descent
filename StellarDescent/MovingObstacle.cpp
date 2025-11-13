#include "MovingObstacle.h"
#include "raylib.h"
#include <cmath>

MovingObstacle::MovingObstacle(Rectangle r,
    ObstaclePattern p,
    float amp,
    float freq,
    float phaseOffset,
    float angVel)
{
    rect = r;
    basePos = { r.x + r.width * 0.5f, r.y + r.height * 0.5f }; // store center
    pattern = p;
    amplitude = amp;
    frequency = freq;
    phase = phaseOffset;
    rotation = 0.0f;
    angularVelocity = angVel;
}

void MovingObstacle::Update(float dt)
{
    // Advance rotation
    rotation += angularVelocity * dt;

    // Time for sine wave (we piggy-back on rotation for simplicity)
    float t = rotation * DEG2RAD * frequency + phase;

    // Start at base position
    float x = basePos.x;
    float y = basePos.y;

    switch (pattern) {
    case ObstaclePattern::STATIC:
        // no movement
        break;
    case ObstaclePattern::HORIZONTAL:
        x += std::sinf(t) * amplitude;
        break;
    case ObstaclePattern::VERTICAL:
        y += std::sinf(t) * amplitude;
        break;
    }

    // Update rect position from center back to top-left
    rect.x = x - rect.width * 0.5f;
    rect.y = y - rect.height * 0.5f;
}

void MovingObstacle::Draw() const
{
    // Draw as a rotating rectangle
    Vector2 origin = { rect.width * 0.5f, rect.height * 0.5f };
    DrawRectanglePro(rect, origin, rotation, RED);
}

bool MovingObstacle::CheckCollision(const Rectangle& other) const
{
    return CheckCollisionRecs(rect, other);
}

bool MovingObstacle::IsNear(const Rectangle& area) const
{
    // Simple AABB check against a "view rectangle"
    return CheckCollisionRecs(rect, area);
}
