package tests

import (
	"testing"

	"platformer/internal/assets"
)

// TestAssetManagerLoadImage tests that the asset manager can load an image
func TestAssetManagerLoadImage(t *testing.T) {
	am := assets.NewAssetManager()

	imagePath := GetImagePath(t, "block-tile.png")

	img, err := am.LoadImage(imagePath)
	if err != nil {
		t.Fatalf("failed to load image: %v", err)
	}

	if img == nil {
		t.Fatal("loaded image is nil")
	}

	// Check image dimensions are reasonable
	bounds := img.Bounds()
	if bounds.Dx() <= 0 || bounds.Dy() <= 0 {
		t.Fatalf("invalid image dimensions: %d x %d", bounds.Dx(), bounds.Dy())
	}

	t.Logf("Loaded image: %d x %d pixels", bounds.Dx(), bounds.Dy())
}

// TestAssetManagerCaching tests that the asset manager caches loaded images
func TestAssetManagerCaching(t *testing.T) {
	am := assets.NewAssetManager()

	imagePath := GetImagePath(t, "block-tile.png")

	// Load the same image twice
	img1, err := am.LoadImage(imagePath)
	if err != nil {
		t.Fatalf("first load failed: %v", err)
	}

	img2, err := am.LoadImage(imagePath)
	if err != nil {
		t.Fatalf("second load failed: %v", err)
	}

	// Should be the same pointer (cached)
	if img1 != img2 {
		t.Fatal("cached images should be the same pointer")
	}

	t.Log("Image caching works correctly")
}

// TestAssetManagerMultipleImages tests loading multiple different images
func TestAssetManagerMultipleImages(t *testing.T) {
	am := assets.NewAssetManager()

	images := []string{"block-tile.png", "brick-tile.png", "goomba-walk-01.png"}

	for _, imgName := range images {
		imgPath := GetImagePath(t, imgName)
		img, err := am.LoadImage(imgPath)
		if err != nil {
			t.Fatalf("failed to load %s: %v", imgName, err)
		}

		if img == nil {
			t.Fatalf("loaded image %s is nil", imgName)
		}

		t.Logf("Loaded %s: %d x %d", imgName, img.Bounds().Dx(), img.Bounds().Dy())
	}
}

// TestAssetManagerInvalidPath tests error handling for missing files
func TestAssetManagerInvalidPath(t *testing.T) {
	am := assets.NewAssetManager()

	_, err := am.LoadImage("/nonexistent/path/fake-image.png")
	if err == nil {
		t.Fatal("should return error for nonexistent image")
	}

	t.Logf("Correctly returned error for missing file: %v", err)
}
