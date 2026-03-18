package world

import (
	"fmt"
	"image/color"
	"log"
	"math"

	"platformer/internal/animation"
	"platformer/internal/assets"
	"platformer/internal/camera"
	"platformer/internal/collision"
	"platformer/internal/config"
	"platformer/internal/object"
	"platformer/internal/script"

	"github.com/hajimehoshi/ebiten/v2"
)

// World represents the game world containing objects, camera, and physics
type World struct {
	name              string
	width             int
	height            int
	objects           []object.Object
	backgrounds       []Background // Scenic layers (rendered before objects)
	foregrounds       []Foreground // Decorative layers (rendered after objects)
	camera            *camera.Camera
	gravityX          float64
	gravityY          float64
	acceleration      float64
	scrolling         bool
	animationDefs     map[string]config.AnimationDef
	objectScripts     map[string]config.ObjectScriptDef // Object type definitions
	collisionMap      []CollisionArea                   // World collision areas
	fillColor         [3]int
	assetMgr          *assets.AssetManager
	scriptMgr         *script.ScriptManager
	scriptImportPaths []string
}

type LayerTile struct {
	animation *animation.SpriteAnimation
	x         float64
	y         float64
}

// Background represents a scenic layer with parallax support
type Background struct {
	animation      *animation.SpriteAnimation
	tileAnimations []*animation.SpriteAnimation
	tiles          []LayerTile
	scrollX        float64 // Parallax factor (0=static, 1=with camera)
	scrollY        float64
}

// Foreground represents a decorative layer rendered on top of objects
type Foreground struct {
	animation      *animation.SpriteAnimation
	tileAnimations []*animation.SpriteAnimation
	tiles          []LayerTile
	scrollX        float64 // Parallax factor (0=static, 1=with camera)
	scrollY        float64
}

// CollisionArea represents a world collision area
type CollisionArea struct {
	id     string
	x, y   float64
	width  float64
	height float64
}

type objectPosition struct {
	x float64
	y float64
}

type collisionSides map[string]bool

func newCollisionSides() collisionSides {
	return collisionSides{
		"top":    false,
		"bottom": false,
		"left":   false,
		"right":  false,
	}
}

func (s collisionSides) any() bool {
	return s["top"] || s["bottom"] || s["left"] || s["right"]
}

type mapCollisionNotifier interface {
	NotifyMapCollision(side map[string]bool)
}

type objectCollisionNotifier interface {
	NotifyObjectCollision(other object.Object, side map[string]bool)
}

// NewWorld creates a new empty world
func NewWorld() (*World, error) {
	w := &World{
		name:              "TestWorld",
		width:             3392,
		height:            240,
		objects:           make([]object.Object, 0),
		backgrounds:       make([]Background, 0),
		foregrounds:       make([]Foreground, 0),
		collisionMap:      make([]CollisionArea, 0),
		camera:            camera.NewCamera(320, 200, 3392, 240),
		gravityX:          0,
		gravityY:          1.5,
		acceleration:      0.05,
		animationDefs:     make(map[string]config.AnimationDef),
		objectScripts:     make(map[string]config.ObjectScriptDef),
		fillColor:         [3]int{51, 255, 255},
		assetMgr:          assets.NewAssetManager(),
		scriptMgr:         script.NewScriptManager(),
		scriptImportPaths: []string{},
	}

	return w, nil
}

