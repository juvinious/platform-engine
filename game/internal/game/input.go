package game

import (
	"platformer/internal/logger"
	"platformer/internal/object"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
)

// keyNames maps config key name strings to ebiten.Key constants.
// Names may appear with or without the "Key" prefix (e.g. "Z" and "KeyZ" are both valid).
var keyNames = map[string]ebiten.Key{
	"ArrowLeft": ebiten.KeyArrowLeft, "ArrowRight": ebiten.KeyArrowRight,
	"ArrowUp": ebiten.KeyArrowUp, "ArrowDown": ebiten.KeyArrowDown,
	"A": ebiten.KeyA, "KeyA": ebiten.KeyA,
	"B": ebiten.KeyB, "KeyB": ebiten.KeyB,
	"C": ebiten.KeyC, "KeyC": ebiten.KeyC,
	"D": ebiten.KeyD, "KeyD": ebiten.KeyD,
	"E": ebiten.KeyE, "KeyE": ebiten.KeyE,
	"F": ebiten.KeyF, "KeyF": ebiten.KeyF,
	"G": ebiten.KeyG, "KeyG": ebiten.KeyG,
	"H": ebiten.KeyH, "KeyH": ebiten.KeyH,
	"I": ebiten.KeyI, "KeyI": ebiten.KeyI,
	"J": ebiten.KeyJ, "KeyJ": ebiten.KeyJ,
	"K": ebiten.KeyK, "KeyK": ebiten.KeyK,
	"L": ebiten.KeyL, "KeyL": ebiten.KeyL,
	"M": ebiten.KeyM, "KeyM": ebiten.KeyM,
	"N": ebiten.KeyN, "KeyN": ebiten.KeyN,
	"O": ebiten.KeyO, "KeyO": ebiten.KeyO,
	"P": ebiten.KeyP, "KeyP": ebiten.KeyP,
	"Q": ebiten.KeyQ, "KeyQ": ebiten.KeyQ,
	"R": ebiten.KeyR, "KeyR": ebiten.KeyR,
	"S": ebiten.KeyS, "KeyS": ebiten.KeyS,
	"T": ebiten.KeyT, "KeyT": ebiten.KeyT,
	"U": ebiten.KeyU, "KeyU": ebiten.KeyU,
	"V": ebiten.KeyV, "KeyV": ebiten.KeyV,
	"W": ebiten.KeyW, "KeyW": ebiten.KeyW,
	"X": ebiten.KeyX, "KeyX": ebiten.KeyX,
	"Y": ebiten.KeyY, "KeyY": ebiten.KeyY,
	"Z": ebiten.KeyZ, "KeyZ": ebiten.KeyZ,
	"Enter": ebiten.KeyEnter, "KeyEnter": ebiten.KeyEnter,
	"Tab": ebiten.KeyTab, "KeyTab": ebiten.KeyTab,
	"Space": ebiten.KeySpace, "KeySpace": ebiten.KeySpace,
	"ShiftLeft": ebiten.KeyShiftLeft, "KeyShiftLeft": ebiten.KeyShiftLeft,
	"ShiftRight": ebiten.KeyShiftRight, "KeyShiftRight": ebiten.KeyShiftRight,
	"Escape": ebiten.KeyEscape,
}

func keyHeld(name string) bool {
	if k, ok := keyNames[name]; ok {
		return ebiten.IsKeyPressed(k)
	}
	return false
}

func keyJustPressed(name string) bool {
	if k, ok := keyNames[name]; ok {
		return inpututil.IsKeyJustPressed(k)
	}
	return false
}

// handlePlayerInput reads the config-defined P1 button bindings each frame and
// pushes standard named input properties to the scripted player object.
//
// Property names pushed (all prefixed "input."):
//
//	left, right, up, down     bool    - directional held state
//	move                      float64 - -1 / 0 / 1 derived from left/right
//	a, aHeld                  bool    - A button: just-pressed / held  (jump)
//	b, bHeld                  bool    - B button: just-pressed / held  (attack)
//	start, startHeld          bool    - Start button: just-pressed / held
//	select, selectHeld        bool    - Select button: just-pressed / held
func (g *Game) handlePlayerInput() {
	if g.player == nil {
		return
	}
	ps, ok := g.player.(object.LuaPropertySetter)
	if !ok {
		return
	}

	kb := g.inputCfg.Keyboard

	left := keyHeld(kb.Left)
	right := keyHeld(kb.Right)
	up := keyHeld(kb.Up)
	down := keyHeld(kb.Down)
	a := keyJustPressed(kb.A)
	aHeld := keyHeld(kb.A)
	b := keyJustPressed(kb.B)
	bHeld := keyHeld(kb.B)
	start := keyJustPressed(kb.Start)
	startHeld := keyHeld(kb.Start)
	sel := keyJustPressed(kb.Select)
	selHeld := keyHeld(kb.Select)

	moveAxis := 0.0
	if left && !right {
		moveAxis = -1
	} else if right && !left {
		moveAxis = 1
	}

	ps.SetLuaProperty("input.left", left)
	ps.SetLuaProperty("input.right", right)
	ps.SetLuaProperty("input.up", up)
	ps.SetLuaProperty("input.down", down)
	ps.SetLuaProperty("input.move", moveAxis)
	ps.SetLuaProperty("input.a", a)
	ps.SetLuaProperty("input.aHeld", aHeld)
	ps.SetLuaProperty("input.b", b)
	ps.SetLuaProperty("input.bHeld", bHeld)
	ps.SetLuaProperty("input.start", start)
	ps.SetLuaProperty("input.startHeld", startHeld)
	ps.SetLuaProperty("input.select", sel)
	ps.SetLuaProperty("input.selectHeld", selHeld)

	if a {
		logger.Debug("input.a propagated to scripted player")
	}
}
