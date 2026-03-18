# Platformer Engine - Go/Ebiten Port

A work-in-progress port of the C++ platformer engine to Go using Ebiten for graphics rendering.

## Project Structure

```
game/
├── main.go                 # Entry point, Ebiten app
├── go.mod                  # Go module definition with dependencies
├── YAML_PARSER_GUIDE.md    # Detailed parser documentation
├── README.md               # This file
└── internal/
    ├── game/              # Game orchestrator
    ├── world/             # World/level management
    ├── object/            # Object types and behaviors
    ├── animation/         # Animation frame sequencing
    ├── physics/           # Movement and velocity
    ├── collision/         # Collision detection
    ├── camera/            # Camera/viewport management
    ├── assets/            # Asset loading and caching
    ├── tile/              # Tilemap system
    └── config/            # YAML parsing and structures
data/
├── platformer/
    ├── smb/
    │   ├── mario.yaml         # Game entry point
    │   ├── worlds/
    │   │   └── world-1-1.yaml # Level definition
    │   └── images/            # Sprite assets (symlink to original data)
```

## Configuration Format

The engine uses **YAML** for all configuration files instead of S-expressions. 

### Game Entry Point (mario.yaml)

```yaml
game:
  name: "Super Mario Bros."
  initialWorld: platformer/smb/worlds/world-1-1.yaml
  fonts:
    - name: small
      filename: platformer/smb/images/font1.png
      characterMap: "..."
```

### World Definition (world-1-1.yaml)

```yaml
world:
  name: world-1-1
  resolution:
    width: 320
    height: 240
  dimensions:
    width: 3392
    height: 240
  fillColor:
    r: 51
    g: 255
    b: 255
  mechanics:
    gravity:
      x: 0.0
      y: 1.5
    acceleration: 0.05
  cameras:
    - id: 0
      dimensions: { width: 3392, height: 240 }
      viewport: { x: 0, y: 0, width: 320, height: 240 }
  animations:
    - id: block
      basedir: platformer/smb/images/
      images: [block-tile.png]
      frames:
        - imageIndex: 0
          alpha: 255
          duration: -1
          collisions:
            - id: collision
              x: 0
              y: 0
              width: 16
              height: 16
```

See [YAML_PARSER_GUIDE.md](YAML_PARSER_GUIDE.md) for detailed parser flow and S-expression → YAML migration details.

## Requirements

- Go 1.21+
- Ebiten v2.6.0+
- Dependencies will be fetched automatically with `go mod download`

## Building

```bash
cd game
go mod download
go build -o platformer
```

## Running

```bash
cd game
go run main.go
```

The game will load from `data/platformer/smb/mario.yaml` which defines the initial world.

## Implementation Status

### ✅ Completed
- [x] Ebiten game loop scaffolding
- [x] Core type system (physics, animation, collision, camera)
- [x] YAML configuration parsing
- [x] World loading from YAML definitions
- [x] Game entry point resolution

### 🔄 In Progress
- [ ] Sprite/image loading and rendering
- [ ] Object instantiation from animation definitions
- [ ] Input handling (keyboard/gamepad)

### 📋 TODO
- [ ] Full animation system with sprite rendering
- [ ] Character controller and physics response
- [ ] Enemy entities (Goomba, Turtle, etc.)
- [ ] Platform/tile collision and response
- [ ] Level data format (object placement, tilemaps)
- [ ] Audio system (music, SFX)
- [ ] Menu/UI system
- [ ] Save/load system

## Architecture Notes

- **Immutable Config**: Configuration is parsed once at startup and stored in World
- **Type Safety**: YAML parsing uses Go struct tags with strict type conversion
- **Plugin Ready**: Animation system supports arbitrary animation definitions loaded from YAML
- **Camera System**: Supports multiple cameras and follow targets (expandable)
- **Physics**: Decoupled physics engine ready for scripting integration

## Data Migration

Original S-expression files are preserved in `../data/platformer/` for reference. New YAML files are in `game/data/platformer/`. Eventually, the old files can be migrated or removed.