// NewWorldFromDef creates a world from a YAML definition
func NewWorldFromDef(def *config.WorldDef, dataDir string) (*World, error) {
	w := &World{
		name:              def.World.Name,
		width:             def.World.Dimensions.Width,
		height:            def.World.Dimensions.Height,
		objects:           make([]object.Object, 0),
		backgrounds:       make([]Background, 0),
		foregrounds:       make([]Foreground, 0),
		collisionMap:      make([]CollisionArea, 0),
		gravityX:          def.World.Mechanics.Gravity.X,
		gravityY:          def.World.Mechanics.Gravity.Y,
		acceleration:      def.World.Mechanics.Acceleration,
		animationDefs:     make(map[string]config.AnimationDef),
		objectScripts:     make(map[string]config.ObjectScriptDef),
		fillColor:         [3]int{def.World.FillColor.R, def.World.FillColor.G, def.World.FillColor.B},
		assetMgr:          assets.NewAssetManager(dataDir),
		scriptMgr:         script.NewScriptManager(),
		scriptImportPaths: def.World.ScriptImportPaths,
	}

	// Create camera from first camera definition
	if len(def.World.Cameras) > 0 {
		cam := def.World.Cameras[0]
		w.camera = camera.NewCamera(
			int(cam.Viewport.Width),
			int(cam.Viewport.Height),
			cam.Dimensions.Width,
			cam.Dimensions.Height,
		)
		w.camera.SetPosition(cam.Start.X, cam.Start.Y)
		w.camera.Speed = cam.Speed
		w.camera.Variance = cam.FollowVariance
		w.camera.SmoothScrolling = cam.SmoothScrolling
	} else {
		w.camera = camera.NewCamera(
			def.World.Resolution.Width,
			def.World.Resolution.Height,
			w.width,
			w.height,
		)
	}

	// Store animation definitions for later use
	for _, anim := range def.World.Animations {
		w.animationDefs[anim.ID] = anim
	}

	// Store object script definitions for later use
	for _, scriptDef := range def.World.ObjectScripts {
		w.objectScripts[scriptDef.ID] = scriptDef
	}

	// Store collision map areas
	for _, area := range def.World.CollisionMap.Areas {
		w.collisionMap = append(w.collisionMap, CollisionArea{
			id:     area.ID,
			x:      area.Position.X,
			y:      area.Position.Y,
			width:  area.Position.Width,
			height: area.Position.Height,
		})
	}

	// Load objects from world definition
	if len(def.World.Objects) > 0 {
		if err := w.populateWorldObjects(def.World.Objects, def.World.Backgrounds, def.World.Foregrounds); err != nil {
			return nil, fmt.Errorf("failed to populate world objects: %w", err)
		}
	} else {
		// Fallback to test objects for demonstration
		if err := w.populateTestWorld(); err != nil {
			return nil, fmt.Errorf("failed to populate test world: %w", err)
		}
	}

	return w, nil
}

// populateTestWorld adds test objects to the world for rendering demo
func (w *World) populateTestWorld() error {
	// Add background layers
	w.populateLegacyBackgrounds()

	// Add static tiles
	staticTiles := []string{"block", "brick", "question"}
	tileX := 50.0
	tileY := 200.0

	for i, tileName := range staticTiles {
		obj, err := w.CreateTestObject(tileName, tileX+float64(i*20), tileY)
		if err != nil {
			// Skip if animation not found
			continue
		}
		w.AddObject(obj)
	}

	// Add enemy sprites for animation testing
	if obj, err := w.CreateTestObject("goomba", 200, 200); err == nil {
		w.AddObject(obj)
	}

	if obj, err := w.CreateTestObject("turtle", 300, 200); err == nil {
		w.AddObject(obj)
	}

	return nil
}

// populateBackgrounds creates scenic layers with parallax
func (w *World) populateBackgrounds(defs []config.BackgroundDef) {
	if len(defs) == 0 {
		w.populateLegacyBackgrounds()
		return
	}

	for _, def := range defs {
		layer := Background{
			scrollX: def.ScrollX,
			scrollY: def.ScrollY,
			tiles:   make([]LayerTile, 0),
		}

		if def.Type == "animation" && def.Animation != "" {
			if animDef, ok := w.animationDefs[def.Animation]; ok {
				anim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
				if err == nil {
					layer.animation = anim
				}
			}
		}

		if def.Type == "tileset" || len(def.Tileset.Tiles) > 0 {
			tileAnimByID := make(map[string]*animation.SpriteAnimation)

			for _, tile := range def.Tileset.Tiles {
				anim, ok := tileAnimByID[tile.Animation]
				if !ok {
					animDef, exists := w.animationDefs[tile.Animation]
					if !exists {
						continue
					}

					newAnim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
					if err != nil {
						log.Printf("Skipping background tile animation '%s': %v", tile.Animation, err)
						continue
					}
					anim = newAnim
					tileAnimByID[tile.Animation] = anim
					layer.tileAnimations = append(layer.tileAnimations, anim)
				}

				tileX := tile.Position.X * def.Tileset.TileSize.X
				tileY := tile.Position.Y * def.Tileset.TileSize.Y
				layer.tiles = append(layer.tiles, LayerTile{animation: anim, x: tileX, y: tileY})
			}
		}

		if layer.animation != nil || len(layer.tiles) > 0 {
			w.backgrounds = append(w.backgrounds, layer)
		}
	}
}

