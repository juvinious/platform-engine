package object

import (
	"platformer/internal/animation"
	"platformer/internal/camera"
	"platformer/internal/collision"
	"platformer/internal/physics"

	"github.com/hajimehoshi/ebiten/v2"
)

// PhysicsType controls how the physics engine treats an object each frame.
//
//	PhysicsTypeDynamic   — gravity + velocity integration (enemies, player). Default.
//	PhysicsTypeStatic    — immovable; physics is completely skipped (blocks, terrain).
//	PhysicsTypeKinematic — velocity-driven but no gravity (moving platforms, projectiles).
const (
	PhysicsTypeDynamic   = "dynamic"
	PhysicsTypeStatic    = "static"
	PhysicsTypeKinematic = "kinematic"
)

// Object is the interface every world entity must satisfy.
// BaseObject provides a default implementation; ScriptedObject extends it with Lua callbacks.
type Object interface {
	// Act is called once per frame to update engine state.
	Act(world interface{})

	// Draw renders the object using the given camera offset.
	Draw(screen *ebiten.Image, cam *camera.Camera)

	// GetPosition / SetPosition — world-space position, top-left corner.
	GetPosition() (float64, float64)
	SetPosition(x, y float64)

	// GetSize / SetSize — logical width and height (usually the hitbox dimensions).
	GetSize() (float64, float64)
	SetSize(w, h float64)

	// GetPhysics returns the mutable physics component.
	GetPhysics() *physics.Physics

	// GetCollision returns the mutable collision component.
	GetCollision() *collision.Collision

	// GetLabel / SetLabel — human-readable identifier ("goomba", "question-block", …).
	GetLabel() string
	SetLabel(label string)

	// GetPhysicsType / SetPhysicsType — one of the PhysicsType* constants.
	GetPhysicsType() string
	SetPhysicsType(t string)

	// Contact state from the most recent collision pass.
	ResetContactState()
	MergeContactStateFromSides(sides map[string]bool)
	IsGrounded() bool
	IsOnCeiling() bool
	IsOnWallLeft() bool
	IsOnWallRight() bool

	// Canonical runtime state machine fields.
	GetMovementState() string
	SetMovementState(state string)
	GetLifeState() string
	SetLifeState(state string)
	HasStatusFlag(flag string) bool
	SetStatusFlag(flag string, enabled bool)

	// Animation identification.
	GetAnimationID() string
	SetAnimationID(id string)
}

// BaseObject is the engine-side representation of a world entity.
// It owns position, size, physics, collision, animation, and physicsType.
// ScriptedObject embeds *BaseObject to add Lua scripting on top.
type BaseObject struct {
	label         string
	x, y          float64
	width         float64
	height        float64
	baseWidth     float64
	baseHeight    float64
	physics       *physics.Physics
	collision     *collision.Collision
	object        uint32
	animation     *animation.SpriteAnimation
	animationID   string
	hflip         bool
	vflip         bool
	physicsType   string
	grounded      bool
	onCeiling     bool
	onWallLeft    bool
	onWallRight   bool
	movementState string
	lifeState     string
	statusFlags   map[string]bool
}

// NewBaseObject creates a BaseObject with dynamic physics and zero position/size.
func NewBaseObject() *BaseObject {
	return &BaseObject{
		physics:       physics.NewPhysics(),
		collision:     collision.NewCollision(),
		physicsType:   PhysicsTypeDynamic,
		movementState: "standing",
		lifeState:     "alive",
		statusFlags: map[string]bool{
			"visible":    true,
			"vulnerable": true,
		},
	}
}

func (o *BaseObject) GetPosition() (float64, float64) { return o.x, o.y }
func (o *BaseObject) SetPosition(x, y float64)        { o.x = x; o.y = y }

func (o *BaseObject) GetSize() (float64, float64) { return o.width, o.height }
func (o *BaseObject) SetSize(w, h float64)        { o.width = w; o.height = h }

func (o *BaseObject) GetBaseDimensions() (float64, float64) { return o.baseWidth, o.baseHeight }
func (o *BaseObject) SetBaseDimensions(w, h float64)        { o.baseWidth = w; o.baseHeight = h }

func (o *BaseObject) GetPhysics() *physics.Physics       { return o.physics }
func (o *BaseObject) GetCollision() *collision.Collision { return o.collision }

func (o *BaseObject) GetLabel() string  { return o.label }
func (o *BaseObject) SetLabel(l string) { o.label = l }

func (o *BaseObject) GetPhysicsType() string  { return o.physicsType }
func (o *BaseObject) SetPhysicsType(t string) { o.physicsType = t }

