package tests

import (
	"os"
	"path/filepath"
	"platformer/internal/config"
	"testing"
)

func TestParseWorldDefaultsMissingAcceleration(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "world.yaml")

	content := `world:
  name: test-world
  resolution:
    width: 320
    height: 240
  dimensions:
    width: 320
    height: 240
  fillColor:
    r: 0
    g: 0
    b: 0
  mechanics:
    gravity:
      x: 0.0
      y: 1.5
  cameras:
    - id: 0
      dimensions:
        width: 320
        height: 240
      start:
        x: 0.0
        y: 0.0
      viewport:
        x: 0.0
        y: 0.0
        width: 320.0
        height: 240.0
      speed: 1.0
      velocity: 0.0
      followVariance: 0.0
      smoothScrolling: false
      smoothScrollModifier: 0.0
`

	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatalf("failed to write world yaml: %v", err)
	}

	parser := config.NewParser()
	def, err := parser.ParseWorld(path)
	if err != nil {
		t.Fatalf("ParseWorld failed: %v", err)
	}

	if def.World.Mechanics.Acceleration != 0.05 {
		t.Fatalf("expected default acceleration 0.05 when omitted, got %.4f", def.World.Mechanics.Acceleration)
	}
}

func TestParseWorldRespectsExplicitZeroAcceleration(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "world.yaml")

	content := `world:
  name: test-world
  resolution:
    width: 320
    height: 240
  dimensions:
    width: 320
    height: 240
  fillColor:
    r: 0
    g: 0
    b: 0
  mechanics:
    gravity:
      x: 0.0
      y: 1.5
    acceleration: 0.0
  cameras:
    - id: 0
      dimensions:
        width: 320
        height: 240
      start:
        x: 0.0
        y: 0.0
      viewport:
        x: 0.0
        y: 0.0
        width: 320.0
        height: 240.0
      speed: 1.0
      velocity: 0.0
      followVariance: 0.0
      smoothScrolling: false
      smoothScrollModifier: 0.0
`

	if err := os.WriteFile(path, []byte(content), 0o644); err != nil {
		t.Fatalf("failed to write world yaml: %v", err)
	}

	parser := config.NewParser()
	def, err := parser.ParseWorld(path)
	if err != nil {
		t.Fatalf("ParseWorld failed: %v", err)
	}

	if def.World.Mechanics.Acceleration != 0.0 {
		t.Fatalf("expected explicit acceleration 0.0 to be preserved, got %.4f", def.World.Mechanics.Acceleration)
	}
}