func (w *World) populateLegacyBackgrounds() {
	// Background layer order (bottom to top): clouds, mountains, hills
	backgroundLayers := []struct {
		animID  string
		scrollX float64
		scrollY float64
	}{
		{"clouds", 0.3, 0.3},    // Very slow parallax
		{"mountains", 0.5, 0.5}, // Slow parallax
		{"hills", 0.7, 0.7},     // Medium parallax
	}

	for _, layer := range backgroundLayers {
		if animDef, ok := w.animationDefs[layer.animID]; ok {
			anim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
			if err == nil {
				w.backgrounds = append(w.backgrounds, Background{
					animation:      anim,
					tileAnimations: make([]*animation.SpriteAnimation, 0),
					tiles:          make([]LayerTile, 0),
					scrollX:        layer.scrollX,
					scrollY:        layer.scrollY,
				})
			}
		}
	}
}

// populateForegrounds creates decorative layers rendered on top of objects
func (w *World) populateForegrounds(defs []config.ForegroundDef) {
	for _, def := range defs {
		layer := Foreground{
			scrollX: def.ScrollX,
			scrollY: def.ScrollY,
			tiles:   make([]LayerTile, 0),
		}

		if def.Type == "animation" && def.Animation != "" {
			if animDef, ok := w.animationDefs[def.Animation]; ok {
				anim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
				if err == nil {
					layer.animation = anim
				}
			}
		}

		if def.Type == "tileset" || len(def.Tileset.Tiles) > 0 {
			tileAnimByID := make(map[string]*animation.SpriteAnimation)

			for _, tile := range def.Tileset.Tiles {
				anim, ok := tileAnimByID[tile.Animation]
				if !ok {
					animDef, exists := w.animationDefs[tile.Animation]
					if !exists {
						continue
					}

					newAnim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
					if err != nil {
						log.Printf("Skipping foreground tile animation '%s': %v", tile.Animation, err)
						continue
					}
					anim = newAnim
					tileAnimByID[tile.Animation] = anim
					layer.tileAnimations = append(layer.tileAnimations, anim)
				}

				tileX := tile.Position.X * def.Tileset.TileSize.X
				tileY := tile.Position.Y * def.Tileset.TileSize.Y
				layer.tiles = append(layer.tiles, LayerTile{animation: anim, x: tileX, y: tileY})
			}
		}

		if layer.animation != nil || len(layer.tiles) > 0 {
			w.foregrounds = append(w.foregrounds, layer)
		}
	}
}

// CreateObjectFromScript creates an object from an object script definition
func (w *World) CreateObjectFromScript(scriptDef config.ObjectScriptDef, x, y float64) (object.Object, error) {
	// Create the base object
	base := object.NewBaseObject()

	// Set position
	base.SetPosition(x, y)
	base.SetLabel(scriptDef.ID)

	animDef := scriptDef.Animation
	if animDef.ID == "" && len(scriptDef.Animations) > 0 {
		animDef = scriptDef.Animations[0]
	}
	if animDef.ID == "" {
		return nil, fmt.Errorf("object script '%s' has no animation definition", scriptDef.ID)
	}

	// Create the animation for this object
	anim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
	if err != nil {
		return nil, fmt.Errorf("failed to create animation for object '%s': %w", scriptDef.ID, err)
	}

	// Set the animation
	base.SetAnimation(anim)

	// Seed size + collision box from first frame
	if len(animDef.Frames) > 0 && len(animDef.Frames[0].Collisions) > 0 {
		cb := animDef.Frames[0].Collisions[0]
		base.SetSize(cb.Width, cb.Height)
		base.GetCollision().AddBox(cb.X, cb.Y, cb.Width, cb.Height)
	}

	if scriptDef.PhysicsType != "" {
		base.SetPhysicsType(scriptDef.PhysicsType)
	}

	if scriptDef.Module == "" {
		return base, nil
	}

	scriptPath, found := w.scriptMgr.FindScript(
		w.assetMgr.DataDir(), scriptDef.Module, w.scriptImportPaths,
	)
	if !found {
		return base, nil
	}

	scriptTable, err := w.scriptMgr.LoadScript(scriptPath)
	if err != nil {
		log.Printf("Script '%s' failed for object '%s'; running without behavior: %v",
			scriptDef.Module, scriptDef.ID, err)
		return base, nil
	}

	selfTable := w.scriptMgr.NewSelfTable(base)
	so := object.NewScriptedObject(base, w.scriptMgr.L, scriptTable, selfTable)

	if err := w.scriptMgr.CallInit(scriptTable, selfTable); err != nil {
		log.Printf("init() error for object '%s': %v", scriptDef.ID, err)
	}

	// If no collision boxes were seeded from frame data, fall back to the object's
	// logical dimensions — mirroring the C++ ScriptObject::getCollisionArea() fallback
	// which returns Area(x, y, width, height) when no animation area is defined.
	if len(base.GetCollision().Boxes) == 0 {
		w, h := base.GetSize()
		if w > 0 && h > 0 {
			base.GetCollision().AddBox(0, 0, w, h)
		}
	}

	return so, nil
}

