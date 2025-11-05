#include "CameraController.h"

void CameraController::Init(Vector2 startPos) {
    camera.target = startPos;
    camera.offset = { 640, 360 }; // center of screen
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void CameraController::Update(Vector2 target) {
    camera.target = target;
}
