#include "nl_api_blueprint.h"
#include <iostream>
#include <thread>
#include <chrono>

// Forward declarations for Host init
namespace NLBridge {
    bool InitializeV8();
    void ShutdownV8();
    bool InitializeMetal();
    void ShutdownMetal();
    bool InitializeAudio();
    void ShutdownAudio();
    bool InitializeHostWindow();
    void ShutdownHost();
}

void RenderLoop() {
    std::cout << "[Host] Entering synchronized Render Loop..." << std::endl;
    for (int frame = 0; frame < 100; ++frame) {
        // The logic is now driven by CVDisplayLink in the background
        std::cout << "[Frame " << frame << "] VSync Sync: Updating UMA buffers..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[Host] Render Loop terminated." << std::endl;
}

int main() {
    NLBridge::nl_info("Initializing Control Resonant Mac Port Host...");

    if (!NLBridge::InitializeV8()) {
        std::cerr << "Critical Failure: V8 Init failed." << std::endl;
        return -1;
    }

    if (!NLBridge::InitializeMetal()) {
        std::cerr << "Critical Failure: Metal Init failed." << std::endl;
        NLBridge::ShutdownV8();
        return -1;
    }

    if (!NLBridge::InitializeAudio()) {
        std::cerr << "Critical Failure: Audio Init failed." << std::endl;
        NLBridge::ShutdownMetal();
        NLBridge::ShutdownV8();
        return -1;
    }

    if (!NLBridge::InitializeHostWindow()) {
        std::cerr << "Critical Failure: Host Window Init failed." << std::endl;
        NLBridge::ShutdownAudio();
        NLBridge::ShutdownMetal();
        NLBridge::ShutdownV8();
        return -1;
    }

    // Initial boot sequence
    auto script = NLBridge::nl_asset_load("scripts/main.lua", NLBridge::AssetType::ScriptJS);
    NLBridge::nl_v8_load_script(script, "GameCore");

    // Test Audio Trigger
    auto sfx = NLBridge::nl_asset_load("audio/startup.lua", NLBridge::AssetType::Audio);
    NLBridge::nl_audio_play(sfx, 0.8f, false);

    RenderLoop();

    NLBridge::ShutdownMetal();
    NLBridge::ShutdownAudio();
    NLBridge::ShutdownHost();
    NLBridge::ShutdownV8();

    NLBridge::nl_info("Host shutdown cleanly.");
    return 0;
}
