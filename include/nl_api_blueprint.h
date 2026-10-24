#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/**
 * NLBridge: Native macOS Port Bridge for Control Resonant
 * Optimized for Apple Silicon (UMA & Metal)
 */
namespace NLBridge {

    using EntityID = uint64_t;
    using AssetHandle = uint64_t;

    enum class AssetType : uint8_t {
        BinaryContainer, // The ".lua" binary files
        Mesh,
        Texture,
        ScriptJS,        // V8 JavaScript logic
        Audio,
        Configuration
    };

    struct BinaryContainerHeader {
        uint32_t magic;          // File signature
        uint32_t version;        // Container version
        uint32_t data_offset;     // Offset to actual JS/Data
        uint32_t data_size;       // Size of extracted payload
        uint32_t checksum;        // Integrity check
    };

    // --- Metal/UMA Resource Types ---
    struct MetalResource {
        void* device;         // MTLDevice
        void* commandQueue;   // MTLCommandQueue
        void* sharedBuffer;    // MTLBuffer (storageModeShared)
        size_t bufferSize;
    };

    struct RenderState {
        bool is_running;
        float clear_color[4];
        uint32_t frame_count;
    };

    // --- Asset Management (VFS & Resource Loading) ---
    // Handles the transition from original game assets to the Mac port
    AssetHandle nl_asset_load(const std::string& path, AssetType type);
    void nl_asset_unload(AssetHandle handle);
    bool nl_asset_exists(const std::string& path);

    // Bridges the loaded asset to a game entity
    void nl_entity_bind_asset(EntityID entity_id, AssetHandle handle);

    // V8 Integration: Loads a JS script into the global V8 context
    bool nl_v8_load_script(AssetHandle script_handle, const std::string& module_name);
    bool nl_v8_execute_function(const std::string& func_name, const std::vector<std::string>& args);

    // --- Audio (AVFoundation) ---
    void nl_audio_play(AssetHandle audio_handle, float volume = 1.0f, bool loop = false);
    void nl_audio_stop(AssetHandle audio_handle);
    void nl_audio_set_volume(AssetHandle audio_handle, float volume);

    // --- Rendering (Metal Accelerated) ---
    // All rendering calls target a Shared Metal Buffer for UMA efficiency
    void nl_mesh_show(EntityID entity_id);
    void nl_mesh_hide(EntityID entity_id);
    void nl_material_float(EntityID entity_id, uint32_t param_id, float value);
    void nl_material_float2(EntityID entity_id, uint32_t param_id, glm::vec2 value);
    void nl_material_float3(EntityID entity_id, uint32_t param_id, glm::vec3 value);
    void nl_material_float4(EntityID entity_id, uint32_t param_id, glm::vec4 value);
    void nl_camera_horizontal_fov(EntityID entity_id, float fov);
    void nl_camera_vertical_fov(EntityID entity_id, float fov);
    void nl_light_color(EntityID entity_id, glm::vec4 color);
    void nl_light_intensity(EntityID entity_id, float intensity);

    // --- Raytracing (Metal RT / MPS) ---
    void nl_rt_enable(EntityID entity_id, bool enabled);
    void nl_rt_set_quality(EntityID entity_id, int level);
    void nl_rt_debug_draw(EntityID entity_id, bool enabled);

    // --- Physics (SIMD / NEON Optimized) ---
    void nl_body_world_position(EntityID entity_id, glm::vec3 position);
    void nl_body_world_rotation(EntityID entity_id, glm::quat rotation);
    void nl_body_add_force(EntityID entity_id, glm::vec3 force);
    void nl_body_add_point_force(EntityID entity_id, glm::vec3 position, glm::vec3 force);
    void nl_raycast(EntityID entity_id, glm::vec3 origin, glm::vec3 direction, float max_dist, float* out_hit_dist);

    // --- Input (macOS HID / AppKit) ---
    bool nl_key_pressed(uint32_t key_code);
    bool nl_key_down(uint32_t key_code);
    bool nl_key_released(uint32_t key_code);
    glm::vec2 nl_mouse_position();
    glm::vec2 nl_mouse_delta();
    bool nl_mouse_button_pressed(uint32_t button_id);
    float nl_gamepad_axis(uint32_t axis_id);
    bool nl_gamepad_button(uint32_t button_id);

    // --- Core (High Resolution Timer) ---
    double nl_time();
    float nl_delta_time();
    uint64_t nl_clock();
    void nl_info(const std::string& message);

    // --- Window Management ---
    void nl_window_set_title(const std::string& title);
    void nl_window_set_size(int width, int height);
    void nl_window_set_fullscreen(bool enabled);

    // --- Transformations (GLM / SIMD) ---
    glm::vec3 nl_transform_point_local_to_world(EntityID entity_id, glm::vec3 point);
    glm::vec3 nl_transform_vector_world_to_local(EntityID entity_id, glm::vec3 vector);
    void nl_rotate_local_x(EntityID entity_id, float angle);
    void nl_rotate_world_y(EntityID entity_id, float angle);
    void nl_move_world(EntityID entity_id, glm::vec3 offset);

}
