#!/usr/bin/env python3
"""
Convert a platformer world .txt s-expression file to YAML for the Go engine.

Usage: python3 txt_to_yaml.py world-1-1.txt > world-1-1.yaml

Parsers understood (from src/):
  world.cpp     : name, resolution, dimensions, fill-color, mechanics,
                  camera, animation, background, foreground, collision-map,
                  script-import-path, script, object-script, object
  background.cpp: type, scroll-x, scroll-y, animation, tileset
  tile.cpp      : TileManager -> tile-size, dimensions, tile
                  Tile        -> animation, position (row col),
                                 repeat-up/down/left/right
  animation.cpp : id, basedir, image (N file), frame, loop
  animation.cpp : frame -> image, alpha, hflip, vflip, time, collision
  collisions.cpp: collision -> id, position (x y w h), color
  collisions.cpp: collision-map -> area -> position (x y w h)
  camera.cpp    : id, dimensions, start, viewport, speed, velocity,
                  follow-variance, smooth-scrolling, smooth-scroll-modifier
  script.cpp    : object-script -> id, module, function, position, animation
  world.cpp     : object -> position (x y), script
"""

import sys
import re

# ---------------------------------------------------------------------------
# S-expression tokeniser / parser
# ---------------------------------------------------------------------------

def tokenise(text):
    """Return list of tokens: '(', ')', or a bare atom/string."""
    tokens = []
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c in ' \t\n\r':
            i += 1
        elif c == ';':            # line comment
            while i < n and text[i] != '\n':
                i += 1
        elif c == '(':
            tokens.append('(')
            i += 1
        elif c == ')':
            tokens.append(')')
            i += 1
        elif c == '"':
            j = i + 1
            while j < n and text[j] != '"':
                if text[j] == '\\':
                    j += 1
                j += 1
            tokens.append(text[i+1:j])
            i = j + 1
        else:
            j = i
            while j < n and text[j] not in ' \t\n\r();"':
                j += 1
            tokens.append(text[i:j])
            i = j
    return tokens


def parse(tokens, pos):
    """Return (node, new_pos). tokens is a list, pos is current index."""
    tok = tokens[pos]
    if tok == '(':
        lst = []
        pos += 1
        while pos < len(tokens) and tokens[pos] != ')':
            node, pos = parse(tokens, pos)
            lst.append(node)
        if pos < len(tokens):  # consume ')'
            pos += 1
        return lst, pos
    elif tok == ')':
        raise SyntaxError("Unexpected ')'")
    else:
        return tok, pos + 1


def parse_all(text):
    toks = tokenise(text)
    results = []
    pos = 0
    while pos < len(toks):
        node, pos = parse(toks, pos)
        results.append(node)
    return results


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def node_name(node):
    """First element of a list node (the keyword)."""
    if isinstance(node, list) and node:
        return str(node[0])
    return None


def children(node):
    """Child nodes (sub-lists) of a list node."""
    return [c for c in node[1:] if isinstance(c, list)]


def atoms(node):
    """Atom values (non-list) of a node."""
    return [c for c in node[1:] if not isinstance(c, list)]


def first_child(node, key):
    for c in node[1:]:
        if isinstance(c, list) and c and str(c[0]) == key:
            return c
    return None


def all_children(node, key):
    return [c for c in node[1:] if isinstance(c, list) and c and str(c[0]) == key]


def atom_val(node, key, default=None):
    """Return first atom value of a child node matching key."""
    c = first_child(node, key)
    if c is None:
        return default
    vals = atoms(c)
    return vals[0] if vals else default


def to_int(v, default=0):
    try:
        return int(v)
    except (TypeError, ValueError):
        return default


def to_float(v, default=0.0):
    try:
        return float(v)
    except (TypeError, ValueError):
        return default


def to_bool(v, default=False):
    if isinstance(v, bool):
        return v
    if v in ('1', 'true', 'True'):
        return True
    if v in ('0', 'false', 'False'):
        return False
    return default


# ---------------------------------------------------------------------------
# Section converters
# ---------------------------------------------------------------------------

