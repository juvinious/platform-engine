# Architecture Audit - Go Game Engine vs C++ Original

## Current Issue
The Go engine architecture diverges from the proven C++ design. The demo creates and manages objects manually instead of letting the engine handle it through config files.

## C++ Architecture (Proven)
```
main.cpp:
  - Create Game("data/platformer/smb/mario.txt")
  - game.run()

Game class:
  - Load config file
  - Create World from config
  - Run standard loop: world.act() -> world.draw()

World class:
  - Manages all objects
  - Loads object definitions from YAML
  - Handles physics, collision, rendering
```

## Current Go Architecture (Wrong)
```
demo.go:
  - Create ScriptManager
  - Load script directly
  - Manually create GameObject
  - Manually apply physics
  - Manually call script functions
  - (Reimplements engine logic)
```

## What Needs to Change

### 1. Create World/Level Manager
- Load object definitions from YAML files (game/data/platformer/smb/enemies/goomba.yaml)
- Create objects from definitions
- Manage object lifecycle

### 2. Move Object Initialization
- YAML defines: animation, size, initial velocity, physics
- Lua script defines: behavior logic only
- Engine loads and applies both

### 3. Simplify Script Integration
- Load script from YAML definition, not manually
- Call standard lifecycle: init(), act(), onCollision()
- Don't duplicate setup logic in demo

### 4. Proper Game Loop
```
Setup:
  - Load config (mario.yaml)
  - Create World
  - World loads creatures from definitions

Loop:
  - Input handling  
  - world.Act()     // All objects call act()
  - world.Render()  // All objects render
```

## Files Involved
- game/internal/game/game.go - NEW: Main game orchestrator
- game/internal/world/world.go - REFACTOR: Load from YAML, manage objects
- game/internal/script/script_manager.go - Keep as-is
- game/demos/goomba_demo.go - SIMPLIFY: Just load config and run

## Example Result (What It Should Look Like)
```go
func main() {
    game := game.NewGame("data/platformer/smb/mario.yaml")
    game.Run()
}
```

That's it. No manual object creation, no script loading, no physics management in the demo.
