# Roadmap: Control Resonant Native Mac Port

This document outlines the strategic plan for porting Control Resonant to a native macOS application.

## 🗺️ Project Vision
Create a high-performance, native macOS host that executes the original game logic (Lua) by providing a native Metal-based implementation of the `NLBridge` API.

---

## 🚩 Phases & Milestones

### Phase 1: API Specification (The Blueprint)
*Goal: Move from "TODO" to a concrete technical specification.*
- [ ] **Analyze API Map**: Thoroughly review `nl_api_map.txt` to determine exact function signatures.
- [ ] **Finalize Header**: Update `nl_api_blueprint.h` with correct types (e.g., `glm::vec3`, `float`, `uint32_t`).
- [ ] **Asset Protocol**: Define how the native host will identify and load meshes, textures, and sounds referenced in Lua.

### Phase 2: The Native Host (The Shell)
*Goal: Create a running application that can execute a Lua script.*
- [ ] **Project Setup**: Initialize macOS C++ project (Xcode/CMake).
- [ ] **Lua Integration**: Embed LuaJIT and verify script execution.
- [ ] **Windowing**: Implement a basic Metal window using AppKit or GLFW.
- [ ] **Binding Layer**: Create the glue code that allows Lua scripts to call C++ `NLBridge` functions.

### Phase 3: Bridge Implementation (The Muscle)
*Goal: Implement the actual functionality behind the API calls.*
- [ ] **Rendering Engine**: 
    - [ ] Mesh rendering pipeline (Metal).
    - [ ] Material and lighting system.
    - [ ] Camera and FOV controls.
- [ ] **Physics System**:
    - [ ] World position and force application.
    - [ ] Raycasting implementation.
- [ ] **Input System**:
    - [ ] Map macOS keyboard/mouse/gamepad to `NLBridge` input calls.
- [ ] **Core Systems**:
    - [ ] High-resolution game clock and delta-time calculations.

### Phase 4: Integration & Testing (The Breath)
*Goal: Run original game logic in the new host.*
- [ ] **Script Loading**: Successfully load and run extracted `.lua` files.
- [ ] **API Validation**: Use logs to verify that Lua calls are hitting the correct C++ implementations.
- [ ] **Asset Integration**: Connect the asset loader to the rendering pipeline.
- [ ] **Behavioral Sync**: Compare behavior with the original game to ensure logic parity.

### Phase 5: Optimization & Polishing (The Shine)
*Goal: Make it feel like a professional native Mac app.*
- [ ] **Apple Silicon Optimization**: Optimize Metal shaders for M1/M2/M3 chips.
- [ ] **macOS Packaging**: Create a proper `.app` bundle with icons and permissions.
- [ ] **Performance Tuning**: Frame-rate stabilization and memory optimization.

---

## 🛠️ Technical Stack
- **Language**: C++ (Host), Lua/LuaJIT (Logic).
- **Graphics API**: Metal.
- **OS Framework**: AppKit.
- **Math Library**: GLM (OpenGL Mathematics).
- **Tooling**: Xcode, CMake.

## 📝 Notes
- *The bridge is the critical path; if the API signatures are wrong, the logic will crash or behave unexpectedly.*
