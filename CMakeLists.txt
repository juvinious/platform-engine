# -------------------------------------------------------
# util cmake build script for paintown.
# Written by: juvinious
# Modified by: kazzmir
# -------------------------------------------------------

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------

set(ARGUMENT_SRC
argument.cpp)

set(GAME_SRC
game/animation.cpp
game/background.cpp
game/camera.cpp
game/collision-map.cpp
game/game.cpp
game/options.cpp
game/tile.cpp
game/world.cpp)

set(OBJECT_SRC
object/object.cpp)

add_subdirectory(script)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (platformer_module ${ARGUMENT_SRC} ${GAME_SRC} ${OBJECT_SRC})
