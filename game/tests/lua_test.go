package tests

import (
	"os"
	"path/filepath"
	"platformer/internal/config"
	"platformer/internal/object"
	"platformer/internal/script"
	"platformer/internal/world"
	"testing"

	lua "github.com/yuin/gopher-lua"
)

func loadBehaviorScript(t *testing.T, name string) (*script.ScriptManager, *lua.LTable) {
	t.Helper()

	sm := script.NewScriptManager()
	scriptPath := filepath.Join(GetSMBDir(t), "behaviors", name+".lua")
	if _, err := os.Stat(scriptPath); err != nil {
		sm.Close()
		t.Skipf("Behavior script not found at %s", scriptPath)
	}

	scriptTable, err := sm.LoadScript(scriptPath)
	if err != nil {
		sm.Close()
		t.Fatalf("Failed to load script: %v", err)
	}

	return sm, scriptTable
}

func callScriptFunc(t *testing.T, sm *script.ScriptManager, scriptTable, selfTable *lua.LTable, name string, args ...lua.LValue) {
	t.Helper()

	fn := scriptTable.RawGetString(name)
	if fn.Type() == lua.LTNil {
		return
	}
	if fn.Type() != lua.LTFunction {
		t.Fatalf("script field '%s' is not a function", name)
	}

	params := []lua.LValue{selfTable}
	params = append(params, args...)
	if err := sm.L.CallByParam(lua.P{Fn: fn, NRet: 0, Protect: true}, params...); err != nil {
		t.Fatalf("error calling %s(): %v", name, err)
	}
}

func sideTable(L *lua.LState, side map[string]bool) *lua.LTable {
	tbl := L.NewTable()
	for k, v := range side {
		if v {
			tbl.RawSetString(k, lua.LTrue)
		} else {
			tbl.RawSetString(k, lua.LFalse)
		}
	}
	return tbl
}

func TestLuaScriptLoading(t *testing.T) {
	sm, scriptTable := loadBehaviorScript(t, "goomba")
	defer sm.Close()

	if scriptTable == nil {
		t.Fatalf("script table is nil")
	}

	for _, fnName := range []string{"init", "act", "onMapCollision", "onObjectCollision"} {
		fn := scriptTable.RawGetString(fnName)
		if fn.Type() == lua.LTNil {
			t.Errorf("script missing %s function", fnName)
		}
	}
}

func TestLuaSelfTableMethods(t *testing.T) {
	sm := script.NewScriptManager()
	defer sm.Close()

	base := object.NewBaseObject()
	selfTable := sm.NewSelfTable(base)

	methods := []string{
		"getX", "setX", "addX",
		"getY", "setY", "addY",
		"getWidth", "setWidth",
		"getHeight", "setHeight",
		"getVelocityX", "setVelocityX",
		"getVelocityY", "setVelocityY",
		"getLabel", "setLabel",
		"getPhysicsType", "setPhysicsType",
		"isGrounded", "isOnCeiling", "isOnWallLeft", "isOnWallRight",
		"getProperty", "setProperty",
	}

	for _, method := range methods {
		fn := selfTable.RawGetString(method)
		if fn.Type() == lua.LTNil {
			t.Errorf("self table missing method: %s", method)
		}
	}
}

func TestLuaPositionVelocityAndProperties(t *testing.T) {
	sm := script.NewScriptManager()
	defer sm.Close()

	base := object.NewBaseObject()
	base.SetPosition(100, 200)
	selfTable := sm.NewSelfTable(base)
	sm.L.SetGlobal("obj", selfTable)

	err := sm.L.DoString(`
obj:setX(150)
obj:addX(25)
obj:setVelocityX(5.0)
obj:setVelocityY(-10.0)
obj:setProperty("direction", -1)
obj:setProperty("speed", 0.4)
`)
	if err != nil {
		t.Fatalf("Lua error: %v", err)
	}

	x, _ := base.GetPosition()
	if x != 175 {
		t.Errorf("expected X=175, got %v", x)
	}

	if base.GetPhysics().VelocityX != 5.0 {
		t.Errorf("expected VelocityX=5.0, got %v", base.GetPhysics().VelocityX)
	}
	if base.GetPhysics().VelocityY != -10.0 {
		t.Errorf("expected VelocityY=-10.0, got %v", base.GetPhysics().VelocityY)
	}

	propsVal := selfTable.RawGetString("_props")
	props, ok := propsVal.(*lua.LTable)
	if !ok {
		t.Fatalf("expected _props table on self table")
	}
	if dir := props.RawGetString("direction"); dir != lua.LNumber(-1) {
		t.Errorf("expected direction=-1, got %v", dir)
	}
	if speed := props.RawGetString("speed"); speed != lua.LNumber(0.4) {
		t.Errorf("expected speed=0.4, got %v", speed)
	}
}

