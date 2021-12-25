//-------------------------------------------------------------------------
/*
Copyright (C) 1997, 2005 - 3D Realms Entertainment

This file is part of Shadow Warrior version 1.2

Shadow Warrior is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Original Source: 1997 - Frank Maddin and Jim Norwood
Prepared for public release: 03/28/2005 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------
#include "ns.h"

#include "build.h"

#include "names2.h"
#include "panel.h"
#include "misc.h"
#include "tags.h"
#include "break.h"
#include "network.h"
#include "pal.h"

#include "ai.h"
#include "weapon.h"

#include "sprite.h"
#include "sector.h"

BEGIN_SW_NS

ANIMATOR DoSuicide;
ANIMATOR DoBloodSpray;
void SpawnFlashBombOnActor(DSWActor* actor);

ANIMATOR DoPuff, BloodSprayFall;
extern STATE s_Puff[];
extern STATE s_FireballFlames[];
extern STATE s_GoreFloorSplash[];
extern STATE s_GoreSplash[];
extern bool GlobalSkipZrange;

#define CHEMTICS SEC(40)

#define GOREDrip 1562 //2430
#define BLOODSPRAY_RATE 20

STATE s_BloodSpray[] =
{
    {GOREDrip + 0, BLOODSPRAY_RATE, BloodSprayFall, &s_BloodSpray[1]},
    {GOREDrip + 1, BLOODSPRAY_RATE, BloodSprayFall, &s_BloodSpray[2]},
    {GOREDrip + 2, BLOODSPRAY_RATE, BloodSprayFall, &s_BloodSpray[3]},
    {GOREDrip + 3, BLOODSPRAY_RATE, BloodSprayFall, &s_BloodSpray[4]},
    {GOREDrip + 3, 100, DoSuicide, &s_BloodSpray[0]}
};


#define EXP_RATE 2
STATE s_PhosphorExp[] =
{
    {EXP + 0, EXP_RATE, NullAnimator, &s_PhosphorExp[1]},
    {EXP + 1, EXP_RATE, NullAnimator, &s_PhosphorExp[2]},
    {EXP + 2, EXP_RATE, NullAnimator, &s_PhosphorExp[3]},
    {EXP + 3, EXP_RATE, NullAnimator, &s_PhosphorExp[4]},
    {EXP + 4, EXP_RATE, NullAnimator, &s_PhosphorExp[5]},
    {EXP + 5, EXP_RATE, NullAnimator, &s_PhosphorExp[6]},
    {EXP + 6, EXP_RATE, NullAnimator, &s_PhosphorExp[7]},
    {EXP + 7, EXP_RATE, NullAnimator, &s_PhosphorExp[8]},
    {EXP + 8, EXP_RATE, NullAnimator, &s_PhosphorExp[9]},
    {EXP + 9, EXP_RATE, NullAnimator, &s_PhosphorExp[10]},
    {EXP + 10, EXP_RATE, NullAnimator, &s_PhosphorExp[11]},
    {EXP + 11, EXP_RATE, NullAnimator, &s_PhosphorExp[12]},
    {EXP + 12, EXP_RATE, NullAnimator, &s_PhosphorExp[13]},
    {EXP + 13, EXP_RATE, NullAnimator, &s_PhosphorExp[14]},
    {EXP + 14, EXP_RATE, NullAnimator, &s_PhosphorExp[15]},
    {EXP + 15, EXP_RATE, NullAnimator, &s_PhosphorExp[16]},
    {EXP + 16, EXP_RATE, NullAnimator, &s_PhosphorExp[17]},
    {EXP + 17, EXP_RATE, NullAnimator, &s_PhosphorExp[18]},
    {EXP + 18, EXP_RATE, NullAnimator, &s_PhosphorExp[19]},
    {EXP + 19, EXP_RATE, NullAnimator, &s_PhosphorExp[20]},
    {EXP + 20, 100, DoSuicide, &s_PhosphorExp[0]}
};

#define MUSHROOM_RATE 25

STATE s_NukeMushroom[] =
{
    {MUSHROOM_CLOUD + 0, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[1]},
    {MUSHROOM_CLOUD + 1, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[2]},
    {MUSHROOM_CLOUD + 2, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[3]},
    {MUSHROOM_CLOUD + 3, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[4]},
    {MUSHROOM_CLOUD + 4, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[5]},
    {MUSHROOM_CLOUD + 5, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[6]},
    {MUSHROOM_CLOUD + 6, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[7]},
    {MUSHROOM_CLOUD + 7, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[8]},
    {MUSHROOM_CLOUD + 8, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[9]},
    {MUSHROOM_CLOUD + 9, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[10]},
    {MUSHROOM_CLOUD + 10, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[11]},
    {MUSHROOM_CLOUD + 11, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[12]},
    {MUSHROOM_CLOUD + 12, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[13]},
    {MUSHROOM_CLOUD + 13, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[14]},
    {MUSHROOM_CLOUD + 14, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[15]},
    {MUSHROOM_CLOUD + 15, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[16]},
    {MUSHROOM_CLOUD + 16, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[17]},
    {MUSHROOM_CLOUD + 17, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[18]},
    {MUSHROOM_CLOUD + 18, MUSHROOM_RATE, NullAnimator, &s_NukeMushroom[19]},
    {MUSHROOM_CLOUD + 19, 100, DoSuicide, &s_NukeMushroom[0]},
};

ANIMATOR DoRadiationCloud;

#define RADIATION_RATE 16

STATE s_RadiationCloud[] =
{
    {RADIATION_CLOUD + 0, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[1]},
    {RADIATION_CLOUD + 1, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[2]},
    {RADIATION_CLOUD + 2, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[3]},
    {RADIATION_CLOUD + 3, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[4]},
    {RADIATION_CLOUD + 4, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[5]},
    {RADIATION_CLOUD + 5, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[6]},
    {RADIATION_CLOUD + 6, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[7]},
    {RADIATION_CLOUD + 7, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[8]},
    {RADIATION_CLOUD + 8, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[9]},
    {RADIATION_CLOUD + 9, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[10]},
    {RADIATION_CLOUD + 10, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[11]},
    {RADIATION_CLOUD + 11, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[12]},
    {RADIATION_CLOUD + 12, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[13]},
    {RADIATION_CLOUD + 13, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[14]},
    {RADIATION_CLOUD + 14, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[15]},
    {RADIATION_CLOUD + 15, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[16]},
    {RADIATION_CLOUD + 16, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[17]},
    {RADIATION_CLOUD + 17, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[18]},
    {RADIATION_CLOUD + 18, RADIATION_RATE, DoRadiationCloud, &s_RadiationCloud[19]},
    {RADIATION_CLOUD + 19, 100, DoSuicide, &s_RadiationCloud[0]},
};

#define CHEMBOMB_FRAMES 1
#define CHEMBOMB_R0 3038
#define CHEMBOMB_R1 CHEMBOMB_R0 + (CHEMBOMB_FRAMES * 1)
#define CHEMBOMB_R2 CHEMBOMB_R0 + (CHEMBOMB_FRAMES * 2)
#define CHEMBOMB_R3 CHEMBOMB_R0 + (CHEMBOMB_FRAMES * 3)
#define CHEMBOMB_R4 CHEMBOMB_R0 + (CHEMBOMB_FRAMES * 4)

#define CHEMBOMB CHEMBOMB_R0
#define CHEMBOMB_RATE 8
ANIMATOR DoChemBomb;

STATE s_ChemBomb[5] =
{
    {CHEMBOMB_R0 + 0, CHEMBOMB_RATE, DoChemBomb, &s_ChemBomb[1]},
    {CHEMBOMB_R1 + 0, CHEMBOMB_RATE, DoChemBomb, &s_ChemBomb[2]},
    {CHEMBOMB_R2 + 0, CHEMBOMB_RATE, DoChemBomb, &s_ChemBomb[3]},
    {CHEMBOMB_R3 + 0, CHEMBOMB_RATE, DoChemBomb, &s_ChemBomb[4]},
    {CHEMBOMB_R4 + 0, CHEMBOMB_RATE, DoChemBomb, &s_ChemBomb[0]},
};


#define CALTROPS_FRAMES 1
#define CALTROPS_R0 CALTROPS-1

#define CALTROPS_RATE 8

ANIMATOR DoCaltrops, DoCaltropsStick;

STATE s_Caltrops[] =
{
    {CALTROPS_R0 + 0, CALTROPS_RATE, DoCaltrops, &s_Caltrops[1]},
    {CALTROPS_R0 + 1, CALTROPS_RATE, DoCaltrops, &s_Caltrops[2]},
    {CALTROPS_R0 + 2, CALTROPS_RATE, DoCaltrops, &s_Caltrops[0]},
};

STATE s_CaltropsStick[] =
{
    {CALTROPS_R0 + 2, CALTROPS_RATE, DoCaltropsStick, &s_CaltropsStick[0]},
};

//////////////////////
//
// CAPTURE FLAG
//
//////////////////////

ANIMATOR DoFlag, DoCarryFlag, DoCarryFlagNoDet;

#undef FLAG
#define FLAG 2520
#define FLAG_RATE 16

STATE s_CarryFlag[] =
{
    {FLAG + 0, FLAG_RATE, DoCarryFlag, &s_CarryFlag[1]},
    {FLAG + 1, FLAG_RATE, DoCarryFlag, &s_CarryFlag[2]},
    {FLAG + 2, FLAG_RATE, DoCarryFlag, &s_CarryFlag[0]}
};

STATE s_CarryFlagNoDet[] =
{
    {FLAG + 0, FLAG_RATE, DoCarryFlagNoDet, &s_CarryFlagNoDet[1]},
    {FLAG + 1, FLAG_RATE, DoCarryFlagNoDet, &s_CarryFlagNoDet[2]},
    {FLAG + 2, FLAG_RATE, DoCarryFlagNoDet, &s_CarryFlagNoDet[0]}
};

STATE s_Flag[] =
{
    {FLAG + 0, FLAG_RATE, DoFlag, &s_Flag[1]},
    {FLAG + 1, FLAG_RATE, DoFlag, &s_Flag[2]},
    {FLAG + 2, FLAG_RATE, DoFlag, &s_Flag[0]}
};

#define PHOSPHORUS_RATE 8
ANIMATOR DoPhosphorus;

STATE s_Phosphorus[] =
{
    {PHOSPHORUS + 0, PHOSPHORUS_RATE, DoPhosphorus, &s_Phosphorus[1]},
    {PHOSPHORUS + 1, PHOSPHORUS_RATE, DoPhosphorus, &s_Phosphorus[0]},
};

ANIMATOR DoBloodSpray;

#define CHUNK1 1685
STATE s_BloodSprayChunk[] =
{
    {CHUNK1 + 0, 8, DoBloodSpray, &s_BloodSprayChunk[1]},
    {CHUNK1 + 1, 8, DoBloodSpray, &s_BloodSprayChunk[2]},
    {CHUNK1 + 2, 8, DoBloodSpray, &s_BloodSprayChunk[3]},
    {CHUNK1 + 3, 8, DoBloodSpray, &s_BloodSprayChunk[4]},
    {CHUNK1 + 4, 8, DoBloodSpray, &s_BloodSprayChunk[5]},
    {CHUNK1 + 5, 8, DoBloodSpray, &s_BloodSprayChunk[0]},
};

ANIMATOR DoWallBloodDrip;

#define DRIP 1566
STATE s_BloodSprayDrip[] =
{
    {DRIP + 0, PHOSPHORUS_RATE, DoWallBloodDrip, &s_BloodSprayDrip[1]},
    {DRIP + 1, PHOSPHORUS_RATE, DoWallBloodDrip, &s_BloodSprayDrip[2]},
    {DRIP + 2, PHOSPHORUS_RATE, DoWallBloodDrip, &s_BloodSprayDrip[0]},
};

/////////////////////////////////////////////////////////////////////////////////////////////

int DoWallBloodDrip(DSWActor* actor)
{
    USER* u = actor->u();

    //actor->spr.z += (300+RandomRange(2300)) >> 1;

    // sy & sz are the ceiling and floor of the sector you are sliding down
    if (u->sz != u->sy)
    {
        // if you are between the ceiling and floor fall fast
        if (actor->spr.pos.Z > u->sy && actor->spr.pos.Z < u->sz)
        {
            actor->spr.zvel += 300;
            actor->spr.pos.Z += actor->spr.zvel;
        }
        else
        {
            actor->spr.zvel = (300+RandomRange(2300)) >> 1;
            actor->spr.pos.Z += actor->spr.zvel;
        }
    }
    else
    {
        actor->spr.zvel = (300+RandomRange(2300)) >> 1;
        actor->spr.pos.Z += actor->spr.zvel;
    }

    if (actor->spr.pos.Z >= u->loz)
    {
        actor->spr.pos.Z = u->loz;
        SpawnFloorSplash(actor);
        KillActor(actor);
        return 0;
    }

    return 0;
}

void SpawnMidSplash(DSWActor* actor)
{
    USERp u = actor->u();
    USERp nu;

    auto actorNew = SpawnActor(STAT_MISSILE, GOREDrip, s_GoreSplash, actor->spr.sector(),
                      actor->spr.pos.X, actor->spr.pos.Y, ActorZOfMiddle(actor), actor->spr.ang, 0);

    nu = actorNew->u();

    actorNew->spr.shade = -12;
    actorNew->spr.xrepeat = 70-RandomRange(20);
    actorNew->spr.yrepeat = 70-RandomRange(20);
    actorNew->spr.opos = actor->spr.opos;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);

    if (RANDOM_P2(1024) < 512)
        SET(actorNew->spr.cstat, CSTAT_SPRITE_XFLIP);

    actorNew->user.xchange = 0;
    actorNew->user.ychange = 0;
    actorNew->user.zchange = 0;

    if (TEST(u->Flags, SPR_UNDERWATER))
        SET(actorNew->user.Flags, SPR_UNDERWATER);
}

void SpawnFloorSplash(DSWActor* actor)
{
    USERp u = actor->u();
    USERp nu;

    auto actorNew = SpawnActor(STAT_MISSILE, GOREDrip, s_GoreFloorSplash, actor->spr.sector(),
                      actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, 0);

    nu = actorNew->u();

    actorNew->spr.shade = -12;
    actorNew->spr.xrepeat = 70-RandomRange(20);
    actorNew->spr.yrepeat = 70-RandomRange(20);
    actorNew->spr.opos = actor->spr.opos;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);

    if (RANDOM_P2(1024) < 512)
        SET(actorNew->spr.cstat, CSTAT_SPRITE_XFLIP);

    actorNew->user.xchange = 0;
    actorNew->user.ychange = 0;
    actorNew->user.zchange = 0;

    if (TEST(u->Flags, SPR_UNDERWATER))
        SET(actorNew->user.Flags, SPR_UNDERWATER);
}


int DoBloodSpray(DSWActor* actor)
{
    USER* u = actor->u();
    int cz,fz;

    if (TEST(u->Flags, SPR_UNDERWATER))
    {
        ScaleSpriteVector(actor, 50000);

        u->Counter += 20;  // These are STAT_SKIIP4 now, so * 2
        u->zchange += u->Counter;
    }
    else
    {
        u->Counter += 20;
        u->zchange += u->Counter;
    }

    if (actor->spr.xvel <= 2)
    {
        // special stuff for blood worm
        actor->spr.pos.Z += (u->zchange >> 1);

        getzsofslopeptr(actor->spr.sector(), actor->spr.pos.X, actor->spr.pos.Y, &cz, &fz);
        // pretend like we hit a sector
        if (actor->spr.pos.Z >= fz)
        {
            actor->spr.pos.Z = fz;
            SpawnFloorSplash(actor);
            KillActor(actor);
            return true;
        }
    }
    else
    {
        u->coll = move_missile(actor, u->xchange, u->ychange, u->zchange,
                              u->ceiling_dist, u->floor_dist, CLIPMASK_MISSILE, MISSILEMOVETICS);
    }


    MissileHitDiveArea(actor);

    {
        switch (u->coll.type)
        {
        case kHitVoid:
            KillActor(actor);
            return true;
        case kHitSprite:
        {
            short wall_ang;
            auto hitActor = u->coll.actor();

            if (TEST(hitActor->spr.cstat, CSTAT_SPRITE_ALIGNMENT_WALL))
            {
                wall_ang = NORM_ANGLE(hitActor->spr.ang);
                SpawnMidSplash(actor);
                QueueWallBlood(actor, hitActor->spr.ang);
                WallBounce(actor, wall_ang);
                ScaleSpriteVector(actor, 32000);
            }
            else
            {
                u->xchange = u->ychange = 0;
                SpawnMidSplash(actor);
                QueueWallBlood(actor, hitActor->spr.ang);
                KillActor(actor);
                return true;
            }


            break;
        }

        case kHitWall:
        {
            short hit_wall, nw, wall_ang;
            WALLp wph;
            short wb;

            wph = u->coll.hitWall;

            if (wph->lotag == TAG_WALL_BREAK)
            {
                HitBreakWall(wph, actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, u->ID);
                u->coll.setNone();
                break;
            }

            wall_ang = NORM_ANGLE(getangle(wph->delta()) + 512);

            SpawnMidSplash(actor);
            auto bldActor = QueueWallBlood(actor, NORM_ANGLE(wall_ang+1024));

            if (bldActor== nullptr)
            {
                KillActor(actor);
                return 0;
            }
            else
            {
                if (FAF_Sector(bldActor->spr.sector()) || FAF_ConnectArea(bldActor->spr.sector()))
                {
                    KillActor(actor);
                    return 0;
                }

                actor->spr.xvel = actor->spr.yvel = u->xchange = u->ychange = 0;
                actor->spr.xrepeat = actor->spr.yrepeat = 70 - RandomRange(25);
                actor->spr.pos.X = bldActor->spr.pos.X;
                actor->spr.pos.Y = bldActor->spr.pos.Y;

                // !FRANK! bit of a hack
                // yvel is the hit_wall
                if (bldActor->tempwall)
                {
                    // sy & sz are the ceiling and floor of the sector you are sliding down
                    if (bldActor->tempwall->twoSided())
                        getzsofslopeptr(bldActor->tempwall->nextSector(), actor->spr.pos.X, actor->spr.pos.Y, &u->sy, &u->sz);
                    else
                        u->sy = u->sz; // ceiling and floor are equal - white wall
                }

                RESET(actor->spr.cstat,CSTAT_SPRITE_INVISIBLE);
                ChangeState(actor, s_BloodSprayDrip);
            }

            break;
        }

        case kHitSector:
        {
            // hit floor
            if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
            {
                if (TEST(u->Flags, SPR_UNDERWATER))
                    SET(u->Flags, SPR_BOUNCE);  // no bouncing
                // underwater

                if (u->lo_sectp && actor->spr.sector()->hasU() && FixedToInt(actor->spr.sector()->depth_fixed))
                    SET(u->Flags, SPR_BOUNCE);  // no bouncing on
                // shallow water

#if 0
                if (!TEST(u->Flags, SPR_BOUNCE))
                {
                    SpawnFloorSplash(actor);
                    SET(u->Flags, SPR_BOUNCE);
                    u->coll.setNone();
                    u->Counter = 0;
                    u->zchange = -u->zchange;
                    ScaleSpriteVector(actor, 32000);   // Was 18000
                    u->zchange /= 6;
                }
                else
#endif
                {
                    u->xchange = u->ychange = 0;
                    SpawnFloorSplash(actor);
                    KillActor(actor);
                    return true;
                }
            }
            else
            // hit something above
            {
                u->zchange = -u->zchange;
                ScaleSpriteVector(actor, 32000);       // was 22000
            }
            break;
        }
        }
    }



    // if you haven't bounced or your going slow do some puffs
    if (!TEST(u->Flags, SPR_BOUNCE | SPR_UNDERWATER))
    {

        auto actorNew = SpawnActor(STAT_MISSILE, GOREDrip, s_BloodSpray, actor->spr.sector(),
                          actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, 100);

        auto nu = actorNew->u();

        SetOwner(actor, actorNew);
        actorNew->spr.shade = -12;
        actorNew->spr.xrepeat = 40-RandomRange(30);
        actorNew->spr.yrepeat = 40-RandomRange(30);
        actorNew->spr.opos = actor->spr.opos;
        SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
        RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);

        if (RANDOM_P2(1024) < 512)
            SET(actorNew->spr.cstat, CSTAT_SPRITE_XFLIP);
        if (RANDOM_P2(1024) < 512)
            SET(actorNew->spr.cstat, CSTAT_SPRITE_YFLIP);

        actorNew->user.xchange = u->xchange;
        actorNew->user.ychange = u->ychange;
        actorNew->user.zchange = u->zchange;

        ScaleSpriteVector(actorNew, 20000);

        if (TEST(u->Flags, SPR_UNDERWATER))
            SET(actorNew->user.Flags, SPR_UNDERWATER);
    }

    return false;
}


int DoPhosphorus(DSWActor* actor)
{
    USER* u = actor->u();

    if (TEST(u->Flags, SPR_UNDERWATER))
    {
        ScaleSpriteVector(actor, 50000);

        u->Counter += 20*2;
        u->zchange += u->Counter;
    }
    else
    {
        u->Counter += 20*2;
        u->zchange += u->Counter;
    }

    u->coll = move_missile(actor, u->xchange, u->ychange, u->zchange,
                          u->ceiling_dist, u->floor_dist, CLIPMASK_MISSILE, MISSILEMOVETICS*2);

    MissileHitDiveArea(actor);

    if (TEST(u->Flags, SPR_UNDERWATER) && (RANDOM_P2(1024 << 4) >> 4) < 256)
        SpawnBubble(actor);

    {
        switch (u->coll.type)
        {
        case kHitVoid:
            KillActor(actor);
            return true;
        case kHitSprite:
        {
            short wall_ang;
            USERp hu;

            auto hitActor = u->coll.actor();
            hu = hitActor->u();

            if (TEST(hitActor->spr.cstat, CSTAT_SPRITE_ALIGNMENT_WALL))
            {
                wall_ang = NORM_ANGLE(hitActor->spr.ang);
                WallBounce(actor, wall_ang);
                ScaleSpriteVector(actor, 32000);
            }
            else
            {
                if (TEST(hitActor->spr.extra, SPRX_BURNABLE))
                {
                    if (!hu)
                        hu = SpawnUser(hitActor, hitActor->spr.picnum, nullptr);
                    SpawnFireballExp(actor);
                    if (hu)
                        SpawnFireballFlames(actor, hitActor);
                    DoFlamesDamageTest(actor);
                }
                u->xchange = u->ychange = 0;
                KillActor(actor);
                return true;
            }


            break;
        }

        case kHitWall:
        {
            short hit_wall, nw, wall_ang;
            WALLp wph;

            wph = u->coll.hitWall;

            if (wph->lotag == TAG_WALL_BREAK)
            {
                HitBreakWall(wph, actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, u->ID);
                u->coll.setNone();
                break;
            }

            wall_ang = NORM_ANGLE(getangle(wph->delta()) + 512);

            WallBounce(actor, wall_ang);
            ScaleSpriteVector(actor, 32000);
            break;
        }

        case kHitSector:
        {
            bool did_hit_wall;

            if (SlopeBounce(actor, &did_hit_wall))
            {
                if (did_hit_wall)
                {
                    // hit a wall
                    ScaleSpriteVector(actor, 28000);
                    u->coll.setNone();
                    u->Counter = 0;
                }
                else
                {
                    // hit a sector
                    if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
                    {
                        // hit a floor
                        if (!TEST(u->Flags, SPR_BOUNCE))
                        {
                            SET(u->Flags, SPR_BOUNCE);
                            ScaleSpriteVector(actor, 32000);       // was 18000
                            u->zchange /= 6;
                            u->coll.setNone();
                            u->Counter = 0;
                        }
                        else
                        {
                            u->xchange = u->ychange = 0;
                            SpawnFireballExp(actor);
                            KillActor(actor);
                            return true;
                        }
                    }
                    else
                    {
                        // hit a ceiling
                        ScaleSpriteVector(actor, 32000);   // was 22000
                    }
                }
            }
            else
            {
                // hit floor
                if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
                {
                    if (TEST(u->Flags, SPR_UNDERWATER))
                        SET(u->Flags, SPR_BOUNCE);  // no bouncing
                    // underwater

                    if (u->lo_sectp && actor->spr.sector()->hasU() && FixedToInt(actor->spr.sector()->depth_fixed))
                        SET(u->Flags, SPR_BOUNCE);  // no bouncing on
                    // shallow water

                    if (!TEST(u->Flags, SPR_BOUNCE))
                    {
                        SET(u->Flags, SPR_BOUNCE);
                        u->coll.setNone();
                        u->Counter = 0;
                        u->zchange = -u->zchange;
                        ScaleSpriteVector(actor, 32000);   // Was 18000
                        u->zchange /= 6;
                    }
                    else
                    {
                        u->xchange = u->ychange = 0;
                        SpawnFireballExp(actor);
                        KillActor(actor);
                        return true;
                    }
                }
                else
                // hit something above
                {
                    u->zchange = -u->zchange;
                    ScaleSpriteVector(actor, 32000);       // was 22000
                }
            }
            break;
        }
        }
    }



    // if you haven't bounced or your going slow do some puffs
    if (!TEST(u->Flags, SPR_BOUNCE | SPR_UNDERWATER) && !TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
    {

        auto actorNew = SpawnActor(STAT_SKIP4, PUFF, s_PhosphorExp, actor->spr.sector(),
                          actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, 100);

        auto nu = actorNew->u();

        actorNew->spr.hitag = LUMINOUS;           // Always full brightness
        SetOwner(actor, actorNew);
        actorNew->spr.shade = -40;
        actorNew->spr.xrepeat = 12 + RandomRange(10);
        actorNew->spr.yrepeat = 12 + RandomRange(10);
        actorNew->spr.opos = actor->spr.opos;
        SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
        RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);

        if (RANDOM_P2(1024) < 512)
            SET(actorNew->spr.cstat, CSTAT_SPRITE_XFLIP);
        if (RANDOM_P2(1024) < 512)
            SET(actorNew->spr.cstat, CSTAT_SPRITE_YFLIP);

        actorNew->user.xchange = u->xchange;
        actorNew->user.ychange = u->ychange;
        actorNew->user.zchange = u->zchange;

        actorNew->user.spal = actorNew->spr.pal = PALETTE_PLAYER3;   // RED

        ScaleSpriteVector(actorNew, 20000);

        if (TEST(u->Flags, SPR_UNDERWATER))
            SET(actorNew->user.Flags, SPR_UNDERWATER);
    }

    return false;
}

int DoChemBomb(DSWActor* actor)
{
    USER* u = actor->u();

    if (TEST(u->Flags, SPR_UNDERWATER))
    {
        ScaleSpriteVector(actor, 50000);

        u->Counter += 20;
        u->zchange += u->Counter;
    }
    else
    {
        u->Counter += 20;
        u->zchange += u->Counter;
    }

    u->coll = move_missile(actor, u->xchange, u->ychange, u->zchange,
                          u->ceiling_dist, u->floor_dist, CLIPMASK_MISSILE, MISSILEMOVETICS);

    MissileHitDiveArea(actor);

    if (TEST(u->Flags, SPR_UNDERWATER) && (RANDOM_P2(1024 << 4) >> 4) < 256)
        SpawnBubble(actor);

    {
        switch (u->coll.type)
        {
        case kHitVoid:
            KillActor(actor);
            return true;
        case kHitSprite:
        {
            short wall_ang;

            if (!TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                PlaySound(DIGI_CHEMBOUNCE, actor, v3df_dontpan);

            auto hitActor = u->coll.actor();

            if (TEST(hitActor->spr.cstat, CSTAT_SPRITE_ALIGNMENT_WALL))
            {
                wall_ang = NORM_ANGLE(hitActor->spr.ang);
                WallBounce(actor, wall_ang);
                ScaleSpriteVector(actor, 32000);
            }
            else
            {
                // Canister pops when first smoke starts out
                if (u->WaitTics == CHEMTICS && !TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                {
                    PlaySound(DIGI_GASPOP, actor, v3df_dontpan | v3df_doppler);
                    PlaySound(DIGI_CHEMGAS, actor, v3df_dontpan | v3df_doppler);
                }
                u->xchange = u->ychange = 0;
                u->WaitTics -= (MISSILEMOVETICS * 2);
                if (u->WaitTics <= 0)
                    KillActor(actor);
                return true;
            }


            break;
        }

        case kHitWall:
        {
            auto wph = u->coll.hitWall;

            if (wph->lotag == TAG_WALL_BREAK)
            {
                HitBreakWall(wph, actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, u->ID);
                u->coll.setNone();
                break;
            }

            if (!TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                PlaySound(DIGI_CHEMBOUNCE, actor, v3df_dontpan);

            int wall_ang = NORM_ANGLE(getangle(wph->delta()) + 512);

            WallBounce(actor, wall_ang);
            ScaleSpriteVector(actor, 32000);
            break;
        }

        case kHitSector:
        {
            bool did_hit_wall;

            if (SlopeBounce(actor, &did_hit_wall))
            {
                if (did_hit_wall)
                {
                    // hit a wall
                    ScaleSpriteVector(actor, 28000);
                    u->coll.setNone();
                    u->Counter = 0;
                }
                else
                {
                    // hit a sector
                    if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
                    {
                        // hit a floor
                        if (!TEST(u->Flags, SPR_BOUNCE))
                        {
                            if (!TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                                PlaySound(DIGI_CHEMBOUNCE, actor, v3df_dontpan);
                            SET(u->Flags, SPR_BOUNCE);
                            ScaleSpriteVector(actor, 32000);       // was 18000
                            u->zchange /= 6;
                            u->coll.setNone();
                            u->Counter = 0;
                        }
                        else
                        {
                            // Canister pops when first smoke starts out
                            if (u->WaitTics == CHEMTICS && !TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                            {
                                PlaySound(DIGI_GASPOP, actor, v3df_dontpan | v3df_doppler);
                                PlaySound(DIGI_CHEMGAS, actor, v3df_dontpan | v3df_doppler);
                            }
                            SpawnRadiationCloud(actor);
                            u->xchange = u->ychange = 0;
                            u->WaitTics -= (MISSILEMOVETICS * 2);
                            if (u->WaitTics <= 0)
                                KillActor(actor);
                            return true;
                        }
                    }
                    else
                    {
                        // hit a ceiling
                        ScaleSpriteVector(actor, 32000);   // was 22000
                    }
                }
            }
            else
            {
                // hit floor
                if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
                {
                    if (TEST(u->Flags, SPR_UNDERWATER))
                        SET(u->Flags, SPR_BOUNCE);  // no bouncing
                    // underwater

                    if (u->lo_sectp && actor->spr.sector()->hasU() && FixedToInt(actor->spr.sector()->depth_fixed))
                        SET(u->Flags, SPR_BOUNCE);  // no bouncing on
                    // shallow water

                    if (!TEST(u->Flags, SPR_BOUNCE))
                    {
                        if (!TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                            PlaySound(DIGI_CHEMBOUNCE, actor, v3df_dontpan);
                        SET(u->Flags, SPR_BOUNCE);
                        u->coll.setNone();
                        u->Counter = 0;
                        u->zchange = -u->zchange;
                        ScaleSpriteVector(actor, 32000);   // Was 18000
                        u->zchange /= 6;
                    }
                    else
                    {
                        // Canister pops when first smoke starts out
                        if (u->WaitTics == CHEMTICS && !TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
                        {
                            PlaySound(DIGI_GASPOP, actor, v3df_dontpan | v3df_doppler);
                            PlaySound(DIGI_CHEMGAS, actor, v3df_dontpan | v3df_doppler);
                        }
                        SpawnRadiationCloud(actor);
                        u->xchange = u->ychange = 0;
                        u->WaitTics -= (MISSILEMOVETICS * 2);
                        if (u->WaitTics <= 0)
                            KillActor(actor);
                        return true;
                    }
                }
                else
                // hit something above
                {
                    u->zchange = -u->zchange;
                    ScaleSpriteVector(actor, 32000);       // was 22000
                }
            }
            break;
        }
        }
    }

    // if you haven't bounced or your going slow do some puffs
    if (!TEST(u->Flags, SPR_BOUNCE | SPR_UNDERWATER) && !TEST(actor->spr.cstat, CSTAT_SPRITE_INVISIBLE))
    {
        auto actorNew = SpawnActor(STAT_MISSILE, PUFF, s_Puff, actor->spr.sector(),
                          actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, 100);

        auto nu = actorNew->u();

        SetOwner(actor, actorNew);
        actorNew->spr.shade = -40;
        actorNew->spr.xrepeat = 40;
        actorNew->spr.yrepeat = 40;
        actorNew->spr.opos = actor->spr.opos;
        // !Frank - dont do translucent
        SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
        // SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER|CSTAT_SPRITE_TRANSLUCENT);
        RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);

        actorNew->user.xchange = u->xchange;
        actorNew->user.ychange = u->ychange;
        actorNew->user.zchange = u->zchange;

        actorNew->user.spal = actorNew->spr.pal = PALETTE_PLAYER6;

        ScaleSpriteVector(actorNew, 20000);

        if (TEST(u->Flags, SPR_UNDERWATER))
            SET(actorNew->user.Flags, SPR_UNDERWATER);
    }

    return false;
}

int DoCaltropsStick(DSWActor* actor)
{
    USER* u = actor->u();

    u->Counter = !u->Counter;

    if (u->Counter)
        DoFlamesDamageTest(actor);

    return 0;
}

int DoCaltrops(DSWActor* actor)
{
    USER* u = actor->u();

    if (TEST(u->Flags, SPR_UNDERWATER))
    {
        ScaleSpriteVector(actor, 50000);

        u->Counter += 20;
        u->zchange += u->Counter;
    }
    else
    {
        u->Counter += 70;
        u->zchange += u->Counter;
    }

    u->coll = move_missile(actor, u->xchange, u->ychange, u->zchange,
                          u->ceiling_dist, u->floor_dist, CLIPMASK_MISSILE, MISSILEMOVETICS);

    MissileHitDiveArea(actor);

    {
        switch (u->coll.type)
        {
        case kHitVoid:
            KillActor(actor);
            return true;
        case kHitSprite:
        {
            short wall_ang;

            PlaySound(DIGI_CALTROPS, actor, v3df_dontpan);

            auto hitActor = u->coll.actor();

            if (TEST(hitActor->spr.cstat, CSTAT_SPRITE_ALIGNMENT_WALL))
            {
                wall_ang = NORM_ANGLE(hitActor->spr.ang);
                WallBounce(actor, wall_ang);
                ScaleSpriteVector(actor, 10000);
            }
            else
            {
                // fall to the ground
                u->xchange = u->ychange = 0;
            }


            break;
        }

        case kHitWall:
        {
            auto wph = u->coll.hitWall;

            if (wph->lotag == TAG_WALL_BREAK)
            {
                HitBreakWall(wph, actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, u->ID);
                u->coll.setNone();
                break;
            }

            PlaySound(DIGI_CALTROPS, actor, v3df_dontpan);

            int wall_ang = NORM_ANGLE(getangle(wph->delta()) + 512);

            WallBounce(actor, wall_ang);
            ScaleSpriteVector(actor, 1000);
            break;
        }

        case kHitSector:
        {
            bool did_hit_wall;

            if (SlopeBounce(actor, &did_hit_wall))
            {
                if (did_hit_wall)
                {
                    // hit a wall
                    ScaleSpriteVector(actor, 1000);
                    u->coll.setNone();
                    u->Counter = 0;
                }
                else
                {
                    // hit a sector
                    if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
                    {
                        // hit a floor
                        if (!TEST(u->Flags, SPR_BOUNCE))
                        {
                            PlaySound(DIGI_CALTROPS, actor, v3df_dontpan);
                            SET(u->Flags, SPR_BOUNCE);
                            ScaleSpriteVector(actor, 1000);        // was 18000
                            u->coll.setNone();
                            u->Counter = 0;
                        }
                        else
                        {
                            u->xchange = u->ychange = 0;
                            SET(actor->spr.extra, SPRX_BREAKABLE);
                            SET(actor->spr.cstat,CSTAT_SPRITE_BREAKABLE);
                            ChangeState(actor, s_CaltropsStick);
                            return true;
                        }
                    }
                    else
                    {
                        // hit a ceiling
                        ScaleSpriteVector(actor, 1000);    // was 22000
                    }
                }
            }
            else
            {
                // hit floor
                if (actor->spr.pos.Z > ((u->hiz + u->loz) >> 1))
                {
                    if (TEST(u->Flags, SPR_UNDERWATER))
                        SET(u->Flags, SPR_BOUNCE);  // no bouncing
                    // underwater

                    if (u->lo_sectp && actor->spr.sector()->hasU() && FixedToInt(actor->spr.sector()->depth_fixed))
                        SET(u->Flags, SPR_BOUNCE);  // no bouncing on
                    // shallow water

                    if (!TEST(u->Flags, SPR_BOUNCE))
                    {
                        PlaySound(DIGI_CALTROPS, actor, v3df_dontpan);
                        SET(u->Flags, SPR_BOUNCE);
                        u->coll.setNone();
                        u->Counter = 0;
                        u->zchange = -u->zchange;
                        ScaleSpriteVector(actor, 1000);    // Was 18000
                    }
                    else
                    {
                        u->xchange = u->ychange = 0;
                        SET(actor->spr.extra, SPRX_BREAKABLE);
                        SET(actor->spr.cstat,CSTAT_SPRITE_BREAKABLE);
                        ChangeState(actor, s_CaltropsStick);
                        return true;
                    }
                }
                else
                // hit something above
                {
                    u->zchange = -u->zchange;
                    ScaleSpriteVector(actor, 1000);        // was 22000
                }
            }
            break;
        }
        }
    }


    return false;
}

/////////////////////////////
//
// Deadly green gas clouds
//
/////////////////////////////

int SpawnRadiationCloud(DSWActor* actor)
{
    USERp u = actor->u(), nu;

    if (!MoveSkip4)
        return false;

    // This basically works like a MoveSkip8, if one existed
//  u->Counter2 = !u->Counter2;
    if (u->ID == MUSHROOM_CLOUD || u->ID == 3121)
    {
        if ((u->Counter2++) > 16)
            u->Counter2 = 0;
        if (u->Counter2)
            return false;
    }
    else
    {
        if ((u->Counter2++) > 2)
            u->Counter2 = 0;
        if (u->Counter2)
            return false;
    }

    if (TEST(u->Flags, SPR_UNDERWATER))
        return -1;

    auto actorNew = SpawnActor(STAT_MISSILE, RADIATION_CLOUD, s_RadiationCloud, actor->spr.sector(),
                      actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z - RANDOM_P2(Z(8)), actor->spr.ang, 0);

    nu = actorNew->u();

    SetOwner(GetOwner(actor), actorNew);
    actorNew->user.WaitTics = 1 * 120;
    actorNew->spr.shade = -40;
    actorNew->spr.xrepeat = 32;
    actorNew->spr.yrepeat = 32;
    actorNew->spr.clipdist = actor->spr.clipdist;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);
    actorNew->user.spal = actorNew->spr.pal = PALETTE_PLAYER6;
    // Won't take floor palettes
    actorNew->spr.hitag = SECTFU_DONT_COPY_PALETTE;

    if (RANDOM_P2(1024) < 512)
        SET(actorNew->spr.cstat, CSTAT_SPRITE_XFLIP);
    //if (RANDOM_P2(1024) < 512)
    //SET(actorNew->spr.cstat, CSTAT_SPRITE_YFLIP);

    actorNew->spr.ang = RANDOM_P2(2048);
    actorNew->spr.xvel = RANDOM_P2(32);

    actorNew->user.Counter = 0;
    actorNew->user.Counter2 = 0;

    if (u->ID == MUSHROOM_CLOUD || u->ID == 3121)
    {
        actorNew->user.Radius = 2000;
        actorNew->user.xchange = (MOVEx(actorNew->spr.xvel>>2, actorNew->spr.ang));
        actorNew->user.ychange = (MOVEy(actorNew->spr.xvel>>2, actorNew->spr.ang));
        actorNew->spr.zvel = Z(1) + RANDOM_P2(Z(2));
    }
    else
    {
        actorNew->user.xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
        actorNew->user.ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
        actorNew->spr.zvel = Z(4) + RANDOM_P2(Z(4));
        actorNew->user.Radius = 4000;
    }

    return false;
}

int DoRadiationCloud(DSWActor* actor)
{
    USER* u = actor->u();

    actor->spr.pos.Z -= actor->spr.zvel;

    actor->spr.pos.X += u->xchange;
    actor->spr.pos.Y += u->ychange;

    if (u->ID)
    {
        DoFlamesDamageTest(actor);
    }

    return false;
}

//////////////////////////////////////////////
//
// Inventory Chemical Bombs
//
//////////////////////////////////////////////
int PlayerInitChemBomb(PLAYERp pp)
{
    USERp u = pp->Actor()->u();
    USERp wu;
    int nx, ny, nz;
    short oclipdist;


    PlaySound(DIGI_THROW, pp, v3df_dontpan | v3df_doppler);

    if (!pp->insector())
        return 0;

    nx = pp->pos.X;
    ny = pp->pos.Y;
    nz = pp->pos.Z + pp->bob_z + Z(8);

    // Spawn a shot
    // Inserting and setting up variables
    auto actorNew = SpawnActor(STAT_MISSILE, CHEMBOMB, s_ChemBomb, pp->cursector,
                    nx, ny, nz, pp->angle.ang.asbuild(), CHEMBOMB_VELOCITY);

    wu = actorNew->u();

    // don't throw it as far if crawling
    if (TEST(pp->Flags, PF_CRAWLING))
    {
        actorNew->spr.xvel -= (actorNew->spr.xvel >> 2);
    }

//    wu->RotNum = 5;
    SET(wu->Flags, SPR_XFLIP_TOGGLE);

    SetOwner(pp->Actor(), actorNew);
    actorNew->spr.yrepeat = 32;
    actorNew->spr.xrepeat = 32;
    actorNew->spr.shade = -15;
    wu->WeaponNum = u->WeaponNum;
    wu->Radius = 200;
    wu->ceiling_dist = Z(3);
    wu->floor_dist = Z(3);
    wu->Counter = 0;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    SET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK);

    if (TEST(pp->Flags, PF_DIVING) || SpriteInUnderwaterArea(actorNew))
        SET(wu->Flags, SPR_UNDERWATER);

    actorNew->spr.zvel = -pp->horizon.horiz.asq16() >> 9;

    DSWActor* plActor = pp->actor;
    oclipdist = plActor->spr.clipdist;
    plActor->spr.clipdist = 0;
    actorNew->spr.clipdist = 0;

    MissileSetPos(actorNew, DoChemBomb, 1000);

    plActor->spr.clipdist = uint8_t(oclipdist);
    actorNew->spr.clipdist = 80L >> 2;

    wu->xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
    wu->ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
    wu->zchange = actorNew->spr.zvel >> 1;

    // adjust xvel according to player velocity
    wu->xchange += pp->xvect >> 14;
    wu->ychange += pp->yvect >> 14;

    // Smoke will come out for this many seconds
    wu->WaitTics = CHEMTICS;

    return 0;
}

int InitSpriteChemBomb(DSWActor* actor)
{
    USERp u = actor->u();
    USERp wu;
    int nx, ny, nz;


    PlaySound(DIGI_THROW, actor, v3df_dontpan | v3df_doppler);

    nx = actor->spr.pos.X;
    ny = actor->spr.pos.Y;
    nz = actor->spr.pos.Z;

    // Spawn a shot
    // Inserting and setting up variables
    auto actorNew = SpawnActor(STAT_MISSILE, CHEMBOMB, s_ChemBomb, actor->spr.sector(),
                    nx, ny, nz, actor->spr.ang, CHEMBOMB_VELOCITY);

    wu = actorNew->u();

    SET(wu->Flags, SPR_XFLIP_TOGGLE);

    SetOwner(actor, actorNew);
    actorNew->spr.yrepeat = 32;
    actorNew->spr.xrepeat = 32;
    actorNew->spr.shade = -15;
    wu->WeaponNum = u->WeaponNum;
    wu->Radius = 200;
    wu->ceiling_dist = Z(3);
    wu->floor_dist = Z(3);
    wu->Counter = 0;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    SET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK);

    actorNew->spr.zvel = short(-RandomRange(100) * HORIZ_MULT);

    actorNew->spr.clipdist = 80L >> 2;

    wu->xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
    wu->ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
    wu->zchange = actorNew->spr.zvel >> 1;

    // Smoke will come out for this many seconds
    wu->WaitTics = CHEMTICS;

    return 0;
}


int InitChemBomb(DSWActor* actor)
{
    USERp u = actor->u();
    USERp wu;
    int nx, ny, nz;


// Need to make it take away from inventory weapon list
//    PlayerUpdateAmmo(pp, u->WeaponNum, -1);

    nx = actor->spr.pos.X;
    ny = actor->spr.pos.Y;
    nz = actor->spr.pos.Z;

    // Spawn a shot
    // Inserting and setting up variables
    auto actorNew = SpawnActor(STAT_MISSILE, MUSHROOM_CLOUD, s_ChemBomb, actor->spr.sector(),
                    nx, ny, nz, actor->spr.ang, CHEMBOMB_VELOCITY);

    wu = actorNew->u();

    SET(wu->Flags, SPR_XFLIP_TOGGLE);

    SetOwner(GetOwner(actor), actorNew);
    actorNew->spr.yrepeat = 32;
    actorNew->spr.xrepeat = 32;
    actorNew->spr.shade = -15;
    wu->Radius = 200;
    wu->ceiling_dist = Z(3);
    wu->floor_dist = Z(3);
    wu->Counter = 0;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER | CSTAT_SPRITE_INVISIBLE);      // Make nuke radiation
    // invis.
    RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK);

    if (SpriteInUnderwaterArea(actorNew))
        SET(wu->Flags, SPR_UNDERWATER);

    actorNew->spr.zvel = short(-RandomRange(100) * HORIZ_MULT);
    actorNew->spr.clipdist = 0;

    if (u->ID == MUSHROOM_CLOUD || u->ID == 3121 || u->ID == SUMO_RUN_R0) // 3121 == GRENADE_EXP
    {
        wu->xchange = 0;
        wu->ychange = 0;
        wu->zchange = 0;
        actorNew->spr.xvel = actorNew->spr.yvel = actorNew->spr.zvel = 0;
        // Smoke will come out for this many seconds
        wu->WaitTics = 40*120;
    }
    else
    {
        wu->xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
        wu->ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
        wu->zchange = actorNew->spr.zvel >> 1;
        // Smoke will come out for this many seconds
        wu->WaitTics = 3*120;
    }


    return 0;
}

//////////////////////////////////////////////
//
// Inventory Flash Bombs
//
//////////////////////////////////////////////

int PlayerInitFlashBomb(PLAYERp pp)
{
    unsigned int stat;
    int dist, tx, ty, tmin;
    short damage;
    DSWActor* actor = pp->actor;
    USERp hu;

    PlaySound(DIGI_GASPOP, pp, v3df_dontpan | v3df_doppler);

    // Set it just a little to let player know what he just did
    SetFadeAmt(pp, -30, 1);             // White flash

    for (stat = 0; stat < SIZ(StatDamageList); stat++)
    {
        SWStatIterator it(StatDamageList[stat]);
        while (auto itActor = it.Next())
        {
            hu = itActor->u();

            if (itActor == pp->Actor())
                break;

            DISTANCE(itActor->spr.pos.X, itActor->spr.pos.Y, actor->spr.pos.X, actor->spr.pos.Y, dist, tx, ty, tmin);
            if (dist > 16384)           // Flash radius
                continue;

            if (!TEST(actor->spr.cstat, CSTAT_SPRITE_BLOCK))
                continue;

            if (!FAFcansee(itActor->spr.pos.X, itActor->spr.pos.Y, itActor->spr.pos.Z, itActor->spr.sector(), actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z - ActorSizeZ(actor), actor->spr.sector()))
                continue;

            damage = GetDamage(itActor, pp->Actor(), DMG_FLASHBOMB);

            if (hu->sop_parent)
            {
                break;
            }
            else if (hu->PlayerP)
            {
//              if(hu->PlayerP->NightVision)
//              {
//                  SetFadeAmt(hu->PlayerP, -200, 1); // Got him with night vision on!
//                  PlayerUpdateHealth(hu->PlayerP, -15); // Hurt eyes
//              }else
                if (damage < -70)
                {
                    int choosesnd = 0;

                    choosesnd = RandomRange(MAX_PAIN);

                    PlayerSound(PlayerLowHealthPainVocs[choosesnd],v3df_dontpan|v3df_doppler|v3df_follow,pp);
                }
                SetFadeAmt(hu->PlayerP, damage, 1);     // White flash
            }
            else
            {
                ActorPain(itActor);
                SpawnFlashBombOnActor(itActor);
            }
        }
    }

    return 0;
}

int InitFlashBomb(DSWActor* actor)
{
    int i;
    unsigned int stat;
    int dist, tx, ty, tmin;
    short damage;
    USERp hu;
    PLAYERp pp = Player + screenpeek;

    PlaySound(DIGI_GASPOP, actor, v3df_dontpan | v3df_doppler);

    for (stat = 0; stat < SIZ(StatDamageList); stat++)
    {
        SWStatIterator it(StatDamageList[stat]);
        while (auto itActor = it.Next())
        {
            hu = itActor->u();

            DISTANCE(itActor->spr.pos.X, itActor->spr.pos.Y, actor->spr.pos.X, actor->spr.pos.Y, dist, tx, ty, tmin);
            if (dist > 16384)           // Flash radius
                continue;

            if (!TEST(actor->spr.cstat, CSTAT_SPRITE_BLOCK))
                continue;

            if (!FAFcansee(itActor->spr.pos.X, itActor->spr.pos.Y, itActor->spr.pos.Z, itActor->spr.sector(), actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z - ActorSizeZ(actor), actor->spr.sector()))
                continue;

            damage = GetDamage(itActor, actor, DMG_FLASHBOMB);

            if (hu->sop_parent)
            {
                break;
            }
            else if (hu->PlayerP)
            {
                if (damage < -70)
                {
                    int choosesnd = 0;

                    choosesnd = RandomRange(MAX_PAIN);

                    PlayerSound(PlayerLowHealthPainVocs[choosesnd],v3df_dontpan|v3df_doppler|v3df_follow,pp);
                }
                SetFadeAmt(hu->PlayerP, damage, 1);     // White flash
            }
            else
            {
                if (itActor != actor)
                {
                    ActorPain(itActor);
                    SpawnFlashBombOnActor(itActor);
                }
            }
        }
    }

    return 0;
}


// This is a sneaky function to make actors look blinded by flashbomb while using flaming code
void SpawnFlashBombOnActor(DSWActor* actor)
{
    if (!actor->hasU()) return;
    USERp u = actor->u();

    // Forget about burnable sprites
    if (TEST(actor->spr.extra, SPRX_BURNABLE))
        return;

    if (actor != nullptr)
    {
        DSWActor* flameActor = u->flameActor;
        if (flameActor != nullptr)
        {
            int sizez = (ActorSizeZ(actor) * 5) >> 2;

            if (flameActor->user.Counter >= GetRepeatFromHeight(flameActor, sizez))
            {
                // keep flame only slightly bigger than the enemy itself
                flameActor->user.Counter = GetRepeatFromHeight(flameActor, sizez) * 2;
            }
            else
            {
                // increase max size
                flameActor->user.Counter += GetRepeatFromHeight(flameActor, 8 << 8) * 2;
            }

            // Counter is max size
            if (flameActor->user.Counter >= 230)
            {
                // this is far too big
                flameActor->user.Counter = 230;
            }

            if (flameActor->user.WaitTics < 2 * 120)
                flameActor->user.WaitTics = 2 * 120; // allow it to grow again

            return;
        }
    }

    auto actorNew = SpawnActor(STAT_MISSILE, FIREBALL_FLAMES, s_FireballFlames, actor->spr.sector(),
                      actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.ang, 0);
    auto nu = actorNew->u();

    if (u->flameActor != nullptr)
        u->flameActor = actorNew;

    actorNew->spr.xrepeat = 16;
    actorNew->spr.yrepeat = 16;

    if (u->flameActor != nullptr)
    {
        actorNew->user.Counter = GetRepeatFromHeight(actorNew, ActorSizeZ(actor) >> 1) * 4;
    }
    else
        actorNew->user.Counter = 0;                // max flame size

    actorNew->spr.shade = -40;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER | CSTAT_SPRITE_INVISIBLE);
    RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);

    actorNew->user.Radius = 200;

    if (u->flameActor != nullptr)
    {
        SetAttach(actor, actorNew);
    }

    return;
}

//////////////////////////////////////////////
//
// Inventory Caltrops
//
//////////////////////////////////////////////

int PlayerInitCaltrops(PLAYERp pp)
{
    USERp u = pp->Actor()->u();
    USERp wu;
    int nx, ny, nz;
    short oclipdist;

    PlaySound(DIGI_THROW, pp, v3df_dontpan | v3df_doppler);

    if (!pp->insector())
        return 0;

    nx = pp->pos.X;
    ny = pp->pos.Y;
    nz = pp->pos.Z + pp->bob_z + Z(8);

    auto spawnedActor = SpawnActor(STAT_DEAD_ACTOR, CALTROPS, s_Caltrops, pp->cursector,
                    nx, ny, nz, pp->angle.ang.asbuild(), (CHEMBOMB_VELOCITY + RandomRange(CHEMBOMB_VELOCITY)) / 2);

    wu = spawnedActor->u();

    // don't throw it as far if crawling
    if (TEST(pp->Flags, PF_CRAWLING))
    {
        spawnedActor->spr.xvel -= (spawnedActor->spr.xvel >> 2);
    }

    SET(wu->Flags, SPR_XFLIP_TOGGLE);

    SetOwner(pp->Actor(), spawnedActor);
    spawnedActor->spr.yrepeat = 64;
    spawnedActor->spr.xrepeat = 64;
    spawnedActor->spr.shade = -15;
    wu->WeaponNum = u->WeaponNum;
    wu->Radius = 200;
    wu->ceiling_dist = Z(3);
    wu->floor_dist = Z(3);
    wu->Counter = 0;
//      SET(spawnedActor->spr.cstat, CSTAT_SPRITE_BLOCK);

    if (TEST(pp->Flags, PF_DIVING) || SpriteInUnderwaterArea(spawnedActor))
        SET(wu->Flags, SPR_UNDERWATER);

    // They go out at different angles
//        spawnedActor->spr.ang = NORM_ANGLE(pp->angle.ang.asbuild() + (RandomRange(50) - 25));

    spawnedActor->spr.zvel = -pp->horizon.horiz.asq16() >> 9;

    DSWActor* plActor = pp->actor;
    oclipdist = plActor->spr.clipdist;
    plActor->spr.clipdist = 0;
    spawnedActor->spr.clipdist = 0;

    MissileSetPos(spawnedActor, DoCaltrops, 1000);

    plActor->spr.clipdist = uint8_t(oclipdist);
    spawnedActor->spr.clipdist = 80L >> 2;

    wu->xchange = MOVEx(spawnedActor->spr.xvel, spawnedActor->spr.ang);
    wu->ychange = MOVEy(spawnedActor->spr.xvel, spawnedActor->spr.ang);
    wu->zchange = spawnedActor->spr.zvel >> 1;

    // adjust xvel according to player velocity
    wu->xchange += pp->xvect >> 14;
    wu->ychange += pp->yvect >> 14;

    SetupSpriteForBreak(spawnedActor);            // Put Caltrops in the break queue
    return 0;
}

int InitCaltrops(DSWActor* actor)
{
    USERp u = actor->u();
    USERp wu;
    int nx, ny, nz;

    PlaySound(DIGI_THROW, actor, v3df_dontpan | v3df_doppler);

    nx = actor->spr.pos.X;
    ny = actor->spr.pos.Y;
    nz = actor->spr.pos.Z;

    // Spawn a shot
    // Inserting and setting up variables
    auto spawnedActor = SpawnActor(STAT_DEAD_ACTOR, CALTROPS, s_Caltrops, actor->spr.sector(),
                    nx, ny, nz, actor->spr.ang, CHEMBOMB_VELOCITY / 2);

    wu = spawnedActor->u();

    SET(wu->Flags, SPR_XFLIP_TOGGLE);

    SetOwner(actor, spawnedActor);
    spawnedActor->spr.yrepeat = 64;
    spawnedActor->spr.xrepeat = 64;
    spawnedActor->spr.shade = -15;
    // !FRANK - clipbox must be <= weapon otherwise can clip thru walls
    spawnedActor->spr.clipdist = actor->spr.clipdist;
    wu->WeaponNum = u->WeaponNum;
    wu->Radius = 200;
    wu->ceiling_dist = Z(3);
    wu->floor_dist = Z(3);
    wu->Counter = 0;

    spawnedActor->spr.zvel = short(-RandomRange(100) * HORIZ_MULT);

    // spawnedActor->spr.clipdist = 80L>>2;

    wu->xchange = MOVEx(spawnedActor->spr.xvel, spawnedActor->spr.ang);
    wu->ychange = MOVEy(spawnedActor->spr.xvel, spawnedActor->spr.ang);
    wu->zchange = spawnedActor->spr.zvel >> 1;

    SetupSpriteForBreak(spawnedActor);            // Put Caltrops in the break queue
    return 0;
}

int InitPhosphorus(DSWActor* actor)
{
    USERp u = actor->u();
    USERp wu;
    int nx, ny, nz;
    short daang;


    PlaySound(DIGI_FIREBALL1, actor, v3df_follow);

    nx = actor->spr.pos.X;
    ny = actor->spr.pos.Y;
    nz = actor->spr.pos.Z;

    daang = NORM_ANGLE(RandomRange(2048));

    // Spawn a shot
    // Inserting and setting up variables
    auto actorNew = SpawnActor(STAT_SKIP4, FIREBALL1, s_Phosphorus, actor->spr.sector(),
                    nx, ny, nz, daang, CHEMBOMB_VELOCITY/3);

    wu = actorNew->u();

    actorNew->spr.hitag = LUMINOUS;               // Always full brightness
    SET(wu->Flags, SPR_XFLIP_TOGGLE);
    // !Frank - don't do translucent
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    // SET(actorNew->spr.cstat, CSTAT_SPRITE_TRANSLUCENT|CSTAT_SPRITE_YCENTER);
    actorNew->spr.shade = -128;

    actorNew->spr.yrepeat = 64;
    actorNew->spr.xrepeat = 64;
    actorNew->spr.shade = -15;
    // !FRANK - clipbox must be <= weapon otherwise can clip thru walls
    if (actor->spr.clipdist > 0)
        actorNew->spr.clipdist = actor->spr.clipdist-1;
    else
        actorNew->spr.clipdist = actor->spr.clipdist;
    wu->WeaponNum = u->WeaponNum;
    wu->Radius = 600;
    wu->ceiling_dist = Z(3);
    wu->floor_dist = Z(3);
    wu->Counter = 0;

    actorNew->spr.zvel = short(-RandomRange(100) * HORIZ_MULT);

    wu->xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
    wu->ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
    wu->zchange = (actorNew->spr.zvel >> 1);

    return 0;
}

int InitBloodSpray(DSWActor* actor, bool dogib, short velocity)
{
    USERp u = actor->u();
    USERp wu;
    int nx, ny, nz;
    short i, cnt, ang, vel, rnd;


    if (dogib)
        cnt = RandomRange(3)+1;
    else
        cnt = 1;

    //if(dogib)
    //    {
    rnd = RandomRange(1000);
    if (rnd > 650)
        PlaySound(DIGI_GIBS1, actor, v3df_none);
    else if (rnd > 350)
        PlaySound(DIGI_GIBS2, actor, v3df_none);
    else
        PlaySound(DIGI_GIBS3, actor, v3df_none);
    //    }

    ang = actor->spr.ang;
    vel = velocity;

    for (i=0; i<cnt; i++)
    {

        if (velocity == -1)
            vel = 105+RandomRange(320);
        else if (velocity == -2)
            vel = 105+RandomRange(100);

        if (dogib)
            ang = NORM_ANGLE(ang + 512 + RandomRange(200));
        else
            ang = NORM_ANGLE(ang+1024+256 - RandomRange(256));

        nx = actor->spr.pos.X;
        ny = actor->spr.pos.Y;
        nz = ActorZOfTop(actor)-20;

        // Spawn a shot
        auto actorNew = SpawnActor(STAT_MISSILE, GOREDrip, s_BloodSprayChunk, actor->spr.sector(),
                        nx, ny, nz, ang, vel*2);

        wu = actorNew->u();

        SET(wu->Flags, SPR_XFLIP_TOGGLE);
        if (dogib)
            SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
        else
            SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER | CSTAT_SPRITE_INVISIBLE);
        actorNew->spr.shade = -12;

        SetOwner(actor, actorNew);
        actorNew->spr.yrepeat = 64-RandomRange(35);
        actorNew->spr.xrepeat = 64-RandomRange(35);
        actorNew->spr.shade = -15;
        actorNew->spr.clipdist = actor->spr.clipdist;
        wu->WeaponNum = u->WeaponNum;
        wu->Radius = 600;
        wu->ceiling_dist = Z(3);
        wu->floor_dist = Z(3);
        wu->Counter = 0;

        actorNew->spr.zvel = short((-10 - RandomRange(50)) * HORIZ_MULT);

        wu->xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
        wu->ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
        wu->zchange = actorNew->spr.zvel >> 1;

        if (!GlobalSkipZrange)
            DoActorZrange(actorNew);
    }

    return 0;
}

int BloodSprayFall(DSWActor* actor)
{
    actor->spr.pos.Z += 1500;
    return 0;
}

////////////////// DEATHFLAG! ////////////////////////////////////////////////////////////////
// Rules: Run to an enemy flag, run over it an it will stick to you.
// The goal is to run the enemy's flag back to your startpoint.
// If an enemy flag touches a friendly start sector, then the opposing team explodes and
// your team wins and the level restarts.
// Once you pick up a flag, you have 30 seconds to score, otherwise, the flag detonates
// an explosion, killing you and anyone in the vicinity, and you don't score.
//////////////////////////////////////////////////////////////////////////////////////////////

// Update the scoreboard for team color that just scored.
void DoFlagScore(int16_t pal)
{
    SWStatIterator it(STAT_DEFAULT);
    while (auto actor = it.Next())
    {
        if (actor->spr.picnum < 1900 || actor->spr.picnum > 1999)
            continue;

        if (actor->spr.pal == pal)
            actor->spr.picnum++;               // Increment the counter

        if (actor->spr.picnum > 1999)
            actor->spr.picnum = 1900;          // Roll it over if you must

    }
}

DSWActor* DoFlagRangeTest(DSWActor* actor, int range)
{
    unsigned int stat;
    int dist, tx, ty;
    int tmin;

    for (stat = 0; stat < SIZ(StatDamageList); stat++)
    {
        SWStatIterator it(StatDamageList[stat]);
        while (auto itActor = it.Next())
        {
            DISTANCE(itActor->spr.pos.X, itActor->spr.pos.Y, actor->spr.pos.X, actor->spr.pos.Y, dist, tx, ty, tmin);
            if (dist > range)
                continue;

            if (actor == itActor)
                continue;

            if (!TEST(itActor->spr.cstat, CSTAT_SPRITE_BLOCK))
                continue;

            if (!TEST(itActor->spr.extra, SPRX_PLAYER_OR_ENEMY))
                continue;

            if (!FAFcansee(itActor->spr.pos.X, itActor->spr.pos.Y, itActor->spr.pos.Z, itActor->spr.sector(), actor->spr.pos.X, actor->spr.pos.Y, actor->spr.pos.Z, actor->spr.sector()))
                continue;

            dist = FindDistance3D(actor->spr.pos.X - itActor->spr.pos.X, actor->spr.pos.Y - itActor->spr.pos.Y, actor->spr.pos.Z - itActor->spr.pos.Z);
            if (dist > range)
                continue;

            return itActor;
        }
    }

    return nullptr;
}

int DoCarryFlag(DSWActor* actor)
{
    USER* u = actor->u();

    const int FLAG_DETONATE_STATE = 99;
    DSWActor* fown = u->flagOwnerActor;
    if (!fown) return 0;
    USERp fu = fown->u();

    DSWActor* attached = u->attachActor;

    // if no Owner then die
    if (attached != nullptr)
    {
        vec3_t pos = { attached->spr.pos.X, attached->spr.pos.Y, ActorZOfMiddle(attached) };
        SetActorZ(actor, &pos);
        actor->spr.ang = NORM_ANGLE(attached->spr.ang + 1536);
    }

    // not activated yet
    if (!TEST(u->Flags, SPR_ACTIVE))
    {
        if ((u->WaitTics -= (MISSILEMOVETICS * 2)) > 0)
            return false;

        // activate it
        u->WaitTics = SEC(30);          // You have 30 seconds to get it to
        // scorebox
        u->Counter2 = 0;
        SET(u->Flags, SPR_ACTIVE);
    }

    // limit the number of times DoFlagRangeTest is called
    u->Counter++;
    if (u->Counter > 1)
        u->Counter = 0;

    if (!u->Counter)
    {
        // not already in detonate state
        if (u->Counter2 < FLAG_DETONATE_STATE)
        {
            USERp au = attached->u();

            if (!attached->hasU() || au->Health <= 0)
            {
                u->Counter2 = FLAG_DETONATE_STATE;
                u->WaitTics = SEC(1) / 2;
            }
            // if in score box, score.
            if (attached->spr.sector()->hitag == 9000 && attached->spr.sector()->lotag == attached->spr.pal
                && attached->spr.pal != actor->spr.pal)
            {
                if (fown != nullptr)
                {
                    if (fown->spr.lotag)      // Trigger everything if there is a lotag
                        DoMatchEverything(nullptr, fown->spr.lotag, ON);
                }
                if (!TEST_BOOL1(fown))
                {
                    PlaySound(DIGI_BIGITEM, u->attachActor, v3df_none);
                    DoFlagScore(attached->spr.pal);
                    if (SP_TAG5(fown) > 0)
                    {
                        fu->filler++;
                        if (fu->filler >= SP_TAG5(fown))
                        {
                            fu->filler = 0;
                            DoMatchEverything(nullptr, SP_TAG6(fown), ON);
                        }
                    }
                }
                SetSuicide(actor);     // Kill the flag, you scored!
            }
        }
        else
        {
            // Time's up! Move directly to detonate state
            u->Counter2 = FLAG_DETONATE_STATE;
            u->WaitTics = SEC(1) / 2;
        }

    }

    u->WaitTics -= (MISSILEMOVETICS * 2);

    switch (u->Counter2)
    {
    case 0:
        if (u->WaitTics < SEC(30))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2++;
        }
        break;
    case 1:
        if (u->WaitTics < SEC(20))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2++;
        }
        break;
    case 2:
        if (u->WaitTics < SEC(10))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2++;
        }
        break;
    case 3:
        if (u->WaitTics < SEC(5))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2++;
        }
        break;
    case 4:
        if (u->WaitTics < SEC(4))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2++;
        }
        break;
    case 5:
        if (u->WaitTics < SEC(3))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2++;
        }
        break;
    case 6:
        if (u->WaitTics < SEC(2))
        {
            PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
            u->Counter2 = FLAG_DETONATE_STATE;
        }
        break;
    case FLAG_DETONATE_STATE:
        // start frantic beeping
        PlaySound(DIGI_MINEBEEP, actor, v3df_dontpan);
        u->Counter2++;
        break;
    case FLAG_DETONATE_STATE + 1:
        SpawnGrenadeExp(actor);
        SetSuicide(actor);
        return false;
        break;
    }

    return false;
}

int DoCarryFlagNoDet(DSWActor* actor)
{
    USER* u = actor->u();

    DSWActor* attached = u->attachActor;
    USERp au = attached->u();
    DSWActor* fown = u->flagOwnerActor;
    if (!fown) return 0;
    USERp fu = fown->u();


    if (u->flagOwnerActor != nullptr)
        fu->WaitTics = 30 * 120;        // Keep setting respawn tics so it won't respawn

    // if no Owner then die
    if (attached != nullptr)
    {
        vec3_t pos = { attached->spr.pos.X, attached->spr.pos.Y, ActorZOfMiddle(attached) };
        SetActorZ(actor, &pos);
        actor->spr.ang = NORM_ANGLE(attached->spr.ang + 1536);
        actor->spr.pos.Z = attached->spr.pos.Z - (ActorSizeZ(attached) >> 1);
    }

    if (!au || au->Health <= 0)
    {
        if (u->flagOwnerActor != nullptr)
            fu->WaitTics = 0;           // Tell it to respawn
        SetSuicide(actor);
        return false;
    }

    // if in score box, score.
    if (attached->spr.sector()->hitag == 9000 && attached->spr.sector()->lotag == attached->spr.pal
        && attached->spr.pal != actor->spr.pal)
    {
        if (u->flagOwnerActor != nullptr)
        {
            if (fown->spr.lotag)              // Trigger everything if there is a lotag
                DoMatchEverything(nullptr, fown->spr.lotag, ON);
            fu->WaitTics = 0;           // Tell it to respawn
        }
        if (!TEST_BOOL1(fown))
        {
            PlaySound(DIGI_BIGITEM, u->attachActor, v3df_none);
            DoFlagScore(attached->spr.pal);
            if (SP_TAG5(fown) > 0)
            {
                fu->filler++;
                if (fu->filler >= SP_TAG5(fown))
                {
                    fu->filler = 0;
                    DoMatchEverything(nullptr, SP_TAG6(fown), ON);
                }
            }
        }
        SetSuicide(actor);             // Kill the flag, you scored!
    }

    return false;
}


int SetCarryFlag(DSWActor* actor)
{
    USERp u = actor->u();

    // stuck
    SET(u->Flags, SPR_BOUNCE);
    // not yet active for 1 sec
//    RESET(u->Flags, SPR_ACTIVE);
//    u->WaitTics = SEC(3);
    SET(actor->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);
    u->Counter = 0;
    change_actor_stat(actor, STAT_ITEM);
    if (actor->spr.hitag == 1)
        ChangeState(actor, s_CarryFlagNoDet);
    else
        ChangeState(actor, s_CarryFlag);

    return false;
}

int DoFlag(DSWActor* actor)
{
    USER* u = actor->u();

    auto hitActor = DoFlagRangeTest(actor, 1000);

    if (hitActor)
    {
        SetCarryFlag(actor);

        // check to see if sprite is player or enemy
        if (TEST(hitActor->spr.extra, SPRX_PLAYER_OR_ENEMY))
        {
            // attach weapon to sprite
            RESET(actor->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);
            SetAttach(hitActor, actor);
            u->sz = hitActor->spr.pos.Z - (ActorSizeZ(hitActor) >> 1);
        }
    }

    return false;
}


int SpawnShell(DSWActor* actor, int ShellNum)
{
    USERp u = actor->u();
    USERp wu;
    int nx, ny, nz;
    short id=0,velocity=0;    
    STATEp p=nullptr;
    extern STATE s_UziShellShrap[];
    extern STATE s_ShotgunShellShrap[];


    nx = actor->spr.pos.X;
    ny = actor->spr.pos.Y;
    nz = ActorZOfMiddle(actor);

    switch (ShellNum)
    {
    case -2:
    case -3:
        id = UZI_SHELL;
        p = s_UziShellShrap;
        velocity = 1500 + RandomRange(1000);
        break;
    case -4:
        id = SHOT_SHELL;
        p = s_ShotgunShellShrap;
        velocity = 2000 + RandomRange(1000);
        break;
    }

    auto actorNew = SpawnActor(STAT_SKIP4, id, p, actor->spr.sector(), nx, ny, nz, actor->spr.ang, 64);

    wu = actorNew->u();

    actorNew->spr.zvel = -(velocity);

    if (u->PlayerP)
    {
        actorNew->spr.pos.Z += xs_CRoundToInt(-MulScaleF(u->PlayerP->horizon.horiz.asq16(), HORIZ_MULT / 3., 16));
    }

    switch (wu->ID)
    {
    case UZI_SHELL:
        actorNew->spr.pos.Z -= Z(13);

        if (ShellNum == -3)
        {
            actorNew->spr.ang = actor->spr.ang;
            HelpMissileLateral(actorNew,2500);
            actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang-512);
            HelpMissileLateral(actorNew,1000); // Was 1500
            actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang+712);
        }
        else
        {
            actorNew->spr.ang = actor->spr.ang;
            HelpMissileLateral(actorNew,2500);
            actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang+512);
            HelpMissileLateral(actorNew,1500);
            actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang-128);
        }
        actorNew->spr.ang += (RANDOM_P2(128<<5)>>5) - DIV2(128);
        actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang);

        // Set the shell number
        wu->ShellNum = ShellCount;
        actorNew->spr.yrepeat = actorNew->spr.xrepeat = 13;
        break;
    case SHOT_SHELL:
        actorNew->spr.pos.Z -= Z(13);
        actorNew->spr.ang = actor->spr.ang;
        HelpMissileLateral(actorNew,2500);
        actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang+512);
        HelpMissileLateral(actorNew,1300);
        actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang-128-64);
        actorNew->spr.ang += (RANDOM_P2(128<<5)>>5) - DIV2(128);
        actorNew->spr.ang = NORM_ANGLE(actorNew->spr.ang);

        // Set the shell number
        wu->ShellNum = ShellCount;
        actorNew->spr.yrepeat = actorNew->spr.xrepeat = 18;
        break;
    }

    SetOwner(actor, actorNew);
    actorNew->spr.shade = -15;
    wu->ceiling_dist = Z(1);
    wu->floor_dist = Z(1);
    wu->Counter = 0;
    SET(actorNew->spr.cstat, CSTAT_SPRITE_YCENTER);
    RESET(actorNew->spr.cstat, CSTAT_SPRITE_BLOCK | CSTAT_SPRITE_BLOCK_HITSCAN);
    RESET(wu->Flags, SPR_BOUNCE|SPR_UNDERWATER); // Make em' bounce

    wu->xchange = MOVEx(actorNew->spr.xvel, actorNew->spr.ang);
    wu->ychange = MOVEy(actorNew->spr.xvel, actorNew->spr.ang);
    wu->zchange = actorNew->spr.zvel;
    
    wu->jump_speed = 200;
    wu->jump_speed += RandomRange(400);
    wu->jump_speed = -wu->jump_speed;

    DoBeginJump(actor);
    wu->jump_grav = ACTOR_GRAVITY;

    return 0;
}


#include "saveable.h"

static saveable_data saveable_jweapon_data[] =
{
    SAVE_DATA(s_BloodSpray),
    SAVE_DATA(s_PhosphorExp),
    SAVE_DATA(s_NukeMushroom),
    SAVE_DATA(s_RadiationCloud),
    SAVE_DATA(s_ChemBomb),
    SAVE_DATA(s_Caltrops),
    SAVE_DATA(s_CaltropsStick),
    SAVE_DATA(s_CarryFlag),
    SAVE_DATA(s_CarryFlagNoDet),
    SAVE_DATA(s_Flag),
    SAVE_DATA(s_Phosphorus),
    SAVE_DATA(s_BloodSprayChunk),
    SAVE_DATA(s_BloodSprayDrip),
};

saveable_module saveable_jweapon =
{
    // code
    nullptr,0,

    // data
    saveable_jweapon_data,
    SIZ(saveable_jweapon_data)
};
END_SW_NS
