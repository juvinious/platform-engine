#!/usr/bin/env python3
"""
Extract individual sprites from a sprite sheet PNG.

Detects the background color from the image corners, finds all contiguous
non-background regions, tight-crops each sprite, and writes it as a PNG with
a magenta (255, 0, 255) mask background.

Usage:
    python3 sprite_dumper.py <spritesheet.png> [options]

Options:
    -o, --output DIR        Output directory (default: ./sprites)
    -b, --bgcolor R,G,B     Override background color (repeatable for multiple)
    -p, --padding N         Pixels of padding to add around each sprite (default: 0)
    -m, --min-size N        Minimum sprite width or height in pixels (default: 4)
    -t, --tolerance N       Color distance tolerance for background (default: 10)
    --preview               Generate a preview image with bounding boxes drawn
"""

import argparse
import os
import sys
from collections import Counter, deque

from PIL import Image, ImageDraw


MASK_COLOR = (255, 0, 255)  # magenta mask background


def detect_background_colors(img):
    """Detect background colors by sampling corners and scanning for border colors."""
    w, h = img.size
    corners = [
        img.getpixel((0, 0)),
        img.getpixel((w - 1, 0)),
        img.getpixel((0, h - 1)),
        img.getpixel((w - 1, h - 1)),
    ]
    bg_rgb = Counter(c[:3] for c in corners).most_common(1)[0][0]
    colors = [bg_rgb]

    # Scan for border/separator colors: look for full-width rows that are a
    # single non-bg color (common in sprite sheet rips with group panels).
    pixels = img.load()
    row_colors = Counter()
    for y in range(h):
        unique = set()
        for x in range(w):
            p = pixels[x, y]
            rgb = (p[0], p[1], p[2])
            if rgb != bg_rgb:
                unique.add(rgb)
        if len(unique) == 1:
            row_colors[list(unique)[0]] += 1

    # Any color that fills 10+ full-width rows is likely a border/separator
    for color, count in row_colors.most_common(5):
        if count >= 10 and color not in colors:
            colors.append(color)

    return colors


def is_background(r, g, b, bg_colors, tolerance):
    """Check if a pixel color matches any of the background/border colors."""
    for br, bg, bb in bg_colors:
        if (abs(r - br) <= tolerance and
            abs(g - bg) <= tolerance and
            abs(b - bb) <= tolerance):
            return True
    return False


def build_foreground_mask(img, bg_colors, tolerance=10):
    """Return a 2D boolean array: True where pixel is NOT background."""
    w, h = img.size
    pixels = img.load()
    mask = [[False] * w for _ in range(h)]
    for y in range(h):
        for x in range(w):
            p = pixels[x, y]
            r, g, b = p[0], p[1], p[2]
            a = p[3] if len(p) > 3 else 255
            if a < 10:
                continue
            if not is_background(r, g, b, bg_colors, tolerance):
                mask[y][x] = True
    return mask


def find_bounding_boxes(mask, w, h, min_size):
    """Find bounding boxes of connected foreground regions using flood fill."""
    visited = [[False] * w for _ in range(h)]
    boxes = []

    for sy in range(h):
        for sx in range(w):
            if not mask[sy][sx] or visited[sy][sx]:
                continue

            # BFS flood fill
            queue = deque()
            queue.append((sx, sy))
            visited[sy][sx] = True
            min_x, min_y = sx, sy
            max_x, max_y = sx, sy

            while queue:
                cx, cy = queue.popleft()
                min_x = min(min_x, cx)
                min_y = min(min_y, cy)
                max_x = max(max_x, cx)
                max_y = max(max_y, cy)

                for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                    nx, ny = cx + dx, cy + dy
                    if 0 <= nx < w and 0 <= ny < h and not visited[ny][nx] and mask[ny][nx]:
                        visited[ny][nx] = True
                        queue.append((nx, ny))

            bw = max_x - min_x + 1
            bh = max_y - min_y + 1
            if bw >= min_size and bh >= min_size:
                boxes.append((min_x, min_y, max_x, max_y))

    # Sort: top-to-bottom, then left-to-right (with row grouping)
    boxes.sort(key=lambda b: (b[1], b[0]))
    return boxes


