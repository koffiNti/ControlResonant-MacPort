import os

root_dir = r"C:\Games\CONTROL Resonant"
output_path = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\asset_audit.txt"
log_path = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\asset_audit_log.txt"

results = []
try:
    with open(log_path, "w") as log:
        log.write("Starting Asset Audit...\n")
        for root, dirs, files in os.walk(root_dir):
            log.write(f"Scanning: {root}\n")
            for file in files:
                f_lower = file.lower()
                if "bink" in f_lower or "cohtml" in f_lower:
                    found = os.path.join(root, file)
                    results.append(found)
                    log.write(f"FOUND: {found}\n")
    
    with open(output_path, "w") as out:
        out.write("\n".join(results))
except Exception as e:
    with open(log_path, "a") as log:
        log.write(f"ERROR: {str(e)}\n")
