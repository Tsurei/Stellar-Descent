#include "raylib.h"
#include "Rocket.h"
#include "UIManager.h"
#include "AudioSystem.h"
#include "CameraController.h"
#include "PhysicsSystem.h"
#include "GameStateManager.h"
#include "Planet.h"
#include "MovingObstacle.h"

#include <vector>
#include <cmath>

/**
 * @brief Represents a difficulty preset for Stellar Descent.
 *
 * Each preset controls gravity strength, starting fuel, obstacle density and landing pad size.
 * These values are applied to both the rocket and planet/world.
 */
struct DifficultyPreset {
    const char* name;
    float gravity;
    float startingFuel;
    float padWidth;
    int   obstacleCount;
};

/// Generate a set of moving obstacles for a given difficulty.
static void SetupObstaclesForDifficulty(const DifficultyPreset& diff,
    std::vector<MovingObstacle>& obstacles)
{
    obstacles.clear();

    // Base area around landing zone
    float minX = -220.0f;
    float maxX = 220.0f;
    float minY = -200.0f;
    float maxY = 260.0f; // above ground (y=310)

    for (int i = 0; i < diff.obstacleCount; ++i) {
        // Random size
        float w = (float)GetRandomValue(40, 80);
        float h = (float)GetRandomValue(8, 18);

        // Random position in the play area
        float x = (float)GetRandomValue((int)minX, (int)maxX);
        float y = (float)GetRandomValue((int)minY, (int)maxY);

        Rectangle r{ x, y, w, h };

        // Random pattern
        int patternRoll = GetRandomValue(0, 2);
        ObstaclePattern pattern =
            (patternRoll == 0) ? ObstaclePattern::STATIC :
            (patternRoll == 1) ? ObstaclePattern::HORIZONTAL :
            ObstaclePattern::VERTICAL;

        // Random movement params
        float amplitude = (pattern == ObstaclePattern::STATIC)
            ? 0.0f
            : (float)GetRandomValue(20, 80);

        float frequency = (pattern == ObstaclePattern::STATIC)
            ? 0.0f
            : (float)GetRandomValue(1, 3) / 2.0f; // 0.5–1.5

        float phase = (float)GetRandomValue(0, 628) / 100.0f; // 0–6.28
        float angVel = (float)GetRandomValue(-90, 90);        // -90..90 deg/sec

        obstacles.emplace_back(r, pattern, amplitude, frequency, phase, angVel);
    }
}

