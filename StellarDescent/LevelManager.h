#pragma once
#include "raylib.h"
#include <vector>

#include "MovingObstacle.h"
#include "Rocket.h"
#include "Planet.h"
#include "GameStateManager.h"

/**
 * @brief Manages difficulty presets, level layouts, and obstacle generation.
 *
 * This keeps main.cpp cleaner by handling:
 *  - Difficulty & level selection
 *  - Applying presets to rocket / planet
 *  - Spawning obstacles for the current level
 */
class LevelManager {
public:
    LevelManager();

    // Initialize presets and apply the starting difficulty+level.
    void Init(Rocket& rocket, Planet& planet, std::vector<MovingObstacle>& obstacles);

    // Handle key input on the MENU screen:
    //  - 1/2/3 change difficulty
    //  - LEFT/RIGHT change level
    //  - ENTER starts the level (sets state to PLAYING)
    void HandleMenu(GameState& state,
        Rocket& rocket,
        Planet& planet,
        std::vector<MovingObstacle>& obstacles,
        float& timer,
        bool& startGame);

    // Restart the current level (used from PAUSED, WIN, CRASH when pressing R)
    void RestartCurrentLevel(Rocket& rocket,
        Planet& planet,
        std::vector<MovingObstacle>& obstacles,
        float& timer,
        bool& startGame);

    // Go to the next level (used from WIN when pressing ENTER)
    void AdvanceToNextLevel(Rocket& rocket,
        Planet& planet,
        std::vector<MovingObstacle>& obstacles,
        float& timer,
        bool& startGame);

    // For UI
    const char* GetDifficultyName() const;
    const char* GetLevelName() const;

private:
    struct DifficultyPreset {
        const char* name;
        float gravity;
        float startingFuel;
        float padWidth;
        int   obstacleCount;
    };

    struct LevelPreset {
        const char* name;
        Vector2 startPos;   // where the rocket spawns
        float padCenterX;   // X position of landing pad center
    };

    static constexpr int DIFFICULTY_COUNT = 3;
    static constexpr int LEVEL_COUNT = 6;   // <-- now 6 levels

    DifficultyPreset difficulties[DIFFICULTY_COUNT];
    LevelPreset      levels[LEVEL_COUNT];

    int currentDifficultyIndex;
    int currentLevelIndex;

    void ApplyCurrentPreset(Rocket& rocket,
        Planet& planet,
        std::vector<MovingObstacle>& obstacles);

    void SetupObstacles(const DifficultyPreset& diff,
        const LevelPreset& level,
        std::vector<MovingObstacle>& obstacles);
};
