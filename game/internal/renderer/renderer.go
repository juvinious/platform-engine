package renderer

import (
	"platformer/internal/assets"
	"platformer/internal/sprite"

	"github.com/hajimehoshi/ebiten/v2"
)

// SpriteRenderer renders sprites to the screen
type SpriteRenderer struct {
	sprites  map[string]*sprite.Sprite
	assetMgr *assets.AssetManager
}

// NewSpriteRenderer creates a new sprite renderer
func NewSpriteRenderer(assetMgr *assets.AssetManager) *SpriteRenderer {
	return &SpriteRenderer{
		sprites:  make(map[string]*sprite.Sprite),
		assetMgr: assetMgr,
	}
}

// RenderSprite renders a sprite by ID at the given position
func (sr *SpriteRenderer) RenderSprite(screen *ebiten.Image, spriteID string, x, y float64) error {
	s, ok := sr.sprites[spriteID]
	if !ok {
		return nil // Sprite not loaded yet, skip
	}

	s.Draw(screen, x, y)
	return nil
}

// RenderSpriteWithCamera renders a sprite with camera offset
func (sr *SpriteRenderer) RenderSpriteWithCamera(screen *ebiten.Image, spriteID string, x, y, camX, camY float64) error {
	s, ok := sr.sprites[spriteID]
	if !ok {
		return nil
	}

	s.DrawWithCamera(screen, x, y, camX, camY)
	return nil
}

// GetSprite returns a sprite by ID
func (sr *SpriteRenderer) GetSprite(spriteID string) *sprite.Sprite {
	return sr.sprites[spriteID]
}

// RegisterSprite registers a sprite for rendering
func (sr *SpriteRenderer) RegisterSprite(spriteID string, s *sprite.Sprite) {
	sr.sprites[spriteID] = s
}

// UpdateSprites updates all registered sprites
func (sr *SpriteRenderer) UpdateSprites() {
	for _, s := range sr.sprites {
		s.Update()
	}
}
