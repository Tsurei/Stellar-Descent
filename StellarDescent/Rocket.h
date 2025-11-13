#pragma once
#include "raylib.h"
#include <vector>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life;       // remaining life in seconds
    Color color;
};

/**
 * @brief Represents the player's rocket and handles its physics and rendering.
 *
 * The Rocket class stores position, velocity, rotation, fuel, and status flags.
 * It provides methods for updating movement, drawing the rocket, and resetting its state.
 */
class Rocket {
public:
    /// Current position of the rocket in world coordinates
    Vector2 position;

    /// Current velocity of the rocket in units per second
    Vector2 velocity;

    /// Current rotation angle in degrees (0 = upright)
    float rotation;

    /// Remaining fuel for thrust
    float fuel;

    /// Maximum fuel capacity for the rocket (used when resetting/difficulty changes)
    float maxFuel = 100.0f;

    /// Gravity strength currently affecting the rocket (pixels/sec^2)
    float gravity = 80.0f;

    /// Flag indicating whether the rocket is still active/alive
    bool isAlive;

    /// Flag indicating whether the rocket has successfully landed
    bool hasLanded;

    std::vector<Particle> particles;

    void UpdateParticles(float dt);
    void DrawParticles();

    /**
     * @brief Constructor to initialize the rocket at a starting position.
     *
     * @param startPos Initial position in world coordinates
     */
    Rocket(Vector2 startPos);

    /**
     * @brief Update the rocket's physics and input per frame.
     *
     * @param dt Delta time since the last frame (seconds)
     *
     * Handles:
     * - Gravity affecting vertical velocity
     * - Rotation input via left/right keys
     * - Thrust input via up key
     * - Fuel consumption
     * - Movement based on velocity
     */
    void Update(float dt);

    /**
     * @brief Draws the rocket to the screen.
     *
     * Uses Raylib's DrawRectanglePro for the body and DrawTriangle for the flame.
     * Only shows flame if the rocket is actively thrusting and has fuel.
     */
    void Draw();

    /**
     * @brief Resets the rocket's state to a new starting position.
     *
     * @param startPos New position to reset the rocket to
     *
     * Resets velocity, rotation, fuel, and status flags.
     */
    void Reset(Vector2 startPos);

    /**
     * @brief Get remaining fuel as a normalized fraction.
     *
     * @return Value between 0.0f (empty) and 1.0f (full).
     *         Uses maxFuel as the reference capacity.
     */
    float GetFuelFraction() const { return maxFuel > 0 ? fuel / maxFuel : 0.0f; }

    /**
     * @brief Configure the rocket's physics based on difficulty.
     *
     * @param gravityStrength New gravity acceleration in pixels/sec^2
     * @param startingFuel    New maximum/starting fuel amount
     *
     * This does not move the rocket; it just changes how fast it falls
     * and how much fuel it starts with on reset.
     */
    void SetDifficultyParams(float gravityStrength, float startingFuel);


private:
    /// Constant thrust power applied when the up key is pressed
    const float thrustPower = 200.0f;
};
