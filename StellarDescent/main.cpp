#include "raylib.h"
#include "Rocket.h"
#include "UIManager.h"
#include "AudioSystem.h"
#include "CameraController.h"
#include "PhysicsSystem.h"
#include "GameStateManager.h"
#include "Planet.h"
#include <cmath>

int main() {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    // -------------------- INITIALIZATION --------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Stellar Descent");
    SetExitKey(0); // Disable default ESC exit
    SetTargetFPS(60);

    Rocket rocket({ 0, -200 });
    Planet planet = { 80.0f, Rectangle{ -50, 310, 100, 10 } };
    UIManager ui;
    AudioSystem audio;
    CameraController cam;
    PhysicsSystem physics;

    audio.Init();
    cam.Init(rocket.position);
    cam.camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    Texture2D starfield = LoadTexture("assets/textures/starfield.png");
    float parallaxFactor = 0.3f;

    GameState state = GameState::MENU;
    float timer = 0.0f;

    // -------------------- GAME LOOP --------------------
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        audio.Update();

        // ----------------- STATE LOGIC -----------------
        switch (state) {
        case GameState::MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
            }
            if (IsKeyPressed(KEY_Q)) CloseWindow(); // Quit from menu
            break;

        case GameState::PLAYING:
            // Pause
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::PAUSED;

            timer += dt;
            rocket.Update(dt);
            if (IsKeyDown(KEY_UP)) audio.PlayThrust(true);

            // Collision logic
            {
                Rectangle groundRect = { -1000, 310, 2000, 400 };
                Rectangle rocketRect = { rocket.position.x - 5, rocket.position.y - 12, 10, 24 };

                bool onPad = CheckCollisionRecs(rocketRect, planet.landingPad);
                bool hitsGround = CheckCollisionRecs(rocketRect, groundRect);

                if (hitsGround) {
                    if (onPad) {
                        float padCenterX = planet.landingPad.x + planet.landingPad.width / 2.0f;
                        float rocketCenterX = rocket.position.x;
                        float horizontalTolerance = planet.landingPad.width / 2.0f - 5;

                        bool withinPadHoriz = fabs(rocketCenterX - padCenterX) <= horizontalTolerance;
                        float maxVerticalSpeed = 50.0f;
                        float maxRotationDeg = 30.0f;

                        if (withinPadHoriz && physics.CheckLanding(rocket, planet.landingPad, maxVerticalSpeed, maxRotationDeg)) {
                            rocket.hasLanded = true;
                            state = GameState::WIN;
                            audio.PlayLand();
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
                    rocket.position.y = 310 - 12;
                    rocket.velocity = { 0, 0 };
                }
            }

            cam.Update(rocket.position, dt);
            break;

        case GameState::PAUSED:
            // Resume
            if (IsKeyPressed(KEY_ESCAPE)) state = GameState::PLAYING;
            // Restart
            if (IsKeyPressed(KEY_R)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
            }
            // Quit
            if (IsKeyPressed(KEY_Q)) CloseWindow();
            break;

        case GameState::WIN:
        case GameState::CRASH:
            if (IsKeyPressed(KEY_R)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
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
        rocket.Draw();

        EndMode2D();

        // Draw UI
        switch (state) {
        case GameState::MENU:
            ui.DrawMenu();
            DrawText("Press [Q] to Quit", 500, 380, 20, WHITE);
            break;
        case GameState::PLAYING:
            ui.DrawHUD(rocket.fuel, 300 - rocket.position.y, timer);
            break;
        case GameState::PAUSED:
            ui.DrawPause(); // Implement this in UIManager
            break;
        case GameState::WIN:
            ui.DrawWin();
            break;
        case GameState::CRASH:
            ui.DrawCrash();
            break;
        }

        EndDrawing();
    }

    // -------------------- CLEANUP --------------------
    UnloadTexture(starfield);
    audio.Close();
    CloseWindow();
    return 0;
}
