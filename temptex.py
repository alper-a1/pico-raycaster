import random

# Constants
WIDTH = 64
HEIGHT = 64

def to_rgb565_be(r, g, b):
    """
    Converts RGB to 16-bit RGB565, then swaps bytes for Big Endian.
    """
    # Clamp
    r = max(0, min(255, int(r)))
    g = max(0, min(255, int(g)))
    b = max(0, min(255, int(b)))
    
    # 1. Standard RGB565 (Little Endian on x86)
    # R: 5 bits, G: 6 bits, B: 5 bits
    val = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
    
    # 2. Swap Bytes (Big Endian)
    # (High Byte -> Low Pos) | (Low Byte -> High Pos)
    swapped = ((val >> 8) & 0xFF) | ((val << 8) & 0xFF00)
    
    return swapped

def format_cpp_array(name, pixels):
    """
    Formats the array string with specific constraints.
    """
    out = [f"// Texture: {name} (Column Major, Big Endian)"]
    # Explicit definition format requested
    out.append(f"alignas(4) static constexpr uint16_t {name}[4096] = {{")
    
    # Print formatted rows
    row_str = []
    for i, val in enumerate(pixels):
        row_str.append(f"0x{val:04X}")
        
        # New line every 16 elements for readability
        if (i + 1) % 16 == 0:
            out.append("    " + ", ".join(row_str) + ",")
            row_str = []
            
    out.append("};")
    return "\n".join(out)

# --- Procedural Texture Logic (Coordinate Based) ---

def get_xor_pixel(x, y):
    val = (x ^ y)
    return to_rgb565_be(val * 4, val * 2, val * 4)

def get_gradient_pixel(x, y):
    # Vertical gradient logic
    r = int((y / HEIGHT) * 255)
    g = int((x / WIDTH) * 50) # Slight horizontal tint
    b = int(((HEIGHT - y) / HEIGHT) * 255)
    return to_rgb565_be(r, g, b)

def get_checkers_pixel(x, y):
    size = 8
    if ((x // size) % 2) ^ ((y // size) % 2):
        return to_rgb565_be(200, 200, 200) # Light
    else:
        return to_rgb565_be(50, 50, 50)    # Dark

def get_bricks_pixel(x, y):
    brick_w, brick_h = 16, 16
    mortar = 1
    
    # Calculate row to determine offset
    row = y // brick_h
    x_offset = (brick_w // 2) if (row % 2) else 0
    
    tx = (x + x_offset) % brick_w
    ty = y % brick_h
    
    if tx < mortar or ty < mortar:
        return to_rgb565_be(80, 80, 80) # Grey mortar
    else:
        # Pseudo-random noise based on coords (deterministic)
        noise = ((x * y * 57) % 30) - 15
        return to_rgb565_be(160 + noise, 50 + noise, 40 + noise)

def generate_texture_data(func):
    """
    Generates pixel data in COLUMN MAJOR order.
    Outer Loop: X (Columns)
    Inner Loop: Y (Rows)
    """
    data = []
    for x in range(WIDTH):      # Column
        for y in range(HEIGHT): # Row
            data.append(func(x, y))
    return data

def main():
    # Define textures and their generation functions
    textures = [
        # ("tex_xor", get_xor_pixel),
        # ("tex_gradient", get_gradient_pixel),
        ("tex_checkers", get_checkers_pixel),
        ("tex_bricks", get_bricks_pixel)
    ]
    
    first = True
    for name, func in textures:
        if not first:
            print("") # Spacer between arrays
        first = False
        
        pixels = generate_texture_data(func)
        print(format_cpp_array(name, pixels))

if __name__ == "__main__":
    main()