#include "Rocket.h"
#include <cmath>
#include "raylib.h"

Rocket::Rocket(Vector2 startPos) {
    // Initialize rocket state
    position = startPos;       // Starting position in world coordinates
    velocity = { 0, 0 };       // No initial movement
    rotation = 0;              // Upright orientation

    // Base difficulty defaults (can be overridden by difficulty presets)
    maxFuel = 100.0f;          // Default max fuel
    gravity = 80.0f;           // Default gravity strength

    fuel = maxFuel;            // Start with full fuel
    isAlive = true;            // Rocket is active
    hasLanded = false;         // Not yet landed
}


void Rocket::Update(float dt) {
    // -------------------- GRAVITY --------------------
    // Simple downward acceleration (pixels/sec^2)
    velocity.y += gravity * dt;

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
    fuel = maxFuel;       // refill according to current difficulty
    isAlive = true;
    hasLanded = false;
}

void Rocket::UpdateParticles(float dt) {
    // Spawn new particles ONLY when thrusting
    if (IsKeyDown(KEY_UP) && fuel > 0)
    {
        Particle p;

        // -------------------- PARTICLE SPAWN POSITION --------------------
        // Thruster is 15px "below" the rocket in *local* space
        Vector2 localOffset = { 0.0f, 15.0f };

        float rad = rotation * DEG2RAD;
        float c = cosf(rad);
        float s = sinf(rad);

        // Rotate local offset by rocket orientation
        p.position.x = position.x + (localOffset.x * c - localOffset.y * s);
        p.position.y = position.y + (localOffset.x * s + localOffset.y * c);

        // -------------------- PARTICLE VELOCITY --------------------
        // Raw randomized exhaust direction (local rocket space)
        float vx = ((float)rand() / RAND_MAX - 0.5f) * 30;   // small sideways jitter
        float vy = 80 + (float)rand() / RAND_MAX * 40;        // strong backward thrust

        // Rotate velocity into world space (same rotation as rocket)
        p.velocity.x = vx * c - vy * s;
        p.velocity.y = vx * s + vy * c;

        // -------------------- PARTICLE SETTINGS --------------------
        p.life = 0.4f + (float)rand() / RAND_MAX * 0.3f;
        p.color = YELLOW;

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

void Rocket::SetDifficultyParams(float gravityStrength, float startingFuel) {
    // -------------------- DIFFICULTY PRESET --------------------
    // Store the new gravity and fuel settings so that all future
    // updates and resets use these values.
    gravity = gravityStrength;
    maxFuel = startingFuel;

    // Ensure current fuel never exceeds the new capacity
    if (fuel > maxFuel) {
        fuel = maxFuel;
    }
}


void Rocket::DrawParticles() {
    for (auto& p : particles) {
        DrawCircle((int)p.position.x, (int)p.position.y, 2, p.color);
    }
}
