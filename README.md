# Control Resonant macOS Port

A high-performance native macOS host for Control Resonant, optimized for Apple Silicon.

## 🚀 Architecture
This project implements a "Zero-Copy" architecture leveraging Apple's Unified Memory Architecture (UMA).

- **Logic Engine**: V8 JavaScript Engine (bridged to C++).
- **Rendering**: Metal API with `MTLResourceStorageModeShared` for direct CPU $\rightarrow$ GPU communication.
- **Audio**: AVFoundation (`AVAudioEngine`) for low-latency sound.
- **Sync**: `CVDisplayLink` for VSync-aligned frame pacing.
- **Asset Pipeline**: Custom binary parser to extract assets from original game containers.

## 🛠️ Build Requirements
- macOS (Apple Silicon)
- Xcode (latest)
- V8 Engine (`brew install v8`)

## 📦 Compilation
Run the following command from the project root:

```bash
clang++ -std=c++17 -O3 -arch arm64 \
    -I./include \
    -I/opt/homebrew/include \
    src/nl_bridge.cpp src/binary_parser.cpp src/main.cpp \
    -L/opt/homebrew/lib -lv8 \
    -framework Metal -framework Foundation -framework AppKit -framework CoreVideo -framework AVFoundation \
    -o ControlResonant_Mac
```

## 🕹️ Running
```bash
./ControlResonant_Mac
```
