#include "Rocket.h"
#include <cmath>
#include "raylib.h"

Rocket::Rocket(Vector2 startPos) {
    // Initialize rocket state
    position = startPos;       // Starting position in world coordinates
    velocity = { 0, 0 };       // No initial movement
    rotation = 0;              // Upright orientation
    fuel = 100;                // Full fuel
    isAlive = true;            // Rocket is active
    hasLanded = false;         // Not yet landed
}

void Rocket::Update(float dt) {
    // -------------------- GRAVITY --------------------
    // Simple downward acceleration (pixels/sec^2)
    velocity.y += 80.0f * dt;

    // -------------------- ROTATION --------------------
    // Rotate left/right based on key input (degrees/sec)
    if (IsKeyDown(KEY_LEFT)) rotation -= 120 * dt;
    if (IsKeyDown(KEY_RIGHT)) rotation += 120 * dt;

    // -------------------- THRUST --------------------
    // Apply thrust if up key is pressed and fuel is available
    if (IsKeyDown(KEY_UP) && fuel > 0) {
        // Convert rotation to radians and adjust so 0° = pointing up
        float rad = (rotation - 90) * DEG2RAD;

        // Apply thrust vector to velocity using basic trigonometry
        velocity.x += cosf(rad) * thrustPower * dt; // horizontal acceleration
        velocity.y += sinf(rad) * thrustPower * dt; // vertical acceleration

        // Consume fuel proportional to time
        fuel -= dt * 10;
        if (fuel < 0) fuel = 0; // clamp to zero
    }

    // -------------------- MOVEMENT --------------------
    // Update position based on velocity
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
}

void Rocket::Draw() {
    // -------------------- ROCKET BODY --------------------
    // Draw a rectangle centered on the rocket's position
    Rectangle body = { position.x, position.y, 10, 30 };
    Vector2 origin = { 5, 15 }; // origin at center of rectangle
    DrawRectanglePro(body, origin, rotation, RAYWHITE);

    // -------------------- THRUST FLAME --------------------
    // Draw flame triangle if thrusting
    if (IsKeyDown(KEY_UP) && fuel > 0) {
        DrawTriangle(
            { position.x - 5, position.y + 15 }, // bottom-left
            { position.x + 5, position.y + 15 }, // bottom-right
            { position.x, position.y + 30 },     // tip of flame
            ORANGE
        );
    }
}

void Rocket::Reset(Vector2 startPos) {
    // Reset rocket state to initial conditions
    position = startPos;
    velocity = { 0, 0 };
    rotation = 0;
    fuel = 100;
    isAlive = true;
    hasLanded = false;
}