def detect_local_backgrounds(raw_pixels, w, h, bg_colors, tolerance):
    """Detect local panel background colors from edge pixels of the crop.

    Scans the border (top/bottom rows, left/right columns) for non-global-bg
    colors, excluding very dark pixels (outlines). Returns colors that appear
    in a significant fraction of the edge.
    """
    edge = Counter()
    total_edge = 0

    for x in range(w):
        for y_off in (0, h - 1) if h > 1 else (0,):
            p = raw_pixels[x, y_off]
            rgb = (p[0], p[1], p[2])
            total_edge += 1
            if is_background(rgb[0], rgb[1], rgb[2], bg_colors, tolerance):
                continue
            if max(rgb) < 50:
                continue
            edge[rgb] += 1

    for y in range(1, h - 1):
        for x_off in (0, w - 1) if w > 1 else (0,):
            p = raw_pixels[x_off, y]
            rgb = (p[0], p[1], p[2])
            total_edge += 1
            if is_background(rgb[0], rgb[1], rgb[2], bg_colors, tolerance):
                continue
            if max(rgb) < 50:
                continue
            edge[rgb] += 1

    if not edge or total_edge == 0:
        return []

    local_bgs = []
    seen = set()
    for color, _ in edge.most_common(5):
        if any(abs(color[0] - s[0]) <= tolerance and
               abs(color[1] - s[1]) <= tolerance and
               abs(color[2] - s[2]) <= tolerance for s in seen):
            continue
        group_count = sum(c for rgb, c in edge.items()
                         if abs(rgb[0] - color[0]) <= tolerance and
                            abs(rgb[1] - color[1]) <= tolerance and
                            abs(rgb[2] - color[2]) <= tolerance)
        if group_count / total_edge >= 0.15:
            local_bgs.append(color)
            seen.add(color)

    return local_bgs


def flood_fill_bg_from_edges(raw_pixels, w, h, bg_colors, tolerance):
    """Flood fill from all border pixels to mark background regions.

    Only marks pixels as background if they match a known bg color AND are
    reachable from the image border through other bg pixels. Interior pixels
    that happen to match a bg color but are surrounded by sprite pixels are
    left untouched.

    Returns a 2D boolean mask: True = background (should become magenta).
    """
    is_bg = [[False] * w for _ in range(h)]
    visited = [[False] * w for _ in range(h)]
    queue = deque()

    # Seed with all border pixels that match background
    for x in range(w):
        for y in (0, h - 1) if h > 1 else (0,):
            p = raw_pixels[x, y]
            r, g, b = p[0], p[1], p[2]
            a = p[3] if len(p) > 3 else 255
            if a < 10 or is_background(r, g, b, bg_colors, tolerance):
                if not visited[y][x]:
                    visited[y][x] = True
                    is_bg[y][x] = True
                    queue.append((x, y))

    for y in range(1, h - 1):
        for x in (0, w - 1) if w > 1 else (0,):
            p = raw_pixels[x, y]
            r, g, b = p[0], p[1], p[2]
            a = p[3] if len(p) > 3 else 255
            if a < 10 or is_background(r, g, b, bg_colors, tolerance):
                if not visited[y][x]:
                    visited[y][x] = True
                    is_bg[y][x] = True
                    queue.append((x, y))

    # BFS: expand through adjacent bg-colored pixels
    while queue:
        cx, cy = queue.popleft()
        for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
            nx, ny = cx + dx, cy + dy
            if 0 <= nx < w and 0 <= ny < h and not visited[ny][nx]:
                visited[ny][nx] = True
                p = raw_pixels[nx, ny]
                r, g, b = p[0], p[1], p[2]
                a = p[3] if len(p) > 3 else 255
                if a < 10 or is_background(r, g, b, bg_colors, tolerance):
                    is_bg[ny][nx] = True
                    queue.append((nx, ny))

    return is_bg


def extract_sprite(img, box, bg_colors, padding, tolerance=10):
    """Extract a sprite using edge-connected flood fill for background removal.

    1. Crop the bounding box from the sheet.
    2. Detect local panel backgrounds from edge pixels.
    3. Flood fill from the border to find all edge-connected background pixels.
    4. Only those flood-filled pixels become magenta. Interior pixels matching
       a bg color are preserved (they're part of the sprite).
    5. Tight-crop to the remaining foreground pixels.
    """
    min_x, min_y, max_x, max_y = box
    raw = img.crop((min_x, min_y, max_x + 1, max_y + 1)).convert("RGBA")
    raw_pixels = raw.load()
    w, h = raw.size

    # Detect local panel backgrounds from edges
    all_bg = list(bg_colors)
    local_bgs = detect_local_backgrounds(raw_pixels, w, h, all_bg, tolerance)
    all_bg.extend(c for c in local_bgs if c not in all_bg)

    # Flood fill from edges to mark only reachable background
    bg_mask = flood_fill_bg_from_edges(raw_pixels, w, h, all_bg, tolerance)

    # Find tight bounds of non-background pixels
    crop_min_x, crop_min_y = w, h
    crop_max_x, crop_max_y = -1, -1
    for y in range(h):
        for x in range(w):
            if not bg_mask[y][x]:
                crop_min_x = min(crop_min_x, x)
                crop_min_y = min(crop_min_y, y)
                crop_max_x = max(crop_max_x, x)
                crop_max_y = max(crop_max_y, y)

    if crop_max_x < 0:
        return Image.new("RGB", (1, 1), MASK_COLOR)

    cw = crop_max_x - crop_min_x + 1
    ch = crop_max_y - crop_min_y + 1

    sprite = Image.new("RGB", (cw + padding * 2, ch + padding * 2), MASK_COLOR)
    dst = sprite.load()

    for y in range(ch):
        for x in range(cw):
            if not bg_mask[crop_min_y + y][crop_min_x + x]:
                p = raw_pixels[crop_min_x + x, crop_min_y + y]
                dst[padding + x, padding + y] = (p[0], p[1], p[2])

    return sprite


