package tile

import (
	"platformer/internal/camera"

	"github.com/hajimehoshi/ebiten/v2"
)

// Tile represents a single tile in the tilemap
type Tile struct {
	X           int
	Y           int
	AnimationID string
	Solid       bool
}

// TileMap manages a grid of tiles
type TileMap struct {
	Width    int
	Height   int
	Tiles    [][]Tile
	TileSize int
}

// NewTileMap creates a new tilemap
func NewTileMap(width, height, tileSize int) *TileMap {
	tiles := make([][]Tile, height)
	for y := range tiles {
		tiles[y] = make([]Tile, width)
	}

	return &TileMap{
		Width:    width,
		Height:   height,
		Tiles:    tiles,
		TileSize: tileSize,
	}
}

// SetTile sets a tile at a specific grid position
func (tm *TileMap) SetTile(x, y int, animationID string, solid bool) {
	if x >= 0 && x < tm.Width && y >= 0 && y < tm.Height {
		tm.Tiles[y][x] = Tile{
			X:           x,
			Y:           y,
			AnimationID: animationID,
			Solid:       solid,
		}
	}
}

// GetTile gets a tile at a specific grid position
func (tm *TileMap) GetTile(x, y int) *Tile {
	if x >= 0 && x < tm.Width && y >= 0 && y < tm.Height {
		return &tm.Tiles[y][x]
	}
	return nil
}

// Draw renders the tilemap
func (tm *TileMap) Draw(screen *ebiten.Image, camera *camera.Camera) {
	// TODO: Render tiles using camera position
}
