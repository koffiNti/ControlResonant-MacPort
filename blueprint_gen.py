import os

# We take the nl_ list and create a C++ header draft
output_path = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\nl_api_blueprint.h"
nl_functions = [
    "nl_mesh_show", "nl_mesh_hide", "nl_body_world_position", 
    "nl_camera_horizontal_fov", "nl_light_color", "nl_time"
    # This is a subset; in a real run we would read the full list from the previous tool output
]

header = ["#pragma once\n#include <vector>\n#include <string>\n\nnamespace NLBridge {\n"]
for func in nl_functions:
    header.append(f"    void {func}(void* entity_id, ...); // Signature TBD")

header.append("\n}")

with open(output_path, "w") as out:
    out.write("\n".join(header))