int main() {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    // -------------------- INITIALIZATION --------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Stellar Descent");
    SetExitKey(0); // Disable default ESC exit
    SetTargetFPS(60);

    // -------------------- DIFFICULTY PRESETS --------------------
    DifficultyPreset difficulties[] = {
        { "Easy",   60.0f, 150.0f, 140.0f, 2 },
        { "Normal", 80.0f, 100.0f, 100.0f, 4 },
        { "Hard",  100.0f,  70.0f,  70.0f, 6 },
    };
    int currentDifficultyIndex = 1; // Start at Normal

    Rocket rocket({ 0, -200 });

    DifficultyPreset& diff = difficulties[currentDifficultyIndex];
    Planet planet = {
        diff.gravity,
        Rectangle{ -diff.padWidth / 2.0f, 310, diff.padWidth, 10 }
    };

    // Apply initial difficulty to rocket physics and fuel
    rocket.SetDifficultyParams(diff.gravity, diff.startingFuel);

    UIManager ui;
    AudioSystem audio;
    CameraController cam;
    PhysicsSystem physics;

    ui.Init();
    audio.Init();
    cam.Init(rocket.position);
    cam.camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    Texture2D starfield = LoadTexture("assets/textures/starfield.png");
    float parallaxFactor = 0.3f;

    GameState state = GameState::MENU;
    float timer = 0.0f;
    float score = 0;
    bool startGame = false;

    // -------------------- MOVING OBSTACLES --------------------
    std::vector<MovingObstacle> obstacles;
    SetupObstaclesForDifficulty(diff, obstacles);

    // -------------------- GAME LOOP --------------------
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        audio.Update();

        // ----------------- AUDIO CONTROLS -----------------
        if (IsKeyPressed(KEY_M)) {
            audio.ToggleMute();
        }

        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
            audio.SetVolume(audio.GetVolume() + 0.1f);
        }
        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
            audio.SetVolume(audio.GetVolume() - 0.1f);
        }

        // ----------------- STATE LOGIC -----------------
        switch (state) {
        case GameState::MENU:
        {
            bool difficultyChanged = false;

            // Change difficulty with number keys
            if (IsKeyPressed(KEY_ONE)) { currentDifficultyIndex = 0; difficultyChanged = true; }
            if (IsKeyPressed(KEY_TWO)) { currentDifficultyIndex = 1; difficultyChanged = true; }
            if (IsKeyPressed(KEY_THREE)) { currentDifficultyIndex = 2; difficultyChanged = true; }

            if (difficultyChanged) {
                DifficultyPreset& d = difficulties[currentDifficultyIndex];

                // Apply difficulty to planet
                planet.gravity = d.gravity;
                planet.landingPad.width = d.padWidth;
                planet.landingPad.x = -d.padWidth / 2.0f; // keep centered

                // Apply to rocket (gravity/fuel)
                rocket.SetDifficultyParams(d.gravity, d.startingFuel);

                // Regenerate obstacles for this difficulty
                SetupObstaclesForDifficulty(d, obstacles);
            }

            if (IsKeyPressed(KEY_ENTER)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
                startGame = false;
            }
            if (IsKeyPressed(KEY_Q)) CloseWindow(); // Quit from menu
            break;
        }

        case GameState::PLAYING:
        {
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::PAUSED;
            if (IsKeyPressed(KEY_UP)) startGame = true;
            if (startGame) {
                timer += dt;
            }

            rocket.Update(dt);
            cam.Update(rocket.position, dt);

            if (IsKeyDown(KEY_UP)) audio.PlayThrust(true);

            // -------------------- UPDATE OBSTACLES --------------------
            for (auto& o : obstacles) {
                o.Update(dt);
            }

            // -------------------- COLLISION LOGIC --------------------
            {
                Rectangle groundRect = { -1000, 310, 2000, 400 };
                Rectangle rocketRect = { rocket.position.x - 5, rocket.position.y - 15, 10, 30 };

                bool onPad = CheckCollisionRecs(rocketRect, planet.landingPad);
                bool hitsGround = CheckCollisionRecs(rocketRect, groundRect);

                // -------------------- VIEW RECTANGLE --------------------
                Rectangle viewRect;
                viewRect.width = SCREEN_WIDTH / cam.camera.zoom;
                viewRect.height = SCREEN_HEIGHT / cam.camera.zoom;
                viewRect.x = cam.camera.target.x - viewRect.width / 2.0f;
                viewRect.y = cam.camera.target.y - viewRect.height / 2.0f;

                const float margin = 40.0f;
                viewRect.x -= margin;
                viewRect.y -= margin;
                viewRect.width += margin * 2.0f;
                viewRect.height += margin * 2.0f;

                // -------------------- OBSTACLE COLLISION --------------------
                bool hitsObstacle = false;

                // Rocket as an oriented box (10x30, centered at rocket.position)
                Vector2 rocketCenter = rocket.position;
                Vector2 rocketHalfExtents = { 5.0f, 15.0f };

                for (const auto& o : obstacles) {
                    if (!o.IsNear(viewRect)) continue;

                    if (o.CheckCollisionOBB(rocketCenter, rocketHalfExtents, rocket.rotation)) {
                        hitsObstacle = true;
                        break;
                    }
                }

                if (hitsObstacle) {
                    state = GameState::CRASH;
                    audio.PlayCrash();
                    rocket.velocity = { 0, 0 };
                }
                else if (hitsGround) {
                    if (onPad) {
                        float padCenterX = planet.landingPad.x + planet.landingPad.width / 2.0f;
                        float rocketCenterX = rocket.position.x;
                        float horizontalTolerance = planet.landingPad.width / 2.0f - 5;

                        bool withinPadHoriz = std::fabs(rocketCenterX - padCenterX) <= horizontalTolerance;
                        float maxVerticalSpeed = 50.0f;
                        float maxRotationDeg = 30.0f;

                        if (withinPadHoriz &&
                            physics.CheckLanding(rocket, planet.landingPad, maxVerticalSpeed, maxRotationDeg)) {

                            rocket.hasLanded = true;
                            state = GameState::WIN;
                            audio.PlayLand();

                            float accuracy = 1.0f - (std::fabs(rocketCenterX - padCenterX) / (planet.landingPad.width / 2.0f));
                            float timeFactor = 1.0f / (1.0f + timer);
                            float fuelFactor = rocket.fuel / 100.0f;

                            score = (accuracy * 0.5f + timeFactor * 0.3f + fuelFactor * 0.2f) * 1000.0f;
                        }
                        else {
                            state = GameState::CRASH;
                            audio.PlayCrash();
                        }
                    }
                    else {
                        state = GameState::CRASH;
                        audio.PlayCrash();
                    }
                    rocket.position.y = 310 - 15;
                    rocket.velocity = { 0, 0 };
                }
            }
            break;
        }

        case GameState::PAUSED:
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::PLAYING;
            if (IsKeyPressed(KEY_R)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
            }
            if (IsKeyPressed(KEY_Q)) CloseWindow();
            break;

        case GameState::WIN:
        case GameState::CRASH:
            if (IsKeyPressed(KEY_R)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
                startGame = false;
            }
            if (IsKeyPressed(KEY_Q)) break;
            break;
        }

        // ----------------- DRAW -----------------
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(cam.camera);

        Vector2 parallaxOffset = { cam.camera.target.x * parallaxFactor, cam.camera.target.y * parallaxFactor };
        int tilesX = SCREEN_WIDTH / starfield.width + 3;
        int tilesY = SCREEN_HEIGHT / starfield.height + 3;

        for (int x = -1; x < tilesX - 1; x++) {
            for (int y = -1; y < tilesY - 1; y++) {
                DrawTexture(starfield,
                    (int)(x * starfield.width - fmod(parallaxOffset.x, starfield.width)),
                    (int)(y * starfield.height - fmod(parallaxOffset.y, starfield.height)),
                    WHITE);
            }
        }

        DrawRectangle(-1000, 310, 2000, 400, DARKGRAY);
        DrawRectangleRec(planet.landingPad, GREEN);

        // Draw obstacles
        for (const auto& o : obstacles) {
            o.Draw();
        }

        rocket.Draw();

        EndMode2D();

        // Draw UI
        switch (state) {
        case GameState::MENU:
            ui.DrawMenu(difficulties[currentDifficultyIndex].name);
            break;
        case GameState::PLAYING:
            ui.DrawHUD(rocket.fuel, 300 - rocket.position.y, timer);
            break;
        case GameState::PAUSED:
            ui.DrawPause();
            break;
        case GameState::WIN:
            ui.DrawWin(score);
            break;
        case GameState::CRASH:
            ui.DrawCrash();
            break;
        }

        // --- Volume HUD (bottom-left) ---
        int volPercent = (int)(audio.GetVolume() * 100.0f);
        DrawText(
            TextFormat("Volume: %s%d%%  [M] Mute  [-/+]",
                audio.IsMuted() ? "X " : "",
                volPercent),
            20, SCREEN_HEIGHT - 40, 20, RAYWHITE
        );

        EndDrawing();
    }

    // -------------------- CLEANUP --------------------
    UnloadTexture(starfield);
    audio.Close();
    ui.Close();
    CloseWindow();
    return 0;
}
