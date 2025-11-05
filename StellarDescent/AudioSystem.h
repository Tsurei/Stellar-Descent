#pragma once
#include "raylib.h"

/**
 * @brief Handles all audio functionality for the Stellar Descent game.
 *
 * This includes background music, thrust sound effects, crash sounds, and landing sounds.
 * AudioSystem abstracts Raylib's audio API to provide simple function calls for playing and updating audio.
 */
class AudioSystem {
public:
    /// Background music stream
    Music bgMusic;

    /// Sound effect for rocket thrust
    Music sfxThrust;

    /// Sound effect for crashing
    Sound sfxCrash;

    /// Sound effect for successful landing
    Sound sfxLand;

    /// Flag to track whether the thrust sound is currently playing
    bool thrustPlaying = false;

    /**
     * @brief Initialize the audio system.
     *
     * Opens the audio device and loads all music and sound assets.
     * This should be called once at the start of the game.
     */
    void Init();

    /**
     * @brief Update the audio system.
     *
     * Updates background music streaming.
     * Must be called every frame to ensure music plays correctly.
     */
    void Update();

    /**
     * @brief Play or loop the rocket thrust sound effect.
     *
     * If the key is held down, the thrust sound will play continuously without restarting.
     * When the key is released, the sound stops.
     *
     * @param isKeyDown True if the thrust key (UP) is currently pressed; false otherwise.
     */
    void PlayThrust(bool isKeyDown);

    /**
     * @brief Play the crash sound effect.
     */
    void PlayCrash();

    /**
     * @brief Play the landing sound effect.
     */
    void PlayLand();

    /**
     * @brief Close the audio system and free resources.
     *
     * Stops music, unloads music and sound assets, and closes the audio device.
     * Should be called once when the game exits.
     */
    void Close();
};
