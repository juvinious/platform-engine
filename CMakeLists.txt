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
game/game.cpp
game/options.cpp
game/world.cpp)

set(RESOURCES_SRC
resources/animation.cpp
resources/background.cpp
resources/camera.cpp
resources/collisions.cpp
resources/object.cpp
resources/tile.cpp
resources/value.cpp)

add_subdirectory(script)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (platformer_module ${ARGUMENT_SRC} ${GAME_SRC} ${RESOURCES_SRC})
