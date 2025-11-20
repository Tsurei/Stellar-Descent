#include "MovingObstacle.h"
#include "raylib.h"
#include <cmath>

// -------------------- OBB COLLISION HELPERS --------------------
namespace
{
    // Build 4 world-space vertices for a box with given center, half-extents and rotation.
    // v[0] = top-left, v[1] = top-right, v[2] = bottom-right, v[3] = bottom-left
    static void BuildBoxVertices(Vector2 center,
        Vector2 half,
        float rotDeg,
        Vector2 out[4])
    {
        float rad = rotDeg * DEG2RAD;
        float c = cosf(rad);
        float s = sinf(rad);

        Vector2 local[4] = {
            { -half.x, -half.y },
            {  half.x, -half.y },
            {  half.x,  half.y },
            { -half.x,  half.y }
        };

        for (int i = 0; i < 4; ++i) {
            float x = local[i].x;
            float y = local[i].y;

            out[i].x = center.x + x * c - y * s;
            out[i].y = center.y + x * s + y * c;
        }
    }

    inline Vector2 Sub(Vector2 a, Vector2 b) { return { a.x - b.x, a.y - b.y }; }
    inline float   Dot(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }

    inline Vector2 Normalize(Vector2 v)
    {
        float len = sqrtf(v.x * v.x + v.y * v.y);
        if (len <= 1e-6f) return { 0.0f, 0.0f };
        return { v.x / len, v.y / len };
    }

    void ProjectOntoAxis(const Vector2 verts[4], Vector2 axis, float& outMin, float& outMax)
    {
        float p0 = Dot(verts[0], axis);
        float p1 = Dot(verts[1], axis);
        float p2 = Dot(verts[2], axis);
        float p3 = Dot(verts[3], axis);

        outMin = fminf(fminf(p0, p1), fminf(p2, p3));
        outMax = fmaxf(fmaxf(p0, p1), fmaxf(p2, p3));
    }

    bool OverlapsOnAxis(const Vector2 vertsA[4], const Vector2 vertsB[4], Vector2 axis)
    {
        axis = Normalize(axis);
        if (axis.x == 0.0f && axis.y == 0.0f) return true;

        float minA, maxA;
        float minB, maxB;
        ProjectOntoAxis(vertsA, axis, minA, maxA);
        ProjectOntoAxis(vertsB, axis, minB, maxB);

        return !(maxA < minB || maxB < minA);
    }

    bool CheckOBBCollision(const Vector2 vertsA[4], const Vector2 vertsB[4])
    {
        // axes: 2 from A, 2 from B
        Vector2 axes[4];
        axes[0] = Sub(vertsA[1], vertsA[0]);
        axes[1] = Sub(vertsA[3], vertsA[0]);
        axes[2] = Sub(vertsB[1], vertsB[0]);
        axes[3] = Sub(vertsB[3], vertsB[0]);

        for (int i = 0; i < 4; ++i) {
            if (!OverlapsOnAxis(vertsA, vertsB, axes[i])) return false;
        }
        return true;
    }
} // anonymous namespace

// ---------------------------------------------------------------

MovingObstacle::MovingObstacle(Rectangle r,
    ObstaclePattern p,
    float amp,
    float freq,
    float phaseOffset,
    float angVel)
    : rect(r),
    basePos{ r.x + r.width * 0.5f, r.y + r.height * 0.5f },
    pattern(p),
    amplitude(amp),
    frequency(freq),
    phase(phaseOffset),
    rotation(0.0f),
    angularVelocity(angVel)
{
    // everything initialized in initializer list
}

void MovingObstacle::Update(float dt)
{
    // -------------------- ROTATION --------------------
    rotation += angularVelocity * dt;

    // -------------------- POSITION (SINE MOTION) --------------------
    float t = rotation * DEG2RAD * frequency + phase;

    // Center starts at base position
    Vector2 center = basePos;

    switch (pattern) {
    case ObstaclePattern::STATIC:
        break;
    case ObstaclePattern::HORIZONTAL:
        center.x += std::sinf(t) * amplitude;
        break;
    case ObstaclePattern::VERTICAL:
        center.y += std::sinf(t) * amplitude;
        break;
    }

    // Keep rect synced: rect is always TOP-LEFT based on this center
    rect.x = center.x - rect.width * 0.5f;
    rect.y = center.y - rect.height * 0.5f;
}

void MovingObstacle::Draw() const
{
    // Compute center & half-extents directly from rect every frame
    Vector2 center = { rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f };
    Vector2 half = { rect.width * 0.5f, rect.height * 0.5f };

    // Build the 4 vertices used drawing and collision
    Vector2 v[4];
    BuildBoxVertices(center, half, rotation, v);


    // -------------------- DEBUG OUTLINE (Currently only visible way) --------------------
    for (int i = 0; i < 4; ++i) {
        DrawLineV(v[i], v[(i + 1) % 4], RED);
    }
}

bool MovingObstacle::CheckCollisionOBB(Vector2 otherCenter,
    Vector2 otherHalfExtents,
    float otherRotationDeg) const
{
    // Obstacle OBB (from rect)
    Vector2 centerObs = { rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f };
    Vector2 halfObs = { rect.width * 0.5f, rect.height * 0.5f };

    Vector2 vertsObstacle[4];
    BuildBoxVertices(centerObs, halfObs, rotation, vertsObstacle);

    // Rocket OBB
    Vector2 vertsRocket[4];
    BuildBoxVertices(otherCenter, otherHalfExtents, otherRotationDeg, vertsRocket);

    return CheckOBBCollision(vertsObstacle, vertsRocket);
}

bool MovingObstacle::IsNear(const Rectangle& area) const
{
    // Simple broad-phase AABB against camera/view rect
    return CheckCollisionRecs(rect, area);
}