// populateWorldObjects creates and places all objects from the world definition
func (w *World) populateWorldObjects(objDefs []config.ObjectDef, bgDefs []config.BackgroundDef, fgDefs []config.ForegroundDef) error {
	// Add background layers first
	w.populateBackgrounds(bgDefs)

	// Add foreground layers
	w.populateForegrounds(fgDefs)

	for _, objDef := range objDefs {
		// Look up the object script definition
		scriptDef, ok := w.objectScripts[objDef.Script]
		if !ok {
			// Skip unknown scripts
			continue
		}

		// Create object from script definition
		obj, err := w.CreateObjectFromScript(scriptDef, objDef.Position.X, objDef.Position.Y)
		if err != nil {
			log.Printf("Skipping object '%s' at (%.1f, %.1f): %v", objDef.Script, objDef.Position.X, objDef.Position.Y, err)
			// Skip objects that can't be created
			continue
		}
		w.AddObject(obj)
	}
	return nil
}

// mapTypeToAnimation maps object type names to animation IDs
func (w *World) mapTypeToAnimation(objType string) string {
	// Simple mapping from object type to animation ID
	animMap := map[string]string{
		"goomba":                   "goomba",
		"turtle":                   "turtle",
		"brick":                    "brick",
		"question-block":           "question",
		"invisible-question-block": "question",
	}

	if animID, ok := animMap[objType]; ok {
		return animID
	}
	return objType // fallback to using type as animation ID
}

// Update updates the world state
func (w *World) Update() {
	for _, bg := range w.backgrounds {
		if bg.animation != nil {
			bg.animation.Update()
		}
		for _, anim := range bg.tileAnimations {
			anim.Update()
		}
	}

	previousPositions := make([]objectPosition, len(w.objects))

	// Update all objects
	for i, obj := range w.objects {
		previousPositions[i].x, previousPositions[i].y = obj.GetPosition()
		obj.ResetContactState()
		obj.Act(w)
	}

	w.resolveMapCollisions(previousPositions)
	w.resolveObjectCollisions(previousPositions)

	for _, fg := range w.foregrounds {
		if fg.animation != nil {
			fg.animation.Update()
		}
		for _, anim := range fg.tileAnimations {
			anim.Update()
		}
	}

	// Update camera
	w.camera.Update()
}

func (w *World) resolveMapCollisions(previousPositions []objectPosition) {
	if len(w.collisionMap) == 0 {
		return
	}

	for i, obj := range w.objects {
		if len(obj.GetCollision().Boxes) == 0 {
			continue
		}

		prev := previousPositions[i]
		currentX, currentY := obj.GetPosition()
		resolvedX, horizontalSides := w.resolveMapAxis(obj, prev, currentX, prev.y, true)
		obj.SetPosition(resolvedX, prev.y)
		resolvedY, verticalSides := w.resolveMapAxis(obj, objectPosition{x: resolvedX, y: prev.y}, resolvedX, currentY, false)
		obj.SetPosition(resolvedX, resolvedY)

		sides := newCollisionSides()
		for key, value := range horizontalSides {
			if value {
				sides[key] = true
			}
		}
		for key, value := range verticalSides {
			if value {
				sides[key] = true
			}
		}

		if sides.any() {
			obj.MergeContactStateFromSides(sides)
			if notifier, ok := obj.(mapCollisionNotifier); ok {
				notifier.NotifyMapCollision(sides)
			}
		}
	}
}

