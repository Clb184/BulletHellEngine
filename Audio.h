#ifndef AUDIO_INCLUDED
#define AUDIO_INCLUDED
#include "SqHandlers.h"
#include "ClbLibMain.h"

#define SOUND_MAX 64

SQInteger LoadMusic(HSQUIRRELVM v);
SQInteger SetMusicLoopPoints(HSQUIRRELVM v);
SQInteger SetMusicLoopState(HSQUIRRELVM v);
SQInteger PlayMusic(HSQUIRRELVM v);
SQInteger PauseMusic(HSQUIRRELVM v);
SQInteger StopMusic(HSQUIRRELVM v);

SQInteger LoadSound(HSQUIRRELVM v);
SQInteger PlaySound(HSQUIRRELVM v);

void InitAudio();
void DestroyAudio();

inline void RegisterAudio(HSQUIRRELVM v) {
	RegisterSQFunc(v, LoadMusic, _SC("LoadMusic"));
	RegisterSQFunc(v, SetMusicLoopPoints, _SC("SetMusicLoopPoints"));
	RegisterSQFunc(v, SetMusicLoopState, _SC("SetMusicLoopState"));
	RegisterSQFunc(v, PlayMusic, _SC("PlayMusic"));
	RegisterSQFunc(v, PauseMusic, _SC("PauseMusic"));
	RegisterSQFunc(v, StopMusic, _SC("StopMusic"));

	RegisterSQFunc(v, LoadSound, _SC("LoadSound"));
	RegisterSQFunc(v, PlaySound, _SC("PlaySound"));
}

#endif // !1
