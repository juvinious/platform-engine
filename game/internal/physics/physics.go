package physics

// Physics handles movement, velocity, and acceleration for objects
type Physics struct {
	VelocityX     float64
	VelocityY     float64
	AccelerationX float64
	AccelerationY float64
	Friction      float64
}

// NewPhysics creates a new physics component
func NewPhysics() *Physics {
	return &Physics{
		VelocityX:     0,
		VelocityY:     0,
		AccelerationX: 0,
		AccelerationY: 0,
		Friction:      0.0,
	}
}

// Update updates velocity based on acceleration
func (p *Physics) Update() {
	p.VelocityX += p.AccelerationX
	p.VelocityY += p.AccelerationY

	// Apply friction
	if p.Friction > 0 {
		p.VelocityX *= (1 - p.Friction)
		p.VelocityY *= (1 - p.Friction)
	}
}

// ApplyGravity applies gravity acceleration
func (p *Physics) ApplyGravity(gravity float64) {
	p.AccelerationY += gravity
}

// SetVelocity sets both velocity components
func (p *Physics) SetVelocity(vx, vy float64) {
	p.VelocityX = vx
	p.VelocityY = vy
}

// SetAcceleration sets both acceleration components
func (p *Physics) SetAcceleration(ax, ay float64) {
	p.AccelerationX = ax
	p.AccelerationY = ay
}
