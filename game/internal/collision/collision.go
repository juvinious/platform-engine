package collision

// Rect represents a rectangular collision box
type Rect struct {
	X      float64
	Y      float64
	Width  float64
	Height float64
}

// Collision handles collision detection and response
type Collision struct {
	Boxes   []Rect
	Enabled bool
	Solid   bool
}

// NewCollision creates a new collision component
func NewCollision() *Collision {
	return &Collision{
		Boxes:   make([]Rect, 0),
		Enabled: true,
		Solid:   true,
	}
}

// AddBox adds a collision box
func (c *Collision) AddBox(x, y, width, height float64) {
	c.Boxes = append(c.Boxes, Rect{X: x, Y: y, Width: width, Height: height})
}

// Clear removes all collision boxes
func (c *Collision) Clear() {
	c.Boxes = c.Boxes[:0]
}

// Intersects checks if this collision intersects with another
func (c *Collision) Intersects(other *Collision, offsetX, offsetY float64) bool {
	if !c.Enabled || !other.Enabled {
		return false
	}

	for _, box1 := range c.Boxes {
		for _, box2 := range other.Boxes {
			if rectsIntersect(box1, box2, offsetX, offsetY) {
				return true
			}
		}
	}
	return false
}

// rectsIntersect checks if two rectangles intersect
func rectsIntersect(r1, r2 Rect, offsetX, offsetY float64) bool {
	return r1.X < r2.X+offsetX+r2.Width &&
		r1.X+r1.Width > r2.X+offsetX &&
		r1.Y < r2.Y+offsetY+r2.Height &&
		r1.Y+r1.Height > r2.Y+offsetY
}
