package camera

import (
	"github.com/hajimehoshi/ebiten/v2"
)

// Camera handles viewport and following objects
type Camera struct {
	ViewportWidth   int
	ViewportHeight  int
	WorldWidth      int
	WorldHeight     int
	X               float64
	Y               float64
	FollowTarget    interface{} // Object to follow
	SmoothScrolling bool
	Speed           float64
	Variance        float64
}

// NewCamera creates a new camera
func NewCamera(vpWidth, vpHeight, worldWidth, worldHeight int) *Camera {
	return &Camera{
		ViewportWidth:   vpWidth,
		ViewportHeight:  vpHeight,
		WorldWidth:      worldWidth,
		WorldHeight:     worldHeight,
		X:               0,
		Y:               0,
		SmoothScrolling: true,
		Speed:           1.5,
		Variance:        2,
	}
}

// Update updates the camera position
func (c *Camera) Update() {
	// TODO: Follow target if set
	// Clamp to world bounds
	if c.X < 0 {
		c.X = 0
	}
	if c.X > float64(c.WorldWidth-c.ViewportWidth) {
		c.X = float64(c.WorldWidth - c.ViewportWidth)
	}
	if c.Y < 0 {
		c.Y = 0
	}
	if c.Y > float64(c.WorldHeight-c.ViewportHeight) {
		c.Y = float64(c.WorldHeight - c.ViewportHeight)
	}
}

// GetDrawOptions returns the draw options for translating by camera position
func (c *Camera) GetDrawOptions() *ebiten.DrawImageOptions {
	opts := &ebiten.DrawImageOptions{}
	opts.GeoM.Translate(-c.X, -c.Y)
	return opts
}

// SetPosition sets the camera position
func (c *Camera) SetPosition(x, y float64) {
	c.X = x
	c.Y = y
}

// GetPosition returns the camera position
func (c *Camera) GetPosition() (float64, float64) {
	return c.X, c.Y
}
