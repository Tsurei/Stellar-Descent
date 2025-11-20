#include "LevelManager.h"
#include "raylib.h"
#include <cmath>

LevelManager::LevelManager()
    : currentDifficultyIndex(1), // Normal
    currentLevelIndex(0)      // First level
{
}

void LevelManager::Init(Rocket& rocket, Planet& planet, std::vector<MovingObstacle>& obstacles)
{
    // -------------------- DIFFICULTY PRESETS --------------------
    difficulties[0] = { "Easy",   60.0f, 150.0f, 140.0f, 2 };
    difficulties[1] = { "Normal", 80.0f, 100.0f, 100.0f, 4 };
    difficulties[2] = { "Hard",  100.0f,  70.0f,  70.0f, 6 };

    // -------------------- LEVEL PRESETS --------------------
    // You can tweak these numbers however you like.
    // y is mostly the spawn height; pad is always at y = 310 in ApplyCurrentPreset.

    // 0: Straightforward central drop
    levels[0] = {
        "Central Valley",
        {   0.0f,  -200.0f },   // start pos
        0.0f                     // pad center X
    };

    // 1: Approach from the far left
    levels[1] = {
        "Left Approach",
        { -260.0f, -220.0f },
        -180.0f
    };

    // 2: Approach from the far right
    levels[2] = {
        "Right Ridge",
        {  260.0f, -220.0f },
        180.0f
    };

    // 3: High drop over central pad (more time to drift)
    levels[3] = {
        "High Descent",
        {   0.0f,  -320.0f },
        0.0f
    };

    // 4: Off-screen left start, pad near center
    levels[4] = {
        "Blind Left",
        { -380.0f, -240.0f },  // start way off-screen on the left
        -60.0f                 // pad slightly left
    };

    // 5: Off-screen right start, pad near center
    levels[5] = {
        "Blind Right",
        {  380.0f, -240.0f },
        60.0f
    };

    // Apply starting difficulty & level to world
    ApplyCurrentPreset(rocket, planet, obstacles);
}

void LevelManager::SetupObstacles(const DifficultyPreset& diff,
    const LevelPreset& level,
    std::vector<MovingObstacle>& obstacles)
{
    obstacles.clear();

    // Base area around landing pad, centered on padCenterX
    float centerX = level.padCenterX;
    float minX = centerX - 220.0f;
    float maxX = centerX + 220.0f;
    float minY = -220.0f;
    float maxY = 260.0f; // above ground (y=310)

    for (int i = 0; i < diff.obstacleCount; ++i) {
        float w = (float)GetRandomValue(40, 80);
        float h = (float)GetRandomValue(8, 18);

        float x = (float)GetRandomValue((int)minX, (int)maxX);
        float y = (float)GetRandomValue((int)minY, (int)maxY);

        Rectangle r{ x, y, w, h };

        int patternRoll = GetRandomValue(0, 2);
        ObstaclePattern pattern =
            (patternRoll == 0) ? ObstaclePattern::STATIC :
            (patternRoll == 1) ? ObstaclePattern::HORIZONTAL :
            ObstaclePattern::VERTICAL;

        float amplitude = (pattern == ObstaclePattern::STATIC)
            ? 0.0f
            : (float)GetRandomValue(20, 80);

        float frequency = (pattern == ObstaclePattern::STATIC)
            ? 0.0f
            : (float)GetRandomValue(1, 3) / 2.0f; // 0.5–1.5

        float phase = (float)GetRandomValue(0, 628) / 100.0f; // 0–6.28
        float angVel = (float)GetRandomValue(-90, 90);         // -90..90 deg/sec

        obstacles.emplace_back(r, pattern, amplitude, frequency, phase, angVel);
    }
}

void LevelManager::ApplyCurrentPreset(Rocket& rocket,
    Planet& planet,
    std::vector<MovingObstacle>& obstacles)
{
    DifficultyPreset& d = difficulties[currentDifficultyIndex];
    LevelPreset& l = levels[currentLevelIndex];

    // Planet settings
    planet.gravity = d.gravity;
    planet.landingPad.y = 310.0f;
    planet.landingPad.height = 10.0f;
    planet.landingPad.width = d.padWidth;
    planet.landingPad.x = l.padCenterX - d.padWidth / 2.0f;

    // Rocket settings
    rocket.SetDifficultyParams(d.gravity, d.startingFuel);
    rocket.Reset(l.startPos);

    // Obstacles
    SetupObstacles(d, l, obstacles);
}

void LevelManager::HandleMenu(GameState& state,
    Rocket& rocket,
    Planet& planet,
    std::vector<MovingObstacle>& obstacles,
    float& timer,
    bool& startGame)
{
    bool difficultyChanged = false;
    bool levelChanged = false;

    // Change difficulty with 1/2/3
    if (IsKeyPressed(KEY_ONE)) { currentDifficultyIndex = 0; difficultyChanged = true; }
    if (IsKeyPressed(KEY_TWO)) { currentDifficultyIndex = 1; difficultyChanged = true; }
    if (IsKeyPressed(KEY_THREE)) { currentDifficultyIndex = 2; difficultyChanged = true; }

    // Change level with LEFT/RIGHT
    if (IsKeyPressed(KEY_LEFT)) {
        currentLevelIndex--;
        if (currentLevelIndex < 0) currentLevelIndex = LEVEL_COUNT - 1;
        levelChanged = true;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        currentLevelIndex++;
        if (currentLevelIndex >= LEVEL_COUNT) currentLevelIndex = 0;
        levelChanged = true;
    }

    if (difficultyChanged || levelChanged) {
        ApplyCurrentPreset(rocket, planet, obstacles);
    }

    // ENTER starts the level
    if (IsKeyPressed(KEY_ENTER)) {
        timer = 0.0f;
        startGame = false;
        state = GameState::PLAYING;
    }
}

void LevelManager::RestartCurrentLevel(Rocket& rocket,
    Planet& planet,
    std::vector<MovingObstacle>& obstacles,
    float& timer,
    bool& startGame)
{
    ApplyCurrentPreset(rocket, planet, obstacles);
    timer = 0.0f;
    startGame = false;
}

void LevelManager::AdvanceToNextLevel(Rocket& rocket,
    Planet& planet,
    std::vector<MovingObstacle>& obstacles,
    float& timer,
    bool& startGame)
{
    currentLevelIndex++;
    if (currentLevelIndex >= LEVEL_COUNT) currentLevelIndex = 0;

    ApplyCurrentPreset(rocket, planet, obstacles);
    timer = 0.0f;
    startGame = false;
}

const char* LevelManager::GetDifficultyName() const
{
    return difficulties[currentDifficultyIndex].name;
}

const char* LevelManager::GetLevelName() const
{
    return levels[currentLevelIndex].name;
}