func (w *World) resolveMapAxis(obj object.Object, previous objectPosition, targetX, targetY float64, horizontal bool) (float64, collisionSides) {
	resolvedX := targetX
	resolvedY := targetY
	sides := newCollisionSides()
	velocity := obj.GetPhysics()

	for _, area := range w.collisionMap {
		for _, box := range obj.GetCollision().Boxes {
			objRect := collision.Rect{X: resolvedX + box.X, Y: resolvedY + box.Y, Width: box.Width, Height: box.Height}
			areaRect := collision.Rect{X: area.x, Y: area.y, Width: area.width, Height: area.height}
			if !rectsOverlap(objRect, areaRect) {
				continue
			}

			if horizontal {
				if targetX > previous.x {
					candidate := area.x - box.X - box.Width
					resolvedX = math.Min(resolvedX, candidate)
					sides["left"] = true
					if velocity.VelocityX > 0 {
						velocity.VelocityX = 0
					}
				} else if targetX < previous.x {
					candidate := area.x + area.width - box.X
					resolvedX = math.Max(resolvedX, candidate)
					sides["right"] = true
					if velocity.VelocityX < 0 {
						velocity.VelocityX = 0
					}
				}
			} else {
				if targetY > previous.y {
					candidate := area.y - box.Y - box.Height
					resolvedY = math.Min(resolvedY, candidate)
					sides["top"] = true
					if velocity.VelocityY > 0 {
						velocity.VelocityY = 0
					}
				} else if targetY < previous.y {
					candidate := area.y + area.height - box.Y
					resolvedY = math.Max(resolvedY, candidate)
					sides["bottom"] = true
					if velocity.VelocityY < 0 {
						velocity.VelocityY = 0
					}
				}
			}
		}
	}

	if horizontal {
		return resolvedX, sides
	}
	return resolvedY, sides
}

func (w *World) resolveObjectCollisions(previousPositions []objectPosition) {
	for i := 0; i < len(w.objects); i++ {
		for j := i + 1; j < len(w.objects); j++ {
			objA := w.objects[i]
			objB := w.objects[j]
			sidesA, sidesB, ok := detectObjectCollision(objA, previousPositions[i], objB, previousPositions[j])
			if !ok {
				continue
			}

			resolveObjectPair(objA, objB, sidesA, sidesB)
			objA.MergeContactStateFromSides(sidesA)
			objB.MergeContactStateFromSides(sidesB)

			if notifier, ok := objA.(objectCollisionNotifier); ok && sidesA.any() {
				notifier.NotifyObjectCollision(objB, sidesA)
			}
			if notifier, ok := objB.(objectCollisionNotifier); ok && sidesB.any() {
				notifier.NotifyObjectCollision(objA, sidesB)
			}
		}
	}
}

func detectObjectCollision(objA object.Object, prevA objectPosition, objB object.Object, prevB objectPosition) (collisionSides, collisionSides, bool) {
	sidesA := newCollisionSides()
	sidesB := newCollisionSides()

	for _, boxA := range objA.GetCollision().Boxes {
		currARect := collision.Rect{X: objA.GetCollision().Boxes[0].X, Y: objA.GetCollision().Boxes[0].Y, Width: boxA.Width, Height: boxA.Height}
		_ = currARect
		for _, boxB := range objB.GetCollision().Boxes {
			currentA := collision.Rect{X: objPositionX(objA) + boxA.X, Y: objPositionY(objA) + boxA.Y, Width: boxA.Width, Height: boxA.Height}
			currentB := collision.Rect{X: objPositionX(objB) + boxB.X, Y: objPositionY(objB) + boxB.Y, Width: boxB.Width, Height: boxB.Height}
			if !rectsOverlap(currentA, currentB) {
				continue
			}

			previousA := collision.Rect{X: prevA.x + boxA.X, Y: prevA.y + boxA.Y, Width: boxA.Width, Height: boxA.Height}
			previousB := collision.Rect{X: prevB.x + boxB.X, Y: prevB.y + boxB.Y, Width: boxB.Width, Height: boxB.Height}
			inferredA, inferredB := inferCollisionSides(previousA, currentA, previousB, currentB)
			mergeCollisionSides(sidesA, sidesB, inferredA, inferredB)
		}
	}

	return sidesA, sidesB, sidesA.any() || sidesB.any()
}

