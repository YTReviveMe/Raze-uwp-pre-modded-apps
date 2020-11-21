//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 Nuke.YKT

This file is part of NBlood.

NBlood is free software; you can redistribute it and/or
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

#include <stdio.h>
#include "build.h"
#include "compat.h"
#include "mmulti.h"
#include "common_game.h"
#include "ai.h"
#include "blood.h"
#include "globals.h"
#include "db.h"
#include "messages.h"
#include "loadsave.h"
#include "sectorfx.h"
#include "seq.h"
#include "sound.h"
#include "i_specialpaths.h"
#include "view.h"
#include "nnexts.h"
#include "savegamehelp.h"
#include "raze_music.h"
#include "mapinfo.h"
#include "gamestate.h"
#include "d_net.h"

#include "aistate.h"
#include "aiunicult.h"


BEGIN_BLD_NS

// All AI states for assigning an index.
static AISTATE* allAIStates[] =
{
    nullptr,
    &genIdle,
    &genRecoil,
    &batIdle,
    &batFlyIdle,
    &batChase,
    &batPonder,
    &batGoto,
    &batBite,
    &batRecoil,
    &batSearch,
    &batSwoop,
    &batFly,
    &batTurn,
    &batHide,
    &batDodgeUp,
    &batDodgeUpRight,
    &batDodgeUpLeft,
    &batDodgeDown,
    &batDodgeDownRight,
    &batDodgeDownLeft,
    &beastIdle,
    &beastChase,
    &beastDodge,
    &beastGoto,
    &beastSlash,
    &beastStomp,
    &beastSearch,
    &beastRecoil,
    &beastTeslaRecoil,
    &beastSwimIdle,
    &beastSwimChase,
    &beastSwimDodge,
    &beastSwimGoto,
    &beastSwimSearch,
    &beastSwimSlash,
    &beastSwimRecoil,
    &beastMorphToBeast,
    &beastMorphFromCultist,
    &beast138FB4,
    &beast138FD0,
    &beast138FEC,
    &eelIdle,
    &eelFlyIdle,
    &eelChase,
    &eelPonder,
    &eelGoto,
    &eelBite,
    &eelRecoil,
    &eelSearch,
    &eelSwoop,
    &eelFly,
    &eelTurn,
    &eelHide,
    &eelDodgeUp,
    &eelDodgeUpRight,
    &eelDodgeUpLeft,
    &eelDodgeDown,
    &eelDodgeDownRight,
    &eelDodgeDownLeft,
    &cultistBurnIdle,
    &cultistBurnChase,
    &cultistBurnGoto,
    &cultistBurnSearch,
    &cultistBurnAttack,
    &zombieABurnChase,
    &zombieABurnGoto,
    &zombieABurnSearch,
    &zombieABurnAttack,
    &zombieFBurnChase,
    &zombieFBurnGoto,
    &zombieFBurnSearch,
    &zombieFBurnAttack,
    &innocentBurnChase,
    &innocentBurnGoto,
    &innocentBurnSearch,
    &innocentBurnAttack,
    &beastBurnChase,
    &beastBurnGoto,
    &beastBurnSearch,
    &beastBurnAttack,
    &tinycalebBurnChase,
    &tinycalebBurnGoto,
    &tinycalebBurnSearch,
    &tinycalebBurnAttack,
    &genDudeBurnIdle,
    &genDudeBurnChase,
    &genDudeBurnGoto,
    &genDudeBurnSearch,
    &genDudeBurnAttack,
    &tinycalebIdle,
    &tinycalebChase,
    &tinycalebDodge,
    &tinycalebGoto,
    &tinycalebAttack,
    &tinycalebSearch,
    &tinycalebRecoil,
    &tinycalebTeslaRecoil,
    &tinycalebSwimIdle,
    &tinycalebSwimChase,
    &tinycalebSwimDodge,
    &tinycalebSwimGoto,
    &tinycalebSwimSearch,
    &tinycalebSwimAttack,
    &tinycalebSwimRecoil,
    &tinycaleb139660,
    &tinycaleb13967C,
    &tinycaleb139698,
    &cerberusIdle,
    &cerberusSearch,
    &cerberusChase,
    &cerberusRecoil,
    &cerberusTeslaRecoil,
    &cerberusGoto,
    &cerberusBite,
    &cerberusBurn,
    &cerberus3Burn,
    &cerberus2Idle,
    &cerberus2Search,
    &cerberus2Chase,
    &cerberus2Recoil,
    &cerberus2Goto,
    &cerberus2Bite,
    &cerberus2Burn,
    &cerberus4Burn,
    &cerberus139890,
    &cerberus1398AC,
    &cultistIdle,
    &cultistProneIdle,
    &fanaticProneIdle,
    &cultistProneIdle3,
    &cultistChase,
    &fanaticChase,
    &cultistDodge,
    &cultistGoto,
    &cultistProneChase,
    &cultistProneDodge,
    &cultistTThrow,
    &cultistSThrow,
    &cultistTsThrow,
    &cultistDThrow,
    &cultist139A78,
    &cultist139A94,
    &cultist139AB0,
    &cultist139ACC,
    &cultist139AE8,
    &cultistSearch,
    &cultistSFire,
    &cultistTFire,
    &cultistTsFire,
    &cultistSProneFire,
    &cultistTProneFire,
    &cultistTsProneFire,
    &cultistRecoil,
    &cultistProneRecoil,
    &cultistTeslaRecoil,
    &cultistSwimIdle,
    &cultistSwimChase,
    &cultistSwimDodge,
    &cultistSwimGoto,
    &cultistSwimSearch,
    &cultistSSwimFire,
    &cultistTSwimFire,
    &cultistTsSwimFire,
    &cultistSwimRecoil,
    &gargoyleFIdle,
    &gargoyleStatueIdle,
    &gargoyleFChase,
    &gargoyleFGoto,
    &gargoyleFSlash,
    &gargoyleFThrow,
    &gargoyleSThrow,
    &gargoyleSBlast,
    &gargoyleFRecoil,
    &gargoyleFSearch,
    &gargoyleFMorph2,
    &gargoyleFMorph,
    &gargoyleSMorph2,
    &gargoyleSMorph,
    &gargoyleSwoop,
    &gargoyleFly,
    &gargoyleTurn,
    &gargoyleDodgeUp,
    &gargoyleFDodgeUpRight,
    &gargoyleFDodgeUpLeft,
    &gargoyleDodgeDown,
    &gargoyleFDodgeDownRight,
    &gargoyleFDodgeDownLeft,
    &statueFBreakSEQ,
    &statueSBreakSEQ,
    &ghostIdle,
    &ghostChase,
    &ghostGoto,
    &ghostSlash,
    &ghostThrow,
    &ghostBlast,
    &ghostRecoil,
    &ghostTeslaRecoil,
    &ghostSearch,
    &ghostSwoop,
    &ghostFly,
    &ghostTurn,
    &ghostDodgeUp,
    &ghostDodgeUpRight,
    &ghostDodgeUpLeft,
    &ghostDodgeDown,
    &ghostDodgeDownRight,
    &ghostDodgeDownLeft,
    &gillBeastIdle,
    &gillBeastChase,
    &gillBeastDodge,
    &gillBeastGoto,
    &gillBeastBite,
    &gillBeastSearch,
    &gillBeastRecoil,
    &gillBeastSwimIdle,
    &gillBeastSwimChase,
    &gillBeastSwimDodge,
    &gillBeastSwimGoto,
    &gillBeastSwimSearch,
    &gillBeastSwimBite,
    &gillBeastSwimRecoil,
    &gillBeast13A138,
    &gillBeast13A154,
    &gillBeast13A170,
    &handIdle,
    &hand13A3B4,
    &handSearch,
    &handChase,
    &handRecoil,
    &handGoto,
    &handJump,
    &houndIdle,
    &houndSearch,
    &houndChase,
    &houndRecoil,
    &houndTeslaRecoil,
    &houndGoto,
    &houndBite,
    &houndBurn,
    &innocentIdle,
    &innocentSearch,
    &innocentChase,
    &innocentRecoil,
    &innocentTeslaRecoil,
    &innocentGoto,
    &podIdle,
    &podMove,
    &podSearch,
    &podStartChase,
    &podRecoil,
    &podChase,
    &tentacleIdle,
    &tentacle13A6A8,
    &tentacle13A6C4,
    &tentacle13A6E0,
    &tentacle13A6FC,
    &tentacleMove,
    &tentacleSearch,
    &tentacleStartChase,
    &tentacleRecoil,
    &tentacleChase,
    &ratIdle,
    &ratSearch,
    &ratChase,
    &ratDodge,
    &ratRecoil,
    &ratGoto,
    &ratBite,
    &spidIdle,
    &spidChase,
    &spidDodge,
    &spidGoto,
    &spidSearch,
    &spidBite,
    &spidJump,
    &spid13A92C,
    &tchernobogIdle,
    &tchernobogSearch,
    &tchernobogChase,
    &tchernobogRecoil,
    &tcherno13A9B8,
    &tcherno13A9D4,
    &tcherno13A9F0,
    &tcherno13AA0C,
    &tcherno13AA28,
    &genDudeIdleL,
    &genDudeIdleW,
    &genDudeSearchL,
    &genDudeSearchW,
    &genDudeSearchShortL,
    &genDudeSearchShortW,
    &genDudeSearchNoWalkL,
    &genDudeSearchNoWalkW,
    &genDudeGotoL,
    &genDudeGotoW,
    &genDudeDodgeL,
    &genDudeDodgeD,
    &genDudeDodgeW,
    &genDudeDodgeShortL,
    &genDudeDodgeShortD,
    &genDudeDodgeShortW,
    &genDudeDodgeShorterL,
    &genDudeDodgeShorterD,
    &genDudeDodgeShorterW,
    &genDudeChaseL,
    &genDudeChaseD,
    &genDudeChaseW,
    &genDudeChaseNoWalkL,
    &genDudeChaseNoWalkD,
    &genDudeChaseNoWalkW,
    &genDudeFireL,
    &genDudeFireD,
    &genDudeFireW,
    &genDudeRecoilL,
    &genDudeRecoilD,
    &genDudeRecoilW,
    &genDudeRecoilTesla,
    &genDudeThrow,
    &genDudeThrow2,
    &genDudePunch,
    &zombieAIdle,
    &zombieAChase,
    &zombieAPonder,
    &zombieAGoto,
    &zombieAHack,
    &zombieASearch,
    &zombieARecoil,
    &zombieATeslaRecoil,
    &zombieARecoil2,
    &zombieAStand,
    &zombieEIdle,
    &zombieEUp2,
    &zombieEUp,
    &zombie2Idle,
    &zombie2Search,
    &zombieSIdle,
    &zombie13AC2C,
    &zombieFIdle,
    &zombieFChase,
    &zombieFGoto,
    &zombieFDodge,
    &zombieFHack,
    &zombieFPuke,
    &zombieFThrow,
    &zombieFSearch,
    &zombieFRecoil,
    &zombieFTeslaRecoil,
};

