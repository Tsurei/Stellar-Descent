#include "UIManager.h"

void UIManager::DrawHUD(float fuel, float altitude, float timer) {
    // Draw the current fuel level in the top-left corner
    DrawText(TextFormat("Fuel: %.0f", fuel), 20, 20, 20, RAYWHITE);

    // Draw the current altitude below the fuel
    DrawText(TextFormat("Altitude: %.1f", altitude), 20, 50, 20, RAYWHITE);

    // Draw the elapsed flight time below the altitude
    DrawText(TextFormat("Time: %.1f", timer), 20, 80, 20, RAYWHITE);
}

void UIManager::DrawMenu() {
    // Draw the game title centered near the top
    DrawText("STELLAR DESCENT", 440, 200, 40, RAYWHITE);

    // Draw instruction to start the game below the title
    DrawText("Press [ENTER] to Play", 520, 300, 20, RAYWHITE);

    // Draw instruction to exit the game below the start prompt
    DrawText("Press [Q] to Exit", 540, 340, 20, GRAY);
}

void UIManager::DrawWin(float score) {
    // Draw congratulatory message for successful landing
    DrawText("Landing Successful!", 480, 320, 40, GREEN);

    // Prompt to restart the game
    DrawText("Press [R] to Restart", 520, 380, 20, RAYWHITE);

    DrawText(TextFormat("Score: %.0f", score), 500, 420, 30, YELLOW);

}

void UIManager::DrawCrash() {
    // Display crash message
    DrawText("You Crashed!", 530, 320, 40, RED);

    // Prompt to restart after crash
    DrawText("Press [R] to Restart", 520, 380, 20, RAYWHITE);
}

void UIManager::DrawPause() {
    // Dim the background a bit
    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.5f));

    // Draw pause text
    DrawText("PAUSED", 540, 200, 40, YELLOW);
    DrawText("Press [ESC] to Resume", 480, 300, 20, WHITE);
    DrawText("Press [R] to Restart", 500, 340, 20, WHITE);
    DrawText("Press [Q] to Quit", 500, 360, 20, WHITE);
}


