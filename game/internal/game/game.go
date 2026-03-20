package game

import (
	"fmt"
	"os"
	"path/filepath"

	"platformer/internal/config"
	"platformer/internal/logger"
	"platformer/internal/object"
	"platformer/internal/world"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
)

// VideoSettings exposes display configuration to the caller (e.g. main).
type VideoSettings struct {
	WindowWidth  int
	WindowHeight int
	Title        string
	Fullscreen   bool
	VSync        bool
}

// Game represents the main game instance
type Game struct {
	world         *world.World
	player        object.Object
	backbuffer    *ebiten.Image
	nativeWidth   int
	nativeHeight  int
	displayWidth  int
	displayHeight int
	scaleX        float64
	scaleY        float64
	inputCfg      config.PlayerInputConfig
}

// NewGame creates a new game instance from the system config.yaml.
func NewGame(configPath string) (*Game, *VideoSettings, error) {
	parser := config.NewParser()
	gameDef, err := parser.ParseGame(configPath)
	if err != nil {
		return nil, nil, fmt.Errorf("failed to load game config: %w", err)
	}

	if err := logger.Init(
		gameDef.Game.Settings.Debug,
		gameDef.Game.Settings.Quiet,
		gameDef.Game.Settings.DebugLevel,
		gameDef.Game.Settings.LogFile,
		gameDef.Game.Settings.ErrorLog,
		filepath.Dir(configPath),
	); err != nil {
		return nil, nil, fmt.Errorf("failed to initialize logger: %w", err)
	}
	logger.Info("loaded game config: %s", configPath)

	baseDir := gameDef.Game.Data.BaseDir
	if baseDir == "" {
		baseDir = "data"
	}
	worldPath := baseDir + "/" + gameDef.Game.Data.DefaultWorld
	worldDef, err := parser.ParseWorld(worldPath)
	if err != nil {
		logger.Error("failed to parse world config %s: %v", worldPath, err)
		return nil, nil, fmt.Errorf("failed to load world config: %w", err)
	}
	logger.Info("loaded world config: %s", worldPath)

	w, err := world.NewWorldFromDef(worldDef, baseDir)
	if err != nil {
		logger.Error("failed to create world from %s: %v", worldPath, err)
		return nil, nil, fmt.Errorf("failed to create world: %w", err)
	}

	// Temporary player bootstrap: prefer YAML-defined player stub scripts (e.g. mario-stub).
	camStart := w.GetCamera()
	playerStub, err := w.CreatePlayerStub(camStart.X, camStart.Y)
	if err == nil {
		w.AddObject(playerStub)
		logger.Info("created player stub at x=%.1f y=%.1f", camStart.X, camStart.Y)
	} else {
		logger.Warn("failed to create player stub: %v", err)
	}

	vid := &gameDef.Game.Video

	nativeW := worldDef.World.Resolution.Width
	nativeH := worldDef.World.Resolution.Height
	if nativeW == 0 {
		nativeW = vid.Resolution.Width
	}
	if nativeH == 0 {
		nativeH = vid.Resolution.Height
	}

	displayW := vid.Resolution.Width
	displayH := vid.Resolution.Height
	if displayW == 0 {
		scale := vid.Scale
		if scale <= 0 {
			scale = 1.0
		}
		displayW = int(float64(nativeW) * scale)
		displayH = int(float64(nativeH) * scale)
	}

	title := gameDef.Game.Name
	if title == "" {
		title = "Platform Engine"
	}

	return &Game{
			world:         w,
			player:        playerStub,
			nativeWidth:   nativeW,
			nativeHeight:  nativeH,
			displayWidth:  displayW,
			displayHeight: displayH,
			scaleX:        float64(displayW) / float64(nativeW),
			scaleY:        float64(displayH) / float64(nativeH),
			inputCfg:      gameDef.Game.Input.Player1,
		}, &VideoSettings{
			WindowWidth:  displayW,
			WindowHeight: displayH,
			Title:        title,
			Fullscreen:   vid.Fullscreen,
			VSync:        vid.VSync,
		}, nil
}

// Update updates the game state
func (g *Game) Update() error {
	if inpututil.IsKeyJustPressed(ebiten.KeyEscape) {
		os.Exit(0)
	}
	g.handleInput()
	g.world.Update()
	g.followPlayer()
	return nil
}

// handleInput processes keyboard input
func (g *Game) handleInput() {
	g.handlePlayerInput()
}

// followPlayer centers the camera on the player after the world has updated.
func (g *Game) followPlayer() {
	if g.player == nil {
		return
	}
	px, _ := g.player.GetPosition()
	pw, _ := g.player.GetSize()
	cam := g.world.GetCamera()
	targetX := px + pw/2 - float64(cam.ViewportWidth)/2
	if targetX < 0 {
		targetX = 0
	}
	maxX := float64(cam.WorldWidth - cam.ViewportWidth)
	if targetX > maxX {
		targetX = maxX
	}
	cam.X = targetX
}

// Draw renders the game
func (g *Game) Draw(screen *ebiten.Image) {
	if g.backbuffer == nil {
		g.backbuffer = ebiten.NewImage(g.nativeWidth, g.nativeHeight)
	}
	g.world.Draw(g.backbuffer)
	opts := &ebiten.DrawImageOptions{}
	opts.GeoM.Scale(g.scaleX, g.scaleY)
	screen.DrawImage(g.backbuffer, opts)
}

// Layout returns the game layout dimensions
func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return g.displayWidth, g.displayHeight
}