def convert_collision_area(node, default_id="collision"):
    """collision node inside a frame: (collision (id ..) (position x y w h) (color r g b))"""
    box = {}
    id_node = first_child(node, "id")
    box["id"] = str(atoms(id_node)[0]) if id_node else default_id

    pos_node = first_child(node, "position")
    if pos_node:
        vals = atoms(pos_node)
        box["x"] = to_float(vals[0]) if len(vals) > 0 else 0.0
        box["y"] = to_float(vals[1]) if len(vals) > 1 else 0.0
        box["width"] = to_float(vals[2]) if len(vals) > 2 else 0.0
        box["height"] = to_float(vals[3]) if len(vals) > 3 else 0.0

    color_node = first_child(node, "color")
    if color_node:
        cv = atoms(color_node)
        if len(cv) >= 3:
            box["color"] = "#{:02x}{:02x}{:02x}".format(
                to_int(cv[0]), to_int(cv[1]), to_int(cv[2]))

    return box


def convert_frame(node):
    """frame node inside animation."""
    frame = {}
    frame["imageIndex"] = to_int(atom_val(node, "image", 0))
    frame["alpha"] = to_int(atom_val(node, "alpha", 255))
    frame["hflip"] = to_bool(atom_val(node, "hflip", "0"))
    frame["vflip"] = to_bool(atom_val(node, "vflip", "0"))
    frame["duration"] = to_int(atom_val(node, "time", -1))

    coll_nodes = all_children(node, "collision")
    if coll_nodes:
        frame["collisions"] = [convert_collision_area(c) for c in coll_nodes]

    return frame


def convert_animation(node):
    """animation node."""
    anim = {}
    anim["id"] = str(atom_val(node, "id", ""))
    anim["basedir"] = str(atom_val(node, "basedir", ""))

    # images: (image N file) -> ordered list by index
    image_map = {}
    for img_node in all_children(node, "image"):
        vals = atoms(img_node)
        if len(vals) >= 2:
            image_map[to_int(vals[0])] = str(vals[1])
    anim["images"] = [image_map[k] for k in sorted(image_map)]

    anim["frames"] = [convert_frame(f) for f in all_children(node, "frame")]

    loop_node = first_child(node, "loop")
    anim["loop"] = loop_node is not None

    return anim


def convert_tile(node):
    """tile node inside tileset."""
    tile = {}
    tile["animation"] = str(atom_val(node, "animation", ""))

    pos_node = first_child(node, "position")
    if pos_node:
        vals = atoms(pos_node)
        # (position ROW COL) — row=y axis, col=x axis
        row = to_int(vals[0]) if len(vals) > 0 else 0
        col = to_int(vals[1]) if len(vals) > 1 else 0
        tile["position"] = {"x": col, "y": row}
    else:
        tile["position"] = {"x": 0, "y": 0}

    # repeat modifiers
    for key in ("repeat-up", "repeat-down", "repeat-left", "repeat-right"):
        val = atom_val(node, key)
        if val is not None:
            tile[key.replace("-", "_")] = to_int(val)

    return tile


def convert_tileset(node):
    """tileset node inside background/foreground."""
    ts = {}

    ts_node = first_child(node, "tile-size")
    if ts_node:
        vals = atoms(ts_node)
        ts["tileSize"] = {
            "x": to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y": to_float(vals[1]) if len(vals) > 1 else 0.0,
        }
    else:
        ts["tileSize"] = {"x": 0.0, "y": 0.0}

    dim_node = first_child(node, "dimensions")
    if dim_node:
        vals = atoms(dim_node)
        ts["dimensions"] = {
            "x": to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y": to_float(vals[1]) if len(vals) > 1 else 0.0,
        }
    else:
        ts["dimensions"] = {"x": 0.0, "y": 0.0}

    tiles = []
    for tile_node in all_children(node, "tile"):
        tile = convert_tile(tile_node)
        # Expand repeat directives inline
        base_row = tile["position"]["y"]
        base_col = tile["position"]["x"]
        tiles.append(tile)
        if "repeat_up" in tile:
            for i in range(1, tile["repeat_up"] + 1):
                t = dict(tile)
                t["position"] = {"x": base_col, "y": base_row - i}
                tiles.append(t)
            tile.pop("repeat_up")
        if "repeat_down" in tile:
            for i in range(1, tile["repeat_down"] + 1):
                t = dict(tile)
                t["position"] = {"x": base_col, "y": base_row + i}
                tiles.append(t)
            tile.pop("repeat_down")
        if "repeat_left" in tile:
            for i in range(1, tile["repeat_left"] + 1):
                t = dict(tile)
                t["position"] = {"x": base_col - i, "y": base_row}
                tiles.append(t)
            tile.pop("repeat_left")
        if "repeat_right" in tile:
            for i in range(1, tile["repeat_right"] + 1):
                t = dict(tile)
                t["position"] = {"x": base_col + i, "y": base_row}
                tiles.append(t)
            tile.pop("repeat_right")

    ts["tiles"] = tiles
    return ts


