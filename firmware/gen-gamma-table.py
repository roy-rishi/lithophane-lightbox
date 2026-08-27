# Generate an 8-bit LED gamma-correction table for C programs.
# Adapted from Adafruit's Gamma Correction article:
#   https://learn.adafruit.com/led-tricks-gamma-correction/the-longer-fix

gamma = 2.2

print("const uint8_t gamma_table[] = {", end='')
for i in range(256):
    if i > 0:
        print(',', end='')
    if (i & 15) == 0:
        print("\n  ", end='')
    val = int(pow(i / 255, gamma) * 255 + 0.5)
    print("%3d" % val, end='')
print(" };")
