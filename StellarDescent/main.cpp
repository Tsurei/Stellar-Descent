#include "raylib.h"
#include "Rocket.h"
#include "UIManager.h"
#include "AudioSystem.h"
#include "CameraController.h"
#include "PhysicsSystem.h"
#include "GameStateManager.h"
#include "Planet.h"
#include "MovingObstacle.h"
#include "LevelManager.h"

#include <vector>
#include <cmath>

int main() {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    // -------------------- INITIALIZATION --------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Stellar Descent");
    SetExitKey(0); // Disable default ESC exit
    SetTargetFPS(60);

    // Rocket + planet (planet will be overridden by LevelManager)
    Rocket rocket({ 0, -200 });
    Planet planet = { 0.0f, Rectangle{ 0, 310, 100, 10 } };

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
    float score = 0.0f;
    bool  startGame = false;

    // -------------------- LEVELS / OBSTACLES --------------------
    std::vector<MovingObstacle> obstacles;
    LevelManager levelManager;
    levelManager.Init(rocket, planet, obstacles); // applies starting difficulty + level

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

            // ----- MENU -----
        case GameState::MENU:
        {
            levelManager.HandleMenu(state, rocket, planet, obstacles, timer, startGame);

            if (IsKeyPressed(KEY_Q)) {
                CloseWindow();
            }
            break;
        }

        // ----- PLAYING -----
        case GameState::PLAYING:
        {
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::PAUSED;
            if (IsKeyPressed(KEY_UP))     startGame = true;
            if (startGame) {
                timer += dt;
            }

            rocket.Update(dt);
            cam.Update(rocket.position, dt);

            if (IsKeyDown(KEY_UP)) audio.PlayThrust(true);

            // Update obstacles
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

                        bool  withinPadHoriz = std::fabs(rocketCenterX - padCenterX) <= horizontalTolerance;
                        float maxVerticalSpeed = 50.0f;
                        float maxRotationDeg = 30.0f;

                        if (withinPadHoriz &&
                            physics.CheckLanding(rocket, planet.landingPad, maxVerticalSpeed, maxRotationDeg)) {

                            rocket.hasLanded = true;
                            state = GameState::WIN;
                            audio.PlayLand();

                            float accuracy = 1.0f - (std::fabs(rocketCenterX - padCenterX) /
                                (planet.landingPad.width / 2.0f));
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

        // ----- PAUSED -----
        case GameState::PAUSED:
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::PLAYING;

            if (IsKeyPressed(KEY_R)) {
                levelManager.RestartCurrentLevel(rocket, planet, obstacles, timer, startGame);
                state = GameState::PLAYING;
            }
            if (IsKeyPressed(KEY_M)) {
                state = GameState::MENU;
            }

            if (IsKeyPressed(KEY_Q)) CloseWindow();
            break;

            // ----- WIN -----
        case GameState::WIN:
            if (IsKeyPressed(KEY_ENTER)) {
                // Next level
                levelManager.AdvanceToNextLevel(rocket, planet, obstacles, timer, startGame);
                state = GameState::PLAYING;
            }
            if (IsKeyPressed(KEY_R)) {
                // Restart current level
                levelManager.RestartCurrentLevel(rocket, planet, obstacles, timer, startGame);
                state = GameState::PLAYING;
            }
            if (IsKeyPressed(KEY_M)) {
                state = GameState::MENU;
            }
            if (IsKeyPressed(KEY_Q)) break;
            break;

            // ----- CRASH -----
        case GameState::CRASH:
            if (IsKeyPressed(KEY_R)) {
                levelManager.RestartCurrentLevel(rocket, planet, obstacles, timer, startGame);
                state = GameState::PLAYING;
            }
            if (IsKeyPressed(KEY_M)) {
                state = GameState::MENU;
            }
            if (IsKeyPressed(KEY_Q)) break;
            break;
        }

        // ----------------- DRAW -----------------
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(cam.camera);

        // Parallax background
        Vector2 parallaxOffset = { cam.camera.target.x * parallaxFactor,
                                   cam.camera.target.y * parallaxFactor };
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

        // Ground + landing pad
        DrawRectangle(-1000, 310, 2000, 400, DARKGRAY);
        DrawRectangleRec(planet.landingPad, GREEN);

        // Obstacles
        for (const auto& o : obstacles) {
            o.Draw();
        }

        // Rocket
        rocket.Draw();

        EndMode2D();

        // ----------------- UI -----------------
        switch (state) {
        case GameState::MENU:
            // Right now DrawMenu only knows about difficulty;
            // you could extend it later to also show levelManager.GetLevelName().
            ui.DrawMenu(levelManager.GetDifficultyName(),
                levelManager.GetLevelName());
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
