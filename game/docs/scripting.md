# Lua Scripting Guide

This document describes the current object scripting system used by the engine.

## Architecture

The scripting system is built around three core ideas:

1. One Lua VM (`LState`) per world.
2. One cached script table per script file/type.
3. One per-instance `self` table per object.

### Why this design

- Creating many Lua VMs is expensive, so worlds share a single VM.
- Script files are loaded once and cached by path.
- Each object still gets isolated state and methods through its own `self` table.

## Runtime Data Flow

At world load time:

1. `World` creates a `ScriptManager`.
2. For each object definition, `CreateObjectFromScript()` creates a `BaseObject`.
3. If a `module` is present and found:
   - Script file is located from `dataDir + scriptImportPaths + module + ".lua"`.
   - Script table is loaded/cached.
   - A per-instance `self` table is created and bound to the object.
   - A `ScriptedObject` wraps the base object.
   - `init(self)` is called once (if defined).
4. If no script module is set/found, a plain `BaseObject` is used.

During update:

1. Engine physics runs via `BaseObject.Act()`.
2. `ScriptedObject.Act()` calls Lua `act(self)` (if defined).

During collisions:

- Map collision calls `onMapCollision(self, side)`.
- Object collision calls `onObjectCollision(self, other, side)`.

## Lua Script Shape

A script file should return a table with optional callbacks:

```lua
local behavior = {}

function behavior.init(self)
end

function behavior.act(self)
end

function behavior.onMapCollision(self, side)
end

function behavior.onObjectCollision(self, other, side)
end

return behavior
```

All callbacks are optional. Missing functions are ignored.

## `self` API

Methods below are available as `self:method(...)`.

### Position

- `self:getX() -> number`
- `self:setX(number)`
- `self:addX(number)`
- `self:getY() -> number`
- `self:setY(number)`
- `self:addY(number)`

### Size

- `self:getWidth() -> number`
- `self:setWidth(number)`
- `self:getHeight() -> number`
- `self:setHeight(number)`

### Velocity

- `self:getVelocityX() -> number`
- `self:setVelocityX(number)`
- `self:getVelocityY() -> number`
- `self:setVelocityY(number)`

### Identity

- `self:getLabel() -> string`
- `self:setLabel(string)`

### Physics Mode

- `self:getPhysicsType() -> string`
- `self:setPhysicsType(string)`

Valid values:

- `dynamic`: gravity + velocity integration.
- `kinematic`: velocity integration only (no gravity).
- `static`: no physics movement.

### Custom Properties

- `self:getProperty(key) -> any`
- `self:setProperty(key, value)`

Properties are stored per object and persist across frames.

## Collision Arguments

### `onMapCollision(self, side)`

`side` is a table with boolean keys:

- `side.top`
- `side.bottom`
- `side.left`
- `side.right`

### `onObjectCollision(self, other, side)`

- `other` is the other object's `self` table if it is scripted, else `nil`.
- `side` is the same top/bottom/left/right boolean table.

## YAML Integration

In world YAML, object scripts can configure:

- `id`: object script id
- `module`: Lua module filename without `.lua`
- `animation` or `animations`
- `physicsType`

Example:

```yaml
world:
  scriptImportPaths:
    - platformer/smb/behaviors

  objectScripts:
    - id: goomba
      module: goomba
      physicsType: dynamic
      animation:
        id: goomba
        basedir: platformer/smb/images/
        images: [goomba-walk-01.png, goomba-walk-02.png]
        frames:
          - imageIndex: 0
            alpha: 255
            duration: 10
          - imageIndex: 1
            alpha: 255
            duration: 10
        loop: true
```

`scriptImportPaths` are searched in order.

## Engine Notes

- `ScriptedObject` embeds `*BaseObject`; there is no separate script-only object model.
- Animation and physics remain engine-owned, script only manipulates exposed values.
- Script load failures return an error during object creation.
- Missing script files log a warning and fallback to unscripted base object.

## Adding A New Scripted Object

1. Add a Lua file under one of the configured import paths.
2. Return a behavior table (`init`, `act`, collisions as needed).
3. Add an `objectScripts` entry in YAML with `module` set to that Lua filename.
4. Set `physicsType` appropriately (`dynamic`, `kinematic`, or `static`).
5. Place object instances in `world.objects` using that script id.

## Testing

The Lua tests in `game/tests/lua_test.go` validate:

- script loading
- self API method availability
- position/velocity/property mutation through Lua
- `goomba` init/act/collision behavior
- `block` init/hit behavior