func inferCollisionSides(previousA, currentA, previousB, currentB collision.Rect) (collisionSides, collisionSides) {
	sidesA := newCollisionSides()
	sidesB := newCollisionSides()

	switch {
	case previousA.Y+previousA.Height <= previousB.Y && currentA.Y+currentA.Height > currentB.Y:
		sidesA["top"] = true
		sidesB["bottom"] = true
	case previousA.Y >= previousB.Y+previousB.Height && currentA.Y < currentB.Y+currentB.Height:
		sidesA["bottom"] = true
		sidesB["top"] = true
	case previousA.X+previousA.Width <= previousB.X && currentA.X+currentA.Width > currentB.X:
		sidesA["left"] = true
		sidesB["right"] = true
	case previousA.X >= previousB.X+previousB.Width && currentA.X < currentB.X+currentB.Width:
		sidesA["right"] = true
		sidesB["left"] = true
	default:
		leftPen := math.Abs((currentA.X + currentA.Width) - currentB.X)
		rightPen := math.Abs((currentB.X + currentB.Width) - currentA.X)
		topPen := math.Abs((currentA.Y + currentA.Height) - currentB.Y)
		bottomPen := math.Abs((currentB.Y + currentB.Height) - currentA.Y)

		minPen := math.Min(math.Min(leftPen, rightPen), math.Min(topPen, bottomPen))
		switch minPen {
		case topPen:
			sidesA["top"] = true
			sidesB["bottom"] = true
		case bottomPen:
			sidesA["bottom"] = true
			sidesB["top"] = true
		case leftPen:
			sidesA["left"] = true
			sidesB["right"] = true
		default:
			sidesA["right"] = true
			sidesB["left"] = true
		}
	}

	return sidesA, sidesB
}

func mergeCollisionSides(dstA, dstB collisionSides, srcA, srcB collisionSides) {
	for key, value := range srcA {
		if value {
			dstA[key] = true
		}
	}
	for key, value := range srcB {
		if value {
			dstB[key] = true
		}
	}
}

func resolveObjectPair(objA, objB object.Object, sidesA, sidesB collisionSides) {
	staticA := objA.GetPhysicsType() == object.PhysicsTypeStatic
	staticB := objB.GetPhysicsType() == object.PhysicsTypeStatic

	switch {
	case staticA && staticB:
		return
	case staticA:
		resolveObjectAgainstStatic(objB, objA, sidesB)
	case staticB:
		resolveObjectAgainstStatic(objA, objB, sidesA)
	default:
		resolveObjectPairDynamic(objA, objB, sidesA, sidesB)
	}
}

// resolveObjectPairDynamic splits the overlap equally between two dynamic (or kinematic) objects.
// The penetration is halved and each object is pushed out on the contact axis; their velocities
// are zeroed on that axis to prevent tunnelling on subsequent frames.
func resolveObjectPairDynamic(objA, objB object.Object, sidesA, sidesB collisionSides) {
	if !sidesA.any() || len(objA.GetCollision().Boxes) == 0 || len(objB.GetCollision().Boxes) == 0 {
		return
	}

	boxA := objA.GetCollision().Boxes[0]
	boxB := objB.GetCollision().Boxes[0]
	ax, ay := objA.GetPosition()
	bx, by := objB.GetPosition()
	physA := objA.GetPhysics()
	physB := objB.GetPhysics()

	rectA := collision.Rect{X: ax + boxA.X, Y: ay + boxA.Y, Width: boxA.Width, Height: boxA.Height}
	rectB := collision.Rect{X: bx + boxB.X, Y: by + boxB.Y, Width: boxB.Width, Height: boxB.Height}

	if sidesA["top"] {
		// A landed on top of B — push A up, B down by equal halves.
		overlap := (rectA.Y + rectA.Height) - rectB.Y
		half := overlap / 2
		objA.SetPosition(ax, ay-half)
		objB.SetPosition(bx, by+half)
		if physA.VelocityY > 0 {
			physA.VelocityY = 0
		}
		if physB.VelocityY < 0 {
			physB.VelocityY = 0
		}
	} else if sidesA["bottom"] {
		// A hit B from below — push A down, B up.
		overlap := (rectB.Y + rectB.Height) - rectA.Y
		half := overlap / 2
		objA.SetPosition(ax, ay+half)
		objB.SetPosition(bx, by-half)
		if physA.VelocityY < 0 {
			physA.VelocityY = 0
		}
		if physB.VelocityY > 0 {
			physB.VelocityY = 0
		}
	} else if sidesA["left"] {
		// A moved into B from the left — push A left, B right.
		overlap := (rectA.X + rectA.Width) - rectB.X
		half := overlap / 2
		objA.SetPosition(ax-half, ay)
		objB.SetPosition(bx+half, by)
		if physA.VelocityX > 0 {
			physA.VelocityX = 0
		}
		if physB.VelocityX < 0 {
			physB.VelocityX = 0
		}
	} else if sidesA["right"] {
		// A moved into B from the right — push A right, B left.
		overlap := (rectB.X + rectB.Width) - rectA.X
		half := overlap / 2
		objA.SetPosition(ax+half, ay)
		objB.SetPosition(bx-half, by)
		if physA.VelocityX < 0 {
			physA.VelocityX = 0
		}
		if physB.VelocityX > 0 {
			physB.VelocityX = 0
		}
	}
}

