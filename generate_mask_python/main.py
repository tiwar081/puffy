import os
from glob import glob
import generate_mask

# Resolve folders relative to this file so it works from any CWD
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
SRC_FOLDER = os.path.join(BASE_DIR, "input")
OUT_FOLDER = os.path.join(BASE_DIR, "output")

os.makedirs(OUT_FOLDER, exist_ok=True)

# Default to strawberry.png; fall back to first png in input
preferred_path = os.path.join(SRC_FOLDER, "dragon.jpeg")
if os.path.exists(preferred_path):
    source_path = preferred_path
else:
    # Find any PNGs (case-insensitive)
    pngs = glob(os.path.join(SRC_FOLDER, "*.png")) + glob(os.path.join(SRC_FOLDER, "*.PNG"))
    if not pngs:
        raise FileNotFoundError(f"No PNG files found in {SRC_FOLDER}")
    source_path = pngs[0]

basename = os.path.splitext(os.path.basename(source_path))[0]
mask_path = os.path.join(OUT_FOLDER, f"mask_{basename}.png")

# You can adjust the left padding value here (in pixels)
left_padding = 200  # Change this value to add more or less padding

generate_mask.interactive_generate_mask(source_path, mask_path, output_size=(256, 256), left_padding=left_padding)
