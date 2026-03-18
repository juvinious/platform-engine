package tests

import (
	"os"
	"path/filepath"
	"testing"

	"platformer/internal/config"
)

// GetTestDataDir returns the path to the test data directory
func GetTestDataDir(t *testing.T) string {
	dir := GetSMBDir(t)
	return filepath.Join(dir, "images")
}

// GetSMBDir returns the path to the SMB data directory
func GetSMBDir(t *testing.T) string {
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("failed to get working directory: %v", err)
	}

	// Try to find SMB directory
	candidates := []string{
		filepath.Join(wd, "data/platformer/smb"),
		filepath.Join(wd, "../data/platformer/smb"),
		filepath.Join(wd, "../../data/platformer/smb"),
	}

	for _, candidate := range candidates {
		if stat, err := os.Stat(candidate); err == nil && stat.IsDir() {
			return candidate
		}
	}

	t.Fatalf("could not find data/platformer/smb directory")
	return ""
}

// GetTestDataDir returns the path to the test data directory
func GetTestDataDirOld(t *testing.T) string {
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("failed to get working directory: %v", err)
	}

	// Try to find data directory - works from both project root and game/ subdirectory
	candidates := []string{
		filepath.Join(wd, "data/platformer/smb/images"),
		filepath.Join(wd, "../data/platformer/smb/images"),
		filepath.Join(wd, "../../data/platformer/smb/images"),
	}

	for _, candidate := range candidates {
		if stat, err := os.Stat(candidate); err == nil && stat.IsDir() {
			return candidate
		}
	}

	t.Fatalf("could not find test data directory in candidates: %v", candidates)
	return ""
}

// GetImagePath returns the full path to a test image
func GetImagePath(t *testing.T, filename string) string {
	dataDir := GetTestDataDir(t)
	return filepath.Join(dataDir, filename)
}

// CreateTestAnimationDef creates a test animation definition
func CreateTestAnimationDef(id, basedir string, images []string, frameDuration int) config.AnimationDef {
	frames := make([]config.FrameDef, len(images))
	for i := range images {
		frames[i] = config.FrameDef{
			ImageIndex: i,
			Alpha:      255,
			HFlip:      false,
			VFlip:      false,
			Duration:   frameDuration,
		}
	}

	return config.AnimationDef{
		ID:      id,
		BaseDir: basedir,
		Images:  images,
		Frames:  frames,
		Loop:    true,
	}
}

// CreateBlockAnimationDef creates the block animation definition from test data
func CreateBlockAnimationDef(t *testing.T) config.AnimationDef {
	dataDir := GetTestDataDir(t)
	return config.AnimationDef{
		ID:      "block",
		BaseDir: dataDir + "/",
		Images:  []string{"block-tile.png"},
		Frames: []config.FrameDef{
			{
				ImageIndex: 0,
				Alpha:      255,
				HFlip:      false,
				VFlip:      false,
				Duration:   -1,
			},
		},
		Loop: false,
	}
}

// CreateGoombaAnimationDef creates the goomba animation definition from test data
func CreateGoombaAnimationDef(t *testing.T) config.AnimationDef {
	dataDir := GetTestDataDir(t)
	return config.AnimationDef{
		ID:      "goomba",
		BaseDir: dataDir + "/",
		Images:  []string{"goomba-walk-01.png", "goomba-walk-02.png"},
		Frames: []config.FrameDef{
			{
				ImageIndex: 0,
				Alpha:      255,
				HFlip:      false,
				VFlip:      false,
				Duration:   15,
			},
			{
				ImageIndex: 1,
				Alpha:      255,
				HFlip:      false,
				VFlip:      false,
				Duration:   15,
			},
		},
		Loop: true,
	}
}