def generate_preview(img, boxes, output_path):
    """Draw bounding boxes on a copy of the source image for verification."""
    preview = img.copy().convert("RGBA")
    draw = ImageDraw.Draw(preview)
    for i, (x1, y1, x2, y2) in enumerate(boxes):
        draw.rectangle([x1, y1, x2, y2], outline=(255, 0, 0, 200), width=1)
        draw.text((x1, y1 - 10), str(i), fill=(255, 0, 0, 255))
    preview.save(output_path)
    print(f"Preview saved to {output_path}")


def main():
    parser = argparse.ArgumentParser(description="Extract sprites from a sprite sheet.")
    parser.add_argument("input", help="Path to the sprite sheet PNG")
    parser.add_argument("-o", "--output", default="./sprites", help="Output directory")
    parser.add_argument("-b", "--bgcolor", action="append", default=None,
                        help="Background color as R,G,B (repeatable for multiple colors)")
    parser.add_argument("-p", "--padding", type=int, default=0,
                        help="Pixels of padding around each sprite")
    parser.add_argument("-m", "--min-size", type=int, default=4,
                        help="Minimum sprite dimension in pixels")
    parser.add_argument("-t", "--tolerance", type=int, default=10,
                        help="Color distance tolerance for background detection")
    parser.add_argument("--preview", action="store_true",
                        help="Generate a preview image with bounding boxes")
    args = parser.parse_args()

    if not os.path.isfile(args.input):
        print(f"Error: file not found: {args.input}", file=sys.stderr)
        sys.exit(1)

    img = Image.open(args.input).convert("RGBA")
    w, h = img.size
    print(f"Loaded {args.input} ({w}x{h})")

    if args.bgcolor:
        bg_colors = []
        for spec in args.bgcolor:
            parts = spec.split(",")
            bg_colors.append((int(parts[0]), int(parts[1]), int(parts[2])))
        print(f"Using provided background colors: {bg_colors}")
    else:
        bg_colors = detect_background_colors(img)
        print(f"Detected background colors: {bg_colors}")

    print("Building foreground mask...")
    mask = build_foreground_mask(img, bg_colors, args.tolerance)

    print("Finding sprite bounding boxes...")
    boxes = find_bounding_boxes(mask, w, h, args.min_size)
    print(f"Found {len(boxes)} sprites")

    if not boxes:
        print("No sprites found. Try adjusting --tolerance or --bgcolor.")
        sys.exit(0)

    os.makedirs(args.output, exist_ok=True)

    if args.preview:
        preview_path = os.path.join(args.output, "preview.png")
        generate_preview(img, boxes, preview_path)

    # Determine zero-padded width for filenames
    digits = len(str(len(boxes) - 1))

    for i, box in enumerate(boxes):
        sprite = extract_sprite(img, box, bg_colors, args.padding, args.tolerance)
        sw, sh = sprite.size
        filename = f"sprite_{str(i).zfill(digits)}_{sw}x{sh}.png"
        filepath = os.path.join(args.output, filename)
        sprite.save(filepath)

    print(f"Extracted {len(boxes)} sprites to {args.output}/")

    # Print a summary table
    print(f"\n{'#':>4}  {'Position':>12}  {'Size':>8}")
    print(f"{'—'*4}  {'—'*12}  {'—'*8}")
    for i, (x1, y1, x2, y2) in enumerate(boxes):
        print(f"{i:4d}  ({x1:4d},{y1:4d})  {x2-x1+1:3d}x{y2-y1+1:3d}")


if __name__ == "__main__":
    main()
