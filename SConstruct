import os

#dirs = ['', '/game', '/script', '/resources'] 

#includedirs = [] 

#for d in dirs:
#    includedirs.append('{0}/src{1}'.format(os.getcwd(), d))

def getDebug():
    try:
        return int(os.environ['DEBUG'])
    except KeyError:
        return 0

VariantDir('build', 'src', duplicate=0)

includedirs = '{0}/src'.format(os.getcwd())

env = Environment(ENV = os.environ, CPPPATH=includedirs, CPPDEFINES = ['USE_ALLEGRO5', 'HAVE_PYTHON', 'DEBUG' if getDebug() else '' ])

env.ParseConfig('pkg-config {0} python zlib --cflags --libs'.format('r-tech1-debug' if getDebug() else 'r-tech1'))

source = ['src/main.cpp',
        'src/argument.cpp',
        'src/game/game.cpp',
        'src/game/options.cpp',
        'src/game/world.cpp',
        'src/resources/animation.cpp',
        'src/resources/background.cpp',
        'src/resources/camera.cpp',
        'src/resources/collisions.cpp',
        'src/resources/font.cpp',
        'src/resources/object.cpp',
        'src/resources/tile.cpp',
        'src/resources/value.cpp',
        'src/script/control.cpp',
        'src/script/object.cpp',
        'src/script/python.cpp',
        'src/script/script.cpp']

env.Program('platformer', [s.replace('src', 'build') for s in source])
