import os
import struct
import re

toc_path = r"C:\Games\CONTROL Resonant\data_pack2\pc\base-en.rmdtoc"
blob_path = r"C:\Games\CONTROL Resonant\data_pack2\pc\base-en-000.rmdblob"
out_dir = r"C:\Users\Daniel Osei-Tutu\Documents\ClaudeWork\ControlResonant_Extracted"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

with open(toc_path, "rb") as f:
    data = f.read()

print("Refining extraction...")
matches = re.finditer(b"[a-zA-Z0-9_\-]{4,}", data)

with open(blob_path, "rb") as blob:
    blob_size = os.path.getsize(blob_path)
    for match in matches:
        name_bytes = match.group(0)
        name = name_bytes.decode('ascii', errors='ignore')
        start_pos = match.start()
        
        for shift in range(0, 16):
            try:
                off_pos = start_pos + len(name_bytes) + shift
                if off_pos + 8 > len(data): continue
                offset, size = struct.unpack("<II", data[off_pos:off_pos+8])
                if 0 < offset < blob_size and 0 < size < 10 * 1024 * 1024:
                    blob.seek(offset)
                    chunk = blob.read(size)
                    # Strict check: Does it start with a common Lua pattern?
                    # We'll check if the first 100 bytes contain a Lua keyword.
                    header = chunk[:100]
                    if b"local " in header or b"function " in header or b"-- " in header:
                        with open(os.path.join(out_dir, name + ".lua"), "wb") as out:
                            out.write(chunk)
                        print(f"Verified Lua: {name}")
            except:
                continue
