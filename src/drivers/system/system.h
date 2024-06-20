#ifndef SYSTEM_H
#define SYSTEM_H

#include <parts.h>

#if SAMD20
#include "sam0/system.h"
#elif XMEGA
#include "xmega/system.h"
#elif MEGA
#include "mega/system.h"
#else
#error Unsupported chip type
#endif

#endif
