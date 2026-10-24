# Project Progress: Control Resonant Mac Port

## 🎯 Current Goal
Create a native macOS host for Control Resonant using Apple Silicon (UMA/Metal) and V8.

## 🛠️ Current Status
- **Last Updated:** 2026-10-24
- **Phase:** Phase 1 (API Specification) - 100% Complete.
- **Key Discovery:** The game uses the **V8 JavaScript Engine**, not Lua. The `.lua` files are binary data containers.
- **Achievements:** 
    - Established `ControlResonant_MacPort` project structure.
    - Created a high-performance `nl_api_blueprint.h` optimized for Apple Silicon (Metal/UMA).
    - Identified the native binary `CONTROLResonant.exe` and confirmed V8 presence.
    - Mapped core API categories (Rendering, Physics, Input, Core).
    - Finalized the asset loading protocol for V8 scripts and binary containers.

## ⏳ Pending / Next Steps
- [ ] **Environment Fix**: User to add Java to System PATH and set `JAVA_HOME` to fix REA/Ghidra toolchain.
- [ ] **Phase 1 Completion**: Finalize asset loading protocol.
- [ ] **Phase 2 Start**: Initialize the macOS C++ project, embed V8, and implement the first `NLBridge` functions.

## 📝 Critical Notes
- **V8 over Lua**: The "lua" files are binary. The logic is JS.
- **Apple Silicon Strategy**: Use `MTLBuffer` with `.storageModeShared` for zero-copy CPU $\rightarrow$ GPU communication.
- **Tools**: REA is ready once Java is in the system path.
