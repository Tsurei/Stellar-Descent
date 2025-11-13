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

    /**
    * @brief Sets the master audio volume.
    *
    * @param volume Value between 0.0f (silent) and 1.0f (full volume).
    *
    * This adjusts the global game volume. It clamps incoming
    * values to a safe range and updates Raylib's internal volume.
    * If mute is currently enabled, the new volume is stored but NOT applied
    * until the player un-mutes.
    */
    void SetVolume(float volume);

    /**
     * @brief Toggles the global mute state.
     *
     * When muted, the game volume is set to 0.0f but the original
     * volume level is preserved internally so unmuting restores it.
     */
    void ToggleMute();

    // Helpers for UI
    float GetVolume() const { return masterVolume; }
    bool IsMuted() const { return isMuted; }

private:
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

    // Master volume and mute state for all audio output.
    // Values are normalized between 0.0 (silent) and 1.0 (full volume).
    float masterVolume = 1.0f; 
    bool isMuted = false;
};
