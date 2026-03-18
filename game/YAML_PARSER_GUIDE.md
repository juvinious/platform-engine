# YAML Parser Implementation - Data Flow

## Overview

This document shows how the platformer engine transitioned from S-expressions to YAML, and the data flow through the parser.

---

## S-Expression → YAML Conversion

### Original S-Expression Format

```lisp
(animation 
  (id block)
  (basedir platformer/smb/images/)
  (image 0 block-tile.png)
  (frame 
    (image 0)
    (alpha 255)
    (hflip 0)
    (vflip 0)
    (time -1)
    (collision 
      (id collision)
      (position 0 0 16 16)
      (color 204 0 51)))
  (loop 0))
```

### Equivalent YAML Format

```yaml
- id: block
  basedir: platformer/smb/images/
  images:
    - block-tile.png
  frames:
    - imageIndex: 0
      alpha: 255
      hflip: false
      vflip: false
      duration: -1  # -1 means infinite/static
      collisions:
        - id: collision
          x: 0
          y: 0
          width: 16
          height: 16
          color: "204,0,51"
  loop: false
```

### Key Differences

| S-Expression | YAML | Benefit |
|---|---|---|
| Positional arguments | Named fields | Clarity, self-documenting |
| Nested parens | Indentation | Visual hierarchy |
| `(image 0 filename)` | `images: [filename]` | Natural list syntax |
| `(position 0 0 16 16)` | `x: 0, y: 0, width: 16, height: 16` | Explicitly named coordinates |
| `1` or `0` for booleans | `true`/`false` | Type-safe, less error-prone |

---

## Data Flow Through Parser

### 1. Game Startup
```
main.go
└─> NewGame("data/platformer/smb/mario.yaml")
    └─> Parser.ParseGame(path)
        └─> os.ReadFile() → YAML text
        └─> yaml.Unmarshal() → GameDef struct
        └─> Returns: game name, fonts, initialWorld path
```

### 2. Load World
```
game.go::NewGame()
├─> Obtains initialWorld: "platformer/smb/worlds/world-1-1.yaml"
└─> Parser.ParseWorld(path)
    └─> os.ReadFile() → YAML text
    └─> yaml.Unmarshal() → WorldDef struct
        └─> Returns: name, resolution, physics, cameras, animations[], objects[]
```

### 3. Create World Instance
```
Parser returns config.WorldDef
└─> world.go::NewWorldFromDef(def)
    ├─> Creates World struct with:
    │   ├─ Dimensions and physics from def
    │   ├─ FillColor from def.FillColor (RGB)
    │   ├─ Camera from first def.Cameras[0]
    │   └─ Animation definitions stored for later use
    └─> Returns: *world.World ready to run
```

### 4. Game Loop
```
Ebiten calls App.Update() and App.Draw() each frame
├─> Update()
│   └─> game.Update()
│       └─> world.Update()
│           ├─> Update all objects
│           └─> Update camera
└─> Draw()
    └─> game.Draw(screen)
        └─> world.Draw(screen)
            ├─ Fill screen with fillColor
            └─ Draw each object with camera offset
```

---

## Go Type Hierarchy

### Core Structures

```go
// In config.go - YAML-to-Go deserialization
type GameDef struct {
  Game struct {
    Name         string
    Version      string
    Fonts        []FontDef
    InitialWorld string  // Path to world YAML
  }
}

type WorldDef struct {
  World struct {
    Name       string
    Version    int
    Resolution IntVec2         // width, height
    Dimensions IntVec2         // world width, height
    FillColor  Color           // RGB
    Mechanics  MechanicsDef    // gravity, acceleration
    Cameras    []CameraDef
    Animations []AnimationDef  // All available animations
    Objects    []ObjectDef     // Object placements
  }
}

type AnimationDef struct {
  ID      string
  BaseDir string
  Images  []string       // Sprite sheet image paths
  Frames  []FrameDef
  Loop    bool
}

type FrameDef struct {
  ImageIndex int
  Alpha      int
  HFlip      bool
  VFlip      bool
  Duration   int
  Collisions []CollisionBoxDef
}
```

### Runtime Objects

```go
// In world.go - Created from parsed config
type World struct {
  name           string
  width          int
  height         int
  objects        []object.Object         // Live objects in world
  camera         *camera.Camera          // Current camera
  gravity        float64                  // From config
  fillColor      [3]int                   // From config
  animationDefs  map[string]config.AnimationDef  // Lookup table
}
```

---

## YAML Tags and Field Mapping

The parser uses Go struct tags to map YAML keys to Go fields:

```go
type WorldDef struct {
	World struct {
		Name       string          `yaml:"name"`           // Matches YAML key "name"
		Resolution IntVec2         `yaml:"resolution"`     // Matches YAML key "resolution"
		FillColor  Color           `yaml:"fillColor"`      // Matches camelCase
		Camera     CameraDef       `yaml:"camera"`
		Animations []AnimationDef  `yaml:"animations"`
		Objects    []ObjectDef     `yaml:"objects,omitempty"` // omitempty = optional
	} `yaml:"world"`
}
```

The `omitempty` tag means that field can be absent from YAML without error.

---

## Benefits of This Approach

1. **Type Safety**: Go's type system catches malformed configs at parse time
2. **Clear Errors**: YAML parser gives line numbers for errors
3. **Extensibility**: Adding new fields is trivial (just add to struct + YAML)
4. **Standardization**: YAML is widely supported in editors/IDEs
5. **Human Readable**: Indentation-based, no quote spam
6. **Validation**: Can add custom validation in `NewWorldFromDef()` if needed

---

## Example Error Handling

If YAML is malformed:

```yaml
world:
  name: incorrect indent here
    dimensions:  # ← Wrong indentation
      width: 320
```

Parser output:
```
failed to parse world YAML from worlds/world-1-1.yaml: 
yaml: line 3: mapping values are not allowed in this context
```

---

## Next Steps

1. ✅ Created YAML files (mario.yaml, world-1-1.yaml)
2. ✅ Built YAML parser with Go unmarshaling
3. ✅ Wired parser into game loading pipeline
4. 🔄 **Next**: Create runtime object system from animation definitions
5. 🔄 **Next**: Implement sprite loading and rendering
