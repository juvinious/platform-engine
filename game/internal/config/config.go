package config

import (
	"fmt"
	"os"

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
	ID      string     `yaml:"id"`
	BaseDir string     `yaml:"basedir"`
	Images  []string   `yaml:"images"`
	Frames  []FrameDef `yaml:"frames"`
	Loop    bool       `yaml:"loop"`
}

// --- Physics ---
type MechanicsDef struct {
	Gravity      Vec2    `yaml:"gravity"`
	Acceleration float64 `yaml:"acceleration"`
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
	ID             string         `yaml:"id"`
	Dimensions     Vec2           `yaml:"dimensions"`
	Module         string         `yaml:"module"`
	Function       string         `yaml:"function"`
	Image          string         `yaml:"image"`
	Animation   AnimationDef   `yaml:"animation"`
	Animations  []AnimationDef `yaml:"animations,omitempty"`
	Color       Color          `yaml:"color"`
	PhysicsType string         `yaml:"physicsType,omitempty"`
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

// --- World Definition ---
type WorldDef struct {
	World struct {
		Name          string            `yaml:"name"`
		Version       int               `yaml:"version"`
		Resolution    IntVec2           `yaml:"resolution"`
		Dimensions    IntVec2           `yaml:"dimensions"`
		FillColor     Color             `yaml:"fillColor"`
		Mechanics     MechanicsDef      `yaml:"mechanics"`
		Cameras       []CameraDef       `yaml:"cameras"`
		Animations    []AnimationDef    `yaml:"animations"`
		ScriptImportPaths []string          `yaml:"scriptImportPaths,omitempty"`
		ObjectScripts     []ObjectScriptDef `yaml:"objectScripts,omitempty"`
		Backgrounds   []BackgroundDef   `yaml:"backgrounds,omitempty"`
		Foregrounds   []ForegroundDef   `yaml:"foregrounds,omitempty"`
		CollisionMap  CollisionMapDef   `yaml:"collisionMap,omitempty"`
		Objects       []ObjectDef       `yaml:"objects,omitempty"`
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

	return worldDef, nil
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
