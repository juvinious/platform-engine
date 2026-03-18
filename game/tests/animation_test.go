package tests

import (
	"testing"

	"platformer/internal/animation"
	"platformer/internal/assets"
)

// TestSpriteAnimationCreation tests creating a sprite animation from config
func TestSpriteAnimationCreation(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateBlockAnimationDef(t)

	anim, err := animation.NewSpriteAnimation(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite animation: %v", err)
	}

	if anim == nil {
		t.Fatal("sprite animation is nil")
	}

	t.Log("Sprite animation created successfully")
}

// TestAnimationFrameSequencing tests that animation frames advance correctly
func TestAnimationFrameSequencing(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateGoombaAnimationDef(t)

	anim, err := animation.NewSpriteAnimation(def, am)
	if err != nil {
		t.Fatalf("failed to create animation: %v", err)
	}

	// Get initial frame
	frame0 := anim.GetCurrentFrame()
	if frame0.ImageIndex != 0 {
		t.Fatalf("initial frame should be 0, got %d", frame0.ImageIndex)
	}

	// Advance through frame 0 (14 updates = still on frame 0)
	for i := 0; i < 14; i++ {
		anim.Update()
	}

	// Should still be on frame 0 after 14 updates
	frameAfter14 := anim.GetCurrentFrame()
	if frameAfter14.ImageIndex != 0 {
		t.Errorf("after 14 updates, should still be frame 0, got %d", frameAfter14.ImageIndex)
	}

	// One more update should advance to frame 1
	anim.Update()
	frame1 := anim.GetCurrentFrame()
	if frame1.ImageIndex != 1 {
		t.Errorf("after 15 updates, should be frame 1, got %d", frame1.ImageIndex)
	}

	t.Log("Frame sequencing works correctly")
}

// TestAnimationLoop tests that animation loops when duration expires
func TestAnimationLoop(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateGoombaAnimationDef(t)

	anim, err := animation.NewSpriteAnimation(def, am)
	if err != nil {
		t.Fatalf("failed to create animation: %v", err)
	}

	// Goomba has 2 frames, each 15 ticks = 30 ticks total
	// Update 31 times to go through full cycle + 1
	for i := 0; i < 31; i++ {
		anim.Update()
	}

	// Should have looped back to frame 0
	loopedFrame := anim.GetCurrentFrame()
	if loopedFrame.ImageIndex != 0 {
		t.Errorf("after looping, should be back at frame 0, got %d", loopedFrame.ImageIndex)
	}

	t.Log("Animation looping works correctly")
}

// TestAnimationReset tests resetting animation to start
func TestAnimationReset(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateGoombaAnimationDef(t)

	anim, err := animation.NewSpriteAnimation(def, am)
	if err != nil {
		t.Fatalf("failed to create animation: %v", err)
	}

	// Advance several updates
	for i := 0; i < 20; i++ {
		anim.Update()
	}

	frame := anim.GetCurrentFrame()
	if frame.ImageIndex == 0 {
		t.Fatal("sanity check: should have advanced past frame 0")
	}

	// Reset
	anim.Reset()

	resetFrame := anim.GetCurrentFrame()
	if resetFrame.ImageIndex != 0 {
		t.Errorf("after reset, should be at frame 0, got %d", resetFrame.ImageIndex)
	}

	t.Log("Animation reset works correctly")
}

// TestStaticAnimationDuration tests animations with duration -1 (infinite/static)
func TestStaticAnimationDuration(t *testing.T) {
	am := assets.NewAssetManager()
	staticDef := CreateBlockAnimationDef(t)

	if staticDef.Frames[0].Duration != -1 {
		t.Fatal("test block animation should have duration -1")
	}

	anim, err := animation.NewSpriteAnimation(staticDef, am)
	if err != nil {
		t.Fatalf("failed to create static animation: %v", err)
	}

	// Update many times
	for i := 0; i < 100; i++ {
		anim.Update()
	}

	// Should still be on frame 0 (static)
	frame := anim.GetCurrentFrame()
	if frame.ImageIndex != 0 {
		t.Errorf("static animation should stay on frame 0, got %d after 100 updates", frame.ImageIndex)
	}

	t.Log("Static animation duration works correctly")
}

// TestAnimationSizeRetrieval tests getting sprite dimensions
func TestAnimationSizeRetrieval(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateBlockAnimationDef(t)

	anim, err := animation.NewSpriteAnimation(def, am)
	if err != nil {
		t.Fatalf("failed to create animation: %v", err)
	}

	width, height := anim.GetSize()
	if width <= 0 || height <= 0 {
		t.Fatalf("invalid sprite size: %d x %d", width, height)
	}

	t.Logf("Sprite size: %d x %d", width, height)
}

// TestMultipleAnimations tests creating multiple different animations
func TestMultipleAnimations(t *testing.T) {
	am := assets.NewAssetManager()

	block := CreateBlockAnimationDef(t)
	goomba := CreateGoombaAnimationDef(t)

	blockAnim, err := animation.NewSpriteAnimation(block, am)
	if err != nil {
		t.Fatalf("failed to create block animation: %v", err)
	}

	goombaAnim, err := animation.NewSpriteAnimation(goomba, am)
	if err != nil {
		t.Fatalf("failed to create goomba animation: %v", err)
	}

	if blockAnim == nil || goombaAnim == nil {
		t.Fatal("animations should not be nil")
	}

	// Both should start at frame 0
	if blockAnim.GetCurrentFrame().ImageIndex != 0 || goombaAnim.GetCurrentFrame().ImageIndex != 0 {
		t.Fatal("both animations should start at frame 0")
	}

	t.Log("Multiple animations created and initialized correctly")
}
