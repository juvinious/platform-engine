#ifndef _platformer_argument_h
#define _platformer_argument_h

#include "util/argument.h"
#include "util/pointer.h"

namespace Platformer{
    std::vector< ::Util::ReferenceCount<Argument> > arguments();
}

#endif
