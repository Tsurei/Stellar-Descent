#include "AudioSystem.h"
#include "raylib.h"

void AudioSystem::Init() {
    // Initialize audio hardware
    InitAudioDevice();
    SetMasterVolume(1.0f);

    // Load background music (looped by default)
    bgMusic = LoadMusicStream("assets/audio/music.wav");

    // Load sound effects
    sfxThrust = LoadMusicStream("assets/audio/thrust.wav");
    sfxCrash = LoadSound("assets/audio/crash.wav");
    sfxLand = LoadSound("assets/audio/land.wav");

    // Start background music
    PlayMusicStream(bgMusic);

    // Ensure thrust sound is not playing initially
    thrustPlaying = false;
}

void AudioSystem::Update() {
    UpdateMusicStream(bgMusic);
}

void AudioSystem::PlayThrust(bool isKeyDown) {
    if (isKeyDown) {
        if (!thrustPlaying) {
            PlayMusicStream(sfxThrust);
            thrustPlaying = true;
        }
    }
    else {
        if (thrustPlaying) {
            StopMusicStream(sfxThrust);
            thrustPlaying = false;
        }
    }
    UpdateMusicStream(sfxThrust); // call every frame while playing
}


void AudioSystem::PlayCrash() {
    PlaySound(sfxCrash);
}

/**
 * @brief Play landing sound effect once.
 */
void AudioSystem::PlayLand() {
    PlaySound(sfxLand);
}

void AudioSystem::Close() {
    StopMusicStream(bgMusic);   // Stop background music
    UnloadMusicStream(bgMusic); // Free music memory
    UnloadMusicStream(sfxThrust);// Free thrust SFX
    UnloadSound(sfxCrash);      // Free crash SFX
    UnloadSound(sfxLand);       // Free landing SFX
    CloseAudioDevice();          // Close audio hardware
}