def convert_background(node):
    """background or foreground node."""
    bg = {}
    bg["type"] = str(atom_val(node, "type", "tileset"))
    bg["scrollX"] = to_float(atom_val(node, "scroll-x", 1.0))
    bg["scrollY"] = to_float(atom_val(node, "scroll-y", 1.0))

    anim_val = atom_val(node, "animation")
    if anim_val:
        bg["animation"] = str(anim_val)

    ts_node = first_child(node, "tileset")
    if ts_node:
        bg["tileset"] = convert_tileset(ts_node)

    return bg


def convert_camera(node):
    """camera node."""
    cam = {}
    id_val = atom_val(node, "id")
    cam["id"] = to_int(id_val) if id_val is not None else 0

    dim_node = first_child(node, "dimensions")
    if dim_node:
        vals = atoms(dim_node)
        cam["dimensions"] = {
            "width": to_int(vals[0]) if len(vals) > 0 else 0,
            "height": to_int(vals[1]) if len(vals) > 1 else 0,
        }

    start_node = first_child(node, "start")
    if start_node:
        vals = atoms(start_node)
        cam["start"] = {
            "x": to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y": to_float(vals[1]) if len(vals) > 1 else 0.0,
        }

    vp_node = first_child(node, "viewport")
    if vp_node:
        vals = atoms(vp_node)
        cam["viewport"] = {
            "x":      to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y":      to_float(vals[1]) if len(vals) > 1 else 0.0,
            "width":  to_float(vals[2]) if len(vals) > 2 else 0.0,
            "height": to_float(vals[3]) if len(vals) > 3 else 0.0,
        }

    speed = atom_val(node, "speed")
    if speed is not None:
        cam["speed"] = to_float(speed)

    vel = atom_val(node, "velocity")
    if vel is not None:
        cam["velocity"] = to_float(vel)

    fv = atom_val(node, "follow-variance")
    if fv is not None:
        cam["followVariance"] = to_float(fv)

    ss = atom_val(node, "smooth-scrolling")
    if ss is not None:
        cam["smoothScrolling"] = to_bool(ss)

    ssm = atom_val(node, "smooth-scroll-modifier")
    if ssm is not None:
        cam["smoothScrollModifier"] = to_float(ssm)

    return cam


def convert_mechanics(node):
    mech = {}
    grav_node = first_child(node, "gravity")
    if grav_node:
        vals = atoms(grav_node)
        mech["gravity"] = {
            "x": to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y": to_float(vals[1]) if len(vals) > 1 else 0.0,
        }
    acc = atom_val(node, "acceleration")
    if acc is not None:
        mech["acceleration"] = to_float(acc)
    return mech


def convert_collision_map(node):
    """collision-map node."""
    cm = {}
    areas = []
    for i, area_node in enumerate(all_children(node, "area")):
        area = {}
        area["id"] = "area-{}".format(i)
        pos_node = first_child(area_node, "position")
        if pos_node:
            vals = atoms(pos_node)
            area["position"] = {
                "x":      to_float(vals[0]) if len(vals) > 0 else 0.0,
                "y":      to_float(vals[1]) if len(vals) > 1 else 0.0,
                "width":  to_float(vals[2]) if len(vals) > 2 else 0.0,
                "height": to_float(vals[3]) if len(vals) > 3 else 0.0,
            }
        areas.append(area)
    cm["areas"] = areas
    return cm


