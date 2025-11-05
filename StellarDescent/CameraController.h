#pragma once
#include "raylib.h"

/**
 * @brief Handles camera movement and positioning in the 2D game world.
 *
 * CameraController wraps Raylib's Camera2D structure to follow a target (e.g., the rocket)
 * and provides a smooth, centralized view of the gameplay area.
 */
class CameraController {
public:
    /// The Raylib 2D camera used to transform the game world
    Camera2D camera;

    /**
     * @brief Initialize the camera with a starting position.
     *
     * @param startPos The initial position in world coordinates that the camera will center on.
     *
     * Sets the camera target to the start position and defines the screen offset and zoom.
     * Typically called once at the start of the game or when resetting.
     */
    void Init(Vector2 startPos);

    /**
     * @brief Update the camera position to follow a target.
     *
     * @param target The world position that the camera should center on (e.g., the rocket's position).
     *
     * Call this every frame during gameplay to keep the rocket centered in view.
     */
    void Update(Vector2 target);
};
