package tests

import (
	"testing"

	"platformer/internal/assets"
	"platformer/internal/sprite"
)

// TestSpriteCreation tests creating a sprite from animation definition
func TestSpriteCreation(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateBlockAnimationDef(t)

	spr, err := sprite.NewSprite(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite: %v", err)
	}

	if spr == nil {
		t.Fatal("sprite should not be nil")
	}

	t.Log("Sprite created successfully")
}

// TestSpriteImageLoading tests that sprite loads all images
func TestSpriteImageLoading(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateGoombaAnimationDef(t)

	spr, err := sprite.NewSprite(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite: %v", err)
	}

	imageCount := spr.GetImageCount()
	if imageCount != len(def.Images) {
		t.Errorf("expected %d images, got %d", len(def.Images), imageCount)
	}

	t.Logf("Sprite loaded %d images", imageCount)
}

// TestSpriteFrameIndex tests getting current frame
func TestSpriteFrameIndex(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateBlockAnimationDef(t)

	spr, err := sprite.NewSprite(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite: %v", err)
	}

	frame := spr.GetCurrentFrame()
	if frame.ImageIndex != 0 {
		t.Errorf("initial frame should be 0, got %d", frame.ImageIndex)
	}

	t.Log("Sprite frame index correct")
}

// TestSpriteUpdate tests updating sprite animation
func TestSpriteUpdate(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateGoombaAnimationDef(t)

	spr, err := sprite.NewSprite(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite: %v", err)
	}

	// Manually update and verify frame changes
	for i := 0; i < 30; i++ {
		spr.Update()
	}

	// After 30 updates (15 per frame), should loop back to frame 0
	frame := spr.GetCurrentFrame()
	if frame.ImageIndex != 0 {
		t.Errorf("after loop, should be at frame 0, got %d", frame.ImageIndex)
	}

	t.Log("Sprite update works correctly")
}

// TestSpriteSizeRetrieval tests getting sprite dimensions
func TestSpriteSizeRetrieval(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateBlockAnimationDef(t)

	spr, err := sprite.NewSprite(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite: %v", err)
	}

	width, height := spr.GetSize()
	if width <= 0 || height <= 0 {
		t.Fatalf("invalid sprite size: %d x %d", width, height)
	}

	t.Logf("Sprite dimensions: %d x %d", width, height)
}

// TestSpriteReset tests resetting sprite animation
func TestSpriteReset(t *testing.T) {
	am := assets.NewAssetManager()
	def := CreateGoombaAnimationDef(t)

	spr, err := sprite.NewSprite(def, am)
	if err != nil {
		t.Fatalf("failed to create sprite: %v", err)
	}

	// Advance the sprite
	for i := 0; i < 20; i++ {
		spr.Update()
	}

	// Should be at frame 1
	frameB := spr.GetCurrentFrame()
	if frameB.ImageIndex == 0 {
		t.Fatal("sprite should have advanced")
	}

	// Reset
	spr.Reset()

	// Should be back at frame 0
	frameA := spr.GetCurrentFrame()
	if frameA.ImageIndex != 0 {
		t.Errorf("after reset, should be at frame 0, got %d", frameA.ImageIndex)
	}

	t.Log("Sprite reset works correctly")
}

// TestMultipleSprites tests creating multiple sprite instances
func TestMultipleSprites(t *testing.T) {
	am := assets.NewAssetManager()

	block := CreateBlockAnimationDef(t)
	goomba := CreateGoombaAnimationDef(t)

	sprBlock, err := sprite.NewSprite(block, am)
	if err != nil {
		t.Fatalf("failed to create block sprite: %v", err)
	}

	sprGoomba, err := sprite.NewSprite(goomba, am)
	if err != nil {
		t.Fatalf("failed to create goomba sprite: %v", err)
	}

	if sprBlock == nil || sprGoomba == nil {
		t.Fatal("sprites should not be nil")
	}

	// Check image counts
	if sprBlock.GetImageCount() != 1 {
		t.Errorf("block should have 1 image, got %d", sprBlock.GetImageCount())
	}

	if sprGoomba.GetImageCount() != 2 {
		t.Errorf("goomba should have 2 images, got %d", sprGoomba.GetImageCount())
	}

	t.Log("Multiple sprite instances created correctly")
}