void IndexAIState(AISTATE*& state)
{
    int i = 0;
	auto savestate = state;
    for (auto cstate : allAIStates)
    {
        if (state == cstate)
        {
            state = (AISTATE*)(intptr_t)i;
            return;
        }
        i++;
    }
    state = nullptr;
}

void UnindexAIState(AISTATE*& state)
{
    auto index = intptr_t(state);
    if (index >= 0 && index < countof(allAIStates))
    {
        state = allAIStates[index];
    }
	else
	{
		state = nullptr;
	}
}



unsigned int dword_27AA38 = 0;
unsigned int dword_27AA3C = 0;
unsigned int dword_27AA40 = 0;

FileWriter *LoadSave::hSFile = NULL;
FileReader LoadSave::hLFile;
TDeletingArray<LoadSave*> LoadSave::loadSaves;

void sub_76FD4(void)
{
}

void LoadSave::Save(void)
{
    I_Error("Pure virtual function called");
}

void LoadSave::Load(void)
{
    I_Error("Pure virtual function called");
}

void LoadSave::Read(void *pData, int nSize)
{
    dword_27AA38 += nSize;
    assert(hLFile.isOpen());
    if (hLFile.Read(pData, nSize) != nSize)
        I_Error("Error reading save file.");
}

void LoadSave::Write(const void *pData, int nSize)
{
    dword_27AA38 += nSize;
    dword_27AA3C += nSize;
    assert(hSFile != NULL);
    if (hSFile->Write(pData, nSize) != (size_t)nSize)
        I_Error("File error #%d writing save file.", errno);
}

