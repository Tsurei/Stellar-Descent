#pragma once
#include "raylib.h"

/**
 * @brief Represents a planet in the game world.
 *
 * Each planet defines its gravitational pull and the position of a landing pad.
 * This struct is used by the PhysicsSystem and gameplay logic to determine
 * how the rocket moves and where it can safely land.
 */
struct Planet {
    /// Gravity strength of the planet (affects rocket acceleration downward)
    float gravity;

    /// Rectangle representing the landing pad in world coordinates
    Rectangle landingPad;
};
