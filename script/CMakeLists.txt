# -------------------------------------------------------
# scripting specific cmake module
# Written by: juvinious
# -------------------------------------------------------

# -------
# macros
# ------
include(TestForPythonEmbed)

# Python
if(PYTHON_EMBEDABLE)
	set(PY_INCLUDE ${PYTHON_INCLUDE_PATH})
	set(PY_LIBS ${PYTHON_LIBRARIES})
	add_definitions(-DHAVE_PYTHON)
endif(PYTHON_EMBEDABLE)

# Source
set(SCRIPT_SRC
script/script.cpp
script/object.cpp
script/python.cpp)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(${PY_INCLUDE})

# -------------------------------------------------------
# module
# -------------------------------------------------------
foreach(src_file ${SCRIPT_SRC})
        string(REPLACE "script/" "" src_file ${src_file})
        list(APPEND SRC ${src_file})
endforeach(src_file ${SCRIPT_SRC})
add_library(platformer_script_module ${SRC})
target_link_libraries(platformer_script_module ${PY_LIBS})