def convert_object_script(node):
    """object-script node (world-level definition)."""
    obj = {}
    obj["id"] = str(atom_val(node, "id", ""))
    obj["module"] = str(atom_val(node, "module", ""))
    obj["function"] = str(atom_val(node, "function", ""))

    # Inline position (added by world.cpp at runtime, sometimes present)
    pos_node = first_child(node, "position")
    if pos_node:
        vals = atoms(pos_node)
        obj["position"] = {
            "x": to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y": to_float(vals[1]) if len(vals) > 1 else 0.0,
        }

    # Animations embedded in object-script
    anim_nodes = all_children(node, "animation")
    if anim_nodes:
        # If there's more than one, store as a list; otherwise single 'animation'
        if len(anim_nodes) == 1:
            obj["animation"] = convert_animation(anim_nodes[0])
        else:
            obj["animations"] = [convert_animation(a) for a in anim_nodes]

    return obj


def convert_object(node):
    """object node (instance)."""
    obj = {}
    obj["script"] = str(atom_val(node, "script", ""))
    pos_node = first_child(node, "position")
    if pos_node:
        vals = atoms(pos_node)
        obj["position"] = {
            "x": to_float(vals[0]) if len(vals) > 0 else 0.0,
            "y": to_float(vals[1]) if len(vals) > 1 else 0.0,
        }
    else:
        obj["position"] = {"x": 0.0, "y": 0.0}
    return obj


# ---------------------------------------------------------------------------
# YAML emitter (manual, to keep output clean)
# ---------------------------------------------------------------------------

def yaml_str(v):
    """Quote a string value if it needs quoting."""
    if not isinstance(v, str):
        return str(v)
    # Quote if empty, contains special chars, looks like a number/bool, or has spaces
    needs_quote = (
        v == '' or
        re.search(r'[:{}\[\],&*?|<>=!%@`#\n]', v) or
        re.match(r'^[-+]?\d', v) or
        v.lower() in ('true', 'false', 'null', 'yes', 'no', 'on', 'off') or
        ' ' in v
    )
    if needs_quote:
        return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    return v


def emit_yaml(data, indent=0):
    """Recursively emit YAML string."""
    pad = '  ' * indent
    lines = []

    if isinstance(data, dict):
        for k, v in data.items():
            if isinstance(v, dict):
                lines.append(pad + str(k) + ':')
                lines.append(emit_yaml(v, indent + 1))
            elif isinstance(v, list):
                lines.append(pad + str(k) + ':')
                lines.append(emit_yaml(v, indent + 1))
            elif isinstance(v, bool):
                lines.append(pad + str(k) + ': ' + ('true' if v else 'false'))
            elif isinstance(v, str):
                lines.append(pad + str(k) + ': ' + yaml_str(v))
            else:
                lines.append(pad + str(k) + ': ' + str(v))

    elif isinstance(data, list):
        for item in data:
            if isinstance(item, dict):
                first = True
                for k, v in item.items():
                    prefix = pad + ('- ' if first else '  ')
                    first = False
                    if isinstance(v, dict):
                        lines.append(prefix + str(k) + ':')
                        lines.append(emit_yaml(v, indent + 2))
                    elif isinstance(v, list):
                        lines.append(prefix + str(k) + ':')
                        lines.append(emit_yaml(v, indent + 2))
                    elif isinstance(v, bool):
                        lines.append(prefix + str(k) + ': ' + ('true' if v else 'false'))
                    elif isinstance(v, str):
                        lines.append(prefix + str(k) + ': ' + yaml_str(v))
                    else:
                        lines.append(prefix + str(k) + ': ' + str(v))
            elif isinstance(item, list):
                lines.append(pad + '- ')
                lines.append(emit_yaml(item, indent + 1))
            elif isinstance(item, bool):
                lines.append(pad + '- ' + ('true' if item else 'false'))
            elif isinstance(item, str):
                lines.append(pad + '- ' + yaml_str(item))
            else:
                lines.append(pad + '- ' + str(item))
    else:
        lines.append(pad + str(data))

    return '\n'.join(lines)


