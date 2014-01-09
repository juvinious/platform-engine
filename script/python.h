#ifndef platformer_script_python_h
#define platformer_script_python_h

#ifdef HAVE_PYTHON
#include <Python.h>

#include <string>
#include <map>

namespace Platformer{
namespace Script{

class AutoRef{
public:
    AutoRef(PyObject *, bool increaseRef = false);
    AutoRef(const AutoRef &);
    ~AutoRef();
    
    const AutoRef & operator=(const AutoRef &);
    
    PyObject * getObject() const;
    
private:
    PyObject * object;
};

typedef std::map<std::string, AutoRef> RefMap;

class Module{
public:
    Module(const std::string &);
    Module(const Module &);
    
    const Module & operator=(const Module &);
    
    void addFunction(const std::string &) const;
    
    const AutoRef getFunction(const std::string &) const;
    
    inline const std::string & getName() const {
        return this->name;
    }
    
private:
    std::string name;
    AutoRef module;
    mutable RefMap functions;
};

typedef std::map<std::string, Module> ModuleMap;

class Runnable{
public:
    Runnable(const AutoRef object);
    Runnable(const std::string &, const std::string &);
    Runnable(const Runnable &);
    ~Runnable();
    
    const Runnable & operator=(const Runnable &);
    
    const Module getModule() const;
    
    const AutoRef getFunction() const;
    
    inline const std::string & getModuleName() const {
        return this->moduleName;
    }
    
    inline const std::string & getFunctionName() const {
        return this->moduleName;
    }
    
private:
    std::string moduleName;
    std::string functionName;
    static ModuleMap modules;
    
    bool isDirect;
    AutoRef direct;
};

typedef std::map<std::string, Runnable> RunMap;

}
}

#endif
#endif
