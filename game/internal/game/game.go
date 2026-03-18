package game

import (
	"fmt"
	"os"

	"platformer/internal/config"
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
}

// NewGame creates a new game instance from the system config.yaml.
func NewGame(configPath string) (*Game, *VideoSettings, error) {
	parser := config.NewParser()
	gameDef, err := parser.ParseGame(configPath)
	if err != nil {
		return nil, nil, fmt.Errorf("failed to load game config: %w", err)
	}

	baseDir := gameDef.Game.Data.BaseDir
	if baseDir == "" {
		baseDir = "data"
	}
	worldPath := baseDir + "/" + gameDef.Game.Data.DefaultWorld
	worldDef, err := parser.ParseWorld(worldPath)
	if err != nil {
		return nil, nil, fmt.Errorf("failed to load world config: %w", err)
	}

	w, err := world.NewWorldFromDef(worldDef, baseDir)
	if err != nil {
		return nil, nil, fmt.Errorf("failed to create world: %w", err)
	}

	// Temporary player bootstrap: prefer YAML-defined player stub scripts (e.g. mario-stub).
	camStart := w.GetCamera()
	playerStub, err := w.CreatePlayerStub(camStart.X, camStart.Y)
	if err == nil {
		w.AddObject(playerStub)
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
	return nil
}

// handleInput processes keyboard input for camera movement
func (g *Game) handleInput() {
	g.handlePlayerInput()
	g.handleCameraInput()
}

// handlePlayerInput maps P1 controls to the temporary player stub.
func (g *Game) handlePlayerInput() {
	if g.player == nil {
		return
	}

	phys := g.player.GetPhysics()
	moveSpeed := 1.8
	jumpSpeed := -4.5

	if ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
		phys.VelocityX = -moveSpeed
	} else if ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
		phys.VelocityX = moveSpeed
	} else {
		phys.VelocityX = 0
	}

	if inpututil.IsKeyJustPressed(ebiten.KeyZ) && g.player.IsGrounded() {
		phys.VelocityY = jumpSpeed
	}
}

// handleCameraInput maps P2 controls to manual camera movement.
func (g *Game) handleCameraInput() {
	cam := g.world.GetCamera()
	moveSpeed := 2.0

	if ebiten.IsKeyPressed(ebiten.KeyA) {
		cam.X -= moveSpeed
	}
	if ebiten.IsKeyPressed(ebiten.KeyD) {
		cam.X += moveSpeed
	}
	if ebiten.IsKeyPressed(ebiten.KeyW) {
		cam.Y -= moveSpeed
	}
	if ebiten.IsKeyPressed(ebiten.KeyS) {
		cam.Y += moveSpeed
	}

	if cam.X < 0 {
		cam.X = 0
	}
	maxX := float64(cam.WorldWidth - cam.ViewportWidth)
	if cam.X > maxX {
		cam.X = maxX
	}
	if cam.Y < 0 {
		cam.Y = 0
	}
	maxY := float64(cam.WorldHeight - cam.ViewportHeight)
	if cam.Y > maxY {
		cam.Y = maxY
	}
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
