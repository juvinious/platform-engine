package script

// newSelfTable builds a per-instance Lua table whose methods are closures
// bound directly to base. It is the only place in the engine where Lua touches
// the Go object state.
//
// API available in Lua scripts (called as self:method()):
//
//	Position
//	  getX() → number           setX(number)           addX(number)
//	  getY() → number           setY(number)           addY(number)
//
//	Size
//	  getWidth()  → number      setWidth(number)
//	  getHeight() → number      setHeight(number)
//
//	Velocity
//	  getVelocityX() → number   setVelocityX(number)
//	  getVelocityY() → number   setVelocityY(number)
//
//	Identity
//	  getLabel() → string       setLabel(string)
//
//	Physics
//	  getPhysicsType() → string  setPhysicsType(string)
//	    valid values: "dynamic", "static", "kinematic"
//
//	Contact state (computed by world collisions each frame)
//	  isGrounded() → bool
//	  isOnCeiling() → bool
//	  isOnWallLeft() → bool
//	  isOnWallRight() → bool
//
//	Custom properties (per-instance, persistent across frames)
//	  getProperty(key) → any    setProperty(key, value)

import (
	"platformer/internal/object"

	lua "github.com/yuin/gopher-lua"
)

func newSelfTable(L *lua.LState, base *object.BaseObject) *lua.LTable {
	t := L.NewTable()

	bind := func(name string, fn func(*lua.LState) int) {
		t.RawSetString(name, L.NewFunction(fn))
	}

	// ── Position ────────────────────────────────────────────────────────────

	bind("getX", func(L *lua.LState) int {
		x, _ := base.GetPosition()
		L.Push(lua.LNumber(x))
		return 1
	})
	bind("setX", func(L *lua.LState) int {
		_, y := base.GetPosition()
		base.SetPosition(float64(L.CheckNumber(2)), y)
		return 0
	})
	bind("addX", func(L *lua.LState) int {
		x, y := base.GetPosition()
		base.SetPosition(x+float64(L.CheckNumber(2)), y)
		return 0
	})

	bind("getY", func(L *lua.LState) int {
		_, y := base.GetPosition()
		L.Push(lua.LNumber(y))
		return 1
	})
	bind("setY", func(L *lua.LState) int {
		x, _ := base.GetPosition()
		base.SetPosition(x, float64(L.CheckNumber(2)))
		return 0
	})
	bind("addY", func(L *lua.LState) int {
		x, y := base.GetPosition()
		base.SetPosition(x, y+float64(L.CheckNumber(2)))
		return 0
	})

	// ── Size ─────────────────────────────────────────────────────────────────

	bind("getWidth", func(L *lua.LState) int {
		w, _ := base.GetSize()
		L.Push(lua.LNumber(w))
		return 1
	})
	bind("setWidth", func(L *lua.LState) int {
		_, h := base.GetSize()
		base.SetSize(float64(L.CheckNumber(2)), h)
		return 0
	})
	bind("getHeight", func(L *lua.LState) int {
		_, h := base.GetSize()
		L.Push(lua.LNumber(h))
		return 1
	})
	bind("setHeight", func(L *lua.LState) int {
		w, _ := base.GetSize()
		base.SetSize(w, float64(L.CheckNumber(2)))
		return 0
	})

	// ── Velocity ─────────────────────────────────────────────────────────────

	bind("getVelocityX", func(L *lua.LState) int {
		L.Push(lua.LNumber(base.GetPhysics().VelocityX))
		return 1
	})
	bind("setVelocityX", func(L *lua.LState) int {
		base.GetPhysics().VelocityX = float64(L.CheckNumber(2))
		return 0
	})
	bind("getVelocityY", func(L *lua.LState) int {
		L.Push(lua.LNumber(base.GetPhysics().VelocityY))
		return 1
	})
	bind("setVelocityY", func(L *lua.LState) int {
		base.GetPhysics().VelocityY = float64(L.CheckNumber(2))
		return 0
	})

	// ── Identity ──────────────────────────────────────────────────────────────

	bind("getLabel", func(L *lua.LState) int {
		L.Push(lua.LString(base.GetLabel()))
		return 1
	})
	bind("setLabel", func(L *lua.LState) int {
		base.SetLabel(L.CheckString(2))
		return 0
	})

	// ── Physics type ──────────────────────────────────────────────────────────

	bind("getPhysicsType", func(L *lua.LState) int {
		L.Push(lua.LString(base.GetPhysicsType()))
		return 1
	})
	bind("setPhysicsType", func(L *lua.LState) int {
		base.SetPhysicsType(L.CheckString(2))
		return 0
	})
	bind("isGrounded", func(L *lua.LState) int {
		if base.IsGrounded() {
			L.Push(lua.LTrue)
		} else {
			L.Push(lua.LFalse)
		}
		return 1
	})
	bind("isOnCeiling", func(L *lua.LState) int {
		if base.IsOnCeiling() {
			L.Push(lua.LTrue)
		} else {
			L.Push(lua.LFalse)
		}
		return 1
	})
	bind("isOnWallLeft", func(L *lua.LState) int {
		if base.IsOnWallLeft() {
			L.Push(lua.LTrue)
		} else {
			L.Push(lua.LFalse)
		}
		return 1
	})
	bind("isOnWallRight", func(L *lua.LState) int {
		if base.IsOnWallRight() {
			L.Push(lua.LTrue)
		} else {
			L.Push(lua.LFalse)
		}
		return 1
	})

	// ── Custom properties ─────────────────────────────────────────────────────
	// Properties are stored in a plain Lua table on the self table itself,
	// so they persist across frames and callbacks without touching Go memory.

	props := L.NewTable()
	t.RawSetString("_props", props)

	bind("getProperty", func(L *lua.LState) int {
		key := L.CheckString(2)
		L.Push(props.RawGetString(key))
		return 1
	})
	bind("setProperty", func(L *lua.LState) int {
		key := L.CheckString(2)
		val := L.CheckAny(3)
		props.RawSetString(key, val)
		return 0
	})

	return t
}
