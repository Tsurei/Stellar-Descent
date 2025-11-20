#include "UIManager.h"
#include "raylib.h"
#include <cmath>

void UIManager::Init() {
    // -------------------- LOAD FONT --------------------
    int asciiCount = 95;
    int arrowsCount = 4;
    int total = asciiCount + arrowsCount;

    int* codes = (int*)malloc(sizeof(int) * total);

    for (int i = 0; i < asciiCount; i++)
        codes[i] = 32 + i;

    codes[asciiCount + 0] = 0x2190;
    codes[asciiCount + 1] = 0x2191;
    codes[asciiCount + 2] = 0x2192;
    codes[asciiCount + 3] = 0x2193;

    unicode = LoadFontEx("assets/fonts/NotoSansSymbols-ExtraBold.ttf", 256, codes, total);
    title = LoadFontEx("assets/fonts/Monlight.otf", 64, nullptr, 0);

    free(codes);
}

void UIManager::DrawHUD(float fuel, float altitude, float timer) {
    DrawText(TextFormat("Fuel: %.0f", fuel), 20, 20, 20, RAYWHITE);
    DrawText(TextFormat("Altitude: %.1f", altitude), 20, 50, 20, RAYWHITE);
    DrawText(TextFormat("Time: %.1f", timer), 20, 80, 20, RAYWHITE);
}

void UIManager::DrawMenu(const std::string& difficultyLabel,
    const std::string& levelLabel)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // -------------------- TITLE --------------------
    const float stellarSize = 156.0f;
    const float descentSize = 100.0f;
    const float spacing = 2.0f;

    Vector2 stellarMeasure = MeasureTextEx(title, "Stellar", stellarSize, spacing);
    Vector2 descentMeasure = MeasureTextEx(title, "Descent", descentSize, spacing);

    float stellarX = screenWidth / 2.0f - stellarMeasure.x / 2.0f;
    float stellarY = 120.0f;

    float descentX = stellarX + stellarMeasure.x * 0.18f;
    float descentY = stellarY + stellarMeasure.y - 70.0f;

    DrawTextEx(title, "Stellar", { stellarX, stellarY }, stellarSize, spacing, SKYBLUE);
    DrawTextEx(title, "Descent", { descentX, descentY }, descentSize, spacing, BLUE);

    // -------------------- START / QUIT --------------------
    const int menuFontSize = 40;

    const char* playText = "Press [ENTER] to Play";
    int playWidth = MeasureText(playText, menuFontSize);
    DrawText(playText, screenWidth / 2 - playWidth / 2, 300, menuFontSize, RAYWHITE);

    const char* quitText = "Press [Q] to Exit";
    int quitWidth = MeasureText(quitText, menuFontSize);
    DrawText(quitText, screenWidth / 2 - quitWidth / 2, 340, menuFontSize, RAYWHITE);

    // -------------------- CONTROLS --------------------
    const float controlsFontSize = 60.0f;

    const char* thrustText = u8"Press ↑ to add thrust!";
    Vector2 thrustSize = MeasureTextEx(unicode, thrustText, controlsFontSize, 2.0f);
    DrawTextEx(unicode, thrustText,
        { screenWidth / 2.0f - thrustSize.x / 2.0f, 390.0f },
        controlsFontSize, 2.0f, BLUE);

    const char* rotateText = u8"Press ← or → to rotate the ship";
    Vector2 rotateSize = MeasureTextEx(unicode, rotateText, controlsFontSize, 2.0f);
    DrawTextEx(unicode, rotateText,
        { screenWidth / 2.0f - rotateSize.x / 2.0f, 390.0f + thrustSize.y - 20 },
        controlsFontSize, 2.0f, BLUE);

    // -------------------- LEVEL & DIFFICULTY (bottom-right) --------------------
    const int infoFontSize = 22;

    std::string info = TextFormat("Difficulty: %s   Level: %s",
        difficultyLabel.c_str(),
        levelLabel.c_str());

    int infoWidth = MeasureText(info.c_str(), infoFontSize);

    DrawText(info.c_str(),
        screenWidth - infoWidth - 20,
        screenHeight - infoFontSize - 20,
        infoFontSize,
        RAYWHITE);
}

void UIManager::DrawWin(float score) {
    DrawText("Landing Successful!", 480, 300, 40, GREEN);
    DrawText(TextFormat("Score: %.0f", score), 500, 350, 30, YELLOW);
    DrawText("Press [ENTER] to Continue", 460, 410, 20, WHITE);
    DrawText("Press [R] to Restart Level", 460, 440, 20, WHITE);
    DrawText("Press [M] to return to Main Menu", 440, 470, 20, WHITE);
}

void UIManager::DrawCrash() {
    DrawText("You Crashed!", 530, 300, 40, RED);
    DrawText("Press [R] to Restart Level", 480, 360, 20, WHITE);
    DrawText("Press [M] to return to Main Menu", 440, 390, 20, WHITE);
}

void UIManager::DrawPause() {
    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.5f));

    DrawText("PAUSED", 540, 200, 40, YELLOW);

    DrawText("Press [ESC] to Resume", 480, 270, 20, WHITE);
    DrawText("Press [R] to Restart Level", 480, 300, 20, WHITE);

    // NEW — return to menu
    DrawText("Press [M] to return to Main Menu", 440, 330, 20, WHITE);

    DrawText("Press [Q] to Quit", 500, 360, 20, WHITE);
}

void UIManager::Close() {
    UnloadFont(unicode);
    UnloadFont(title);
}
