#include "CameraController.h"
#include "raymath.h" // for Vector2Lerp

void CameraController::Init(Vector2 startPos) {
    camera.target = startPos;
    camera.offset = { 640, 360 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void CameraController::Update(Vector2 target, float dt) {
    // Smooth follow using linear interpolation
    Vector2 desired = target;

    // Lerp each axis separately
    camera.target.x = camera.target.x + (desired.x - camera.target.x) * (1 - expf(-dt / smoothTime));
    camera.target.y = camera.target.y + (desired.y - camera.target.y) * (1 - expf(-dt / smoothTime));

    // Clamp to scroll limits
    if (camera.target.x < minScroll.x) camera.target.x = minScroll.x;
    if (camera.target.x > maxScroll.x) camera.target.x = maxScroll.x;
    if (camera.target.y < minScroll.y) camera.target.y = minScroll.y;
    if (camera.target.y > maxScroll.y) camera.target.y = maxScroll.y;
}