# ---------------------------------------------------------------------------
# Main conversion
# ---------------------------------------------------------------------------

def convert_world(world_node):
    world = {}

    # name
    name = atom_val(world_node, "name")
    world["name"] = str(name) if name else ""

    # resolution
    res_node = first_child(world_node, "resolution")
    if res_node:
        vals = atoms(res_node)
        world["resolution"] = {
            "width":  to_int(vals[0]) if len(vals) > 0 else 0,
            "height": to_int(vals[1]) if len(vals) > 1 else 0,
        }

    # dimensions
    dim_node = first_child(world_node, "dimensions")
    if dim_node:
        vals = atoms(dim_node)
        world["dimensions"] = {
            "width":  to_int(vals[0]) if len(vals) > 0 else 0,
            "height": to_int(vals[1]) if len(vals) > 1 else 0,
        }

    # fill-color
    fc_node = first_child(world_node, "fill-color")
    if fc_node:
        vals = atoms(fc_node)
        world["fillColor"] = {
            "r": to_int(vals[0]) if len(vals) > 0 else 0,
            "g": to_int(vals[1]) if len(vals) > 1 else 0,
            "b": to_int(vals[2]) if len(vals) > 2 else 0,
        }

    # mechanics
    mech_node = first_child(world_node, "mechanics")
    if mech_node:
        world["mechanics"] = convert_mechanics(mech_node)

    # cameras
    cams = all_children(world_node, "camera")
    if cams:
        world["cameras"] = [convert_camera(c) for c in cams]

    # animations
    anims = all_children(world_node, "animation")
    if anims:
        world["animations"] = [convert_animation(a) for a in anims]

    # backgrounds
    bgs = all_children(world_node, "background")
    if bgs:
        world["backgrounds"] = [convert_background(b) for b in bgs]

    # foregrounds
    fgs = all_children(world_node, "foreground")
    if fgs:
        world["foregrounds"] = [convert_background(f) for f in fgs]

    # collision-map
    cm_node = first_child(world_node, "collision-map")
    if cm_node:
        world["collisionMap"] = convert_collision_map(cm_node)

    # script-import-path
    sip = [str(atoms(n)[0]) for n in all_children(world_node, "script-import-path") if atoms(n)]
    if sip:
        world["scriptImportPaths"] = sip

    # script (top-level run-once scripts)
    script_nodes = all_children(world_node, "script")
    if script_nodes:
        scripts = []
        for sn in script_nodes:
            s = {}
            m = atom_val(sn, "module")
            f = atom_val(sn, "function")
            if m:
                s["module"] = str(m)
            if f:
                s["function"] = str(f)
            if s:
                scripts.append(s)
        if scripts:
            world["scripts"] = scripts

    # object-scripts
    os_nodes = all_children(world_node, "object-script")
    if os_nodes:
        world["objectScripts"] = [convert_object_script(n) for n in os_nodes]

    # objects
    obj_nodes = all_children(world_node, "object")
    if obj_nodes:
        world["objects"] = [convert_object(o) for o in obj_nodes]

    return {"world": world}


def main():
    if len(sys.argv) < 2:
        print("Usage: txt_to_yaml.py <world.txt> [output.yaml]", file=sys.stderr)
        sys.exit(1)

    in_path = sys.argv[1]
    with open(in_path, 'r') as f:
        text = f.read()

    nodes = parse_all(text)

    # Find the top-level (world ...) node
    world_node = None
    for node in nodes:
        if isinstance(node, list) and node and str(node[0]) == "world":
            world_node = node
            break

    if world_node is None:
        print("Error: no (world ...) node found in input.", file=sys.stderr)
        sys.exit(1)

    data = convert_world(world_node)
    yaml_output = emit_yaml(data)

    if len(sys.argv) >= 3:
        with open(sys.argv[2], 'w') as f:
            f.write(yaml_output + '\n')
        print("Written to", sys.argv[2], file=sys.stderr)
    else:
        print(yaml_output)


if __name__ == "__main__":
    main()
