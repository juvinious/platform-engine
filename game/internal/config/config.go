package config

import (
	"fmt"
	"os"
	"path/filepath"

	"gopkg.in/yaml.v3"
)

// --- Color Component ---
type Color struct {
	R int `yaml:"r"`
	G int `yaml:"g"`
	B int `yaml:"b"`
}

// --- Vector Components ---
type Vec2 struct {
	X float64 `yaml:"x"`
	Y float64 `yaml:"y"`
}

type IntVec2 struct {
	Width  int `yaml:"width"`
	Height int `yaml:"height"`
}

type Size struct {
	Width  float64 `yaml:"width"`
	Height float64 `yaml:"height"`
}

type Vec4 struct {
	X      float64 `yaml:"x"`
	Y      float64 `yaml:"y"`
	Width  float64 `yaml:"width"`
	Height float64 `yaml:"height"`
}

type Rect struct {
	X      float64 `yaml:"x"`
	Y      float64 `yaml:"y"`
	Width  float64 `yaml:"width"`
	Height float64 `yaml:"height"`
}

// --- Animation Structures ---
type CollisionBoxDef struct {
	ID     string  `yaml:"id"`
	X      float64 `yaml:"x"`
	Y      float64 `yaml:"y"`
	Width  float64 `yaml:"width"`
	Height float64 `yaml:"height"`
	Color  string  `yaml:"color,omitempty"`
}

type FrameDef struct {
	ImageIndex int               `yaml:"imageIndex"`
	Alpha      int               `yaml:"alpha"`
	HFlip      bool              `yaml:"hflip"`
	VFlip      bool              `yaml:"vflip"`
	Duration   int               `yaml:"duration"`
	Collisions []CollisionBoxDef `yaml:"collisions,omitempty"`
}

type AnimationDef struct {
	ID        string     `yaml:"id"`
	BaseDir   string     `yaml:"basedir"`
	Images    []string   `yaml:"images"`
	Frames    []FrameDef `yaml:"frames"`
	Loop      bool       `yaml:"loop"`
	SourceDir string     `yaml:"-"`
}

type AllowedStatesDef struct {
	Movement []string `yaml:"movement,omitempty"`
	Life     []string `yaml:"life,omitempty"`
	Status   []string `yaml:"status,omitempty"`
}

// --- Physics ---
type MechanicsDef struct {
	Gravity       Vec2             `yaml:"gravity"`
	Acceleration  float64          `yaml:"acceleration"`
	Profile       string           `yaml:"profile,omitempty"`
	DeadStateMode string           `yaml:"deadStateMode,omitempty"`
	AllowedStates AllowedStatesDef `yaml:"allowedStates,omitempty"`
	Camera        CameraDef        `yaml:"camera"`
}

// --- Camera ---
type CameraDef struct {
	ID                   int     `yaml:"id"`
	Dimensions           IntVec2 `yaml:"dimensions"`
	Start                Vec2    `yaml:"start"`
	Viewport             Rect    `yaml:"viewport"`
	Speed                float64 `yaml:"speed"`
	Velocity             float64 `yaml:"velocity"`
	FollowVariance       float64 `yaml:"followVariance"`
	SmoothScrolling      bool    `yaml:"smoothScrolling"`
	SmoothScrollModifier float64 `yaml:"smoothScrollModifier"`
}

// --- Objects ---
type ObjectDef struct {
	Script   string `yaml:"script"`
	Position Vec2   `yaml:"position"`
}

type ObjectScriptDef struct {
	ID            string         `yaml:"id"`
	Dimensions    Size           `yaml:"dimensions"`
	Module        string         `yaml:"module"`
	ScriptFile    string         `yaml:"scriptFile,omitempty"`
	Function      string         `yaml:"function"`
	Image         string         `yaml:"image"`
	AnimationFile string         `yaml:"animationFile,omitempty"`
	Animation     AnimationDef   `yaml:"animation"`
	Animations    []AnimationDef `yaml:"animations,omitempty"`
	Color         Color          `yaml:"color"`
	PhysicsType   string         `yaml:"physicsType,omitempty"`
	SourceDir     string         `yaml:"-"`
}

