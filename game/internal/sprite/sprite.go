package sprite

import (
	"fmt"

	"platformer/internal/assets"
	"platformer/internal/config"

	"github.com/hajimehoshi/ebiten/v2"
)

// Sprite represents a drawable sprite with animation
type Sprite struct {
	animationDef config.AnimationDef
	images       []*ebiten.Image
	currentFrame int
	frameCounter int
	assetMgr     *assets.AssetManager
	alpha        float32
	hflip        bool
	vflip        bool
}

// NewSprite creates a sprite from an animation definition
func NewSprite(def config.AnimationDef, assetMgr *assets.AssetManager) (*Sprite, error) {
	sprite := &Sprite{
		animationDef: def,
		images:       make([]*ebiten.Image, len(def.Images)),
		currentFrame: 0,
		frameCounter: 0,
		assetMgr:     assetMgr,
		alpha:        1.0,
	}

	// Load all images for this animation
	for i, imgPath := range def.Images {
		fullPath := def.BaseDir + imgPath
		img, err := assetMgr.LoadImage(fullPath)
		if err != nil {
			return nil, fmt.Errorf("failed to load sprite image %s: %w", fullPath, err)
		}
		sprite.images[i] = img
	}

	return sprite, nil
}

// Update advances the animation frame
func (s *Sprite) Update() {
	if len(s.animationDef.Frames) == 0 {
		return
	}

	frame := s.animationDef.Frames[s.currentFrame]

	// Only advance if duration is positive (negative means infinite/static)
	if frame.Duration > 0 {
		s.frameCounter++
		if s.frameCounter >= frame.Duration {
			s.frameCounter = 0
			s.currentFrame++

			if s.currentFrame >= len(s.animationDef.Frames) {
				if s.animationDef.Loop {
					s.currentFrame = 0
				} else {
					s.currentFrame = len(s.animationDef.Frames) - 1
				}
			}
		}
	}
}

// Draw renders the sprite at the given position
func (s *Sprite) Draw(screen *ebiten.Image, x, y float64) {
	if len(s.animationDef.Frames) == 0 || len(s.images) == 0 {
		return
	}

	frame := s.animationDef.Frames[s.currentFrame]
	if frame.ImageIndex < 0 || frame.ImageIndex >= len(s.images) {
		return
	}

	img := s.images[frame.ImageIndex]
	if img == nil {
		return
	}

	opts := &ebiten.DrawImageOptions{}

	// Set alpha (convert 0-255 to 0-1 range for ColorScale)
	alpha := float32(frame.Alpha) / 255.0
	opts.ColorScale.Scale(1, 1, 1, alpha)

	// Handle flips by scaling
	if frame.HFlip {
		opts.GeoM.Scale(-1, 1)
		opts.GeoM.Translate(float64(img.Bounds().Dx()), 0)
	}
	if frame.VFlip {
		opts.GeoM.Scale(1, -1)
		opts.GeoM.Translate(0, float64(img.Bounds().Dy()))
	}

	// Position
	opts.GeoM.Translate(x, y)

	screen.DrawImage(img, opts)
}

// DrawWithCamera renders the sprite at the given position, adjusted for camera offset
func (s *Sprite) DrawWithCamera(screen *ebiten.Image, x, y float64, camX, camY float64) {
	s.Draw(screen, x-camX, y-camY)
}

// DrawWithCameraFlip renders with camera offset and object-level flip overrides.
// Object flip XORs with the per-frame animation flip.
func (s *Sprite) DrawWithCameraFlip(screen *ebiten.Image, x, y float64, camX, camY float64, hflip, vflip bool) {
	s.drawFlipped(screen, x-camX, y-camY, hflip, vflip)
}

func (s *Sprite) drawFlipped(screen *ebiten.Image, x, y float64, objHFlip, objVFlip bool) {
	if len(s.animationDef.Frames) == 0 || len(s.images) == 0 {
		return
	}

	frame := s.animationDef.Frames[s.currentFrame]
	if frame.ImageIndex < 0 || frame.ImageIndex >= len(s.images) {
		return
	}

	img := s.images[frame.ImageIndex]
	if img == nil {
		return
	}

	opts := &ebiten.DrawImageOptions{}

	alpha := float32(frame.Alpha) / 255.0
	opts.ColorScale.Scale(1, 1, 1, alpha)

	// XOR object flip with frame flip
	hflip := objHFlip != frame.HFlip
	vflip := objVFlip != frame.VFlip

	if hflip {
		opts.GeoM.Scale(-1, 1)
		opts.GeoM.Translate(float64(img.Bounds().Dx()), 0)
	}
	if vflip {
		opts.GeoM.Scale(1, -1)
		opts.GeoM.Translate(0, float64(img.Bounds().Dy()))
	}

	opts.GeoM.Translate(x, y)
	screen.DrawImage(img, opts)
}

// GetCurrentFrame returns the current frame definition
func (s *Sprite) GetCurrentFrame() config.FrameDef {
	if len(s.animationDef.Frames) == 0 {
		return config.FrameDef{}
	}
	return s.animationDef.Frames[s.currentFrame]
}

// Reset resets animation to frame 0
func (s *Sprite) Reset() {
	s.currentFrame = 0
	s.frameCounter = 0
}

// SetAlpha sets the sprite alpha (0-255)
func (s *Sprite) SetAlpha(alpha float32) {
	s.alpha = alpha
}

// GetSize returns the width and height of the current sprite image
func (s *Sprite) GetSize() (int, int) {
	frame := s.GetCurrentFrame()
	if frame.ImageIndex < 0 || frame.ImageIndex >= len(s.images) {
		return 0, 0
	}

	img := s.images[frame.ImageIndex]
	if img == nil {
		return 0, 0
	}

	return img.Bounds().Dx(), img.Bounds().Dy()
}

// GetImageCount returns the number of images in this animation
func (s *Sprite) GetImageCount() int {
	return len(s.images)
}
