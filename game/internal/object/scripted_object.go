package object

// ScriptedObject is a world entity driven by a Lua behavior script.
//
// The embedded *BaseObject is the single source of truth for position,
// physics, animation, and collision. Lua receives a per-instance "self"
// table (selfTable) whose methods are closures bound directly to that
// BaseObject — so self:setVelocityX(5) modifies exactly this object's
// velocity, not a copy.
//
// Lifecycle callbacks (all optional — missing functions are silently skipped):
//
//	init(self)
//	    Called once after the object is created and placed in the world.
//
//	act(self)
//	    Called every frame, after the engine physics step.
//
//	onMapCollision(self, side)
//	    Called when the object collides with the world collision map.
//	    side is a Lua table: { top=bool, bottom=bool, left=bool, right=bool }.
//
//	onObjectCollision(self, other, side)
//	    Called when this object overlaps another object.
//	    other is the other object's selfTable (or nil if unscripted).
//	    side is the same { top, bottom, left, right } table.
//
// The L field is a pointer to the shared Lua VM owned by ScriptManager;
// it is never closed by ScriptedObject itself.

import (
	"platformer/internal/camera"
	"platformer/internal/logger"

	"github.com/hajimehoshi/ebiten/v2"
	lua "github.com/yuin/gopher-lua"
)

// LuaPropertySetter is an optional interface implemented by ScriptedObject.
// It lets Go code push named values into the object's Lua property table so
// that the behavior script can read them via self:getProperty(key).
// Supported value types: bool, int, float64, string.
type LuaPropertySetter interface {
	SetLuaProperty(key string, val interface{})
}

// LuaPropertyGetter is an optional interface implemented by ScriptedObject.
// It lets Go code read named values from the object's Lua property table.
type LuaPropertyGetter interface {
	GetLuaProperty(key string) (interface{}, bool)
}

// ScriptedObject wraps BaseObject with a Lua behavior table.
type ScriptedObject struct {
	*BaseObject                        // engine state — owned and definitive
	L           *lua.LState            // shared Lua VM (owned by ScriptManager)
	scriptTable *lua.LTable            // shared behavior table (one per script type)
	selfTable   *lua.LTable            // per-instance API table passed as "self"
	Props       map[string]interface{} // per-instance key/value store for Lua scripts
	warnedMiss  map[string]bool
}

// SetLuaProperty writes a value into this object's Lua property table.
// Values set here are readable in the script via self:getProperty(key).
func (s *ScriptedObject) SetLuaProperty(key string, val interface{}) {
	props, ok := s.selfTable.RawGetString("_props").(*lua.LTable)
	if !ok {
		return
	}
	switch v := val.(type) {
	case bool:
		props.RawSetString(key, lua.LBool(v))
	case int:
		props.RawSetString(key, lua.LNumber(float64(v)))
	case float64:
		props.RawSetString(key, lua.LNumber(v))
	case string:
		props.RawSetString(key, lua.LString(v))
	}
}

// GetLuaProperty reads a named value from this object's Lua property table.
// Returns the value and true if the key exists; nil and false otherwise.
func (s *ScriptedObject) GetLuaProperty(key string) (interface{}, bool) {
	props, ok := s.selfTable.RawGetString("_props").(*lua.LTable)
	if !ok {
		return nil, false
	}
	switch val := props.RawGetString(key).(type) {
	case lua.LBool:
		return bool(val), true
	case lua.LNumber:
		return float64(val), true
	case lua.LString:
		return string(val), true
	}
	return nil, false
}

// NewScriptedObject creates a scripted world object.
// base must not be nil. L, scriptTable, and selfTable are supplied by ScriptManager.
func NewScriptedObject(base *BaseObject, L *lua.LState, scriptTable, selfTable *lua.LTable) *ScriptedObject {
	return &ScriptedObject{
		BaseObject:  base,
		L:           L,
		scriptTable: scriptTable,
		selfTable:   selfTable,
		Props:       make(map[string]interface{}),
		warnedMiss:  make(map[string]bool),
	}
}

func (s *ScriptedObject) warnMissingCallbackOnce(name string) {
	if s.warnedMiss[name] {
		return
	}
	s.warnedMiss[name] = true
	logger.Warn("script callback missing object='%s' function='%s'", s.GetLabel(), name)
}

// GetSelfTable returns the per-instance Lua self table.
// Used by the collision system to pass "other" into onObjectCollision.
func (s *ScriptedObject) GetSelfTable() *lua.LTable {
	return s.selfTable
}

// Act lets Lua update intent/velocity first, then runs the engine physics step.
func (s *ScriptedObject) Act(world interface{}) {
	s.call("act")
	s.BaseObject.Act(world)
}

// Draw renders the object (delegates entirely to BaseObject).
func (s *ScriptedObject) Draw(screen *ebiten.Image, cam *camera.Camera) {
	s.BaseObject.Draw(screen, cam)
}

// NotifyMapCollision calls onMapCollision(self, side) in Lua.
// side keys: "top", "bottom", "left", "right".
func (s *ScriptedObject) NotifyMapCollision(side map[string]bool) {
	sideTable := s.L.NewTable()
	for k, v := range side {
		if v {
			sideTable.RawSetString(k, lua.LTrue)
		} else {
			sideTable.RawSetString(k, lua.LFalse)
		}
	}
	s.callWith("onMapCollision", sideTable)
}

// NotifyObjectCollision calls onObjectCollision(self, other, side) in Lua.
// If other is also a ScriptedObject its selfTable is passed; otherwise lua.LNil.
func (s *ScriptedObject) NotifyObjectCollision(other Object, side map[string]bool) {
	sideTable := s.L.NewTable()
	for k, v := range side {
		if v {
			sideTable.RawSetString(k, lua.LTrue)
		} else {
			sideTable.RawSetString(k, lua.LFalse)
		}
	}
	var otherSelf lua.LValue = lua.LNil
	if so, ok := other.(*ScriptedObject); ok {
		otherSelf = so.selfTable
	}
	s.callWith("onObjectCollision", otherSelf, sideTable)
}

// call invokes name(self) on the script table, ignoring missing functions.
func (s *ScriptedObject) call(name string) {
	if s.scriptTable == nil {
		return
	}
	fn := s.scriptTable.RawGetString(name)
	if fn.Type() != lua.LTFunction {
		if name == "onMapCollision" || name == "onObjectCollision" {
			s.warnMissingCallbackOnce(name)
		}
		return
	}
	s.L.Push(fn)
	s.L.Push(s.selfTable)
	if err := s.L.PCall(1, 0, nil); err != nil {
		logger.Error("script callback failed object='%s' function='%s': %v", s.GetLabel(), name, err)
	}
}

// callWith invokes name(self, args...) on the script table.
func (s *ScriptedObject) callWith(name string, args ...lua.LValue) {
	if s.scriptTable == nil {
		return
	}
	fn := s.scriptTable.RawGetString(name)
	if fn.Type() != lua.LTFunction {
		if name == "onMapCollision" || name == "onObjectCollision" {
			s.warnMissingCallbackOnce(name)
		}
		return
	}
	s.L.Push(fn)
	s.L.Push(s.selfTable)
	for _, arg := range args {
		s.L.Push(arg)
	}
	if err := s.L.PCall(1+len(args), 0, nil); err != nil {
		logger.Error("script callback failed object='%s' function='%s': %v", s.GetLabel(), name, err)
	}
}
