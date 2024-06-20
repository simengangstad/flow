#ifndef RESET_H
#define RESET_H

#include <parts.h>

#if XMEGA
#include "xmega/reset.h"
#elif MEGA || MEGA_RF
#include "mega/reset.h"
#elif SAMD20
#include "sam0/reset.h"
#else
#error Unsupported chip type
#endif

#endif