// --- Background/Foreground Layers ---
type TileDef struct {
	Animation string `yaml:"animation"`
	Position  Vec2   `yaml:"position"`
}

type TilesetDef struct {
	TileSize   Vec2      `yaml:"tileSize"`
	Dimensions Vec2      `yaml:"dimensions"`
	Tiles      []TileDef `yaml:"tiles"`
}

type BackgroundDef struct {
	ID        string     `yaml:"id"`
	Type      string     `yaml:"type"`
	Animation string     `yaml:"animation,omitempty"`
	ScrollX   float64    `yaml:"scrollX"`
	ScrollY   float64    `yaml:"scrollY"`
	Tileset   TilesetDef `yaml:"tileset"`
}

type ForegroundDef struct {
	ID        string     `yaml:"id"`
	Type      string     `yaml:"type"`
	Animation string     `yaml:"animation,omitempty"`
	ScrollX   float64    `yaml:"scrollX"`
	ScrollY   float64    `yaml:"scrollY"`
	Tileset   TilesetDef `yaml:"tileset"`
}

// --- Collision Map ---
type CollisionAreaDef struct {
	ID       string `yaml:"id"`
	Position Vec4   `yaml:"position"` // x, y, width, height
	Color    string `yaml:"color,omitempty"`
}

type CollisionMapDef struct {
	Areas []CollisionAreaDef `yaml:"areas"`
}

type ObjectScriptLibraryDef struct {
	ObjectScripts []ObjectScriptDef `yaml:"objectScripts,omitempty"`
}

type ResourceBundleDef struct {
	ScriptImportPaths []string          `yaml:"scriptImportPaths,omitempty"`
	Animations        []AnimationDef    `yaml:"animations,omitempty"`
	ObjectScripts     []ObjectScriptDef `yaml:"objectScripts,omitempty"`
}

// --- World Definition ---
type WorldDef struct {
	World struct {
		Name                string            `yaml:"name"`
		Type                string            `yaml:"type,omitempty"`
		Version             int               `yaml:"version"`
		Resolution          IntVec2           `yaml:"resolution"`
		Dimensions          IntVec2           `yaml:"dimensions"`
		FillColor           Color             `yaml:"fillColor"`
		Mechanics           MechanicsDef      `yaml:"mechanics"`
		Animations          []AnimationDef    `yaml:"animations"`
		ScriptImportPaths   []string          `yaml:"scriptImportPaths,omitempty"`
		ResourceSources     []string          `yaml:"resourceSources,omitempty"`
		ObjectScriptSources []string          `yaml:"objectScriptSources,omitempty"`
		ObjectScripts       []ObjectScriptDef `yaml:"objectScripts,omitempty"`
		Backgrounds         []BackgroundDef   `yaml:"backgrounds,omitempty"`
		Foregrounds         []ForegroundDef   `yaml:"foregrounds,omitempty"`
		CollisionMap        CollisionMapDef   `yaml:"collisionMap,omitempty"`
		Objects             []ObjectDef       `yaml:"objects,omitempty"`
	} `yaml:"world"`
}

// --- Game Definition ---
type FontDef struct {
	Name         string `yaml:"name"`
	Filename     string `yaml:"filename"`
	CharacterMap string `yaml:"characterMap"`
}

type DataConfig struct {
	BaseDir      string `yaml:"base-dir"`
	DefaultWorld string `yaml:"default-world"`
}

type SettingsConfig struct {
	Gravity      float64 `yaml:"gravity"`
	MaxFallSpeed float64 `yaml:"maxFallSpeed"`
	TileSize     int     `yaml:"tileSize"`
	Debug        bool    `yaml:"debug"`
	Quiet        bool    `yaml:"quiet,omitempty"`
	DebugLevel   int     `yaml:"debugLevel,omitempty"`
	LogFile      string  `yaml:"logFile,omitempty"`
	ErrorLog     string  `yaml:"errorLog,omitempty"`
}

