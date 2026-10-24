import os

output_path = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\nl_api_blueprint.h"

# Expanded function categories based on the discovered nl_ list
categories = {
    "Rendering": ["nl_mesh_show", "nl_mesh_hide", "nl_material_float", "nl_camera_horizontal_fov", "nl_light_color"],
    "Raytracing": ["nl_rt_enable", "nl_rt_set_quality", "nl_rt_debug_draw"], # Speculative names
    "Physics": ["nl_body_world_position", "nl_body_add_force", "nl_raycast"],
    "Input": ["nl_key_pressed", "nl_mouse_position", "nl_gamepad_button"],
    "Core": ["nl_time", "nl_delta_time", "nl_info", "nl_clock"]
}

header = ["#pragma once\n#include <vector>\n#include <string>\n#include <glm/glm.hpp>\n\nnamespace NLBridge {\n"]

for cat, funcs in categories.items():
    header.append(f"\n    // --- {cat} ---")
    for func in funcs:
        header.append(f"    void {func}(void* entity_id, ...); // TODO: Define signature")

header.append("\n}")

with open(output_path, "w") as out:
    out.write("\n".join(header))
