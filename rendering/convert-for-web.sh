#!/bin/bash
# For every PNG in the current directory, output a WEBP to /out.
# Replaces the alpha channel with a solid color, and uses 85% compression level.

mkdir -p out

for f in *.png; do
  ffmpeg -i "$f" -f lavfi -i "color=c=0xDFEDFF" \
    -filter_complex "[1:v][0:v]scale2ref[bg][fg];[bg][fg]overlay=shortest=1" \
    -c:v libwebp -q:v 85 -compression_level 6 -preset picture -pix_fmt yuv420p \
    "out/${f%.png}.webp"
done
