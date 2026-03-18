package main

import (
	"log"

	"platformer/internal/game"

	"github.com/hajimehoshi/ebiten/v2"
)

func main() {
	g, vid, err := game.NewGame("config.yaml")
	if err != nil {
		log.Fatalf("Failed to create game: %v", err)
	}

	ebiten.SetWindowSize(vid.WindowWidth, vid.WindowHeight)
	ebiten.SetWindowTitle(vid.Title)
	ebiten.SetFullscreen(vid.Fullscreen)

	if err := ebiten.RunGame(g); err != nil && err != ebiten.Termination {
		log.Fatalf("Error running game: %v", err)
	}
}