func resolveObjectAgainstStatic(moving, obstacle object.Object, sides collisionSides) {
	if !sides.any() || len(moving.GetCollision().Boxes) == 0 || len(obstacle.GetCollision().Boxes) == 0 {
		return
	}

	moveBox := moving.GetCollision().Boxes[0]
	obstacleBox := obstacle.GetCollision().Boxes[0]
	mx, my := moving.GetPosition()
	ox, oy := obstacle.GetPosition()
	phys := moving.GetPhysics()

	if sides["top"] {
		moving.SetPosition(mx, oy+obstacleBox.Y-moveBox.Y-moveBox.Height)
		if phys.VelocityY > 0 {
			phys.VelocityY = 0
		}
	}
	if sides["bottom"] {
		moving.SetPosition(mx, oy+obstacleBox.Y+obstacleBox.Height-moveBox.Y)
		if phys.VelocityY < 0 {
			phys.VelocityY = 0
		}
	}
	if sides["left"] {
		moving.SetPosition(ox+obstacleBox.X-moveBox.X-moveBox.Width, my)
		if phys.VelocityX > 0 {
			phys.VelocityX = 0
		}
	}
	if sides["right"] {
		moving.SetPosition(ox+obstacleBox.X+obstacleBox.Width-moveBox.X, my)
		if phys.VelocityX < 0 {
			phys.VelocityX = 0
		}
	}
}

func rectsOverlap(a, b collision.Rect) bool {
	return a.X < b.X+b.Width &&
		a.X+a.Width > b.X &&
		a.Y < b.Y+b.Height &&
		a.Y+a.Height > b.Y
}

func objPositionX(obj object.Object) float64 {
	x, _ := obj.GetPosition()
	return x
}

func objPositionY(obj object.Object) float64 {
	_, y := obj.GetPosition()
	return y
}

// Draw renders the world
func (w *World) Draw(screen *ebiten.Image) {
	// Clear screen with fill color
	fillColor := color.RGBA{
		R: uint8(w.fillColor[0]),
		G: uint8(w.fillColor[1]),
		B: uint8(w.fillColor[2]),
		A: 0xFF,
	}
	screen.Fill(fillColor)

	// Draw background layers with parallax
	for _, bg := range w.backgrounds {
		if bg.animation != nil {
			// Calculate parallax offset
			bgX := w.camera.X * bg.scrollX
			bgY := w.camera.Y * bg.scrollY
			bg.animation.Draw(screen, -bgX, -bgY)
		}
		for _, tile := range bg.tiles {
			if tile.animation == nil {
				continue
			}
			tileX := tile.x - (w.camera.X * bg.scrollX)
			tileY := tile.y - (w.camera.Y * bg.scrollY)
			tile.animation.Draw(screen, tileX, tileY)
		}
	}

	// Draw all objects
	for _, obj := range w.objects {
		obj.Draw(screen, w.camera)
	}

	// Draw foreground layers with parallax (on top of objects)
	for _, fg := range w.foregrounds {
		if fg.animation != nil {
			// Calculate parallax offset
			fgX := w.camera.X * fg.scrollX
			fgY := w.camera.Y * fg.scrollY
			fg.animation.Draw(screen, -fgX, -fgY)
		}
		for _, tile := range fg.tiles {
			if tile.animation == nil {
				continue
			}
			tileX := tile.x - (w.camera.X * fg.scrollX)
			tileY := tile.y - (w.camera.Y * fg.scrollY)
			tile.animation.Draw(screen, tileX, tileY)
		}
	}
}