bool GameInterface::LoadGame()
{
    LoadSave::hLFile = ReadSavegameChunk("snapshot.bld");
	if (!LoadSave::hLFile.isOpen())
		return false;

    for (auto rover : LoadSave::loadSaves)
    {
        rover->Load();
    }

	LoadSave::hLFile.Close();
	FinishSavegameRead();
    InitSectorFX();
    viewInitializePrediction();
    PreloadCache();
    if (!gMe->packSlots[1].isActive) // if diving suit is not active, turn off reverb sound effect
        sfxSetReverb(0);
    ambInit();
    for (int i = 0; i < gNetPlayers; i++)
        playerSetRace(&gPlayer[i], gPlayer[i].lifeMode);
	viewSetErrorMessage("");
    Net_ClearFifo();
    paused = 0;
    

#ifdef USE_OPENGL
    Polymost_prepare_loadboard();
#endif

#ifdef POLYMER
    if (videoGetRenderMode() == REND_POLYMER)
        polymer_loadboard();

    // this light pointer nulling needs to be outside the videoGetRenderMode check
    // because we might be loading the savegame using another renderer but
    // change to Polymer later
    for (int i=0; i<kMaxSprites; i++)
    {
        gPolymerLight[i].lightptr = NULL;
        gPolymerLight[i].lightId = -1;
    }
#endif

	Mus_ResumeSaved();

    PROFILE* pProfile = &gProfile[myconnectindex];
    strcpy(pProfile->name, playername);
    pProfile->skill = gSkill;
    return true;
}