func TestLuaContactStateAccessors(t *testing.T) {
	sm := script.NewScriptManager()
	defer sm.Close()

	base := object.NewBaseObject()
	base.MergeContactStateFromSides(map[string]bool{
		"top":    true,
		"bottom": false,
		"left":   true,
		"right":  false,
	})

	selfTable := sm.NewSelfTable(base)
	sm.L.SetGlobal("obj", selfTable)

	err := sm.L.DoString(`
if not obj:isGrounded() then error("expected grounded") end
if obj:isOnCeiling() then error("did not expect ceiling") end
if not obj:isOnWallLeft() then error("expected wall-left") end
if obj:isOnWallRight() then error("did not expect wall-right") end
`)
	if err != nil {
		t.Fatalf("Lua error: %v", err)
	}
}

func TestGoombaInitAndAct(t *testing.T) {
	sm, scriptTable := loadBehaviorScript(t, "goomba")
	defer sm.Close()

	base := object.NewBaseObject()
	selfTable := sm.NewSelfTable(base)

	callScriptFunc(t, sm, scriptTable, selfTable, "init")

	if base.GetLabel() != "goomba" {
		t.Errorf("init() did not set label, got %q", base.GetLabel())
	}

	w, h := base.GetSize()
	if w != 16 || h != 16 {
		t.Errorf("init() did not set size to 16x16, got %.0fx%.0f", w, h)
	}

	callScriptFunc(t, sm, scriptTable, selfTable, "act")

	if base.GetPhysics().VelocityX != -0.4 {
		t.Errorf("expected velocityX -0.4 after act(), got %.2f", base.GetPhysics().VelocityX)
	}
}

func TestGoombaMapCollisionTurnsDirection(t *testing.T) {
	sm, scriptTable := loadBehaviorScript(t, "goomba")
	defer sm.Close()

	base := object.NewBaseObject()
	selfTable := sm.NewSelfTable(base)
	callScriptFunc(t, sm, scriptTable, selfTable, "init")

	callScriptFunc(t, sm, scriptTable, selfTable, "onMapCollision", sideTable(sm.L, map[string]bool{"left": true}))
	callScriptFunc(t, sm, scriptTable, selfTable, "act")

	if base.GetPhysics().VelocityX != 0.4 {
		t.Errorf("expected velocityX 0.4 after left wall collision, got %.2f", base.GetPhysics().VelocityX)
	}
}

func TestBlockBehaviorInitAndHit(t *testing.T) {
	sm, scriptTable := loadBehaviorScript(t, "block")
	defer sm.Close()

	base := object.NewBaseObject()
	base.SetPosition(0, 100)
	selfTable := sm.NewSelfTable(base)

	callScriptFunc(t, sm, scriptTable, selfTable, "init")

	if base.GetLabel() != "block" {
		t.Errorf("block init() label mismatch, got %q", base.GetLabel())
	}

	props, ok := selfTable.RawGetString("_props").(*lua.LTable)
	if !ok {
		t.Fatalf("expected _props table")
	}
	if state := props.RawGetString("state"); state != lua.LString("rest") {
		t.Errorf("expected initial state 'rest', got %v", state)
	}

	callScriptFunc(t, sm, scriptTable, selfTable, "onObjectCollision", lua.LNil, sideTable(sm.L, map[string]bool{"bottom": true}))

	if base.GetPhysics().VelocityY != -0.5 {
		t.Errorf("expected bounce velocity -0.5 after bottom hit, got %.2f", base.GetPhysics().VelocityY)
	}
	if state := props.RawGetString("state"); state != lua.LString("hit") {
		t.Errorf("expected state 'hit' after bottom hit, got %v", state)
	}
}

