Import('use')

import helpers
import os.path
modules = helpers.read_cmake_list(os.path.join(Dir('#src/platformer').abspath, 'CMakeLists.txt'))
modules.update(helpers.read_cmake_list(os.path.join(Dir('#src/platformer/script').abspath, 'CMakeLists.txt')))

def unique(lst):
    m = []
    for item in lst:
        if not item in m:
            m.append(item)
    return m

source = []
for module in modules:
    if module == 'SCRIPT_SRC':
        # call the python and ruby checks here
        env = use.Clone()
        # config = env.Configure()
        # Ugly way to pass the custom tests in
        config = env.Configure(custom_tests = env['PAINTOWN_TESTS'])
        # config.CheckRuby()
        config.CheckPython()
        env = config.Finish()
        use.Append(LIBS = env['LIBS'])
        use.Replace(LIBS = unique(use['LIBS']))
        files = modules[module]
        # abstract this so we don't duplicate code
        source.append(map(lambda x: env.StaticObject(x.replace('src/', '')), files))
    else:
        source.append(modules[module])

library = use.StaticLibrary('platformer-engine', source)
use.Append(ARCHIVES = library)
Return('library')
