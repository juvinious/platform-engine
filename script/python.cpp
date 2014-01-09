#ifdef HAVE_PYTHON

#include "python.h"

#include "util/file-system.h"
#include "util/funcs.h"

using namespace Platformer::Script;

AutoRef::AutoRef(PyObject * object, bool increaseRef):
object(object){
    if (increaseRef){
        Py_INCREF(object);
    }
}

AutoRef::AutoRef(const AutoRef & copy):
object(copy.object){
    Py_XINCREF(object);
}

AutoRef::~AutoRef(){
    Py_XDECREF(object);
}

const AutoRef & AutoRef::operator=(const AutoRef & copy){
    object = copy.object;
    Py_XINCREF(object);
    
    return *this;
}

PyObject * AutoRef::getObject() const {
    return object;
}

Module::Module(const std::string & name):
name(name),
module(NULL){
    PyObject * sysPath = PySys_GetObject((char *)"path");
    // FIXME Do not use a fixed location but for now make it data/platformer
    PyObject * path = PyString_FromString(Storage::instance().find(Filesystem::RelativePath("platformer/")).path().c_str());
    int insertResult = PyList_Insert(sysPath, 0, path);
    
    // Import the module
    PyObject * importedModule = PyImport_ImportModule(name.c_str());
    if (PyErr_Occurred()){
        PyErr_Print();
    }
    
    module = AutoRef(importedModule);
}

Module::Module(const Module & copy):
name(copy.name),
module(copy.module),
functions(copy.functions){
}

const Module & Module::operator=(const Module & copy){
    name = copy.name;
    module = copy.module;
    functions = copy.functions;
    
    return *this;
}

void Module::addFunction(const std::string & functionName) const{
    // Check if added already
    RefMap::iterator check = functions.find(functionName);
    if (check == functions.end()){
        PyObject * retrievedFunction = PyObject_GetAttrString(module.getObject(), functionName.c_str());
        if (retrievedFunction == NULL){
            PyErr_Print();
        }
        functions.insert(std::pair<std::string, AutoRef>(functionName, AutoRef(retrievedFunction)));
    }
}

const AutoRef Module::getFunction(const std::string & functionName) const{
    RefMap::iterator function = functions.find(functionName);
    if (function != functions.end()){
        return function->second;
    }
    addFunction(functionName);
    return getFunction(functionName);
}

ModuleMap Runnable::modules;

Runnable::Runnable(const AutoRef object):
isDirect(true),
direct(object){
}

Runnable::Runnable(const std::string & moduleName, const std::string & functionName):
moduleName(moduleName),
functionName(functionName),
isDirect(false),
direct(NULL){
    ModuleMap::iterator module = modules.find(moduleName);
    if (module == modules.end()){
        // Insert
        Module newModule = Module(moduleName);
        newModule.addFunction(functionName);
        modules.insert(std::pair<std::string, Module>(moduleName, newModule));
    } else {
        Module update = module->second;
        update.addFunction(functionName);
        module->second = update;
    }
}

Runnable::Runnable(const Runnable & copy):
moduleName(copy.moduleName),
functionName(copy.functionName),
isDirect(copy.isDirect),
direct(copy.direct){
}

Runnable::~Runnable(){
}

const Runnable & Runnable::operator=(const Runnable & copy){
    moduleName = copy.moduleName;
    functionName = copy.functionName;
    isDirect = copy.isDirect;
    direct = copy.direct;
    
    return *this;
}

const Module Runnable::getModule() const{
    return modules.find(moduleName)->second;
}

const AutoRef Runnable::getFunction() const{
    if (!isDirect){
        return modules.find(moduleName)->second.getFunction(functionName);
    }
    return direct;
}

#endif