// AddObject adds an object to the world
func (w *World) AddObject(obj object.Object) {
	w.objects = append(w.objects, obj)
}

// AddBackground adds a background layer with parallax support
func (w *World) AddBackground(anim *animation.SpriteAnimation, scrollX, scrollY float64) {
	if anim != nil {
		w.backgrounds = append(w.backgrounds, Background{
			animation:      anim,
			tileAnimations: make([]*animation.SpriteAnimation, 0),
			tiles:          make([]LayerTile, 0),
			scrollX:        scrollX,
			scrollY:        scrollY,
		})
	}
}

// GetGravity returns the world gravity
func (w *World) GetGravity() float64 {
	return w.gravityY
}

// GetGravityVector returns the world gravity vector.
func (w *World) GetGravityVector() (float64, float64) {
	return w.gravityX, w.gravityY
}

// GetAcceleration returns the world's acceleration rate used to apply gravity over time.
func (w *World) GetAcceleration() float64 {
	return w.acceleration
}

// GetCamera returns the world camera
func (w *World) GetCamera() *camera.Camera {
	return w.camera
}

// GetAssetManager returns the asset manager
func (w *World) GetAssetManager() *assets.AssetManager {
	return w.assetMgr
}

// GetAnimationDef returns an animation definition by ID
func (w *World) GetAnimationDef(id string) (config.AnimationDef, bool) {
	def, ok := w.animationDefs[id]
	return def, ok
}

// CreateTestObject creates a test sprite object for demonstration
func (w *World) CreateTestObject(animID string, x, y float64) (object.Object, error) {
	animDef, ok := w.animationDefs[animID]
	if !ok {
		return nil, fmt.Errorf("animation '%s' not found", animID)
	}

	anim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
	if err != nil {
		return nil, fmt.Errorf("failed to create animation: %w", err)
	}

	obj := object.NewBaseObject()
	obj.SetPosition(x, y)
	obj.SetLabel(animID)
	obj.SetAnimation(anim)

	return obj, nil
}

// CreatePlayerStub creates a temporary controllable player object.
//
// It first prefers YAML object script IDs intended for player control
// (`mario-stub`, then `player-stub`). Those should be defined without a module
// so no Lua behavior overrides input-driven movement. If unavailable, it falls
// back to unscripted goomba/turtle animation data.
func (w *World) CreatePlayerStub(x, y float64) (object.Object, error) {
	for _, id := range []string{"mario-stub", "player-stub"} {
		if scriptDef, ok := w.objectScripts[id]; ok {
			obj, err := w.CreateObjectFromScript(scriptDef, x, y)
			if err != nil {
				return nil, err
			}
			obj.SetLabel("player-stub")
			return obj, nil
		}
	}

	var scriptDef config.ObjectScriptDef
	var found bool

	for _, id := range []string{"goomba", "turtle"} {
		if def, ok := w.objectScripts[id]; ok {
			scriptDef = def
			found = true
			break
		}
	}

	if !found {
		return nil, fmt.Errorf("no goomba/turtle script definition available for player stub")
	}

	animDef := scriptDef.Animation
	if animDef.ID == "" && len(scriptDef.Animations) > 0 {
		animDef = scriptDef.Animations[0]
	}
	if animDef.ID == "" {
		return nil, fmt.Errorf("player stub source script '%s' has no animation definition", scriptDef.ID)
	}

	anim, err := animation.NewSpriteAnimation(animDef, w.assetMgr)
	if err != nil {
		return nil, fmt.Errorf("failed to create player stub animation '%s': %w", animDef.ID, err)
	}

	base := object.NewBaseObject()
	base.SetPosition(x, y)
	base.SetLabel("player-stub")
	base.SetAnimation(anim)
	base.SetPhysicsType(object.PhysicsTypeDynamic)

	// Prefer explicit frame collision data; fall back to animation dimensions.
	if len(animDef.Frames) > 0 && len(animDef.Frames[0].Collisions) > 0 {
		cb := animDef.Frames[0].Collisions[0]
		base.SetSize(cb.Width, cb.Height)
		base.GetCollision().AddBox(cb.X, cb.Y, cb.Width, cb.Height)
	} else {
		w, h := base.GetSize()
		if w <= 0 || h <= 0 {
			w = 16
			h = 16
			base.SetSize(w, h)
		}
		base.GetCollision().AddBox(0, 0, w, h)
	}

	return base, nil
}
