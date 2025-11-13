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
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // -------------------- TITLE  --------------------

    const float stellarSize = 156.0f;
    const float descentSize = 100.0f;      
    const float spacing = 2.0f;

    // Measure text sizes
    Vector2 stellarMeasure = MeasureTextEx(title, "Stellar", stellarSize, spacing);
    Vector2 descentMeasure = MeasureTextEx(title, "Descent", descentSize, spacing);

    float stellarX = screenWidth / 2.0f - stellarMeasure.x / 2.0f;
    float stellarY = 120.0f;

    float descentX = stellarX + stellarMeasure.x * 0.18f;
    float descentY = stellarY + stellarMeasure.y - 70.0f;

    // Draw Stellar (main)
    DrawTextEx(
        title,
        "Stellar",
        { stellarX, stellarY },
        stellarSize,
        spacing,
        SKYBLUE
    );

    // Draw Descent (subtitle)
    DrawTextEx(
        title,
        "Descent",
        { descentX, descentY },
        descentSize,
        spacing,
        BLUE
    );


    // -------------------- MENU PROMPTS --------------------
    // Center "Press [ENTER] to Play" and "Press [Q] to Exit" horizontally.
    const int menuFontSize = 40;

    const char* playText = "Press [ENTER] to Play";
    int playWidth = MeasureText(playText, menuFontSize);
    DrawText(
        playText,
        screenWidth / 2 - playWidth / 2,
        300,
        menuFontSize,
        RAYWHITE
    );

    const char* quitText = "Press [Q] to Exit";
    int quitWidth = MeasureText(quitText, menuFontSize);
    DrawText(
        quitText,
        screenWidth / 2 - quitWidth / 2,
        340,
        menuFontSize,
        RAYWHITE
    );

    // -------------------- CONTROL INSTRUCTIONS --------------------
    // Center the Unicode control hints using the Unicode font.
    const float controlsFontSize = 60.0f;
    const float controlsSpacing = 2.0f;

    const char* thrustText = u8"Press ↑ to add thrust!";
    Vector2 thrustSize = MeasureTextEx(
        unicode,
        thrustText,
        controlsFontSize,
        controlsSpacing
    );
    Vector2 thrustPos = {
        screenWidth / 2.0f - thrustSize.x / 2.0f,
        390.0f
    };
    DrawTextEx(
        unicode,
        thrustText,
        thrustPos,
        controlsFontSize,
        controlsSpacing,
        BLUE
    );

    const char* rotateText = u8"Press ← or → to rotate the ship";
    Vector2 rotateSize = MeasureTextEx(
        unicode,
        rotateText,
        controlsFontSize,
        controlsSpacing
    );
    Vector2 rotatePos = {
        screenWidth / 2.0f - rotateSize.x / 2.0f,
        thrustPos.y + thrustSize.y - 20.0f
    };
    DrawTextEx(
        unicode,
        rotateText,
        rotatePos,
        controlsFontSize,
        controlsSpacing,
        BLUE
    );

    // -------------------- DIFFICULTY INFO (BOTTOM-RIGHT) --------------------
    // Draw current difficulty and hotkeys anchored to bottom-right corner.
    const int diffFontSize = 20;
    const char* diffText = TextFormat(
        "Difficulty: %s  [1]=Easy  [2]=Normal  [3]=Hard",
        difficultyLabel.c_str()
    );

    int diffWidth = MeasureText(diffText, diffFontSize);

    DrawText(
        diffText,
        screenWidth - diffWidth - 20,
        screenHeight - diffFontSize - 20,
        diffFontSize,
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


