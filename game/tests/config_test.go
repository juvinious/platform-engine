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
    camera:
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
    camera:
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

func TestParseWorldDefaultsTypeProfileAndAllowedStates(t *testing.T) {
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
    camera:
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

	if def.World.Type != "platformer" {
		t.Fatalf("expected default world.type=platformer, got %q", def.World.Type)
	}
	if def.World.Mechanics.Profile != "platformer" {
		t.Fatalf("expected default mechanics.profile=platformer, got %q", def.World.Mechanics.Profile)
	}
	if def.World.Mechanics.DeadStateMode != "immovable" {
		t.Fatalf("expected default mechanics.deadStateMode=immovable, got %q", def.World.Mechanics.DeadStateMode)
	}
	if len(def.World.Mechanics.AllowedStates.Movement) == 0 {
		t.Fatalf("expected default mechanics.allowedStates.movement to be populated")
	}
	foundDying := false
	for _, s := range def.World.Mechanics.AllowedStates.Life {
		if s == "dying" {
			foundDying = true
			break
		}
	}
	if !foundDying {
		t.Fatalf("expected default mechanics.allowedStates.life to include dying, got %v", def.World.Mechanics.AllowedStates.Life)
	}
}

func TestParseWorldRejectsInvalidDeadStateMode(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "world.yaml")

	content := `world:
  name: test-world
  type: platformer
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
    profile: platformer
    deadStateMode: nope
    gravity:
      x: 0.0
      y: 1.5
    camera:
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
	if _, err := parser.ParseWorld(path); err == nil {
		t.Fatalf("expected ParseWorld to fail for invalid mechanics.deadStateMode")
	}
}

func TestParseWorldLoadsObjectScriptsFromExternalSource(t *testing.T) {
	dir := t.TempDir()
	worldPath := filepath.Join(dir, "world.yaml")
	libPath := filepath.Join(dir, "objects.yaml")

	libContent := `objectScripts:
  - id: shared-goomba
    module: goomba
    function: ""
    physicsType: dynamic
    animation:
      id: goomba
      basedir: platformer/smb/images/
      images: [goomba-walk-01.png]
      frames:
        - imageIndex: 0
          alpha: 255
          hflip: false
          vflip: false
          duration: 15
      loop: true
`
	if err := os.WriteFile(libPath, []byte(libContent), 0o644); err != nil {
		t.Fatalf("failed to write object library yaml: %v", err)
	}

	worldContent := `world:
  name: test-world
  type: platformer
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
    profile: platformer
    gravity:
      x: 0.0
      y: 1.5
    camera:
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
  objectScriptSources:
    - objects.yaml
`
	if err := os.WriteFile(worldPath, []byte(worldContent), 0o644); err != nil {
		t.Fatalf("failed to write world yaml: %v", err)
	}

	parser := config.NewParser()
	def, err := parser.ParseWorld(worldPath)
	if err != nil {
		t.Fatalf("ParseWorld failed: %v", err)
	}

	if len(def.World.ObjectScripts) != 1 {
		t.Fatalf("expected 1 merged object script from source, got %d", len(def.World.ObjectScripts))
	}
	if def.World.ObjectScripts[0].ID != "shared-goomba" {
		t.Fatalf("expected merged object script id shared-goomba, got %q", def.World.ObjectScripts[0].ID)
	}
}

func TestParseWorldLocalObjectScriptOverridesSourceByID(t *testing.T) {
	dir := t.TempDir()
	worldPath := filepath.Join(dir, "world.yaml")
	libPath := filepath.Join(dir, "objects.yaml")

	libContent := `objectScripts:
  - id: shared-player
    module: player
    function: ""
    physicsType: dynamic
    animation:
      id: goomba
      basedir: platformer/smb/images/
      images: [goomba-walk-01.png]
      frames:
        - imageIndex: 0
          alpha: 255
          hflip: false
          vflip: false
          duration: 15
      loop: true
`
	if err := os.WriteFile(libPath, []byte(libContent), 0o644); err != nil {
		t.Fatalf("failed to write object library yaml: %v", err)
	}

	worldContent := `world:
  name: test-world
  type: platformer
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
    profile: platformer
    gravity:
      x: 0.0
      y: 1.5
    camera:
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
  objectScriptSources:
    - objects.yaml
  objectScripts:
    - id: shared-player
      module: player-local
      function: ""
      physicsType: dynamic
      animation:
        id: goomba
        basedir: platformer/smb/images/
        images: [goomba-walk-01.png]
        frames:
          - imageIndex: 0
            alpha: 255
            hflip: false
            vflip: false
            duration: 15
        loop: true
