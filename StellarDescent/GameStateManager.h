#pragma once

/**
 * @brief Represents the different states of the game.
 *
 * The GameState enum is used to control the flow of the game loop,
 * deciding what logic and rendering should occur at any given moment.
 */
enum class GameState {
    /// Main menu screen before gameplay starts
    MENU,

    /// The player is actively controlling the rocket
    PLAYING,

    /// The rocket has landed successfully on the landing pad
    WIN,

    /// The rocket has crashed or failed to land safely
    CRASH
};
