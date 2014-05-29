#include <boost/date_time/posix_time/posix_time.hpp>

#include <interface.h>
#include "eiface.h"
#include "engine/IEngineTrace.h"
#include "defines.h"

#undef min
#undef max

#ifdef USE_BOOST_THREADS
#include <boost/thread/thread.hpp>
#else
#include <jobthread.h>
IThreadPool* threadPool;
#endif

#ifdef FILEBUG
	FILE *pDebugFile = NULL;
#endif

IVEngineServer *engine = NULL;
IEngineTrace *enginetrace = NULL;
