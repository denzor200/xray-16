#include "stdafx.h"

#include "SoundRender_Core.h"
#include "SoundRender_CoreA.h"

XRSOUND_API xr_token* snd_devices_token = nullptr;
XRSOUND_API u32 snd_device_id = u32(-1);

void ISoundManager::_create()
{
    SoundRenderA = new CSoundRender_CoreA();
    SoundRender = SoundRenderA;
    GEnv.Sound = SoundRender;
    SoundRender->bPresent = strstr(Core.Params, "-nosound") == nullptr;
    if (!SoundRender->bPresent)
        return;
    GEnv.Sound->_initialize();
}

void ISoundManager::_destroy()
{
    GEnv.Sound->_clear();
    xr_delete(SoundRender);
    GEnv.Sound = nullptr;
}


void ref_sound::create(const xr_string& name, esound_type sound_type, int game_type, bool replaceWithNoSound)
{
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::create_internal, this, name, sound_type, game_type, replaceWithNoSound));
}

void ref_sound::attach_tail(const xr_string& name)
{
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::attach_tail_internal, this, name));
}

void ref_sound::clone(const ref_sound& from, esound_type sound_type, int game_type)
{
    // TODO: Обеспечить безопасную передачу "const ref_sound& from" другому потоку
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::clone_internal, this, from, sound_type, game_type));
}

void ref_sound::destroy()
{
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::destroy_internal, this));
}

void ref_sound::play(IGameObject* O, u32 flags, float delay)
{
    // TODO: Обеспечить безопасную передачу "IGameObject* O" другому потоку
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::play_internal, this, O, flags, delay));
}

void ref_sound::play_at_pos(IGameObject* O, const Fvector& pos, u32 flags, float delay)
{
    // TODO: Обеспечить безопасную передачу "IGameObject* O" другому потоку
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::play_at_pos_internal, this, O, pos, flags, delay));
}

void ref_sound::play_no_feedback(
    IGameObject* O,
    u32 flags,
    float delay,
    const Fvector* pos,
    const float* vol,
    const float* freq,
    const Fvector2* range)
{
    // TODO: Обеспечить безопасную передачу "IGameObject* O" другому потоку
    const bool HasPos = (pos != nullptr);
    const bool HasVol = (vol != nullptr);
    const bool HasFreq = (freq != nullptr);
    const bool HasRange = (range != nullptr);

    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(
        &ref_sound::play_no_feedback_internal, this, O, flags, delay,
        HasPos,
        HasPos ? *pos : Fvector(),
        HasVol,
        HasVol ? *vol : float(),
        HasFreq,
        HasFreq ? *freq : float(),
        HasRange,
        HasRange ? *range : Fvector2()
    ));
}

void ref_sound::stop()
{
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::stop_internal, this));
}

void ref_sound::stop_deferred()
{
    VERIFY(SoundRender);
    SoundRender->Delegate(std::bind(&ref_sound::stop_deferred_internal, this));
}

void ref_sound::create_internal(const xr_string& name, esound_type sound_type, int game_type, bool replaceWithNoSound)
{
    bool Status;
    VER_SND_UNLOCKED();
    Status = GEnv.Sound->create(*this, name.c_str(), sound_type, game_type, replaceWithNoSound);
    // TODO: return status to user
    VERIFY(Status);
}


void ref_sound::attach_tail_internal(const xr_string& name)
{
    VER_SND_UNLOCKED(); GEnv.Sound->attach_tail(*this, name.c_str());
}

void ref_sound::clone_internal(const ref_sound& from, esound_type sound_type, int game_type)
{
    VER_SND_UNLOCKED(); GEnv.Sound->clone(*this, from, sound_type, game_type);
}

void ref_sound::destroy_internal()
{
    VER_SND_UNLOCKED(); GEnv.Sound->destroy(*this);
}

void ref_sound::play_internal(IGameObject* O, u32 flags, float delay )
{
    VER_SND_UNLOCKED(); GEnv.Sound->play(*this, O, flags, delay);
}

void ref_sound::play_at_pos_internal(IGameObject* O, const Fvector& pos, u32 flags, float delay )
{
    VER_SND_UNLOCKED(); GEnv.Sound->play_at_pos(*this, O, pos, flags, delay);
}

void ref_sound::play_no_feedback_internal(
    IGameObject* O,
    u32 flags,
    float delay,
    bool has_pos,
    const Fvector& pos,
    bool has_vol,
    const float& vol,
    bool has_freq,
    const float& freq,
    bool has_range,
    const Fvector2& range)
{
    const Fvector* pPos = has_pos ? &pos : nullptr;
    const float* pVol = has_vol ? &vol : nullptr;
    const float* pFreq = has_freq ? &freq : nullptr;
    const Fvector2* pRange = has_range ? &range : nullptr;

    VER_SND_UNLOCKED(); GEnv.Sound->play_no_feedback(*this, O, flags, delay, pPos, pVol, pFreq, pRange);
}

void ref_sound::stop_internal() 
{ 
    VER_SND_UNLOCKED(); if (_feedback()) _feedback()->stop(false); 
}

void ref_sound::stop_deferred_internal() 
{ 
    VER_SND_UNLOCKED(); if (_feedback()) _feedback()->stop(true); 
}
