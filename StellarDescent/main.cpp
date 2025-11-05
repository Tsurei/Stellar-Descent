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

    // Load starfield texture
    Texture2D starfield = LoadTexture("assets/textures/starfield.png");
    float parallaxFactor = 0.3f; // slower than foreground

    GameState state = GameState::MENU;
    float timer = 0.0f;

    // -------------------- GAME LOOP --------------------
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        audio.Update();
        
        if (state == GameState::PLAYING) {
            // Pause toggle
            if (IsKeyPressed(KEY_ESCAPE)) {
                state = GameState::PAUSED;
            }

            timer += dt;
            rocket.Update(dt);
            if (IsKeyDown(KEY_UP)) audio.PlayThrust(IsKeyDown(KEY_UP));

            // Collision logic here...
            cam.Update(rocket.position);
        }
        else if (state == GameState::PAUSED) {
            // Resume with ESC
            if (IsKeyPressed(KEY_ESCAPE)) {
                state = GameState::PLAYING;
            }
            // Restart from pause
            if (IsKeyPressed(KEY_R)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
            }
        }

        // ----------------- STATE LOGIC -----------------
        if (state == GameState::MENU) {
            if (IsKeyPressed(KEY_ENTER)) {
                rocket.Reset({ 0, -200 });
                state = GameState::PLAYING;
                timer = 0;
            }
        }
        else if (state == GameState::PLAYING) {
            timer += dt;
            rocket.Update(dt);

            if (IsKeyDown(KEY_UP)) audio.PlayThrust(IsKeyDown(KEY_UP));

            // ----------------- COLLISION LOGIC -----------------
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

            cam.Update(rocket.position);
        }
        else if ((state == GameState::WIN || state == GameState::CRASH) && IsKeyPressed(KEY_R)) {
            rocket.Reset({ 0, -200 });
            state = GameState::PLAYING;
            timer = 0;
        }

        // ----------------- DRAW -----------------
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(cam.camera);

        // ----------------- DRAW PARALLAX STARFIELD -----------------
        Vector2 parallaxOffset = { cam.camera.target.x * parallaxFactor, cam.camera.target.y * parallaxFactor };

        // Calculate number of tiles needed to cover screen in each direction
        int tilesX = SCREEN_WIDTH / starfield.width + 3;  // +3 ensures full coverage even after parallax
        int tilesY = SCREEN_HEIGHT / starfield.height + 3;

        for (int x = -1; x < tilesX - 1; x++) {
            for (int y = -1; y < tilesY - 1; y++) {
                DrawTexture(
                    starfield,
                    (int)(x * starfield.width - fmod(parallaxOffset.x, starfield.width)),
                    (int)(y * starfield.height - fmod(parallaxOffset.y, starfield.height)),
                    WHITE
                );
            }
        }


        DrawRectangle(-1000, 310, 2000, 400, DARKGRAY);
        DrawRectangleRec(planet.landingPad, GREEN);
        rocket.Draw();

        EndMode2D();

        if (state == GameState::PLAYING)
            ui.DrawHUD(rocket.fuel, 300 - rocket.position.y, timer);
        else if (state == GameState::MENU)
            ui.DrawMenu();
        else if (state == GameState::PAUSED)
            ui.DrawPause();
        else if (state == GameState::WIN)
            ui.DrawWin();
        else if (state == GameState::CRASH)
            ui.DrawCrash();


        EndDrawing();
    }

    // -------------------- CLEANUP --------------------
    UnloadTexture(starfield);
    audio.Close();
    CloseWindow();
    return 0;
}
