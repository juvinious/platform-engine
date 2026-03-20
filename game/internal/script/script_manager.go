package script

import (
	"fmt"
	"os"
	"path/filepath"

	"platformer/internal/object"

	lua "github.com/yuin/gopher-lua"
)

// ScriptManager owns the Lua VM and caches loaded script behavior tables.
//
// One ScriptManager is created per World (via world.NewScriptManager()) and
// shared across every scripted object in that world. All objects in a world
// therefore share the same LState, which is the correct and efficient design
// (Lua VMs are expensive to create).
//
// Usage pattern:
//
//  1. Load a behavior table once per script type:
//     tbl, err := sm.LoadScript("/path/to/goomba.lua")
//
//  2. For each instance of that type, create a per-instance self table:
//     self := sm.NewSelfTable(base)
//
//  3. Call init once after placing the object:
//     sm.CallInit(tbl, self)
//
//  4. Each frame the ScriptedObject calls its own Act(), which calls
//     tbl.act(self) internally via ScriptedObject.call().
//
//  5. The collision system calls ScriptedObject.NotifyMapCollision() /
//     NotifyObjectCollision() which call the corresponding Lua functions.
type ScriptManager struct {
	L     *lua.LState
	cache map[string]*lua.LTable // absolute path → cached behavior table
}

// NewScriptManager creates a Lua VM and an empty script cache.
func NewScriptManager() *ScriptManager {
	L := lua.NewState()
	L.OpenLibs()
	return &ScriptManager{
		L:     L,
		cache: make(map[string]*lua.LTable),
	}
}

// Close shuts down the Lua VM. Call this when the world is destroyed.
func (sm *ScriptManager) Close() {
	if sm.L != nil {
		sm.L.Close()
	}
}

// FindScript searches importPaths (rooted at dataDir) for "<module>.lua"
// and returns the first match. Returns ("", false) if not found.
func (sm *ScriptManager) FindScript(dataDir, module string, importPaths []string) (string, bool) {
	for _, ip := range importPaths {
		candidate := ""
		if filepath.IsAbs(ip) {
			candidate = filepath.Join(ip, module+".lua")
		} else {
			candidate = filepath.Join(dataDir, ip, module+".lua")
		}
		if _, err := os.Stat(candidate); err == nil {
			return candidate, true
		}
	}
	return "", false
}

// LoadScript loads and executes a Lua script file, caching the returned
// behavior table by absolute path. Subsequent calls with the same path return
// the cached table without re-executing the file.
//
// The Lua file must end with:
//
//	return BehaviorTable
//
// where BehaviorTable is a table containing the lifecycle callbacks:
// init, act, onMapCollision, onObjectCollision.
func (sm *ScriptManager) LoadScript(scriptPath string) (*lua.LTable, error) {
	absPath, err := filepath.Abs(scriptPath)
	if err != nil {
		absPath = scriptPath
	}

	if cached, ok := sm.cache[absPath]; ok {
		return cached, nil
	}

	stackTop := sm.L.GetTop()
	if err := sm.L.DoFile(absPath); err != nil {
		return nil, fmt.Errorf("failed to load script %s: %w", absPath, err)
	}
	newTop := sm.L.GetTop()
	if newTop <= stackTop {
		return nil, fmt.Errorf("script %s must return a table", absPath)
	}

	val := sm.L.Get(-1)
	// Reset to the previous stack level to avoid leaking returned values.
	sm.L.SetTop(stackTop)

	tbl, ok := val.(*lua.LTable)
	if !ok {
		return nil, fmt.Errorf("script %s must return a table", absPath)
	}

	sm.cache[absPath] = tbl
	return tbl, nil
}

// NewSelfTable creates a per-instance Lua table for base.
// Every method in the table is a closure bound directly to base, so:
//
//	self:setVelocityX(5)   modifies this specific object's VelocityX
//	self:getX()            returns this specific object's X position
//
// This table is passed as the first argument ("self") to every Lua callback.
// See lua_api.go for the full list of available methods.
func (sm *ScriptManager) NewSelfTable(base *object.BaseObject, resolveAnim AnimationResolver) *lua.LTable {
	return newSelfTable(sm.L, base, resolveAnim)
}

// CallInit calls init(self) on scriptTable if the function is defined.
// Call this once after creating the ScriptedObject and adding it to the world.
func (sm *ScriptManager) CallInit(scriptTable, selfTable *lua.LTable) error {
	return sm.callFunc(scriptTable, selfTable, "init")
}

// callFunc calls name(selfTable, extra...) on scriptTable.
// Missing functions are silently skipped (return nil).
func (sm *ScriptManager) callFunc(scriptTable, selfTable *lua.LTable, name string, extra ...lua.LValue) error {
	fn := scriptTable.RawGetString(name)
	if fn.Type() == lua.LTNil {
		return nil
	}
	if fn.Type() != lua.LTFunction {
		return fmt.Errorf("script field '%s' is not a function", name)
	}
	sm.L.Push(fn)
	sm.L.Push(selfTable)
	for _, arg := range extra {
		sm.L.Push(arg)
	}
	return sm.L.PCall(1+len(extra), 0, nil)
}