bool GameInterface::SaveGame()
{
	LoadSave::hSFile = WriteSavegameChunk("snapshot.bld");

	try
	{
		dword_27AA38 = 0;
		dword_27AA40 = 0;
        for (auto rover : LoadSave::loadSaves)
		{
			rover->Save();
			if (dword_27AA38 > dword_27AA40)
				dword_27AA40 = dword_27AA38;
			dword_27AA38 = 0;
		}
	}
	catch (CRecoverableError & err)
	{
		// Let's not abort for write errors.
		Printf(TEXTCOLOR_RED "%s\n", err.what());
		return false;
	}
	LoadSave::hSFile = NULL;

	return 1;
}

class MyLoadSave : public LoadSave
{
public:
    virtual void Load(void);
    virtual void Save(void);
};

void MyLoadSave::Load(void)
{
    psky_t *pSky = tileSetupSky(DEFAULTPSKY);
    int id;
    Read(&id, sizeof(id));
    Read(&gGameOptions, sizeof(gGameOptions));
    
    int nNumSprites;
    Read(&nNumSprites, sizeof(nNumSprites));
    Read(qsector_filler, sizeof(qsector_filler[0])*numsectors);
    Read(&pSky->horizfrac, sizeof(pSky->horizfrac));
    Read(&pSky->yoffs, sizeof(pSky->yoffs));
    Read(&pSky->yscale, sizeof(pSky->yscale));
    Read(&gVisibility, sizeof(gVisibility));
    Read(pSky->tileofs, sizeof(pSky->tileofs));
    Read(&pSky->lognumtiles, sizeof(pSky->lognumtiles));
    Read(gotpic, sizeof(gotpic));
    Read(gotsector, sizeof(gotsector));
    Read(&gFrameClock, sizeof(gFrameClock));
    Read(&gFrameCount, sizeof(gFrameCount));
    Read(&paused, sizeof(paused));
    Read(baseWall, sizeof(baseWall[0])*numwalls);
    Read(baseSprite, sizeof(baseSprite[0])*nNumSprites);
    Read(baseFloor, sizeof(baseFloor[0])*numsectors);
    Read(baseCeil, sizeof(baseCeil[0])*numsectors);
    Read(velFloor, sizeof(velFloor[0])*numsectors);
    Read(velCeil, sizeof(velCeil[0])*numsectors);
    Read(&gHitInfo, sizeof(gHitInfo));
    Read(&byte_1A76C6, sizeof(byte_1A76C6));
    Read(&byte_1A76C8, sizeof(byte_1A76C8));
    Read(&byte_1A76C7, sizeof(byte_1A76C7));
    Read(&byte_19AE44, sizeof(byte_19AE44));
    Read(gStatCount, sizeof(gStatCount));
    Read(nextXSprite, sizeof(nextXSprite));
    Read(&XWallsUsed, sizeof(XWallsUsed));
    Read(&XSectorsUsed, sizeof(XSectorsUsed));
    memset(xsprite, 0, sizeof(xsprite));
    for (int nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus)
        {
            int nXSprite = sprite[nSprite].extra;
            if (nXSprite > 0)
            {
                Read(&xsprite[nXSprite], sizeof(XSPRITE));
                UnindexAIState(xsprite[nXSprite].aiState);
            }
        }
    }
    memset(xwall, 0, sizeof(xwall));
    for (int nWall = 0; nWall < numwalls; nWall++)
    {
        int nXWall = wall[nWall].extra;
        if (nXWall > 0)
            Read(&xwall[nXWall], sizeof(XWALL));
    }
    memset(xsector, 0, sizeof(xsector));
    for (int nSector = 0; nSector < numsectors; nSector++)
    {
        int nXSector = sector[nSector].extra;
        if (nXSector > 0)
            Read(&xsector[nXSector], sizeof(XSECTOR));
    }
    Read(xvel, nNumSprites*sizeof(xvel[0]));
    Read(yvel, nNumSprites*sizeof(yvel[0]));
    Read(zvel, nNumSprites*sizeof(zvel[0]));
    Read(&gMapRev, sizeof(gMapRev));
    Read(&gSongId, sizeof(gSkyCount));
    Read(&gFogMode, sizeof(gFogMode));
