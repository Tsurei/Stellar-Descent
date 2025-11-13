#include "UIManager.h"

void UIManager::Init() {
    // -------------------- LOAD FONT --------------------
    // We want ASCII (32–126) + arrows: ← ↑ → ↓ (U+2190–U+2193)
    int asciiCount = 95;               // ASCII 32–126
    int arrowsCount = 4;               // ← ↑ → ↓
    int totalCodepoints = asciiCount + arrowsCount;

    // Allocate codepoint array
    int* codepoints = (int*)malloc(sizeof(int) * totalCodepoints);

    // Fill ASCII
    for (int i = 0; i < asciiCount; i++) {
        codepoints[i] = 32 + i;
    }

    // Add arrow codepoints
    codepoints[asciiCount + 0] = 0x2190; // ←
    codepoints[asciiCount + 1] = 0x2191; // ↑
    codepoints[asciiCount + 2] = 0x2192; // →
    codepoints[asciiCount + 3] = 0x2193; // ↓


    unicode = LoadFontEx("assets/fonts/NotoSansSymbols-ExtraBold.ttf", 256, codepoints, totalCodepoints);
    title = LoadFontEx("assets/fonts/Monlight.otf", 64, 0, 0);
    // Free temporary array
    free(codepoints);

}
void UIManager::DrawHUD(float fuel, float altitude, float timer) {
    // Draw the current fuel level in the top-left corner
    DrawText(TextFormat("Fuel: %.0f", fuel), 20, 20, 20, RAYWHITE);

    // Draw the current altitude below the fuel
    DrawText(TextFormat("Altitude: %.1f", altitude), 20, 50, 20, RAYWHITE);

    // Draw the elapsed flight time below the altitude
    DrawText(TextFormat("Time: %.1f", timer), 20, 80, 20, RAYWHITE);
}

void UIManager::DrawMenu(const std::string& difficultyLabel) {
    // Draw the game title centered near the top
    //DrawText("STELLAR DESCENT", 440, 200, 40, RAYWHITE);
    DrawTextEx(
        title,
        "Stellar",
        { 480, 130 },
        126,
        2,
        SKYBLUE
    );
    DrawTextEx(
        title,
        "Descent",
        { 550, 200 },
        126,
        2,
        BLUE
    );

    // Draw instruction to start the game below the title
    DrawText("Press [ENTER] to Play", 520, 300, 40, RAYWHITE);

    // Draw instruction to exit the game below the start prompt
    DrawText("Press [Q] to Exit", 520, 340, 40, RAYWHITE);

    // Draw instructions on how to play the game using the Unicode font
    DrawTextEx(
        unicode,
        u8"Press ↑ to add thrust!",
        { 450, 390 },   // position
        60,             // font size
        2,              // spacing between characters
        DARKBLUE
    );

    DrawTextEx(
        unicode,
        u8"Press ← or → to rotate the ship",
        { 385, 420 },   // position
        60,             // font size
        2,              // spacing between characters
        DARKBLUE
    );

    // -------------------- DIFFICULTY INFO --------------------
    // Show current difficulty and how to change it along the bottom.
    DrawText(
        TextFormat("Difficulty: %s  [1]=Easy  [2]=Normal  [3]=Hard", difficultyLabel.c_str()),
        20,
        680,
        20,
        RAYWHITE
    );
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

void UIManager::Close() {
    UnloadFont(unicode);
}


