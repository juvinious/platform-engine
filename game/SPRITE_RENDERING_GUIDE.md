# Sprite Rendering Implementation

## Components Created

### 1. **internal/sprite/sprite.go** - Core Sprite Renderer
- `Sprite` struct: Manages animation frames, images, and rendering
- Features:
  - Loads all sprite sheet images from disk via AssetManager
  - Handles frame-by-frame animation with timing
  - Supports alpha blending, horizontal flip, vertical flip
  - Camera-aware rendering with offset

Key methods:
```go
NewSprite(def, assetMgr)           // Create from animation config
Update()                            // Advance animation frame
Draw(screen, x, y)                  // Render at world position
DrawWithCamera(screen, x, y, camX, camY) // Render with camera offset
GetCurrentFrame()                   // Get collision/render data
GetSize()                           // Get sprite dimensions
```

### 2. **internal/renderer/renderer.go** - Sprite Manager
- `SpriteRenderer` struct: Manages all active sprites
- Registry system to cache sprites by ID
- Batch update and render operations

Key methods:
```go
RegisterSprite(id, sprite)              // Add sprite to registry
RenderSprite(screen, id, x, y)          // Draw sprite by ID
RenderSpriteWithCamera(...)             // Draw with camera offset
UpdateSprites()                         // Update all animations
```

### 3. **internal/animation/animation.go** - Refactored
- New `SpriteAnimation` wrapper around Sprite
- Bridges animation config to sprite rendering
- Handles sprite creation from YAML definitions

### 4. **internal/object/object.go** - Updated
- Added sprite animation support to BaseObject
- `SetAnimation()` and `GetAnimation()` methods
- Object.Act() now updates animation each frame
- Object.Draw() now renders sprite with camera offset

### 5. **internal/world/world.go** - Enhanced
- Added `AssetManager` to World struct
- New `GetAssetManager()` method
- `GetAnimationDef()` to look up animation configs
- `CreateTestObject()` factory method for demo
- `populateTestWorld()` adds test sprites to scene

## Data Flow: File → Screen

```
YAML File (world-1-1.yaml)
    ↓
Parser.ParseWorld() → WorldDef
    ↓
World.NewWorldFromDef()
    ├─ Store AnimationDef in map
    ├─ Create AssetManager
    └─ Call populateTestWorld()
    ↓
World.CreateTestObject("block", x, y)
    ├─ Lookup AnimationDef by ID
    ├─ Create SpriteAnimation (wraps Sprite)
    ├─ Sprite.NewSprite() loads all images
    ├─ Create BaseObject with animation
    └─ Add to world.objects[]
    ↓
Game Loop - each frame:
    ├─ World.Update()
    │   ├─ object.Act() calls animation.Update()
    │   │   └─ Sprite.Update() advances frame
    │   └─ camera.Update()
    └─ World.Draw(screen)
        └─ object.Draw(screen, camera)
            └─ sprite.DrawWithCamera()
                └─ Render to screen at adjusted position
```

## Image Loading Pipeline

```
Animation YAML:
{
  id: "goomba",
  basedir: "platformer/smb/images/",
  images: ["goomba-walk-01.png", "goomba-walk-02.png"]
}
    ↓
Sprite.NewSprite() loops images:
    ├─ fullPath = "platformer/smb/images/" + "goomba-walk-01.png"
    ├─ AssetManager.LoadImage(fullPath)
    │   ├─ os.Open() reads file
    │   ├─ image.Decode() parses PNG
    │   └─ ebiten.NewImageFromImage() converts to GPU texture
    └─ Cache in sprite.images[]
    ↓
Each frame: use sprite.images[frameIndex] for rendering
```

## Rendering with Transforms

Each frame supports:
- **Alpha**: Frame.Alpha (0-255) → ColorScale
- **HFlip**: Negative scale + translate on X
- **VFlip**: Negative scale + translate on Y

All applied via Ebiten's DrawImageOptions:
```go
opts := &ebiten.DrawImageOptions{}
opts.ColorScale.ScaleWithColor(color)  // Alpha blending
opts.GeoM.Scale(scaleX, scaleY)        // Flip
opts.GeoM.Translate(x, y)              // Position
screen.DrawImage(image, opts)
```

## Camera Integration

Camera offset applied during Draw:
```go
screenX = worldX - camera.X
screenY = worldY - camera.Y
sprite.Draw(screen, screenX, screenY)
```

Objects move in world space, camera provides viewport window.

## Test World

`populateTestWorld()` creates demonstration objects:
- **Tiles**: block, brick, question (static animations)
- **Enemies**: goomba, turtle (looping walk animations)

These render on screen to demonstrate:
- Static sprite rendering (duration: -1)
- Animated sprite sequencing (duration: 15 frames each)
- Alpha/flip transforms (from YAML config)
- Camera offset handling

## Usage Example

```go
// In world definition YAML, animations load automatically
// Objects created with CreateTestObject():
obj, err := world.CreateTestObject("turtle", 300, 200)
if err == nil {
    world.AddObject(obj)
}

// Each frame:
// - object.Act() → animation.Update() → sprite advances frame
// - object.Draw(screen, camera) → sprite renders with offset
```

## Next Steps

1. ✅ Sprite loading and rendering
2. ✅ Animation frame sequencing
3. ✅ Camera-aware drawing
4. 🔄 Add actual game objects (player, enemies with AI)
5. 🔄 Collision response based on frame data
6. 🔄 Input handling for player control
7. 🔄 Physics-based movement and gravity
