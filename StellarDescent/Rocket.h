#pragma once
#include "raylib.h"

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

    /// Flag indicating whether the rocket is still active/alive
    bool isAlive;

    /// Flag indicating whether the rocket has successfully landed
    bool hasLanded;

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

private:
    /// Constant thrust power applied when the up key is pressed
    const float thrustPower = 200.0f;
};
