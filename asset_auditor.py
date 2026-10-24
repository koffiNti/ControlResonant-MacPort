import os

root_dir = r"C:\Games\CONTROL Resonant"
output_path = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\asset_audit.txt"

results = []
# Search for Bink and Cohtml signatures
for root, dirs, files in os.walk(root_dir):
    for file in files:
        if "bink" in file.lower() or "cohtml" in file.lower():
            results.append(f"Found dependency: {os.path.join(root, file)}")

with open(output_path, "w") as out:
    out.write("\n".join(results))