#ifdef NOONE_EXTENSIONS
    Read(&gModernMap, sizeof(gModernMap));
#endif
    psky_t *skyInfo = tileSetupSky(DEFAULTPSKY);
    Read(skyInfo, sizeof(*skyInfo));
    skyInfo->combinedtile = -1;
    cheatReset();

}

void MyLoadSave::Save(void)
{
    psky_t *pSky = tileSetupSky(0);
    int nNumSprites = 0;
    int id = 0x5653424e/*'VSBN'*/;
    Write(&id, sizeof(id));
    for (int nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus && nSprite > nNumSprites)
            nNumSprites = nSprite;
    }
    //nNumSprites += 2;
    nNumSprites++;
    Write(&gGameOptions, sizeof(gGameOptions));
    Write(&nNumSprites, sizeof(nNumSprites));
    Write(qsector_filler, sizeof(qsector_filler[0])*numsectors);
    Write(&pSky->horizfrac, sizeof(pSky->horizfrac));
    Write(&pSky->yoffs, sizeof(pSky->yoffs));
    Write(&pSky->yscale, sizeof(pSky->yscale));
    Write(&gVisibility, sizeof(gVisibility));
    Write(pSky->tileofs, sizeof(pSky->tileofs));
    Write(&pSky->lognumtiles, sizeof(pSky->lognumtiles));
    Write(gotpic, sizeof(gotpic));
    Write(gotsector, sizeof(gotsector));
    Write(&gFrameClock, sizeof(gFrameClock));
    Write(&gFrameCount, sizeof(gFrameCount));
    Write(&paused, sizeof(paused));
    Write(baseWall, sizeof(baseWall[0])*numwalls);
    Write(baseSprite, sizeof(baseSprite[0])*nNumSprites);
    Write(baseFloor, sizeof(baseFloor[0])*numsectors);
    Write(baseCeil, sizeof(baseCeil[0])*numsectors);
    Write(velFloor, sizeof(velFloor[0])*numsectors);
    Write(velCeil, sizeof(velCeil[0])*numsectors);
    Write(&gHitInfo, sizeof(gHitInfo));
    Write(&byte_1A76C6, sizeof(byte_1A76C6));
    Write(&byte_1A76C8, sizeof(byte_1A76C8));
    Write(&byte_1A76C7, sizeof(byte_1A76C7));
    Write(&byte_19AE44, sizeof(byte_19AE44));
    Write(gStatCount, sizeof(gStatCount));
    Write(nextXSprite, sizeof(nextXSprite));
    Write(&XWallsUsed, sizeof(XWallsUsed));
    Write(&XSectorsUsed, sizeof(XSectorsUsed));
    for (int nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus)
        {
            int nXSprite = sprite[nSprite].extra;
            if (nXSprite > 0)
            {
                auto saved = xsprite[nXSprite].aiState;
                IndexAIState(xsprite[nXSprite].aiState);
                Write(&xsprite[nXSprite], sizeof(XSPRITE));
                xsprite[nXSprite].aiState = saved;
            }
        }
    }
    for (int nWall = 0; nWall < numwalls; nWall++)
    {
        int nXWall = wall[nWall].extra;
        if (nXWall > 0)
            Write(&xwall[nXWall], sizeof(XWALL));
    }
    for (int nSector = 0; nSector < numsectors; nSector++)
    {
        int nXSector = sector[nSector].extra;
        if (nXSector > 0)
            Write(&xsector[nXSector], sizeof(XSECTOR));
    }
    Write(xvel, nNumSprites*sizeof(xvel[0]));
    Write(yvel, nNumSprites*sizeof(yvel[0]));
    Write(zvel, nNumSprites*sizeof(zvel[0]));
    Write(&gMapRev, sizeof(gMapRev));
    Write(&gSongId, sizeof(gSkyCount));
    Write(&gFogMode, sizeof(gFogMode));
