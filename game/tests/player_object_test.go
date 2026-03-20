package tests

import "testing"

func repeatInputFrame(frame scriptedInputFrame, count int) []scriptedInputFrame {
	frames := make([]scriptedInputFrame, 0, count)
	for i := 0; i < count; i++ {
		frames = append(frames, frame)
	}
	return frames
}

func appendInputFrames(groups ...[]scriptedInputFrame) []scriptedInputFrame {
	total := 0
	for _, group := range groups {
		total += len(group)
	}

	frames := make([]scriptedInputFrame, 0, total)
	for _, group := range groups {
		frames = append(frames, group...)
	}
	return frames
}

func TestMarioPlayerObjectRigor(t *testing.T) {
	t.Run("walks_right_and_updates_facing", func(t *testing.T) {
		w, obj := createTestPlayerObject(t, "mario-stub")
		settleScriptedPlayer(t, w, obj)

		states := runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: 1}, 4))
		if got := obj.GetMovementState(); got != "walking" {
			t.Fatalf("expected walking after moving right, got %q (states=%v)", got, states)
		}
		if obj.GetPhysics().VelocityX <= 0 {
			t.Fatalf("expected positive X velocity while walking right, got %.2f", obj.GetPhysics().VelocityX)
		}

		props := getLuaPropsTable(t, obj)
		if facing := props.RawGetString("facing").String(); facing != "right" {
			t.Fatalf("expected facing=right after moving right, got %s", facing)
		}
		if animID := obj.GetAnimationID(); animID != "mario-walk" {
			t.Fatalf("expected animation=mario-walk, got %s", animID)
		}
	})

	t.Run("jump_arc_reaches_jumping_then_falling", func(t *testing.T) {
		w, obj := createTestPlayerObject(t, "mario-stub")
		settleScriptedPlayer(t, w, obj)
		runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: 1}, 3))

		states := runScriptedInputFrames(t, w, obj, appendInputFrames(
			[]scriptedInputFrame{{move: 1, a: true, aHeld: true}},
			repeatInputFrame(scriptedInputFrame{move: 1, aHeld: true}, 2),
			repeatInputFrame(scriptedInputFrame{move: 1, aHeld: false}, 10),
		))

		jumpingIndex := firstStateIndex(states, "jumping")
		if jumpingIndex == -1 {
			t.Fatalf("expected jump sequence to reach jumping, got states=%v", states)
		}

		fallingIndex := firstStateIndex(states, "falling")
		if fallingIndex == -1 {
			t.Fatalf("expected jump sequence to later reach falling, got states=%v", states)
		}
		if fallingIndex <= jumpingIndex {
			t.Fatalf("expected falling after jumping, got states=%v", states)
		}
		if obj.GetPhysics().VelocityY <= 0 {
			t.Fatalf("expected positive Y velocity by falling phase, got %.2f", obj.GetPhysics().VelocityY)
		}
	})

	t.Run("run_mode_with_b_held", func(t *testing.T) {
		w, obj := createTestPlayerObject(t, "mario-stub")
		settleScriptedPlayer(t, w, obj)

		// Walk right first to build some velocity
		runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: 1}, 3))

		// Hold B while moving right to enter run mode
		states := runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: 1, b: true, bHeld: true}, 5))
		if got := obj.GetMovementState(); got != "running" {
			t.Fatalf("expected running with B held, got %q (states=%v)", got, states)
		}

		if animID := obj.GetAnimationID(); animID != "mario-run" {
			t.Fatalf("expected animation=mario-run during run, got %s", animID)
		}

		// Release B — should drop back to walk
		states = runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: 1}, 3))
		if got := obj.GetMovementState(); got != "walking" {
			t.Fatalf("expected walking after releasing B, got %q (states=%v)", got, states)
		}
	})

	t.Run("turns_left_and_flips_facing", func(t *testing.T) {
		w, obj := createTestPlayerObject(t, "mario-stub")
		settleScriptedPlayer(t, w, obj)

		// Walk right to build velocity
		runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: 1}, 3))

		// Press left — should enter turn state first
		states := runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: -1}, 2))
		if firstStateIndex(states, "turning") == -1 {
			t.Fatalf("expected turning state when reversing direction, got states=%v", states)
		}

		// Continue left to complete turn and start walking left
		states = runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{move: -1}, 10))
		if got := obj.GetMovementState(); got != "walking" {
			t.Fatalf("expected walking after turn completes, got %q (states=%v)", got, states)
		}
		if obj.GetPhysics().VelocityX >= 0 {
			t.Fatalf("expected negative X velocity while walking left, got %.2f", obj.GetPhysics().VelocityX)
		}

		props := getLuaPropsTable(t, obj)
		if facing := props.RawGetString("facing").String(); facing != "left" {
			t.Fatalf("expected facing=left after turn, got %s", facing)
		}
	})

	t.Run("coyote_time_allows_jump_within_window", func(t *testing.T) {
		// Player placed airborne in a world with no floor.
		// coyoteTimer is set to COYOTE_FRAMES (6) by init().
		// After 4 blank frames it has decremented to 2 (still active),
		// so pressing jump should fire via coyote grace.
		w, def := makeTestWorld(t, false)
		source := findObjectScriptDef(t, def, "mario-stub")
		obj, err := w.CreateObjectFromScript(source, 64, 50)
		if err != nil {
			t.Fatalf("CreateObjectFromScript failed: %v", err)
		}
		w.AddObject(obj)

		// 4 blank frames: coyoteTimer 6→5→4→3→2 (still active)
		runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{}, 4))

		// Jump while the coyote window is still open
		states := runScriptedInputFrames(t, w, obj, []scriptedInputFrame{{a: true, aHeld: true}})
		if firstStateIndex(states, "jumping") == -1 {
			t.Fatalf("expected coyote time to allow jump within window, got states=%v (movement=%s)",
				states, obj.GetMovementState())
		}
	})

	t.Run("coyote_time_blocks_jump_after_window_expires", func(t *testing.T) {
		// After 7 frames airborne the coyoteTimer has exhausted (0).
		// Pressing jump should only set the buffer; without a floor to land on
		// the buffer never fires, leaving the player in falling state.
		w, def := makeTestWorld(t, false)
		source := findObjectScriptDef(t, def, "mario-stub")
		obj, err := w.CreateObjectFromScript(source, 64, 50)
		if err != nil {
			t.Fatalf("CreateObjectFromScript failed: %v", err)
		}
		w.AddObject(obj)

		// 7 blank frames: coyoteTimer 6→5→4→3→2→1→0 (expired)
		runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{}, 7))

		// Press jump — coyote is gone; buffer is set but no floor to trigger it
		runScriptedInputFrames(t, w, obj, []scriptedInputFrame{{a: true, aHeld: true}})
		if got := obj.GetMovementState(); got == "jumping" {
			t.Fatalf("expected coyote to be expired (no jump), but got jumping state")
		}
	})

	t.Run("jump_buffer_fires_on_landing", func(t *testing.T) {
		// Player placed 22 px above the floor (y=170, floor resolves at y=192).
		// coyoteTimer exhausts after ~6 airborne frames; the player lands
		// around frame 11-12.  Pressing jump on frame 10 (coyote=0) only sets
		// the buffer.  When the player lands the buffer fires on the next grounded frame.
		w, def := makeTestWorld(t, true)
		source := findObjectScriptDef(t, def, "mario-stub")
		obj, err := w.CreateObjectFromScript(source, 64, 170)
		if err != nil {
			t.Fatalf("CreateObjectFromScript failed: %v", err)
		}
		w.AddObject(obj)

		// 9 blank frames: coyote expired (~frame 6), player still above floor
		runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{}, 9))

		if got := obj.GetMovementState(); got == "standing" || got == "walking" {
			t.Fatalf("expected player to still be airborne after 9 frames from y=170, got %q", got)
		}

		// coyote=0 here: press jump sets the buffer only — no immediate jump
		runScriptedInputFrames(t, w, obj, []scriptedInputFrame{{a: true}})
		if got := obj.GetMovementState(); got == "jumping" {
			t.Fatalf("expected no immediate jump (coyote expired) — buffer only, got jumping")
		}

		// Run until landing; buffer fires on first grounded frame
		states := runScriptedInputFrames(t, w, obj, repeatInputFrame(scriptedInputFrame{}, 5))
		if firstStateIndex(states, "jumping") == -1 {
			t.Fatalf("expected jump buffer to fire on landing, got states=%v (final=%s)",
				states, obj.GetMovementState())
		}
	})
}