type ResolutionConfig struct {
	Width  int `yaml:"width"`
	Height int `yaml:"height"`
}

type VideoConfig struct {
	Resolution ResolutionConfig `yaml:"resolution"`
	Fullscreen bool             `yaml:"fullscreen"`
	VSync      bool             `yaml:"vsync"`
	Scale      float64          `yaml:"scale"`
}

type KeyboardConfig struct {
	Left   string `yaml:"left"`
	Right  string `yaml:"right"`
	Up     string `yaml:"up"`
	Down   string `yaml:"down"`
	A      string `yaml:"a"`
	B      string `yaml:"b"`
	Start  string `yaml:"start"`
	Select string `yaml:"select"`
}

type JoystickConfig struct {
	Left   string `yaml:"left"`
	Right  string `yaml:"right"`
	Up     string `yaml:"up"`
	Down   string `yaml:"down"`
	A      string `yaml:"a"`
	B      string `yaml:"b"`
	Start  string `yaml:"start"`
	Select string `yaml:"select"`
}

type PlayerInputConfig struct {
	Keyboard KeyboardConfig `yaml:"keyboard"`
	Joystick JoystickConfig `yaml:"joystick"`
}

type InputConfig struct {
	Player1 PlayerInputConfig `yaml:"player1"`
	Player2 PlayerInputConfig `yaml:"player2"`
}

type GameDef struct {
	Game struct {
		Name     string         `yaml:"name"`
		Version  string         `yaml:"version"`
		Data     DataConfig     `yaml:"data"`
		Settings SettingsConfig `yaml:"settings"`
		Video    VideoConfig    `yaml:"video"`
		Input    InputConfig    `yaml:"input"`
		Fonts    []FontDef      `yaml:"fonts"`
	} `yaml:"game"`
}

// --- Parser ---
type Parser struct {
}

// NewParser creates a new YAML parser
func NewParser() *Parser {
	return &Parser{}
}

// ParseWorld parses a world definition from a YAML file
func (p *Parser) ParseWorld(path string) (*WorldDef, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read world file %s: %w", path, err)
	}

	worldDef := &WorldDef{}
	// Backward-compatible default: legacy world YAML files may omit acceleration.
	// Pre-seeding before unmarshal preserves explicit YAML values (including 0.0).
	worldDef.World.Mechanics.Acceleration = 0.05
	if err := yaml.Unmarshal(data, worldDef); err != nil {
		return nil, fmt.Errorf("failed to parse world YAML from %s: %w", path, err)
	}

	if worldDef.World.Type == "" {
		worldDef.World.Type = "platformer"
	}
	if !isSupportedWorldType(worldDef.World.Type) {
		return nil, fmt.Errorf("invalid world YAML %s: unsupported world.type '%s'", path, worldDef.World.Type)
	}

	if worldDef.World.Mechanics.Profile == "" {
		worldDef.World.Mechanics.Profile = worldDef.World.Type
	}
	if !isSupportedWorldType(worldDef.World.Mechanics.Profile) {
		return nil, fmt.Errorf("invalid world YAML %s: unsupported mechanics.profile '%s'", path, worldDef.World.Mechanics.Profile)
	}

	if worldDef.World.Mechanics.DeadStateMode == "" {
		worldDef.World.Mechanics.DeadStateMode = "immovable"
	}
	if worldDef.World.Mechanics.DeadStateMode != "immovable" && worldDef.World.Mechanics.DeadStateMode != "transitions" {
		return nil, fmt.Errorf("invalid world YAML %s: unsupported mechanics.deadStateMode '%s'", path, worldDef.World.Mechanics.DeadStateMode)
	}

	normalizeAllowedStates(&worldDef.World.Mechanics.AllowedStates, worldDef.World.Mechanics.Profile)

	// Modernized camera schema: world.mechanics.camera is canonical.
	if worldDef.World.Mechanics.Camera.Viewport.Width <= 0 || worldDef.World.Mechanics.Camera.Viewport.Height <= 0 {
		return nil, fmt.Errorf("invalid world YAML %s: mechanics.camera.viewport.width/height must be > 0", path)
	}

	if worldDef.World.Mechanics.Camera.Dimensions.Width <= 0 || worldDef.World.Mechanics.Camera.Dimensions.Height <= 0 {
		return nil, fmt.Errorf("invalid world YAML %s: mechanics.camera.dimensions.width/height must be > 0", path)
	}

	setAnimationSourceDir(worldDef.World.Animations, filepath.Dir(path))
	setObjectScriptSourceDir(worldDef.World.ObjectScripts, filepath.Dir(path))

	if err := mergeWorldSources(path, worldDef); err != nil {
		return nil, err
	}

	return worldDef, nil
}