func loadSMBWorldDef(t *testing.T) *config.WorldDef {
	t.Helper()

	parser := config.NewParser()
	worldPath := filepath.Join(GetSMBDir(t), "worlds", "world-1-1.yaml")

	def, err := parser.ParseWorld(worldPath)
	if err != nil {
		t.Fatalf("failed to parse world YAML: %v", err)
	}

	return def
}

func makeTestWorld(t *testing.T, includeCollisionMap bool) (*world.World, *config.WorldDef) {
	t.Helper()

	def := loadSMBWorldDef(t)
	def.World.Objects = nil
	def.World.Backgrounds = nil
	def.World.Foregrounds = nil
	if !includeCollisionMap {
		def.World.CollisionMap.Areas = nil
	}

	dataDir := filepath.Dir(filepath.Dir(GetSMBDir(t)))
	w, err := world.NewWorldFromDef(def, dataDir)
	if err != nil {
		t.Fatalf("failed to build world: %v", err)
	}

	return w, def
}

func findObjectScriptDef(t *testing.T, def *config.WorldDef, id string) config.ObjectScriptDef {
	t.Helper()

	for i := range def.World.ObjectScripts {
		if def.World.ObjectScripts[i].ID == id {
			return def.World.ObjectScripts[i]
		}
	}

	t.Fatalf("object script definition %q not found in world config", id)
	return config.ObjectScriptDef{}
}

func getLuaPropsTable(t *testing.T, obj object.Object) *lua.LTable {
	t.Helper()

	so, ok := obj.(*object.ScriptedObject)
	if !ok {
		t.Fatalf("expected scripted object, got %T", obj)
	}

	props, ok := so.GetSelfTable().RawGetString("_props").(*lua.LTable)
	if !ok {
		t.Fatalf("expected _props table on scripted object")
	}

	return props
}

type mechanicsStub struct {
	gravityX     float64
	gravityY     float64
	acceleration float64
}

func (m mechanicsStub) GetGravityVector() (float64, float64) {
	return m.gravityX, m.gravityY
}

func (m mechanicsStub) GetAcceleration() float64 {
	return m.acceleration
}

func TestCreateObjectFromScriptReturnsScriptedObject(t *testing.T) {
	w, def := makeTestWorld(t, true)

	goombaDef := findObjectScriptDef(t, def, "goomba")
	obj, err := w.CreateObjectFromScript(goombaDef, 64, 96)
	if err != nil {
		t.Fatalf("CreateObjectFromScript failed: %v", err)
	}

	if _, ok := obj.(*object.ScriptedObject); !ok {
		t.Fatalf("expected *object.ScriptedObject, got %T", obj)
	}
}

func TestWorldMapCollisionStopsGoombaFall(t *testing.T) {
	w, def := makeTestWorld(t, true)

	goombaDef := findObjectScriptDef(t, def, "goomba")
	obj, err := w.CreateObjectFromScript(goombaDef, 64, 190)
	if err != nil {
		t.Fatalf("CreateObjectFromScript failed: %v", err)
	}
	obj.GetPhysics().VelocityY = 5
	w.AddObject(obj)

	w.Update()

	_, y := obj.GetPosition()
	if y != 192 {
		t.Fatalf("expected goomba to be resolved onto floor at y=192, got %.2f", y)
	}
	if obj.GetPhysics().VelocityY != 0 {
		t.Fatalf("expected goomba vertical velocity to be reset after floor collision, got %.2f", obj.GetPhysics().VelocityY)
	}
	if !obj.IsGrounded() {
		t.Fatalf("expected object grounded contact after floor collision")
	}
	if obj.IsOnCeiling() {
		t.Fatalf("did not expect ceiling contact after floor collision")
	}
}

