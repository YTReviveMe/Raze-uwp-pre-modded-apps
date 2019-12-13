//-------------------------------------------------------------------------
/*
Copyright (C) 2016 EDuke32 developers and contributors

This file is part of EDuke32.

EDuke32 is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//-------------------------------------------------------------------------
#include "ns.h"	// Must come before everything else!

#include "compat.h"

#include "duke3d.h"
#include "renderlayer.h" // for win_gethwnd()
#include "openaudio.h"
#include "z_music.h"
#include <atomic>
#include "mapinfo.h"

BEGIN_RR_NS

#define DQSIZE 128

int32_t g_numEnvSoundsPlaying, g_highestSoundIdx = 0;

static int32_t MusicIsWaveform = 0;
static char *MusicPtr = NULL;
static int32_t MusicVoice = -1;
static bool MusicPaused = false;
static bool SoundPaused = false;

std::atomic<uint32_t> dnum;
uint32_t dq[DQSIZE];

void S_SoundStartup(void)
{
#ifdef MIXERTYPEWIN
    void *initdata = (void *) win_gethwnd(); // used for DirectSound
#else
    void *initdata = NULL;
#endif

    initprintf("Initializing sound... ");

    if (FX_Init(snd_numvoices, snd_numchannels, snd_mixrate, initdata) != FX_Ok)
    {
        initprintf("failed! %s\n", FX_ErrorString(FX_Error));
        return;
    }

    initprintf("%d voices, %d channels, 16-bit %d Hz\n", *snd_numvoices, *snd_numchannels, *snd_mixrate);

    for (int i = 0; i <= g_highestSoundIdx; ++i)
    {
        for (auto & voice : g_sounds[i].voices)
        {
            g_sounds[i].num = 0;
            voice.id        = 0;
            voice.owner     = -1;
            voice.dist      = UINT16_MAX;
            voice.clock     = 0;
        }
    }

    S_PrecacheSounds();

	snd_fxvolume.Callback();

	snd_reversestereo.Callback();
    FX_SetCallBack(S_Callback);
    FX_SetPrintf(OSD_Printf);
}

void S_SoundShutdown(void)
{
    if (FX_Shutdown() != FX_Ok)
    {
        Bsprintf(tempbuf, "S_SoundShutdown(): error: %s", FX_ErrorString(FX_Error));
        G_GameExit(tempbuf);
    }
}


void S_PauseSounds(bool paused)
{
    if (SoundPaused == paused)
        return;

    SoundPaused = paused;

    for (int i = 0; i <= g_highestSoundIdx; ++i)
    {
        for (auto & voice : g_sounds[i].voices)
            if (voice.id > 0)
                FX_PauseVoice(voice.id, paused);
    }
}

void S_MenuSound(void)
{
    static int SoundNum;
    int const menusnds[] = {
        LASERTRIP_EXPLODE, DUKE_GRUNT,       DUKE_LAND_HURT,   CHAINGUN_FIRE, SQUISHED,      KICK_HIT,
        PISTOL_RICOCHET,   PISTOL_BODYHIT,   PISTOL_FIRE,      SHOTGUN_FIRE,  BOS1_WALK,     RPG_EXPLODE,
        PIPEBOMB_BOUNCE,   PIPEBOMB_EXPLODE, NITEVISION_ONOFF, RPG_SHOOT,     SELECT_WEAPON,
    };
    int s = RR ? 390 : menusnds[SoundNum++ % ARRAY_SIZE(menusnds)];
    if (s != -1)
        S_PlaySound(s);
}


void S_PlayLevelMusicOrNothing(unsigned int m)
{
    Mus_Play(mapList[m].labelName, RR ? nullptr : mapList[m].music, true);
}

int S_TryPlaySpecialMusic(unsigned int m)
{
    if (RR)
        return 1;
    auto &musicfn = mapList[m].music;
    if (musicfn.IsNotEmpty())
    {
        if (!Mus_Play(nullptr, musicfn, 1))
        {
            return 0;
        }
    }

    return 1;
}

void S_PlayRRMusic(int newTrack)
{
    char fileName[16];
    if (!RR || !mus_redbook)
        return;
    Mus_Stop();
    g_cdTrack = newTrack != -1 ? newTrack : g_cdTrack+1;
    if (newTrack != 10 && (g_cdTrack > 9 || g_cdTrack < 2))
        g_cdTrack = 2;

    Bsprintf(fileName, "track%.2d.ogg", g_cdTrack);
    Mus_Play(fileName, 0, true);
}

void S_PlaySpecialMusicOrNothing(unsigned int m)
{
    S_TryPlaySpecialMusic(m);
}

void S_Cleanup(void)
{
    static uint32_t ldnum;
    uint32_t const odnum = dnum;

    while (ldnum < odnum)
    {
        uint32_t num = dq[ldnum++ & (DQSIZE - 1)];

        // negative index is RTS playback
        if ((int32_t)num < 0)
        {
            continue;
        }

        // num + (MAXSOUNDS*MAXSOUNDINSTANCES) is a sound played globally
        // for which there was no open slot to keep track of the voice
        if (num >= (MAXSOUNDS*MAXSOUNDINSTANCES))
        {
            continue;
        }

        int const vidx = num & (MAXSOUNDINSTANCES - 1);

        num = (num - vidx) / MAXSOUNDINSTANCES;

        auto &snd   = g_sounds[num];
        auto &voice = g_sounds[num].voices[vidx];

        int const spriteNum = voice.owner;

        if (EDUKE32_PREDICT_FALSE(snd.num > MAXSOUNDINSTANCES))
            OSD_Printf(OSD_ERROR "S_Cleanup(): num exceeds MAXSOUNDINSTANCES! g_sounds[%d].num %d wtf?\n", num, snd.num);

        if (snd.num > 0)
            --snd.num;

        // MUSICANDSFX uses t_data[0] to control restarting the sound
        // CLEAR_SOUND_T0
        if (spriteNum != -1 && S_IsAmbientSFX(spriteNum) && sector[SECT(spriteNum)].lotag < 3)  // ST_2_UNDERWATER
            actor[spriteNum].t_data[0] = 0;

        voice.owner = -1;
        voice.id    = 0;
        voice.dist  = UINT16_MAX;
        voice.clock = 0;
    }
}

// returns number of bytes read
int32_t S_LoadSound(int num)
{
    if ((unsigned)num > (unsigned)g_highestSoundIdx || EDUKE32_PREDICT_FALSE(g_sounds[num].filename == NULL))
        return 0;

    auto &snd = g_sounds[num];

    auto fp = S_OpenAudio(snd.filename, 0, 0);

    if (!fp.isOpen())
    {
        OSD_Printf(OSDTEXT_RED "Sound %s(#%d) not found!\n", snd.filename, num);
        return 0;
    }

	int32_t l = fp.GetLength();
    snd.siz = l;
    cacheAllocateBlock((intptr_t *)&snd.ptr, l, nullptr);
    l = fp.Read(snd.ptr, l);

    return l;
}

void S_PrecacheSounds(void)
{
    for (int32_t i = 0, j = 0; i <= g_highestSoundIdx; ++i)
        if (g_sounds[i].ptr == 0)
        {
            j++;
            if ((j&7) == 0)
                G_HandleAsync();

            S_LoadSound(i);
        }
}

static inline int S_GetPitch(int num)
{
    auto const &snd   = g_sounds[num];
    int const   range = klabs(snd.pe - snd.ps);

    return (range == 0) ? snd.ps : min(snd.ps, snd.pe) + rand() % range;
}

static int S_TakeSlot(int soundNum)
{
    S_Cleanup();

    uint16_t dist  = 0;
    uint16_t clock = 0;

    int bestslot = 0;
    int slot     = 0;

    auto &snd = g_sounds[soundNum];

    while (slot < MAXSOUNDINSTANCES && snd.voices[slot].id > 0)
    {
        auto &voice = snd.voices[slot];

        if (voice.dist > dist || (voice.dist == dist && voice.clock > clock))
        {
            clock = voice.clock;
            dist  = voice.dist;

            bestslot = slot;
        }

        slot++;
    }

    if (slot != MAXSOUNDINSTANCES)
        return slot;

    if (FX_SoundActive(snd.voices[bestslot].id))
        FX_StopSound(snd.voices[bestslot].id);

    dq[dnum++ & (DQSIZE-1)] = (soundNum * MAXSOUNDINSTANCES) + bestslot;
    S_Cleanup();

    return bestslot;
}

static int S_GetSlot(int soundNum)
{
    int slot = 0;

    while (slot < MAXSOUNDINSTANCES && g_sounds[soundNum].voices[slot].id > 0)
        slot++;

    return slot == MAXSOUNDINSTANCES ? S_TakeSlot(soundNum) : slot;
}

static inline int S_GetAngle(int ang, const vec3_t *cam, const vec3_t *pos)
{
    return (2048 + ang - getangle(cam->x - pos->x, cam->y - pos->y)) & 2047;
}

static bool S_CalcDistAndAng(int32_t spriteNum, int32_t soundNum, int32_t sectNum, int32_t angle,
                                const vec3_t *cam, const vec3_t *pos,
                                int32_t *distPtr, int32_t *angPtr)
{
    int32_t sndang = 0, sndist = 0;
    bool explosion = false;

    if (PN(spriteNum) == APLAYER && P_Get(spriteNum) == screenpeek)
        goto sound_further_processing;

    sndang = S_GetAngle(angle, cam, pos);
    sndist = FindDistance3D(cam->x-pos->x, cam->y-pos->y, (cam->z-pos->z));

#ifdef SPLITSCREEN_MOD_HACKS
    if (g_fakeMultiMode==2)
    {
        // HACK for splitscreen mod: take the min of sound distances
        // to 1st and 2nd player.

        if (PN(spriteNum) == APLAYER && P_Get(spriteNum) == 1)
        {
            sndist = sndang = 0;
            goto sound_further_processing;
        }

        {
            const vec3_t *cam2 = &g_player[1].ps->pos;
            int32_t sndist2 = FindDistance3D(cam2->x-pos->x, cam2->y-pos->y, (cam2->z-pos->z));

            if (sndist2 < sndist)
            {
                cam = cam2;
                sectNum = g_player[1].ps->cursectnum;
                angle = g_player[1].ps->ang;

                sndist = sndist2;
                sndang = S_GetAngle(angle, cam, pos);
            }
        }
    }
#endif

    if ((g_sounds[soundNum].m & SF_GLOBAL) == 0 && S_IsAmbientSFX(spriteNum) && (sector[SECT(spriteNum)].lotag&0xff) < 9)  // ST_9_SLIDING_ST_DOOR
        sndist = divscale14(sndist, SHT(spriteNum)+1);

sound_further_processing:
    sndist += g_sounds[soundNum].vo;
    if (sndist < 0)
        sndist = 0;

    if (sectNum > -1 && sndist && PN(spriteNum) != MUSICANDSFX
        && !cansee(cam->x, cam->y, cam->z - (24 << 8), sectNum, SX(spriteNum), SY(spriteNum), SZ(spriteNum) - (24 << 8), SECT(spriteNum)))
        sndist += sndist>>(RR?2:5);

    switch (DYNAMICSOUNDMAP(soundNum))
    {
        case PIPEBOMB_EXPLODE__STATIC:
        case LASERTRIP_EXPLODE__STATIC:
        case RPG_EXPLODE__STATIC:
            explosion = true;
            if (sndist > 6144)
                sndist = 6144;
            break;
    }

    if ((g_sounds[soundNum].m & SF_GLOBAL) || sndist < ((255-LOUDESTVOLUME) << 6))
        sndist = ((255-LOUDESTVOLUME) << 6);

    *distPtr = sndist;
    *angPtr  = sndang;

    return explosion;
}

int S_PlaySound3D(int num, int spriteNum, const vec3_t *pos)
{
    int32_t j;

    if (!SoundEnabled()) // check that the user returned -1, but only if -1 wasn't playing already (in which case, warn)
        return -1;

    int const sndNum = num;
    sound_t & snd    = g_sounds[sndNum];

    if (EDUKE32_PREDICT_FALSE((unsigned) sndNum > (unsigned) g_highestSoundIdx || snd.filename == NULL || snd.ptr == NULL))
    {
        OSD_Printf("WARNING: invalid sound #%d\n", num);
        return -1;
    }

    const DukePlayer_t *const pPlayer = g_player[myconnectindex].ps;

    if (((snd.m & SF_ADULT) && adult_lockout) || (unsigned)spriteNum >= MAXSPRITES || (pPlayer->gm & MODE_MENU) || !FX_VoiceAvailable(snd.pr)
        || (pPlayer->timebeforeexit > 0 && pPlayer->timebeforeexit <= GAMETICSPERSEC * 3))
        return -1;

    // Duke talk
    if (snd.m & SF_TALK)
    {
        if ((g_netServer || ud.multimode > 1) && PN(spriteNum) == APLAYER && P_Get(spriteNum) != screenpeek) // other player sound
        {
            if ((snd_speech & 4) != 4)
                return -1;
        }
        else if ((snd_speech & 1) != 1)
            return -1;

        // don't play if any Duke talk sounds are already playing
        for (j = 0; j <= g_highestSoundIdx; ++j)
            if ((g_sounds[j].m & SF_TALK) && g_sounds[j].num > 0)
                return -1;
    }
    else if (snd.m & SF_DTAG)  // Duke-Tag sound
    {
        int const voice = S_PlaySound(sndNum);

        if (voice <= FX_Ok)
            return -1;

        j = 0;
        while (j < MAXSOUNDINSTANCES && snd.voices[j].id != voice)
            j++;

#ifdef DEBUGGINGAIDS
        if (EDUKE32_PREDICT_FALSE(j >= MAXSOUNDINSTANCES))
        {
            OSD_Printf(OSD_ERROR "%s %d: WTF?\n", __FILE__, __LINE__);
            return -1;
        }
#endif

        snd.voices[j].owner = spriteNum;

        return voice;
    }

    int32_t    sndist, sndang;
    int const  explosionp = S_CalcDistAndAng(spriteNum, sndNum, CAMERA(sect), fix16_to_int(CAMERA(q16ang)), &CAMERA(pos), pos, &sndist, &sndang);
    int        pitch      = S_GetPitch(sndNum);
    auto const pOther     = g_player[screenpeek].ps;

#ifdef SPLITSCREEN_MOD_HACKS
    if (g_fakeMultiMode==2)
    {
        // splitscreen HACK
        if (g_player[1].ps->i == spriteNum)
            pOther = g_player[1].ps;
}
#endif

    if (pOther->sound_pitch)
        pitch += pOther->sound_pitch;

    if (explosionp)
    {
        if (pOther->cursectnum > -1 && sector[pOther->cursectnum].lotag == ST_2_UNDERWATER)
            pitch -= 1024;
    }
    else
    {
        if (sndist > 32767 && PN(spriteNum) != MUSICANDSFX && (snd.m & (SF_LOOP|SF_MSFX)) == 0)
            return -1;

        if (pOther->cursectnum > -1 && sector[pOther->cursectnum].lotag == ST_2_UNDERWATER
            && (snd.m & SF_TALK) == 0)
            pitch = -768;
    }

    if (snd.num > 0 && PN(spriteNum) != MUSICANDSFX)
        S_StopEnvSound(sndNum, spriteNum);


    int const sndSlot = S_GetSlot(sndNum);

    if (sndSlot >= MAXSOUNDINSTANCES)
    {
        return -1;
    }

    int const repeatp = (snd.m & SF_LOOP);

    if (repeatp && (snd.m & SF_ONEINST_INTERNAL) && snd.num > 0)
    {
        return -1;
    }

    // XXX: why is 'right' 0?
    // Ambient MUSICANDSFX always start playing using the 3D routines!
    int const ambsfxp = S_IsAmbientSFX(spriteNum);
    int const voice = (repeatp && !ambsfxp) ? FX_Play(snd.ptr, snd.siz, 0, -1, pitch, sndist >> 6, sndist >> 6, 0, snd.pr,
                                                      snd.volume, (sndNum * MAXSOUNDINSTANCES) + sndSlot)
                                            : FX_Play3D(snd.ptr, snd.siz, repeatp ? FX_LOOP : FX_ONESHOT, pitch, sndang >> 4, sndist >> 6,
                                                        snd.pr, snd.volume, (sndNum * MAXSOUNDINSTANCES) + sndSlot);

    if (voice <= FX_Ok)
    {
        return -1;
    }

    snd.num++;
    snd.voices[sndSlot].owner = spriteNum;
    snd.voices[sndSlot].id    = voice;
    snd.voices[sndSlot].dist  = sndist >> 6;
    snd.voices[sndSlot].clock = 0;

    return voice;
}

int S_PlaySound(int num)
{
    int sndnum;

    if (!SoundEnabled()) // check that the user returned -1, but only if -1 wasn't playing already (in which case, warn)
        return -1;

    sound_t & snd = g_sounds[num];

    if (EDUKE32_PREDICT_FALSE((unsigned)num > (unsigned)g_highestSoundIdx || snd.filename == NULL || snd.ptr == NULL))
    {
        OSD_Printf("WARNING: invalid sound #%d\n",num);
        return -1;
    }

    if ((!(snd_speech & 1) && (snd.m & SF_TALK)) || ((snd.m & SF_ADULT) && adult_lockout) || !FX_VoiceAvailable(snd.pr))
        return -1;

    int const pitch = S_GetPitch(num);

    sndnum = S_GetSlot(num);

    if (sndnum >= MAXSOUNDINSTANCES)
    {
        return -1;
    }

    int const voice = (snd.m & SF_LOOP) ? FX_Play(snd.ptr, snd.siz, 0, -1, pitch, LOUDESTVOLUME, LOUDESTVOLUME,
                                                  LOUDESTVOLUME, snd.siz, snd.volume, (num * MAXSOUNDINSTANCES) + sndnum)
                                        : FX_Play3D(snd.ptr, snd.siz, FX_ONESHOT, pitch, 0, 255 - LOUDESTVOLUME, snd.pr, snd.volume,
                                                    (num * MAXSOUNDINSTANCES) + sndnum);

    if (voice <= FX_Ok)
    {
        return -1;
    }

    snd.num++;
    snd.voices[sndnum].owner = -1;
    snd.voices[sndnum].id    = voice;
    snd.voices[sndnum].dist  = 255 - LOUDESTVOLUME;
    snd.voices[sndnum].clock = 0;

    return voice;
}

int A_PlaySound(int soundNum, int spriteNum)
{
    if (EDUKE32_PREDICT_FALSE((unsigned)soundNum > (unsigned)g_highestSoundIdx)) return -1;

    return (unsigned)spriteNum >= MAXSPRITES ? S_PlaySound(soundNum) :
        S_PlaySound3D(soundNum, spriteNum, (vec3_t *)&sprite[spriteNum]);
}

void S_StopEnvSound(int32_t num, int32_t i)
{
    if (EDUKE32_PREDICT_FALSE((unsigned)num > (unsigned)g_highestSoundIdx) || g_sounds[num].num <= 0)
        return;

    int32_t j;

    do
    {
        for (j=0; j<MAXSOUNDINSTANCES; ++j)
        {
            if ((i == -1 && g_sounds[num].voices[j].id > FX_Ok) || (i != -1 && g_sounds[num].voices[j].owner == i))
            {
#ifdef DEBUGGINGAIDS
                if (EDUKE32_PREDICT_FALSE(i >= 0 && g_sounds[num].voices[j].id <= FX_Ok))
                    initprintf(OSD_ERROR "S_StopEnvSound(): bad voice %d for sound ID %d index %d!\n", g_sounds[num].voices[j].id, num, j);
                else
#endif
                if (g_sounds[num].voices[j].id > FX_Ok)
                {
                    FX_StopSound(g_sounds[num].voices[j].id);
                    S_Cleanup();
                    break;
                }
            }
        }
    }
    while (j < MAXSOUNDINSTANCES);
}

// Do not remove this or make it inline.
void S_StopAllSounds(void)
{
    FX_StopAllSounds();
}

void S_ChangeSoundPitch(int soundNum, int spriteNum, int pitchoffset)
{
    if ((unsigned)soundNum > (unsigned)g_highestSoundIdx || g_sounds[soundNum].num <= 0)
        return;

    for (auto &voice : g_sounds[soundNum].voices)
    {
        if ((spriteNum == -1 && voice.id > FX_Ok) || (spriteNum != -1 && voice.owner == spriteNum))
        {
            if (EDUKE32_PREDICT_FALSE(spriteNum >= 0 && voice.id <= FX_Ok))
                initprintf(OSD_ERROR "S_ChangeSoundPitch(): bad voice %d for sound ID %d!\n", voice.id, soundNum);
            else if (voice.id > FX_Ok && FX_SoundActive(voice.id))
                FX_SetPitch(voice.id, pitchoffset);
            break;
        }
    }
}

void S_Update(void)
{
    S_Cleanup();

    if ((g_player[myconnectindex].ps->gm & (MODE_GAME|MODE_DEMO)) == 0)
        return;

    if (RR && MusicIsWaveform && MusicVoice >= 0 && !FX_SoundActive(MusicVoice))
        S_PlayRRMusic();

    g_numEnvSoundsPlaying = 0;

    const vec3_t *c;
    int32_t ca,cs;

    if (ud.camerasprite == -1)
    {
        c = &CAMERA(pos);
        cs = CAMERA(sect);
        ca = fix16_to_int(CAMERA(q16ang));
    }
    else
    {
        c = (vec3_t *)&sprite[ud.camerasprite];
        cs = sprite[ud.camerasprite].sectnum;
        ca = sprite[ud.camerasprite].ang;
    }

    int       sndnum  = 0;
    int const highest = g_highestSoundIdx;

    do
    {
        if (g_sounds[sndnum].num == 0)
            continue;

        for (auto &voice : g_sounds[sndnum].voices)
        {
            int const spriteNum = voice.owner;

            if ((unsigned)spriteNum >= MAXSPRITES || voice.id <= FX_Ok || !FX_SoundActive(voice.id))
                continue;

            int32_t sndist, sndang;

            S_CalcDistAndAng(spriteNum, sndnum, cs, ca, c, (const vec3_t *)&sprite[spriteNum], &sndist, &sndang);

            if (S_IsAmbientSFX(spriteNum))
                g_numEnvSoundsPlaying++;

            // AMBIENT_SOUND
            FX_Pan3D(voice.id, sndang >> 4, sndist >> 6);
            voice.dist = sndist >> 6;
            voice.clock++;
        }
    } while (++sndnum <= highest);
}

void S_Callback(intptr_t num)
{
    if (num == MUSIC_ID)
        return;

    dq[dnum & (DQSIZE - 1)] = (uint32_t)num;
    dnum++;
}


bool A_CheckSoundPlaying(int spriteNum, int soundNum)
{
    if (EDUKE32_PREDICT_FALSE((unsigned)soundNum > (unsigned)g_highestSoundIdx)) return 0;

    if (g_sounds[soundNum].num > 0 && spriteNum >= 0)
    {
        for (auto &voice : g_sounds[soundNum].voices)
            if (voice.owner == spriteNum)
                return 1;
    }

    return (spriteNum == -1) ? (g_sounds[soundNum].num != 0) : 0;
}

// Check if actor <i> is playing any sound.
bool A_CheckAnySoundPlaying(int spriteNum)
{
    int const msp = g_highestSoundIdx;

    for (int j = 0; j <= msp; ++j)
    {
        for (auto &voice : g_sounds[j].voices)
            if (voice.owner == spriteNum)
                return 1;
    }

    return 0;
}

bool S_CheckSoundPlaying(int spriteNum, int soundNum)
{
    if (EDUKE32_PREDICT_FALSE((unsigned)soundNum > (unsigned)g_highestSoundIdx)) return false;
    return (g_sounds[soundNum].num != 0);
}

END_RR_NS
