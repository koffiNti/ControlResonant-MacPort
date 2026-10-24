import os
import re

exe_path = r"C:\Games\CONTROL Resonant\CONTROLResonant.exe"
output_path = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\nl_api_map.txt"

with open(exe_path, "rb") as f:
    data = f.read()

# We look for the "nl_" strings and then search the surrounding bytes 
# for references to these strings in the code section (typical for Lua registration).
# This is a complex task, so we'll start by listing all occurrences and 
# searching for patterns that look like function pointers.

results = []
nl_functions = re.findall(b"nl_[a-zA-Z0-9_]+", data)

for func in set(nl_functions):
    # Find all occurrences of this function name
    pos = data.find(func)
    while pos != -1:
        # Heuristic: Look for a reference to this address in the binary
        # (simplification for this background script)
        results.append(f"Found {func.decode()} at offset {hex(pos)}")
        pos = data.find(func, pos + 1)

with open(output_path, "w") as out:
    out.write("\n".join(results))