func setAnimationSourceDir(anims []AnimationDef, sourceDir string) {
	for i := range anims {
		anims[i].SourceDir = sourceDir
	}
}

func setObjectScriptSourceDir(scripts []ObjectScriptDef, sourceDir string) {
	for i := range scripts {
		scripts[i].SourceDir = sourceDir
		scripts[i].Animation.SourceDir = sourceDir
		for j := range scripts[i].Animations {
			scripts[i].Animations[j].SourceDir = sourceDir
		}
	}
}

func mergeWorldSources(worldPath string, worldDef *WorldDef) error {
	worldDir := filepath.Dir(worldPath)
	sourceFiles := make([]string, 0, len(worldDef.World.ResourceSources)+len(worldDef.World.ObjectScriptSources))
	sourceFiles = append(sourceFiles, worldDef.World.ResourceSources...)
	sourceFiles = append(sourceFiles, worldDef.World.ObjectScriptSources...)
	if len(sourceFiles) == 0 {
		return nil
	}

	importPaths := append([]string{}, worldDef.World.ScriptImportPaths...)

	animByID := make(map[string]AnimationDef)
	animOrder := make([]string, 0)

	scriptByID := make(map[string]ObjectScriptDef)
	scriptOrder := make([]string, 0)

	for _, src := range sourceFiles {
		resolved := src
		if !filepath.IsAbs(resolved) {
			resolved = filepath.Join(worldDir, resolved)
		}

		data, err := os.ReadFile(resolved)
		if err != nil {
			return fmt.Errorf("invalid world YAML %s: failed to read source file %s: %w", worldPath, resolved, err)
		}

		bundle := ResourceBundleDef{}
		if err := yaml.Unmarshal(data, &bundle); err != nil {
			return fmt.Errorf("invalid world YAML %s: failed to parse source file %s: %w", worldPath, resolved, err)
		}

		sourceDir := filepath.Dir(resolved)
		setAnimationSourceDir(bundle.Animations, sourceDir)
		setObjectScriptSourceDir(bundle.ObjectScripts, sourceDir)

		for _, ip := range bundle.ScriptImportPaths {
			if ip == "" {
				continue
			}
			if filepath.IsAbs(ip) {
				importPaths = append(importPaths, ip)
			} else {
				importPaths = append(importPaths, filepath.Clean(filepath.Join(sourceDir, ip)))
			}
		}

		for _, a := range bundle.Animations {
			if a.ID == "" {
				continue
			}
			if _, exists := animByID[a.ID]; !exists {
				animOrder = append(animOrder, a.ID)
			}
			animByID[a.ID] = a
		}

		for _, s := range bundle.ObjectScripts {
			if s.ID == "" {
				continue
			}
			if _, exists := scriptByID[s.ID]; !exists {
				scriptOrder = append(scriptOrder, s.ID)
			}
			scriptByID[s.ID] = s
		}
	}

	for _, a := range worldDef.World.Animations {
		if a.ID == "" {
			continue
		}
		if _, exists := animByID[a.ID]; !exists {
			animOrder = append(animOrder, a.ID)
		}
		animByID[a.ID] = a
	}

	for _, s := range worldDef.World.ObjectScripts {
		if s.ID == "" {
			continue
		}
		if _, exists := scriptByID[s.ID]; !exists {
			scriptOrder = append(scriptOrder, s.ID)
		}
		scriptByID[s.ID] = s
	}

	worldDef.World.ScriptImportPaths = dedupeStrings(importPaths)

	mergedAnims := make([]AnimationDef, 0, len(animOrder))
	for _, id := range animOrder {
		if a, ok := animByID[id]; ok {
			mergedAnims = append(mergedAnims, a)
		}
	}
	worldDef.World.Animations = mergedAnims

	mergedScripts := make([]ObjectScriptDef, 0, len(scriptOrder))
	for _, id := range scriptOrder {
		if s, ok := scriptByID[id]; ok {
			mergedScripts = append(mergedScripts, s)
		}
	}
	worldDef.World.ObjectScripts = mergedScripts

	return nil
}