func TestWorldMapCollisionStopsGoombaAtLeftBoundary(t *testing.T) {
	w, def := makeTestWorld(t, true)

	goombaDef := findObjectScriptDef(t, def, "goomba")
	obj, err := w.CreateObjectFromScript(goombaDef, 1, 100)
	if err != nil {
		t.Fatalf("CreateObjectFromScript failed: %v", err)
	}
	obj.GetPhysics().VelocityX = -5
	w.AddObject(obj)

	w.Update()

	x, _ := obj.GetPosition()
	if x != 0 {
		t.Fatalf("expected goomba to be resolved at left boundary x=0, got %.2f", x)
	}
	if obj.GetPhysics().VelocityX != 0 {
		t.Fatalf("expected goomba horizontal velocity to be reset after wall collision, got %.2f", obj.GetPhysics().VelocityX)
	}
	if !obj.IsOnWallRight() {
		t.Fatalf("expected right-wall contact after moving into left boundary")
	}
}

func TestWorldObjectCollisionTurnsGoombaAround(t *testing.T) {
	w, def := makeTestWorld(t, false)

	goombaDef := findObjectScriptDef(t, def, "goomba")
	brickDef := findObjectScriptDef(t, def, "brick")

	goombaObj, err := w.CreateObjectFromScript(goombaDef, 100, 100)
	if err != nil {
		t.Fatalf("failed to create goomba: %v", err)
	}
	props := getLuaPropsTable(t, goombaObj)
	props.RawSetString("direction", lua.LNumber(1))
	goombaObj.GetPhysics().VelocityX = 20

	brickObj, err := w.CreateObjectFromScript(brickDef, 116, 100)
	if err != nil {
		t.Fatalf("failed to create brick: %v", err)
	}

	w.AddObject(goombaObj)
	w.AddObject(brickObj)
	w.Update()

	if direction := props.RawGetString("direction"); direction != lua.LNumber(-1) {
		t.Fatalf("expected goomba direction to flip after brick collision, got %v", direction)
	}

	x, _ := goombaObj.GetPosition()
	if x > 100 {
		t.Fatalf("expected goomba to be resolved out of the brick, got x=%.2f", x)
	}
}

func TestWorldDynamicVsDynamicResolvesEqualSplit(t *testing.T) {
	// Two 16x16 dynamic objects approaching each other.
	// Starts: objA right edge = 88+16 = 104, objB left edge = 106 -> 2px gap.
	// After one frame with VelocityX +-5, they overlap by 8px.
	// Uses plain BaseObjects (no Lua act) so the velocity set here is the one integrated.
	w, _ := makeTestWorld(t, false)

	objA := object.NewBaseObject()
	objA.SetPosition(88, 100)
	objA.SetSize(16, 16)
	objA.GetCollision().AddBox(0, 0, 16, 16)
	objA.GetPhysics().VelocityX = 5

	objB := object.NewBaseObject()
	objB.SetPosition(106, 100)
	objB.SetSize(16, 16)
	objB.GetCollision().AddBox(0, 0, 16, 16)
	objB.GetPhysics().VelocityX = -5

	w.AddObject(objA)
	w.AddObject(objB)
	w.Update()

	ax, _ := objA.GetPosition()
	bx, _ := objB.GetPosition()

	// After resolution A must have no overlap with B (right edge of A <= left edge of B).
	if ax+16 > bx {
		t.Fatalf("expected no overlap after dynamic resolution, got ax=%.2f bx=%.2f (overlap=%.2f)", ax, bx, (ax+16)-bx)
	}

	// Both velocities must be zeroed on the contact axis.
	if objA.GetPhysics().VelocityX != 0 {
		t.Fatalf("expected objA VelocityX=0 after contact, got %.2f", objA.GetPhysics().VelocityX)
	}
	if objB.GetPhysics().VelocityX != 0 {
		t.Fatalf("expected objB VelocityX=0 after contact, got %.2f", objB.GetPhysics().VelocityX)
	}
}

func TestDynamicObjectSupportsZeroGravityWorlds(t *testing.T) {
	base := object.NewBaseObject()
	base.GetPhysics().VelocityX = 2.5
	base.GetPhysics().VelocityY = -1.0

	base.Act(mechanicsStub{})

	x, y := base.GetPosition()
	if x != 2.5 || y != -1.0 {
		t.Fatalf("expected motion with no injected gravity, got position (%.2f, %.2f)", x, y)
	}
	if base.GetPhysics().VelocityY != -1.0 {
		t.Fatalf("expected velocityY unchanged without gravity, got %.2f", base.GetPhysics().VelocityY)
	}
}