`
	if err := os.WriteFile(worldPath, []byte(worldContent), 0o644); err != nil {
		t.Fatalf("failed to write world yaml: %v", err)
	}

	parser := config.NewParser()
	def, err := parser.ParseWorld(worldPath)
	if err != nil {
		t.Fatalf("ParseWorld failed: %v", err)
	}

	if len(def.World.ObjectScripts) != 1 {
		t.Fatalf("expected 1 merged object script after override, got %d", len(def.World.ObjectScripts))
	}
	if got := def.World.ObjectScripts[0].Module; got != "player-local" {
		t.Fatalf("expected local object script to override source module, got %q", got)
	}
}

func TestParseWorldResourceSourcesMergeAnimationsAndScripts(t *testing.T) {
	dir := t.TempDir()
	worldPath := filepath.Join(dir, "world.yaml")
	bundlePath := filepath.Join(dir, "goomba", "goomba.yaml")
	if err := os.MkdirAll(filepath.Dir(bundlePath), 0o755); err != nil {
		t.Fatalf("failed to create bundle dir: %v", err)
	}

	bundleContent := `scriptImportPaths:
  - .
animations:
  - id: goomba
    basedir: images/
    images: [goomba-walk-01.png]
    frames:
      - imageIndex: 0
        alpha: 255
        hflip: false
        vflip: false
        duration: 15
    loop: true
objectScripts:
  - id: goomba
    module: behaviors
    function: ""
    physicsType: dynamic
    animation:
      id: goomba
      basedir: images/
      images: [goomba-walk-01.png]
      frames:
        - imageIndex: 0
          alpha: 255
          hflip: false
          vflip: false
          duration: 15
      loop: true
`
	if err := os.WriteFile(bundlePath, []byte(bundleContent), 0o644); err != nil {
		t.Fatalf("failed to write resource bundle: %v", err)
	}

	worldContent := `world:
  name: test-world
  type: platformer
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
    profile: platformer
    gravity:
      x: 0.0
      y: 1.5
    camera:
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
  resourceSources:
    - goomba/goomba.yaml
`
	if err := os.WriteFile(worldPath, []byte(worldContent), 0o644); err != nil {
		t.Fatalf("failed to write world yaml: %v", err)
	}

	parser := config.NewParser()
	def, err := parser.ParseWorld(worldPath)
	if err != nil {
		t.Fatalf("ParseWorld failed: %v", err)
	}

	if len(def.World.Animations) != 1 || def.World.Animations[0].ID != "goomba" {
		t.Fatalf("expected merged animation 'goomba' from resource source, got %+v", def.World.Animations)
	}
	if len(def.World.ObjectScripts) != 1 || def.World.ObjectScripts[0].ID != "goomba" {
		t.Fatalf("expected merged object script 'goomba' from resource source, got %+v", def.World.ObjectScripts)
	}
	if len(def.World.ScriptImportPaths) == 0 || !filepath.IsAbs(def.World.ScriptImportPaths[0]) {
		t.Fatalf("expected source-relative scriptImportPaths to be normalized to absolute path, got %v", def.World.ScriptImportPaths)
	}
}

func TestParseWorldResourceSourcesAllowWorldOverrideByID(t *testing.T) {
	dir := t.TempDir()
	worldPath := filepath.Join(dir, "world.yaml")
	bundlePath := filepath.Join(dir, "bundle.yaml")

	bundleContent := `objectScripts:
  - id: player
    module: player
    function: ""
    physicsType: dynamic
    animation:
      id: goomba
      basedir: platformer/smb/images/
      images: [goomba-walk-01.png]
      frames:
        - imageIndex: 0
          alpha: 255
          hflip: false
          vflip: false
          duration: 15
      loop: true
`
	if err := os.WriteFile(bundlePath, []byte(bundleContent), 0o644); err != nil {
		t.Fatalf("failed to write resource bundle: %v", err)
	}

	worldContent := `world:
  name: test-world
  type: platformer
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
    profile: platformer
    gravity:
      x: 0.0
      y: 1.5
    camera:
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
  resourceSources:
    - bundle.yaml
  objectScripts:
    - id: player
      module: player_override
      function: ""
      physicsType: dynamic
      animation:
        id: goomba
        basedir: platformer/smb/images/
        images: [goomba-walk-01.png]
        frames:
          - imageIndex: 0
            alpha: 255
            hflip: false
            vflip: false
            duration: 15
        loop: true
`
	if err := os.WriteFile(worldPath, []byte(worldContent), 0o644); err != nil {
		t.Fatalf("failed to write world yaml: %v", err)
	}

	parser := config.NewParser()
	def, err := parser.ParseWorld(worldPath)
	if err != nil {
		t.Fatalf("ParseWorld failed: %v", err)
	}

	if len(def.World.ObjectScripts) != 1 {
		t.Fatalf("expected single merged player object script, got %d", len(def.World.ObjectScripts))
	}
	if got := def.World.ObjectScripts[0].Module; got != "player_override" {
		t.Fatalf("expected world objectScripts override to win, got module=%q", got)
	}
}
