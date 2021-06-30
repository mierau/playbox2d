#ifndef PLAYBOX_PLATFORM_H
#define PLAYBOX_PLATFORM_H

#include "pd_api.h"

extern PlaydateAPI* pd;

#ifndef pb_alloc
#define pb_alloc(x) pd->system->realloc(NULL, (x))
#endif
#ifndef pb_free
#define pb_free(a) pd->system->realloc((a), 0)
#endif
#ifndef pb_calloc
#define pb_calloc(a, b) pd->system->realloc(NULL, ((a) * (b)))
#endif
#ifndef pb_realloc
#define pb_realloc pd->system->realloc
#endif

#ifndef pb_log
#define pb_log(s, ...) pd->system->logToConsole((s), ##__VA_ARGS__)
#endif

#ifndef PBPositionCorrection
#define PBPositionCorrection 1
#endif

#ifndef PBWarmStarting
#define PBWarmStarting 0
#endif

#ifndef PBAccumulateImpulses
#define PBAccumulateImpulses 1
#endif

#endif