func dedupeStrings(values []string) []string {
	seen := make(map[string]bool)
	out := make([]string, 0, len(values))
	for _, v := range values {
		if v == "" || seen[v] {
			continue
		}
		seen[v] = true
		out = append(out, v)
	}
	return out
}

func isSupportedWorldType(t string) bool {
	switch t {
	case "platformer", "sidescroller", "topdown", "isometric", "fixed":
		return true
	default:
		return false
	}
}

// ParseAnimationFile reads a YAML file containing animation definitions
// (the same format used by standalone animations.yaml files) and returns
// the animation list with SourceDir set to the file's parent directory.
func ParseAnimationFile(path string) ([]AnimationDef, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read animation file %s: %w", path, err)
	}

	var bundle ResourceBundleDef
	if err := yaml.Unmarshal(data, &bundle); err != nil {
		return nil, fmt.Errorf("failed to parse animation file %s: %w", path, err)
	}

	sourceDir := filepath.Dir(path)
	setAnimationSourceDir(bundle.Animations, sourceDir)
	return bundle.Animations, nil
}

func defaultAllowedStatesForProfile(profile string) AllowedStatesDef {
	switch profile {
	case "topdown", "isometric":
		return AllowedStatesDef{
			Movement: []string{"standing", "walking", "running", "floating", "flying", "immovable"},
			Life:     []string{"alive", "injured", "dying", "dead"},
			Status:   []string{"visible", "invisible", "vulnerable", "invulnerable", "attached", "repelling", "attacking"},
		}
	case "fixed":
		return AllowedStatesDef{
			Movement: []string{"standing", "walking", "running", "jumping", "falling", "immovable"},
			Life:     []string{"alive", "injured", "dying", "dead"},
			Status:   []string{"visible", "invisible", "vulnerable", "invulnerable", "attached", "repelling", "attacking"},
		}
	default: // platformer + sidescroller
		return AllowedStatesDef{
			Movement: []string{"standing", "walking", "running", "ducking", "crawling", "jumping", "falling", "floating", "flying", "sliding", "immovable"},
			Life:     []string{"alive", "injured", "dying", "dead"},
			Status:   []string{"visible", "invisible", "vulnerable", "invulnerable", "attached", "repelling", "attacking"},
		}
	}
}

func normalizeAllowedStates(states *AllowedStatesDef, profile string) {
	defaults := defaultAllowedStatesForProfile(profile)
	if len(states.Movement) == 0 {
		states.Movement = defaults.Movement
	}
	if len(states.Life) == 0 {
		states.Life = defaults.Life
	}
	if len(states.Status) == 0 {
		states.Status = defaults.Status
	}
}

func containsString(values []string, needle string) bool {
	for _, value := range values {
		if value == needle {
			return true
		}
	}
	return false
}

// ParseGame parses a game definition from a YAML file
func (p *Parser) ParseGame(path string) (*GameDef, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read game file %s: %w", path, err)
	}

	gameDef := &GameDef{}
	if err := yaml.Unmarshal(data, gameDef); err != nil {
		return nil, fmt.Errorf("failed to parse game YAML from %s: %w", path, err)
	}

	return gameDef, nil
}
