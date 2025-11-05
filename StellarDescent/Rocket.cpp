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

    UpdateParticles(dt);

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
    DrawRectanglePro(body, origin, rotation, ORANGE);

    // -------------------- THRUST FLAME --------------------
    // Draw flame triangle if thrusting (CURRENTLY NOT WORKING)
    if (IsKeyDown(KEY_UP) && fuel > 0) {
        DrawTriangle(
            { position.x - 5, position.y + 15 }, // bottom-left
            { position.x + 5, position.y + 15 }, // bottom-right
            { position.x, position.y + 30 },     // tip of flame
            ORANGE
        );
    }

    DrawParticles(); // draw particles under rocket
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

void Rocket::UpdateParticles(float dt) {
    // Spawn new particle if thrusting
    if (IsKeyDown(KEY_UP) && fuel > 0) {
        Particle p;
        p.position = { position.x, position.y + 15 }; // under rocket
        p.velocity = { ((float)rand() / RAND_MAX - 0.5f) * 50, 50 + (float)rand() / RAND_MAX * 50 };
        p.life = 0.5f + (float)rand() / RAND_MAX * 0.5f;
        p.color = ORANGE;
        particles.push_back(p);
    }

    // Update all particles
    for (size_t i = 0; i < particles.size(); i++) {
        particles[i].position.x += particles[i].velocity.x * dt;
        particles[i].position.y += particles[i].velocity.y * dt;
        particles[i].life -= dt;
    }

    // Remove dead particles
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](Particle& p) { return p.life <= 0; }), particles.end());
}

void Rocket::DrawParticles() {
    for (auto& p : particles) {
        DrawCircle((int)p.position.x, (int)p.position.y, 2, p.color);
    }
}