#ifdef NOONE_EXTENSIONS
    Write(&gModernMap, sizeof(gModernMap));
#endif
    psky_t *skyInfo = tileSetupSky(DEFAULTPSKY);
    Write(skyInfo, sizeof(*skyInfo));
}

void ActorLoadSaveConstruct(void);
void AILoadSaveConstruct(void);
void EndGameLoadSaveConstruct(void);
void LevelsLoadSaveConstruct(void);
void MessagesLoadSaveConstruct(void);
void MirrorLoadSaveConstruct(void);
void PlayerLoadSaveConstruct(void);
void ViewLoadSaveConstruct(void);
#ifdef NOONE_EXTENSIONS
void NNLoadSaveConstruct(void);
#endif

void LoadSaveSetup(void)
{
    new MyLoadSave();

    ActorLoadSaveConstruct();
    AILoadSaveConstruct();
    EndGameLoadSaveConstruct();
    LevelsLoadSaveConstruct();
    MessagesLoadSaveConstruct();
    MirrorLoadSaveConstruct();
    PlayerLoadSaveConstruct();
    ViewLoadSaveConstruct();
#ifdef NOONE_EXTENSIONS
    NNLoadSaveConstruct();
#endif
}


void SerializeEvents(FSerializer& arc);
void SerializeSequences(FSerializer& arc);
void SerializeWarp(FSerializer& arc);
void SerializeTriggers(FSerializer& arc);

void GameInterface::SerializeGameState(FSerializer& arc)
{
    sndKillAllSounds();
    sfxKillAllSounds();
    ambKillAll();
    seqKillAll();
    if (gamestate != GS_LEVEL)
    {
        memset(xsprite, 0, sizeof(xsprite));
    }

    SerializeEvents(arc);
    SerializeSequences(arc);
    SerializeWarp(arc);
    SerializeTriggers(arc);
}



END_BLD_NS
