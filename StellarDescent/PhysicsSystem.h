#pragma once
#include "raylib.h"
#include "Rocket.h"

/**
 * @brief Handles physics-related calculations for the game.
 *
 * PhysicsSystem is responsible for evaluating collisions, landing conditions,
 * and other physical interactions in the 2D world.
 */
class PhysicsSystem {
public:
    /**
     * @brief Checks whether the rocket has safely landed on the landing pad.
     *
     * @param rocket Reference to the Rocket object.
     * @param pad Rectangle representing the landing pad in world coordinates.
     * @param maxSpeed Maximum allowed vertical speed for a safe landing.
     * @param maxAngle Maximum allowed tilt (rotation in degrees) for a safe landing.
     * @return true if the rocket is on the pad and satisfies the speed and rotation thresholds.
     * @return false if the rocket is off the pad, too fast, or too tilted.
     *
     * This function shrinks the rocket's collision box slightly to prevent false collisions
     * with the ground and checks for both vertical speed and rotation limits.
     * Should be called every frame when checking for landing conditions.
     */
    bool CheckLanding(const Rocket& rocket, Rectangle pad, float maxSpeed, float maxAngle);
};
