# 🌌 Control Resonant macOS Port

![Version](https://img.shields.io/badge/version-0.1.0--alpha-blue)
![Platform](https://img.shields.io/badge/platform-macOS--Apple--Silicon-black)
![Language](https://img.shields.io/badge/language-C++17-orange)
![Engine](https://img.shields.io/badge/Engine-V8--Metal-purple)

A high-performance, native macOS host designed to run **Control Resonant**. This project is not a simple wrapper, but a ground-up re-implementation of the game's host environment, specifically engineered to exploit the hardware capabilities of Apple Silicon (M1/M2/M3).

---

## 🛠️ Technical Architecture

The core philosophy of this port is **Zero-Copy Efficiency**. By leveraging the Apple Silicon Unified Memory Architecture (UMA), we eliminate the traditional bottleneck of copying data between the CPU and GPU.

### 🧠 The Logic Brain (V8 Engine)
The game's logic is powered by the **V8 JavaScript Engine**. 
- **Persistent Context**: Unlike standard implementations, this host maintains a global persistent context, allowing game scripts to maintain state across frames.
- **Native Bridge**: A custom C++ binding layer allows JS scripts to call native macOS functions (Rendering, Audio, HID) with near-zero overhead.

### 🎨 The Visuals (Metal UMA)
The rendering pipeline is built on **Metal**, Apple's low-level graphics API.
- **UMA Integration**: Uses `MTLResourceStorageModeShared`, allowing the V8 engine to write vertex/transform data directly into a buffer that the GPU reads instantly.
- **VSync Synchronization**: Implements `CVDisplayLink` to synchronize the render loop with the display's refresh rate, ensuring a stutter-free experience.

### 🔊 The Audio (AVFoundation)
A low-latency audio pipeline built on **AVAudioEngine**.
- **Dynamic Node Mapping**: Assets are extracted from binary containers and streamed through a network of `AVAudioPlayerNode`s for real-time volume and loop control.

### 📦 The Asset Pipeline (Binary Parser)
The game uses a proprietary binary container format (disguised as `.lua` files).
- **Custom Extraction**: The `BinaryParser` handles the decryption and extraction of JavaScript bytecode, 3D meshes, and texture data.
- **VFS Mapping**: A virtual file system maps original game paths to the macOS directory structure.

---

## 🚀 Build & Installation

### Prerequisites
- **Hardware**: Mac with Apple Silicon (M1, M2, M3, etc.)
- **OS**: macOS 12.0+
- **Tools**: 
  - Xcode (latest)
  - Homebrew
  - V8 Engine: `brew install v8`

### Compilation
From the root of the repository, execute the following build command:

```bash
clang++ -std=c++17 -O3 -arch arm64 \
    -I./include \
    -I/opt/homebrew/include \
    src/nl_bridge.cpp src/binary_parser.cpp src/main.cpp \
    -L/opt/homebrew/lib -lv8 \
    -framework Metal -framework Foundation -framework AppKit -framework CoreVideo -framework AVFoundation \
    -o ControlResonant_Mac
```

### Running the Host
```bash
./ControlResonant_Mac
```

---

## 📂 Project Structure
```text
├── include/            # API Blueprints and Type Definitions
│   └── nl_api_blueprint.h
├── src/                # Implementation Logic
│   ├── nl_bridge.cpp   # Core Host & V8/Metal Integration
│   ├── binary_parser.cpp # Proprietary Asset Extraction
│   └── main.cpp        # Boot Sequence & Render Loop
├── build_config.json   # Build environment specifications
└── .gitignore         # Asset safety filter
```

## ⚖️ Legal Notice
This project is a **host implementation only**. It does not contain any original game assets, scripts, or binaries. Users must provide their own legal copies of the Control Resonant game files to use this software.

---

## 🤝 Contributing
Contributions to the `NLBridge` API are welcome. Please ensure all new native bindings are added to the `nl_api_blueprint.h` first to maintain architectural consistency.
