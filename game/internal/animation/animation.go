package animation

import (
	"platformer/internal/assets"
	"platformer/internal/config"
	"platformer/internal/sprite"

	"github.com/hajimehoshi/ebiten/v2"
)

// SpriteAnimation manages sprite-based animation
type SpriteAnimation struct {
	sprite *sprite.Sprite
}

// NewSpriteAnimation creates sprite animation from a config definition
func NewSpriteAnimation(def config.AnimationDef, assetMgr *assets.AssetManager) (*SpriteAnimation, error) {
	spr, err := sprite.NewSprite(def, assetMgr)
	if err != nil {
		return nil, err
	}

	return &SpriteAnimation{
		sprite: spr,
	}, nil
}

// Update advances the animation
func (sa *SpriteAnimation) Update() {
	sa.sprite.Update()
}

// Draw renders the animation at the given position
func (sa *SpriteAnimation) Draw(screen *ebiten.Image, x, y float64) {
	sa.sprite.Draw(screen, x, y)
}

// DrawWithCamera renders the animation with camera offset
func (sa *SpriteAnimation) DrawWithCamera(screen *ebiten.Image, x, y, camX, camY float64) {
	sa.sprite.DrawWithCamera(screen, x, y, camX, camY)
}

// DrawWithCameraFlip renders with camera offset and object-level flip overrides.
func (sa *SpriteAnimation) DrawWithCameraFlip(screen *ebiten.Image, x, y, camX, camY float64, hflip, vflip bool) {
	sa.sprite.DrawWithCameraFlip(screen, x, y, camX, camY, hflip, vflip)
}

// GetCurrentFrame returns the current frame
func (sa *SpriteAnimation) GetCurrentFrame() config.FrameDef {
	return sa.sprite.GetCurrentFrame()
}

// GetSize returns sprite dimensions
func (sa *SpriteAnimation) GetSize() (int, int) {
	return sa.sprite.GetSize()
}

// Reset resets the animation
func (sa *SpriteAnimation) Reset() {
	sa.sprite.Reset()
}