func (o *BaseObject) ResetContactState() {
	o.grounded = false
	o.onCeiling = false
	o.onWallLeft = false
	o.onWallRight = false
}

func (o *BaseObject) MergeContactStateFromSides(sides map[string]bool) {
	if sides["top"] {
		o.grounded = true
	}
	if sides["bottom"] {
		o.onCeiling = true
	}
	if sides["left"] {
		o.onWallLeft = true
	}
	if sides["right"] {
		o.onWallRight = true
	}
}

func (o *BaseObject) IsGrounded() bool  { return o.grounded }
func (o *BaseObject) IsOnCeiling() bool { return o.onCeiling }
func (o *BaseObject) IsOnWallLeft() bool {
	return o.onWallLeft
}
func (o *BaseObject) IsOnWallRight() bool {
	return o.onWallRight
}

func (o *BaseObject) GetMovementState() string {
	return o.movementState
}

func (o *BaseObject) SetMovementState(state string) {
	o.movementState = state
}

func (o *BaseObject) GetLifeState() string {
	return o.lifeState
}

func (o *BaseObject) SetLifeState(state string) {
	o.lifeState = state
}

func (o *BaseObject) HasStatusFlag(flag string) bool {
	if o.statusFlags == nil {
		return false
	}
	return o.statusFlags[flag]
}

func (o *BaseObject) SetStatusFlag(flag string, enabled bool) {
	if o.statusFlags == nil {
		o.statusFlags = make(map[string]bool)
	}
	o.statusFlags[flag] = enabled
}

// SetAnimation replaces the current sprite animation.
func (o *BaseObject) SetAnimation(anim *animation.SpriteAnimation) { o.animation = anim }

// GetAnimation returns the current sprite animation (may be nil).
func (o *BaseObject) GetAnimation() *animation.SpriteAnimation { return o.animation }

// SetAnimationID records which named animation is currently active.
func (o *BaseObject) SetAnimationID(id string) { o.animationID = id }

// GetAnimationID returns the ID of the current animation (may be empty).
func (o *BaseObject) GetAnimationID() string { return o.animationID }

func (o *BaseObject) GetHFlip() bool  { return o.hflip }
func (o *BaseObject) SetHFlip(v bool) { o.hflip = v }
func (o *BaseObject) GetVFlip() bool  { return o.vflip }
func (o *BaseObject) SetVFlip(v bool) { o.vflip = v }

func applyGravityComponent(velocity *float64, gravity, acceleration float64) {
	if gravity == 0 {
		return
	}

	if *velocity == 0 {
		*velocity = gravity
		return
	}

	if gravity > 0 {
		*velocity += acceleration
	} else {
		*velocity -= acceleration
	}
}

// Act runs engine physics and advances the animation one frame.
// The physicsType determines which physics rules apply:
//
//	static    — skips all physics; object stays exactly where placed.
//	kinematic — integrates velocity without applying gravity.
//	dynamic   — applies gravity then integrates velocity (default).
func (o *BaseObject) Act(world interface{}) {
	switch o.physicsType {
	case PhysicsTypeStatic:
		// Immovable — only tick the animation.

	case PhysicsTypeKinematic:
		o.physics.Update()
		o.x += o.physics.VelocityX
		o.y += o.physics.VelocityY

	default: // PhysicsTypeDynamic
		if w, ok := world.(interface {
			GetGravityVector() (float64, float64)
			GetAcceleration() float64
		}); ok {
			gravityX, gravityY := w.GetGravityVector()
			accel := w.GetAcceleration()
			applyGravityComponent(&o.physics.VelocityX, gravityX, accel)
			applyGravityComponent(&o.physics.VelocityY, gravityY, accel)
		} else if w, ok := world.(interface{ GetGravity() float64 }); ok {
			applyGravityComponent(&o.physics.VelocityY, w.GetGravity(), 0.05)
		}
		o.physics.Update()
		o.x += o.physics.VelocityX
		o.y += o.physics.VelocityY
	}

	if o.animation != nil {
		o.animation.Update()
	}
}

// Draw renders the object at its world position adjusted for the camera.
func (o *BaseObject) Draw(screen *ebiten.Image, cam *camera.Camera) {
	if o.animation != nil {
		if o.hflip || o.vflip {
			o.animation.DrawWithCameraFlip(screen, o.x, o.y, cam.X, cam.Y, o.hflip, o.vflip)
		} else {
			o.animation.DrawWithCamera(screen, o.x, o.y, cam.X, cam.Y)
		}
	}
}
