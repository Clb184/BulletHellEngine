#include "Audio.h"

static Clb184::CMusicStream* g_pMusic;

static Clb184::CSoundBuffer* g_pSoundBuf;
static Clb184::CSound* g_pSound;

SQInteger LoadMusic(HSQUIRRELVM v) {
    const SQChar* pName;
    sq_getstring(v, -1, &pName);
    g_pMusic->Load(pName); 
    g_pMusic->SetLoop(true);
    return 0;
}

SQInteger SetMusicLoopPoints(HSQUIRRELVM v) {
    SQInteger loop_begin;
    SQInteger loop_end;

    sq_getinteger(v, -2, &loop_begin);
    sq_getinteger(v, -1, &loop_end);
    g_pMusic->SetLoopPoints(loop_begin, loop_end);
    return 0;
}

SQInteger SetMusicLoopState(HSQUIRRELVM v) {
    SQBool state;
    sq_getbool(v, -1, &state);
    g_pMusic->SetLoop(state);
    return 0;
}

SQInteger PlayMusic(HSQUIRRELVM v) {
    g_pMusic->Play(0.0f);
    return 0;
}

SQInteger PauseMusic(HSQUIRRELVM v) {
    g_pMusic->Pause();
    return 0;
}

SQInteger StopMusic(HSQUIRRELVM v) {
    g_pMusic->Stop();
    return 0;
}

SQInteger LoadSound(HSQUIRRELVM v) {
    SQInteger idx;
    const SQChar* name;
    SQInteger numbuf;
    sq_getinteger(v, -3, &idx);
    sq_getstring(v, -2, &name);
    sq_getinteger(v, -1, &numbuf);
    if (idx < SOUND_MAX) {
        if (g_pSoundBuf[idx].Load(name)) {
            g_pSound[idx].CreateVoice(numbuf, &g_pSoundBuf[idx]);
        }
    }
    return 0;
}

SQInteger PlaySound(HSQUIRRELVM v) {
    constexpr float delta = 1.0f / 512.0f;
    SQInteger idx;
    SQFloat pos = 0.0f;
    switch (sq_gettop(v)) {
    case 3:
        sq_getfloat(v, 3, &pos);
        pos *= delta;
    case 2:
        sq_getinteger(v, 2, &idx);
        if (idx < SOUND_MAX) {
            g_pSound[idx].Play(pos);

        }
        break;
    }
    return 0;
}

void InitAudio() {
    g_pMusic = new Clb184::CMusicStream();
    g_pSoundBuf = new Clb184::CSoundBuffer[SOUND_MAX];
    g_pSound = new Clb184::CSound[SOUND_MAX];
}

void DestroyAudio() {
    delete g_pMusic;
    delete[] g_pSoundBuf;
    delete[] g_pSound;
}
