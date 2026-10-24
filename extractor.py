import os
import struct

toc_path = r"C:\Games\CONTROL Resonant\data_pack2\pc\base-en.rmdtoc"
blob_path = r"C:\Games\CONTROL Resonant\data_pack2\pc\base-en-000.rmdblob"
out_dir = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\ControlResonant_Extracted"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

with open(toc_path, "rb") as f:
    data = f.read()

# We saw "COTR" at the start. Let's look for the file entries.
# Based on the strings, we see a lot of filenames. 
# We'll try to find patterns of [Name][Offset][Size]
# This is a heuristic approach since we don't have the exact struct yet.

print("Searching for file entries in TOC...")
import re
# Find strings that look like filenames/IDs
files = re.findall(b"[a-zA-Z0-9_\-\.]+", data)

# This is a naive attempt to find offsets. 
# Usually, offsets are 4 or 8 byte integers following the filename.
with open(blob_path, "rb") as blob:
    for file_name in files:
        name = file_name.decode('ascii', errors='ignore')
        if len(name) < 3: continue
        
        # Find where this name is in the TOC
        pos = data.find(file_name)
        # Try to read the next 8-16 bytes as offset/size
        # Heuristic: Offset is likely after the name
        try:
            # Attempt to read two 4-byte ints (Offset, Size)
            # We try a few offsets in case there is padding
            for offset_shift in range(0, 16):
                potential_offset = struct.unpack("<I", data[pos + len(file_name) + offset_shift : pos + len(file_name) + offset_shift + 4])[0]
                potential_size = struct.unpack("<I", data[pos + len(file_name) + offset_shift + 4 : pos + len(file_name) + offset_shift + 8])[0]
                
                # Validation: Offset should be within blob size, size shouldn't be astronomical
                if 0 < potential_offset < os.path.getsize(blob_path) and 0 < potential_size < 100 * 1024 * 1024:
                    blob.seek(potential_offset)
                    chunk = blob.read(potential_size)
                    
                    # If it looks like a Lua script or a known file type, save it
                    if b"function" in chunk or b"local" in chunk or b"require" in chunk:
                        with open(os.path.join(out_dir, name + ".lua"), "wb") as out:
                            out.write(chunk)
                        print(f"Extracted Lua: {name}")
                        break
        except:
            continue
