package assets

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	_ "image/png"
	"os"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
)

// AssetManager handles loading and caching of game assets
type AssetManager struct {
	images  map[string]*ebiten.Image
	fonts   map[string]interface{} // TODO: Font type
	dataDir string
}

// NewAssetManager creates a new asset manager
func NewAssetManager(dataDirs ...string) *AssetManager {
	dataDir := ""
	if len(dataDirs) > 0 {
		dataDir = filepath.Clean(dataDirs[0])
	}

	return &AssetManager{
		images:  make(map[string]*ebiten.Image),
		fonts:   make(map[string]interface{}),
		dataDir: dataDir,
	}
}

// SetDataDir sets the base directory used for relative asset paths.
func (a *AssetManager) SetDataDir(dataDir string) {
	a.dataDir = filepath.Clean(dataDir)
}

// DataDir returns the base data directory.
func (a *AssetManager) DataDir() string {
	return a.dataDir
}

func (a *AssetManager) resolvePath(path string) string {
	if filepath.IsAbs(path) {
		return filepath.Clean(path)
	}

	cleanPath := filepath.Clean(path)
	if a.dataDir == "" {
		return cleanPath
	}

	joined := filepath.Join(a.dataDir, cleanPath)
	if _, err := os.Stat(joined); err == nil {
		return joined
	}

	return cleanPath
}

// LoadImage loads an image from a file path
func (a *AssetManager) LoadImage(path string) (*ebiten.Image, error) {
	resolvedPath := a.resolvePath(path)

	// Check if already loaded
	if img, ok := a.images[resolvedPath]; ok {
		return img, nil
	}

	// Load the image
	file, err := os.Open(resolvedPath)
	if err != nil {
		return nil, fmt.Errorf("failed to open image %s (resolved from %s): %w", resolvedPath, path, err)
	}
	defer file.Close()

	img, _, err := image.Decode(file)
	if err != nil {
		return nil, fmt.Errorf("failed to decode image %s: %w", resolvedPath, err)
	}

	// Apply classic color-key masking for legacy art.
	// Pure magenta (255,0,255) becomes fully transparent.
	img = applyMagentaMask(img)

	ebitenImg := ebiten.NewImageFromImage(img)
	a.images[resolvedPath] = ebitenImg

	return ebitenImg, nil
}

func applyMagentaMask(src image.Image) image.Image {
	bounds := src.Bounds()
	dst := image.NewNRGBA(bounds)
	draw.Draw(dst, bounds, src, bounds.Min, draw.Src)

	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			r, g, b, a := dst.At(x, y).RGBA()
			if a == 0 {
				continue
			}
			if uint8(r>>8) == 255 && uint8(g>>8) == 0 && uint8(b>>8) == 255 {
				dst.Set(x, y, color.NRGBA{R: 255, G: 0, B: 255, A: 0})
			}
		}
	}

	return dst
}

// GetImage retrieves a previously loaded image
func (a *AssetManager) GetImage(path string) (*ebiten.Image, error) {
	if img, ok := a.images[path]; ok {
		return img, nil
	}
	return a.LoadImage(path)
}

// ClearImages clears all loaded images from memory
func (a *AssetManager) ClearImages() {
	for key := range a.images {
		delete(a.images, key)
	}
}
