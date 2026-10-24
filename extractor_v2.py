import os
import struct

toc_path = r"C:\Games\CONTROL Resonant\data_pack2\pc\base-en.rmdtoc"
blob_path = r"C:\Games\CONTROL Resonant\data_pack2\pc\base-en-000.rmdblob"
out_dir = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\ControlResonant_Extracted"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

with open(toc_path, "rb") as f:
    data = f.read()

# Based on the hex dump, the file starts with "COTR" (4 bytes)
# then some integers.
# Let's try to find strings and the bytes immediately following them.

print("Attempting structure analysis...")

import re
# We look for names. Filenames usually end with a null byte or are fixed length.
# Since we see strings like "app_astral_s0040_g020_exit", let's use that.
matches = re.finditer(b"[a-zA-Z0-9_\-]{4,}", data)

with open(blob_path, "rb") as blob:
    blob_size = os.path.getsize(blob_path)
    
    for match in matches:
        name_bytes = match.group(0)
        name = name_bytes.decode('ascii', errors='ignore')
        start_pos = match.start()
        
        # Heuristic: The offset and size are likely stored nearby.
        # Let's check the 8 bytes immediately following the string, 
        # or 8 bytes before the string.
        
        # Try following
        for shift in range(0, 16):
            try:
                off_pos = start_pos + len(name_bytes) + shift
                if off_pos + 8 > len(data): continue
                
                offset, size = struct.unpack("<II", data[off_pos:off_pos+8])
                
                if 0 < offset < blob_size and 0 < size < 10 * 1024 * 1024:
                    # Potential match. Read from blob.
                    blob.seek(offset)
                    chunk = blob.read(size)
                    
                    # Validation: does it look like a script or known asset?
                    if b"function" in chunk or b"local" in chunk or b"require" in chunk:
                        with open(os.path.join(out_dir, name + ".lua"), "wb") as out:
                            out.write(chunk)
                        print(f"Found Lua Script: {name} at {offset} size {size}")
            except:
                continue
