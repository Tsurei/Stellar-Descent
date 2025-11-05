#pragma once
#include "raylib.h"
#include <string>

/**
 * @brief Handles all on-screen user interface (UI) elements.
 *
 * UIManager draws HUD elements such as fuel, altitude, and timer,
 * as well as screens for menu, win, and crash states.
 */
class UIManager {
public:
    /**
     * @brief Draw the in-game HUD (Heads-Up Display).
     *
     * @param fuel Current fuel level of the rocket
     * @param altitude Current altitude of the rocket
     * @param timer Total elapsed flight time in seconds
     *
     * Draws text elements in the top-left corner of the screen to give the player
     * real-time feedback on fuel, altitude, and flight duration.
     */
    void DrawHUD(float fuel, float altitude, float timer);

    /**
     * @brief Draw the main menu screen.
     *
     * Typically called when GameState is MENU. Displays the game title and instructions
     * for starting or exiting the game.
     */
    void DrawMenu();

    /**
     * @brief Draw the win screen after a successful landing.
     *
     * Typically called when GameState is WIN. Shows congratulatory text and restart instructions.
     */
    void DrawWin();

    /**
     * @brief Draw the crash/fail screen.
     *
     * Typically called when GameState is CRASH. Shows crash message and restart instructions.
     */
    void DrawCrash();

    /**
     * @brief Draw the pause menu overlay.
    *
    * Typically called when the game is paused.
    */
    void DrawPause();

};
