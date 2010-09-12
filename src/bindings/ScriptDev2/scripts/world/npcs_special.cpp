/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "GameEventMgr.h"
#include "World.h"
#include "Config/Config.h"
#include "Mail.h"

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_innkeeper            25%    Innkeepers in general. A lot do be done here (misc options for events)
npc_kingdom_of_dalaran_quests   Misc NPC's gossip option related to quests 12791, 12794 and 12796
npc_lunaclaw_spirit     100%    Appears at two different locations, quest 6001/6002
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_rogue_trainer        80%    Scripted trainers, so they are able to offer item 17126 for class quest 6681
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
npc_tabard_vendor        50%    allow recovering quest related tabards, achievement related ones need core support
npc_locksmith            75%    list of keys needs to be confirmed
EndContentData */

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon creature at smaller range
    SPAWNTYPE_ALARMBOT,                                     // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 m_uiThisCreatureEntry;
    uint32 m_uiSpawnedCreatureEntry;
    SpawnType m_SpawnType;
};

enum
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 5000
};

const float RANGE_TRIPWIRE          = 15.0f;
const float RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation m_aSpawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

struct MANGOS_DLL_DECL npc_air_force_botsAI : public ScriptedAI
{
    npc_air_force_botsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pSpawnAssoc = NULL;
        m_uiSpawnedGUID = 0;

        // find the correct spawnhandling
        static uint32 uiEntryCount = sizeof(m_aSpawnAssociations)/sizeof(SpawnAssociation);

        for (uint8 i=0; i<uiEntryCount; ++i)
        {
            if (m_aSpawnAssociations[i].m_uiThisCreatureEntry == pCreature->GetEntry())
            {
                m_pSpawnAssoc = &m_aSpawnAssociations[i];
                break;
            }
        }

        if (!m_pSpawnAssoc)
            error_db_log("SD2: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", pCreature->GetEntry());
        else
        {
            CreatureInfo const* spawnedTemplate = GetCreatureTemplateStore(m_pSpawnAssoc->m_uiSpawnedCreatureEntry);

            if (!spawnedTemplate)
            {
                error_db_log("SD2: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
                m_pSpawnAssoc = NULL;
                return;
            }
        }
    }

    SpawnAssociation* m_pSpawnAssoc;
    uint64 m_uiSpawnedGUID;

    void Reset() { }

    Creature* SummonGuard()
    {
        Creature* pSummoned = m_creature->SummonCreature(m_pSpawnAssoc->m_uiSpawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

        if (pSummoned)
            m_uiSpawnedGUID = pSummoned->GetGUID();
        else
        {
            error_db_log("SD2: npc_air_force_bots: wasn't able to spawn creature %u", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
            m_pSpawnAssoc = NULL;
        }

        return pSummoned;
    }

    Creature* GetSummonedGuard()
    {
        Creature* pCreature = (Creature*)m_creature->GetMap()->GetUnit(m_uiSpawnedGUID);

        if (pCreature && pCreature->isAlive())
            return pCreature;

        return NULL;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pSpawnAssoc)
            return;

        if (pWho->isTargetableForAttack() && m_creature->IsHostileTo(pWho))
        {
            Player* pPlayerTarget = pWho->GetTypeId() == TYPEID_PLAYER ? (Player*)pWho : NULL;

            // airforce guards only spawn for players
            if (!pPlayerTarget)
                return;

            Creature* pLastSpawnedGuard = m_uiSpawnedGUID == 0 ? NULL : GetSummonedGuard();

            // prevent calling Unit::GetUnit at next MoveInLineOfSight call - speedup
            if (!pLastSpawnedGuard)
                m_uiSpawnedGUID = 0;

            switch(m_pSpawnAssoc->m_SpawnType)
            {
                case SPAWNTYPE_ALARMBOT:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_GUARDS_MARK))
                        return;

                    Aura* pMarkAura = pWho->GetAura(SPELL_GUARDS_MARK, SpellEffectIndex(0));
                    if (pMarkAura)
                    {
                        // the target wasn't able to move out of our range within 25 seconds
                        if (!pLastSpawnedGuard)
                        {
                            pLastSpawnedGuard = SummonGuard();

                            if (!pLastSpawnedGuard)
                                return;
                        }

                        if (pMarkAura->GetAuraDuration() < AURA_DURATION_TIME_LEFT)
                        {
                            if (!pLastSpawnedGuard->getVictim())
                                pLastSpawnedGuard->AI()->AttackStart(pWho);
                        }
                    }
                    else
                    {
                        if (!pLastSpawnedGuard)
                            pLastSpawnedGuard = SummonGuard();

                        if (!pLastSpawnedGuard)
                            return;

                        pLastSpawnedGuard->CastSpell(pWho, SPELL_GUARDS_MARK, true);
                    }
                    break;
                }
                case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_TRIPWIRE))
                        return;

                    if (!pLastSpawnedGuard)
                        pLastSpawnedGuard = SummonGuard();

                    if (!pLastSpawnedGuard)
                        return;

                    // ROOFTOP only triggers if the player is on the ground
                    if (!pPlayerTarget->IsFlying())
                    {
                        if (!pLastSpawnedGuard->getVictim())
                            pLastSpawnedGuard->AI()->AttackStart(pWho);
                    }
                    break;
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_air_force_bots(Creature* pCreature)
{
    return new npc_air_force_botsAI(pCreature);
}

/*########
# npc_chicken_cluck
#########*/

enum
{
    EMOTE_A_HELLO           = -1000204,
    EMOTE_H_HELLO           = -1000205,
    EMOTE_CLUCK_TEXT2       = -1000206,

    QUEST_CLUCK             = 3861,
    FACTION_FRIENDLY        = 35,
    FACTION_CHICKEN         = 31
};

struct MANGOS_DLL_DECL npc_chicken_cluckAI : public ScriptedAI
{
    npc_chicken_cluckAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset()
    {
        m_uiResetFlagTimer = 120000;

        m_creature->setFaction(FACTION_CHICKEN);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (uiEmote == TEXTEMOTE_CHICKEN)
        {
            if (!urand(0, 29))
            {
                if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE)
                {
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    m_creature->setFaction(FACTION_FRIENDLY);

                    DoScriptText(EMOTE_A_HELLO, m_creature);

                    /* are there any difference in texts, after 3.x ?
                    if (pPlayer->GetTeam() == HORDE)
                        DoScriptText(EMOTE_H_HELLO, m_creature);
                    else
                        DoScriptText(EMOTE_A_HELLO, m_creature);
                    */
                }
            }
        }

        if (uiEmote == TEXTEMOTE_CHEER)
        {
            if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
            {
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->setFaction(FACTION_FRIENDLY);
                DoScriptText(EMOTE_CLUCK_TEXT2, m_creature);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        {
            if (m_uiResetFlagTimer < uiDiff)
                EnterEvadeMode();
            else
                m_uiResetFlagTimer -= uiDiff;
        }

        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_chicken_cluck(Creature* pCreature)
{
    return new npc_chicken_cluckAI(pCreature);
}

bool QuestAccept_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

bool QuestComplete_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

/*######
## npc_dancing_flames
######*/

struct MANGOS_DLL_DECL npc_dancing_flamesAI : public ScriptedAI
{
    npc_dancing_flamesAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_DANCE)
            m_creature->CastSpell(pPlayer,47057,false);
    }
};

CreatureAI* GetAI_npc_dancing_flames(Creature* pCreature)
{
    return new npc_dancing_flamesAI(pCreature);
}

/*######
## npc_lake_fog
######*/

struct MANGOS_DLL_DECL npc_lake_frogAI : public ScriptedAI
{
    npc_lake_frogAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 maidenTimer;

    void Reset()
    {
        m_creature->RemoveAurasDueToSpell(62550);
        maidenTimer = 40000;
    }

    bool IsMaiden() 
    {
        return (m_creature->getTransForm() == 62550);
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        bool isFrog = !IsMaiden();
        bool giveAshwoodBrand = false;

        if (emote == TEXTEMOTE_KISS) 
        {
            if (isFrog) 
            {
                if (!pPlayer->HasAura(62581)) 
                {
                    if (!pPlayer->HasAura(62574))
                        m_creature->CastSpell(pPlayer, 62581, false);
                    else 
                    {
                        pPlayer->RemoveAurasDueToSpell(62574);
                        giveAshwoodBrand = true;
                    }
                }
            }
            
            if (giveAshwoodBrand) 
            {
                if (urand(0, 4) == 1) 
                {
                    m_creature->CastSpell(m_creature, 62550, false);
                    m_creature->CastSpell(pPlayer, 62554, true, NULL);
                }
            }
        }     
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsMaiden())
        {
            if (maidenTimer <=  diff)
                Reset();
            else
                maidenTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_lake_frog(Creature* pCreature)
{
    return new npc_lake_frogAI(pCreature);
}


/*######
## Triage quest
######*/

enum
{
    SAY_DOC1                    = -1000201,
    SAY_DOC2                    = -1000202,
    SAY_DOC3                    = -1000203,

    QUEST_TRIAGE_H              = 6622,
    QUEST_TRIAGE_A              = 6624,

    DOCTOR_ALLIANCE             = 12939,
    DOCTOR_HORDE                = 12920,
    ALLIANCE_COORDS             = 7,
    HORDE_COORDS                = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                      // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                      // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                      // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                      // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                      // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                      // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                      // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                      // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                      // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                      // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                      // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f},                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

const uint32 AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

const uint32 HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/

struct MANGOS_DLL_DECL npc_doctorAI : public ScriptedAI
{
    npc_doctorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid Playerguid;

    uint32 SummonPatient_Timer;
    uint32 SummonPatientCount;
    uint32 PatientDiedCount;
    uint32 PatientSavedCount;

    bool Event;

    std::list<ObjectGuid> Patients;
    std::vector<Location*> Coordinates;

    void Reset()
    {
        Playerguid = 0;

        SummonPatient_Timer = 10000;
        SummonPatientCount = 0;
        PatientDiedCount = 0;
        PatientSavedCount = 0;

        Patients.clear();
        Coordinates.clear();

        Event = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void BeginEvent(Player* pPlayer);
    void PatientDied(Location* Point);
    void PatientSaved(Creature* soldier, Player* pPlayer, Location* Point);
    void UpdateAI(const uint32 diff);
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

struct MANGOS_DLL_DECL npc_injured_patientAI : public ScriptedAI
{
    npc_injured_patientAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid Doctorguid;
    Location* Coord;

    void Reset()
    {
        Doctorguid = 0;
        Coord = NULL;

        //no select
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //no regen health
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        //to make them lay with face down
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        uint32 mobId = m_creature->GetEntry();

        switch (mobId)
        {                                                   //lower max health
            case 12923:
            case 12938:                                     //Injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.75));
                break;
            case 12924:
            case 12936:                                     //Badly injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.50));
                break;
            case 12925:
            case 12937:                                     //Critically injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.25));
                break;
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == 20804)
        {
            if ((((Player*)caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (((Player*)caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
            {
                if (!Doctorguid.IsEmpty())
                {
                    if (Creature* Doctor = ((Creature*)m_creature->GetMap()->GetUnit(Doctorguid)))
                        ((npc_doctorAI*)Doctor->AI())->PatientSaved(m_creature, ((Player*)caster), Coord);
                }
            }
            //make not selectable
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //regen health
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            //stand up
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_DOC1,m_creature); break;
                case 1: DoScriptText(SAY_DOC2,m_creature); break;
                case 2: DoScriptText(SAY_DOC3,m_creature); break;
            }

            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

            uint32 mobId = m_creature->GetEntry();

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    m_creature->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    m_creature->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //lower HP on every world tick makes it a useful counter, not officlone though
        if (m_creature->isAlive() && m_creature->GetHealth() > 6)
        {
            m_creature->SetHealth(uint32(m_creature->GetHealth()-5));
        }

        if (m_creature->isAlive() && m_creature->GetHealth() <= 6)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->setDeathState(JUST_DIED);
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            if (!Doctorguid.IsEmpty())
            {
                if (Creature* Doctor = ((Creature*)m_creature->GetMap()->GetUnit(Doctorguid)))
                    ((npc_doctorAI*)Doctor->AI())->PatientDied(Coord);
            }
        }
    }
};

CreatureAI* GetAI_npc_injured_patient(Creature* pCreature)
{
    return new npc_injured_patientAI(pCreature);
}

/*
npc_doctor (continue)
*/

void npc_doctorAI::BeginEvent(Player* pPlayer)
{
    Playerguid = pPlayer->GetObjectGuid();

    SummonPatient_Timer = 10000;
    SummonPatientCount = 0;
    PatientDiedCount = 0;
    PatientSavedCount = 0;

    switch(m_creature->GetEntry())
    {
        case DOCTOR_ALLIANCE:
            for(uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                Coordinates.push_back(&AllianceCoords[i]);
            break;
        case DOCTOR_HORDE:
            for(uint8 i = 0; i < HORDE_COORDS; ++i)
                Coordinates.push_back(&HordeCoords[i]);
            break;
    }

    Event = true;
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void npc_doctorAI::PatientDied(Location* Point)
{
    Player* pPlayer = ((Player*)m_creature->GetMap()->GetUnit(Playerguid));

    if (pPlayer && ((pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
    {
        ++PatientDiedCount;

        if (PatientDiedCount > 5 && Event)
        {
            if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_A);
            else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_H);

            Reset();
            return;
        }

        Coordinates.push_back(Point);
    }
    else
        // If no player or player abandon quest in progress
        Reset();
}

void npc_doctorAI::PatientSaved(Creature* soldier, Player* pPlayer, Location* Point)
{
    if (pPlayer && Playerguid == pPlayer->GetObjectGuid())
    {
        if ((pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE))
        {
            ++PatientSavedCount;

            if (PatientSavedCount == 15)
            {
                if (!Patients.empty())
                {
                    std::list<ObjectGuid>::iterator itr;
                    for(itr = Patients.begin(); itr != Patients.end(); ++itr)
                    {
                        if (Creature* Patient = ((Creature*)m_creature->GetMap()->GetUnit(*itr)))
                            Patient->setDeathState(JUST_DIED);
                    }
                }

                if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_A, m_creature);
                else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_H, m_creature);

                Reset();
                return;
            }

            Coordinates.push_back(Point);
        }
    }
}

void npc_doctorAI::UpdateAI(const uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatient_Timer < diff)
        {
            Creature* Patient = NULL;
            Location* Point = NULL;

            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin()+rand()%Coordinates.size();
            uint32 patientEntry = 0;

            switch(m_creature->GetEntry())
            {
                case DOCTOR_ALLIANCE: patientEntry = AllianceSoldierId[urand(0, 2)]; break;
                case DOCTOR_HORDE:    patientEntry = HordeSoldierId[urand(0, 2)]; break;
                default:
                    error_log("SD2: Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            Point = *itr;

            Patient = m_creature->SummonCreature(patientEntry, Point->x, Point->y, Point->z, Point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

            if (Patient)
            {
                //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                Patients.push_back(Patient->GetObjectGuid());
                ((npc_injured_patientAI*)Patient->AI())->Doctorguid = m_creature->GetObjectGuid();

                if (Point)
                    ((npc_injured_patientAI*)Patient->AI())->Coord = Point;

                Coordinates.erase(itr);
            }
            SummonPatient_Timer = 10000;
            ++SummonPatientCount;
        }else SummonPatient_Timer -= diff;
    }
}

bool QuestAccept_npc_doctor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if ((pQuest->GetQuestId() == QUEST_TRIAGE_A) || (pQuest->GetQuestId() == QUEST_TRIAGE_H))
    {
        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pCreature->AI()))
            pDocAI->BeginEvent(pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_doctor(Creature* pCreature)
{
    return new npc_doctorAI(pCreature);
}

/*######
## npc_garments_of_quests
######*/

//TODO: get text for each NPC

enum
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263,
};

struct MANGOS_DLL_DECL npc_garments_of_questsAI : public npc_escortAI
{
    npc_garments_of_questsAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    ObjectGuid caster;

    bool bIsHealed;
    bool bCanRun;

    uint32 RunAwayTimer;

    void Reset()
    {
        caster = 0;

        bIsHealed = false;
        bCanRun = false;

        RunAwayTimer = 5000;

        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        //expect database to have RegenHealth=0
        m_creature->SetHealth(int(m_creature->GetMaxHealth()*0.7));
    }

    void SpellHit(Unit* pCaster, const SpellEntry *Spell)
    {
        if (Spell->Id == SPELL_LESSER_HEAL_R2 || Spell->Id == SPELL_FORTITUDE_R1)
        {
            //not while in combat
            if (m_creature->isInCombat())
                return;

            //nothing to be done now
            if (bIsHealed && bCanRun)
                return;

            if (pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                switch(m_creature->GetEntry())
                {
                    case ENTRY_SHAYA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                }

                //give quest credit, not expect any special quest objectives
                if (bCanRun)
                    ((Player*)pCaster)->TalkedToCreature(m_creature->GetEntry(),m_creature->GetGUID());
            }
        }
    }

    void WaypointReached(uint32 uiPoint)
    {
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (bCanRun && !m_creature->isInCombat())
        {
            if (RunAwayTimer <= diff)
            {
                if (Unit *pUnit = m_creature->GetMap()->GetUnit(caster))
                {
                    switch(m_creature->GetEntry())
                    {
                        case ENTRY_SHAYA: DoScriptText(SAY_SHAYA_GOODBYE,m_creature,pUnit); break;
                        case ENTRY_ROBERTS: DoScriptText(SAY_ROBERTS_GOODBYE,m_creature,pUnit); break;
                        case ENTRY_DOLF: DoScriptText(SAY_DOLF_GOODBYE,m_creature,pUnit); break;
                        case ENTRY_KORJA: DoScriptText(SAY_KORJA_GOODBYE,m_creature,pUnit); break;
                        case ENTRY_DG_KEL: DoScriptText(SAY_DG_KEL_GOODBYE,m_creature,pUnit); break;
                    }

                    Start(false,true);
                }
                else
                    EnterEvadeMode();                       //something went wrong

                RunAwayTimer = 30000;
            }else RunAwayTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_garments_of_quests(Creature* pCreature)
{
    return new npc_garments_of_questsAI(pCreature);
}

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5

struct MANGOS_DLL_DECL npc_guardianAI : public ScriptedAI
{
    npc_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->isAttackReady())
        {
            m_creature->CastSpell(m_creature->getVictim(),SPELL_DEATHTOUCH, true);
            m_creature->resetAttackTimer();
        }
    }
};

CreatureAI* GetAI_npc_guardian(Creature* pCreature)
{
    return new npc_guardianAI(pCreature);
}

/*########
# npc_innkeeper
#########*/

// Script applied to all innkeepers by npcflag.
// Are there any known innkeepers that does not hape the options in the below?
// (remember gossipHello is not called unless npcflag|1 is present)

enum
{
    TEXT_ID_WHAT_TO_DO              = 1853,

    SPELL_TRICK_OR_TREAT            = 24751,                 // create item or random buff
    SPELL_TRICK_OR_TREATED          = 24755,                 // buff player get when tricked or treated
    SPELL_TREAT                     = 24715,
    SPELL_TRICK_NO_ATTACK           = 24753,
    SPELL_TRICK_GNOME               = 24713,
    SPELL_TRICK_GHOST_MALE          = 24735,
    SPELL_TRICK_GHOST_FEMALE        = 24736,
    SPELL_TRICK_NINJA_MALE          = 24710,
    SPELL_TRICK_NINJA_FEMALE        = 24711,
    SPELL_TRICK_PIRATE_MALE         = 24708,
    SPELL_TRICK_PIRATE_FEMALE       = 24709,
    SPELL_TRICK_SKELETON            = 24723,
    SPELL_TRICK_BAT                 = 24732
};

#define GOSSIP_ITEM_TRICK_OR_TREAT  "Trick or Treat!"
#define GOSSIP_ITEM_WHAT_TO_DO      "What can I do at an Inn?"

bool GossipHello_npc_innkeeper(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PrepareGossipMenu(pCreature);

    if (IsHolidayActive(HOLIDAY_HALLOWS_END) && !pPlayer->HasAura(SPELL_TRICK_OR_TREATED,SpellEffectIndex(0)))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRICK_OR_TREAT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    // Should only apply to innkeeper close to start areas.
    if (AreaTableEntry const* pAreaEntry = GetAreaEntryByAreaID(pCreature->GetAreaId()))
    {
        if (pAreaEntry->flags & AREA_FLAG_LOWLEVEL)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_TO_DO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    }

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());
    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_innkeeper(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_WHAT_TO_DO, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            // either trick or treat, 50% chance
            if (urand(0, 1))
            {
                pPlayer->CastSpell(pPlayer, SPELL_TREAT, true);
            }
            else
            {
                uint32 uiTrickSpell = 0;

                switch(urand(0, 9))                             // note that female characters can get male costumes and vice versa
                {
                    case 0: uiTrickSpell = SPELL_TRICK_NO_ATTACK; break;
                    case 1: uiTrickSpell = SPELL_TRICK_GNOME; break;
                    case 2: uiTrickSpell = SPELL_TRICK_GHOST_MALE; break;
                    case 3: uiTrickSpell = SPELL_TRICK_GHOST_FEMALE; break;
                    case 4: uiTrickSpell = SPELL_TRICK_NINJA_MALE; break;
                    case 5: uiTrickSpell = SPELL_TRICK_NINJA_FEMALE; break;
                    case 6: uiTrickSpell = SPELL_TRICK_PIRATE_MALE; break;
                    case 7: uiTrickSpell = SPELL_TRICK_PIRATE_FEMALE; break;
                    case 8: uiTrickSpell = SPELL_TRICK_SKELETON; break;
                    case 9: uiTrickSpell = SPELL_TRICK_BAT; break;
                }

                pPlayer->CastSpell(pPlayer, uiTrickSpell, true);
            }

            pPlayer->CastSpell(pPlayer, SPELL_TRICK_OR_TREATED, true);
            break;
        }

        case GOSSIP_OPTION_VENDOR:
            pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
            break;
        case GOSSIP_OPTION_INNKEEPER:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SetBindPoint(pCreature->GetGUID());
            break;
    }

    return true;
}

/*######
## npc_kingdom_of_dalaran_quests
######*/

enum
{
    SPELL_TELEPORT_DALARAN  = 53360,
    ITEM_KT_SIGNET          = 39740,
    QUEST_MAGICAL_KINGDOM_A = 12794,
    QUEST_MAGICAL_KINGDOM_H = 12791,
    QUEST_MAGICAL_KINGDOM_N = 12796
};

#define GOSSIP_ITEM_TELEPORT_TO "I am ready to be teleported to Dalaran."

bool GossipHello_npc_kingdom_of_dalaran_quests(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->HasItemCount(ITEM_KT_SIGNET,1) && (!pPlayer->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_A) ||
        !pPlayer->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_H) || !pPlayer->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_N)))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT_TO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_kingdom_of_dalaran_quests(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,SPELL_TELEPORT_DALARAN,false);
    }
    return true;
}

/*######
## npc_lunaclaw_spirit
######*/

enum
{
    QUEST_BODY_HEART_A      = 6001,
    QUEST_BODY_HEART_H      = 6002,

    TEXT_ID_DEFAULT         = 4714,
    TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

bool GossipHello_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetGUID());
        pPlayer->AreaExploredOrEventHappens((pPlayer->GetTeam() == ALLIANCE) ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
    }
    return true;
}

/*######
## npc_mount_vendor
######*/

bool GossipHello_npc_mount_vendor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    bool canBuy;
    canBuy = false;
    uint32 vendor = pCreature->GetEntry();
    uint8 race = pPlayer->getRace();

    switch (vendor)
    {
        case 384:                                           //Katie Hunter
        case 1460:                                          //Unger Statforth
        case 2357:                                          //Merideth Carlson
        case 4885:                                          //Gregor MacVince
            if (pPlayer->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                pPlayer->SEND_GOSSIP_MENU(5855, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 1261:                                          //Veron Amberstill
            if (pPlayer->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                pPlayer->SEND_GOSSIP_MENU(5856, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 3362:                                          //Ogunaro Wolfrunner
            if (pPlayer->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                pPlayer->SEND_GOSSIP_MENU(5841, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 3685:                                          //Harb Clawhoof
            if (pPlayer->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                pPlayer->SEND_GOSSIP_MENU(5843, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 4730:                                          //Lelanai
            if (pPlayer->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                pPlayer->SEND_GOSSIP_MENU(5844, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 4731:                                          //Zachariah Post
            if (pPlayer->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
                pPlayer->SEND_GOSSIP_MENU(5840, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 7952:                                          //Zjolnir
            if (pPlayer->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                pPlayer->SEND_GOSSIP_MENU(5842, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 7955:                                          //Milli Featherwhistle
            if (pPlayer->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                pPlayer->SEND_GOSSIP_MENU(5857, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 16264:                                         //Winaestra
            if (pPlayer->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                pPlayer->SEND_GOSSIP_MENU(10305, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 17584:                                         //Torallius the Pack Handler
            if (pPlayer->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                pPlayer->SEND_GOSSIP_MENU(10239, pCreature->GetGUID());
            else canBuy = true;
            break;
    }

    if (canBuy)
    {
        if (pCreature->isVendor())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_mount_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_rogue_trainer
######*/

bool GossipHello_npc_rogue_trainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isTrainer())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

    if (pCreature->isCanTrainingAndResetTalentsOf(pPlayer))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "I wish to unlearn my talents", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

    if (pPlayer->getClass() == CLASS_ROGUE && pPlayer->getLevel() >= 24 && !pPlayer->HasItemCount(17126,1) && !pPlayer->GetQuestRewardStatus(6681))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<Take the letter>", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(5996, pCreature->GetGUID());
    } else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_rogue_trainer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer,21100,false);
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetGUID());
            break;
        case GOSSIP_OPTION_UNLEARNTALENTS:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SendTalentWipeConfirm(pCreature->GetGUID());
            break;
        /*case GOSSIP_OPTION_LEARNDUALSPEC:
            if(pPlayer->GetSpecsCount() == 1 && !(pPlayer->getLevel() < 40))
            {
                if (pPlayer->GetMoney() < 10000000)
                {
                    pPlayer->SendBuyError( BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                }
                else
                {
                    pPlayer->ModifyMoney(-10000000);

                    // Cast spells that teach dual spec
                    // Both are also ImplicitTarget self and must be cast by player
                    pPlayer->CastSpell(pPlayer,63680,true,NULL,NULL,pPlayer->GetGUID());
                    pPlayer->CastSpell(pPlayer,63624,true,NULL,NULL,pPlayer->GetGUID());

                    // Should show another Gossip text with "Congratulations..."
                    pPlayer->CLOSE_GOSSIP_MENU();
                }
            }*/
            break;
    }
    return true;
}

/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

bool GossipHello_npc_sayge(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->HasSpellCooldown(SPELL_INT) ||
        pPlayer->HasSpellCooldown(SPELL_ARM) ||
        pPlayer->HasSpellCooldown(SPELL_DMG) ||
        pPlayer->HasSpellCooldown(SPELL_RES) ||
        pPlayer->HasSpellCooldown(SPELL_STR) ||
        pPlayer->HasSpellCooldown(SPELL_AGI) ||
        pPlayer->HasSpellCooldown(SPELL_STM) ||
        pPlayer->HasSpellCooldown(SPELL_SPI))
        pPlayer->SEND_GOSSIP_MENU(7393, pCreature->GetGUID());
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(7339, pCreature->GetGUID());
    }

    return true;
}

void SendAction_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Slay the Man",                      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn him over to liege",            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confiscate the corn",               GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let him go and have the corn",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(7340, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painfully",     GOSSIP_SENDER_MAIN+1, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painlessly",    GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let your friend go",                GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7341, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confront the diplomat",             GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show not so quiet defiance",        GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remain quiet",                      GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7361, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Speak against your brother openly", GOSSIP_SENDER_MAIN+6, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Help your brother in",              GOSSIP_SENDER_MAIN+7, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Keep your brother out without letting him know", GOSSIP_SENDER_MAIN+8, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7362, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, keep gold",            GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, share the gold",       GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let the knight take credit",        GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7363, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks",                            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(7364, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pCreature->CastSpell(pPlayer, SPELL_FORTUNE, false);
            pPlayer->SEND_GOSSIP_MENU(7365, pCreature->GetGUID());
            break;
    }
}

bool GossipSelect_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiSender)
    {
        case GOSSIP_SENDER_MAIN:
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+1:
            pCreature->CastSpell(pPlayer, SPELL_DMG, false);
            pPlayer->AddSpellCooldown(SPELL_DMG,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+2:
            pCreature->CastSpell(pPlayer, SPELL_RES, false);
            pPlayer->AddSpellCooldown(SPELL_RES,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+3:
            pCreature->CastSpell(pPlayer, SPELL_ARM, false);
            pPlayer->AddSpellCooldown(SPELL_ARM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+4:
            pCreature->CastSpell(pPlayer, SPELL_SPI, false);
            pPlayer->AddSpellCooldown(SPELL_SPI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+5:
            pCreature->CastSpell(pPlayer, SPELL_INT, false);
            pPlayer->AddSpellCooldown(SPELL_INT,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+6:
            pCreature->CastSpell(pPlayer, SPELL_STM, false);
            pPlayer->AddSpellCooldown(SPELL_STM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+7:
            pCreature->CastSpell(pPlayer, SPELL_STR, false);
            pPlayer->AddSpellCooldown(SPELL_STR,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+8:
            pCreature->CastSpell(pPlayer, SPELL_AGI, false);
            pPlayer->AddSpellCooldown(SPELL_AGI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}

bool GossipHello_Bat_Handler_Camille(Player* pPlayer, Creature* pCreature)
{

  if(pPlayer->IsActiveQuest(11170) && pPlayer->GetQuestStatus(11170) != QUEST_STATUS_COMPLETE)
      pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I`m ready to strike alliance reinforcements!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +1);

  if(pPlayer->IsActiveQuest(11229))
      pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I need to fly to the Windrunner Official business!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
  
  pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_Bat_Handler_Camille(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{

  uint32 spellId = 0;

  switch(action)
  {
  case GOSSIP_ACTION_INFO_DEF + 1: spellId = 43136; break;
  case GOSSIP_ACTION_INFO_DEF + 2 : spellId = 43074; break;
  }

  pPlayer->CLOSE_GOSSIP_MENU();

  if(spellId)
      pPlayer->CastSpell(pPlayer, spellId, true);

  return true;
}

bool GossipHello_Greer_OreHammer(Player* pPlayer, Creature* pCreature)
{

  if(pPlayer->GetQuestStatus(11332) == QUEST_STATUS_INCOMPLETE)
      pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I am here to destroy the tanks with the plague!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

  pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

  return true;
}


bool GossipSelect_Greer_OreHammer(Player* pPlayer, Creature* pCreature, uint32 sender,  uint32 action)
{

  if(action == GOSSIP_ACTION_INFO_DEF+1)
    {

      uint32  noSpaceForCount = 0;
      ItemPosCountVec dest;
      uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 33634, 10, &noSpaceForCount);

      if(msg != EQUIP_ERR_OK)
      {
          pPlayer->SendEquipError(msg, NULL, NULL);
          return false;
      }

      Item* bomb = pPlayer->StoreNewItem( dest, 33634, true, Item::GenerateItemRandomPropertyId(33634) );

      pPlayer->SendNewItem(bomb, 10, true, false);
      pPlayer->CLOSE_GOSSIP_MENU();
      pPlayer->ActivateTaxiPathTo(745, 0); //can not find spell for this taxipath.  
   }

   return true;
}

/*######
## npc_wild_wolpertinger
######*/

struct MANGOS_DLL_DECL npc_wild_wolpertingerAI : public ScriptedAI
{
    npc_wild_wolpertingerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_SPELLCLICK);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == 41621)
        {
            if (caster->GetTypeId() == TYPEID_PLAYER && ((Player*)caster)->GetDrunkenstateByValue(((Player*)caster)->GetDrunkValue()) >= DRUNKEN_DRUNK)
                m_creature->SetFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_npc_wild_wolpertinger(Creature* pCreature)
{
    return new npc_wild_wolpertingerAI(pCreature);
}

/*######
## npc_mirror_image
######*/

struct MANGOS_DLL_DECL npc_mirror_imageAI : public ScriptedAI
{
    npc_mirror_imageAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 m_uiFrostboltTimer;
	uint32 m_uiFireblastTimer;
	bool inCombat;

    void Reset() 
	{
		Unit *owner = m_creature->GetOwner();
		if (!owner)
			return;

		if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
		{
			m_creature->GetMotionMaster()->Clear(false);
			m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
		}
		// Inherit Master's Threat List (not yet implemented)
		//owner->CastSpell((Unit*)NULL, 58838, true);
		// here mirror image casts on summoner spell (not present in client dbc) 49866
		// here should be auras (not present in client dbc): 35657, 35658, 35659, 35660 selfcasted by mirror images (stats related?)
		// Clone Me!
		m_uiFrostboltTimer = 0;
		m_uiFireblastTimer = 6100;
		inCombat = false;
	}

	void AttackStart(Unit* pWho)
	{
		if (!pWho)
			return;

		if (m_creature->Attack(pWho, true))
		{
            m_creature->clearUnitState(UNIT_STAT_FOLLOW);
            // TMGs call CreatureRelocation which via MoveInLineOfSight can call this function
            // thus with the following clear the original TMG gets invalidated and crash, doh
            // hope it doesn't start to leak memory without this :-/
            //i_pet->Clear();
            m_creature->GetMotionMaster()->MoveChase(pWho);
            m_creature->getVictim()->AddThreat(m_creature);
            inCombat = true;
        }
	}

	void EnterEvadeMode()
	{
		if (m_creature->IsInEvadeMode() || !m_creature->isAlive())
			return;

		inCombat = false;
		Unit *owner = m_creature->GetCharmerOrOwner();

		m_creature->AttackStop();
		m_creature->CombatStop(true);
		if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
		{
			m_creature->GetMotionMaster()->Clear(false);
			m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!(m_creature->HasAura(45204)))
		{
			Unit *owner = m_creature->GetCharmerOrOwner();
			if (!owner)
				return;
			m_creature->CastSpell(m_creature, 45204, true, NULL, NULL, owner->GetGUID());
		}

		if (!(m_creature->HasAura(58836)))
		{
			Unit *owner = m_creature->GetCharmerOrOwner();
			if (!owner)
				return;
			m_creature->CastSpell(m_creature, 58836, true, NULL, NULL, owner->GetGUID());
		}
		
		if (/*!m_creature->SelectHostileTarget() || */!m_creature->getVictim())
		{
			Unit *owner = m_creature->GetCharmerOrOwner();
			if (owner && owner->getVictim())
				m_creature->AI()->AttackStart(owner->getVictim());
		}

		if (inCombat && !m_creature->getVictim())
		{
			EnterEvadeMode();
			return;
		}

		if (/*!m_creature->SelectHostileTarget() || */!m_creature->getVictim())
			return;

		if (m_uiFrostboltTimer <= diff)
		{
			DoCast(m_creature->getVictim(),59638);
			m_uiFrostboltTimer = 3100;
		}else m_uiFrostboltTimer -= diff;

		if (m_uiFireblastTimer <= diff)
		{
			DoCast(m_creature->getVictim(),59637,true);
			m_uiFireblastTimer = 6000;
		}else m_uiFireblastTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_mirror_image(Creature* pCreature)
{
    return new npc_mirror_imageAI(pCreature);
}

/*######
## npc_winter_reveler
######*/

struct MANGOS_DLL_DECL npc_winter_revelerAI : public ScriptedAI
{
    npc_winter_revelerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_KISS)
        {
            if (pPlayer->HasAura(26218))
                return;

            m_creature->CastSpell(pPlayer, 26218, true);
        }
           
    }
};

CreatureAI* GetAI_npc_winter_reveler(Creature* pCreature)
{
    return new npc_winter_revelerAI(pCreature);
}

/*######
## npc_tabard_vendor
######*/

enum
{
    QUEST_TRUE_MASTERS_OF_LIGHT                = 9737,
    QUEST_THE_UNWRITTEN_PROPHECY               = 9762,
    QUEST_INTO_THE_BREACH                      = 10259,
    QUEST_BATTLE_OF_THE_CRIMSON_WATCH          = 10781,
    QUEST_SHARDS_OF_AHUNE                      = 11972,

    ACHIEVEMENT_EXPLORE_NORTHREND              = 45,
    ACHIEVEMENT_TWENTYFIVE_TABARDS             = 1021,
    ACHIEVEMENT_THE_LOREMASTER_A               = 1681,
    ACHIEVEMENT_THE_LOREMASTER_H               = 1682,

    ITEM_TABARD_OF_THE_HAND                    = 24344,
    ITEM_TABARD_OF_THE_BLOOD_KNIGHT            = 25549,
    ITEM_TABARD_OF_THE_PROTECTOR               = 28788,
    ITEM_OFFERING_OF_THE_SHATAR                = 31408,
    ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI   = 31404,
    ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI  = 31405,
    ITEM_TABARD_OF_THE_SUMMER_SKIES            = 35279,
    ITEM_TABARD_OF_THE_SUMMER_FLAMES           = 35280,
    ITEM_TABARD_OF_THE_ACHIEVER                = 40643,
    ITEM_LOREMASTERS_COLORS                    = 43300,
    ITEM_TABARD_OF_THE_EXPLORER                = 43348,

    SPELL_TABARD_OF_THE_BLOOD_KNIGHT           = 54974,
    SPELL_TABARD_OF_THE_HAND                   = 54976,
    SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI  = 54977,
    SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI = 54982,
    SPELL_TABARD_OF_THE_ACHIEVER               = 55006,
    SPELL_TABARD_OF_THE_PROTECTOR              = 55008,
    SPELL_LOREMASTERS_COLORS                   = 58194,
    SPELL_TABARD_OF_THE_EXPLORER               = 58224,
    SPELL_TABARD_OF_SUMMER_SKIES               = 62768,
    SPELL_TABARD_OF_SUMMER_FLAMES              = 62769
};

#define GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT               "I've lost my Tabard of Blood Knight."
#define GOSSIP_LOST_TABARD_OF_THE_HAND                   "I've lost my Tabard of the Hand."
#define GOSSIP_LOST_TABARD_OF_THE_PROTECTOR              "I've lost my Tabard of the Protector."
#define GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI  "I've lost my Green Trophy Tabard of the Illidari."
#define GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI "I've lost my Purple Trophy Tabard of the Illidari."
#define GOSSIP_LOST_TABARD_OF_SUMMER_SKIES               "I've lost my Tabard of Summer Skies."
#define GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES              "I've lost my Tabard of Summer Flames."
#define GOSSIP_LOST_LOREMASTERS_COLORS                   "I've lost my Loremaster's Colors."
#define GOSSIP_LOST_TABARD_OF_THE_EXPLORER               "I've lost my Tabard of the Explorer."
#define GOSSIP_LOST_TABARD_OF_THE_ACHIEVER               "I've lost my Tabard of the Achiever."

bool GossipHello_npc_tabard_vendor(Player* pPlayer, Creature* pCreature)
{
    bool m_bLostBloodKnight = false;
    bool m_bLostHand        = false;
    bool m_bLostProtector   = false;
    bool m_bLostIllidari    = false;
    bool m_bLostSummer      = false;

    //Tabard of the Blood Knight
    if (pPlayer->GetQuestRewardStatus(QUEST_TRUE_MASTERS_OF_LIGHT) && !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_BLOOD_KNIGHT, 1, true))
        m_bLostBloodKnight = true;

    //Tabard of the Hand
    if (pPlayer->GetQuestRewardStatus(QUEST_THE_UNWRITTEN_PROPHECY) && !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_HAND, 1, true))
        m_bLostHand = true;

    //Tabard of the Protector
    if (pPlayer->GetQuestRewardStatus(QUEST_INTO_THE_BREACH) && !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_PROTECTOR, 1, true))
        m_bLostProtector = true;

    //Green Trophy Tabard of the Illidari
    //Purple Trophy Tabard of the Illidari
    if (pPlayer->GetQuestRewardStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) &&
        (!pPlayer->HasItemCount(ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
        !pPlayer->HasItemCount(ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
        !pPlayer->HasItemCount(ITEM_OFFERING_OF_THE_SHATAR, 1, true)))
        m_bLostIllidari = true;

    //Tabard of Summer Skies
    //Tabard of Summer Flames
    if (pPlayer->GetQuestRewardStatus(QUEST_SHARDS_OF_AHUNE) &&
        !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_SKIES, 1, true) &&
        !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_FLAMES, 1, true))
        m_bLostSummer = true;

    if (m_bLostBloodKnight || m_bLostHand || m_bLostProtector || m_bLostIllidari || m_bLostSummer)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (m_bLostBloodKnight)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +1);

        if (m_bLostHand)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_HAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +2);

        if (m_bLostProtector)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_PROTECTOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

        if (m_bLostIllidari)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
        }

        if (m_bLostSummer)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_SKIES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
        }

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    else
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_tabard_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_THE_BLOOD_KNIGHT, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_THE_HAND, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_THE_PROTECTOR, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_SUMMER_SKIES, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_SUMMER_FLAMES, false);
            break;
    }
    return true;
}

/*######
## npc_locksmith
######*/

enum
{
    QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ = 10704,
    QUEST_DARK_IRON_LEGACY                = 3802,
    QUEST_THE_KEY_TO_SCHOLOMANCE_A        = 5505,
    QUEST_THE_KEY_TO_SCHOLOMANCE_H        = 5511,
    QUEST_HOTTER_THAN_HELL_A              = 10758,
    QUEST_HOTTER_THAN_HELL_H              = 10764,
    QUEST_RETURN_TO_KHAGDAR               = 9837,
    QUEST_SCEPTER_OF_CELEBRAS             = 7046,
    QUEST_CONTAINMENT                     = 13159,

    ITEM_ARCATRAZ_KEY                     = 31084,
    ITEM_SHADOWFORGE_KEY                  = 11000,
    ITEM_SKELETON_KEY                     = 13704,
    ITEM_SHATTERED_HALLS_KEY              = 28395,
    ITEM_THE_MASTERS_KEY                  = 24490,
    ITEM_SCEPTER_OF_CELEBRAS              = 17191,
    ITEM_VIOLET_HOLD_KEY                  = 42482,

    SPELL_ARCATRAZ_KEY                    = 54881,
    SPELL_SHADOWFORGE_KEY                 = 54882,
    SPELL_SKELETON_KEY                    = 54883,
    SPELL_SHATTERED_HALLS_KEY             = 54884,
    SPELL_THE_MASTERS_KEY                 = 54885,
    SPELL_SCEPTER_OF_CELEBRAS             = 56211,
    SPELL_VIOLET_HOLD_KEY                 = 67253
};

#define GOSSIP_LOST_ARCATRAZ_KEY         "I've lost my key to the Arcatraz."
#define GOSSIP_LOST_SHADOWFORGE_KEY      "I've lost my key to the Blackrock Depths."
#define GOSSIP_LOST_SKELETON_KEY         "I've lost my key to the Scholomance."
#define GOSSIP_LOST_SHATTERED_HALLS_KEY  "I've lost my key to the Shattered Halls."
#define GOSSIP_LOST_THE_MASTERS_KEY      "I've lost my key to the Karazhan."
#define GOSSIP_LOST_SCEPTER              "I've lost my Scepter of Celebras"
#define GOSSIP_LOST_VIOLET_HOLD_KEY      "I've lost my key to the Violet Hold."


bool GossipHello_npc_locksmith(Player* pPlayer, Creature* pCreature)
{

    // Arcatraz Key
    if (pPlayer->GetQuestRewardStatus(QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ) && !pPlayer->HasItemCount(ITEM_ARCATRAZ_KEY, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ARCATRAZ_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +1);

    // Shadowforge Key
    if (pPlayer->GetQuestRewardStatus(QUEST_DARK_IRON_LEGACY) && !pPlayer->HasItemCount(ITEM_SHADOWFORGE_KEY, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHADOWFORGE_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +2);

    // Skeleton Key
    if ((pPlayer->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_A) || pPlayer->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_H)) &&
        !pPlayer->HasItemCount(ITEM_SKELETON_KEY, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SKELETON_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +3);

    // Shatered Halls Key
    if ((pPlayer->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_A) || pPlayer->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_H)) &&
        !pPlayer->HasItemCount(ITEM_SHATTERED_HALLS_KEY, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHATTERED_HALLS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +4);

    // Master's Key
    if (pPlayer->GetQuestRewardStatus(QUEST_RETURN_TO_KHAGDAR) && !pPlayer->HasItemCount(ITEM_THE_MASTERS_KEY, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_THE_MASTERS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +5);

    // Scepter of Celebras
    if (pPlayer->GetQuestRewardStatus(QUEST_SCEPTER_OF_CELEBRAS) && !pPlayer->HasItemCount(ITEM_SCEPTER_OF_CELEBRAS, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SCEPTER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +6);

    // Violet Hold Key
    if (pPlayer->GetQuestRewardStatus(QUEST_CONTAINMENT) && !pPlayer->HasItemCount(ITEM_VIOLET_HOLD_KEY, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_VIOLET_HOLD_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +7);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_locksmith(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_ARCATRAZ_KEY, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_SHADOWFORGE_KEY, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_SKELETON_KEY, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_SHATTERED_HALLS_KEY, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_THE_MASTERS_KEY, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_SCEPTER_OF_CELEBRAS, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_VIOLET_HOLD_KEY, false);
            break;
    }
    return true;
}

/*######
## npc_love_air
######*/

struct MANGOS_DLL_DECL npc_love_airAI : public ScriptedAI
{
    npc_love_airAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_LOVE)
        {
            // todo odekolon and gender
            if (pPlayer->HasAura(26680) || pPlayer->HasAura(26898) || !pPlayer->HasAura(26682))
                return;
            
            uint32 roll = 0;
            roll = urand(0, 1);
            switch(roll)
            {
            case 0: pPlayer->CastSpell(pPlayer,27509,true); break;
            //case 1: pPlayer->CastSpell(pPlayer,27242,true); break;
            case 1: pPlayer->CastSpell(pPlayer,26898,true); break;
            }

            if (roll != 1)
                pPlayer->CastSpell(pPlayer,26680,true);
        }           
    }
};

CreatureAI* GetAI_npc_love_air(Creature* pCreature)
{
    return new npc_love_airAI(pCreature);
}

/*######
## npc_fjord_turkey
######*/

struct MANGOS_DLL_DECL npc_fjord_turkeyAI : public ScriptedAI
{
    npc_fjord_turkeyAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    // only cast spell to achievement
    void JustDied(Unit *killer)
    {
        if (!killer)
             return;

        if (killer->GetTypeId() != TYPEID_PLAYER)
            return;

        killer->CastSpell(killer,25285,true);
    }
};

CreatureAI* GetAI_npc_fjord_turkey(Creature* pCreature)
{
    return new npc_fjord_turkeyAI(pCreature);
}

/*######
## npc_spring_rabbit
######*/

struct MANGOS_DLL_DECL npc_spring_rabbitAI : public ScriptedAI
{
    npc_spring_rabbitAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Check_timer;
    uint32 Unlove_timer;
    bool isLove;


    void Reset()
    {
        Unit *owner = m_creature->GetOwner();
        if (!owner)
            return;

        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        Check_timer = 5000;
        Unlove_timer = 4000;
        isLove = false;
    }

    void EnterEvadeMode()
    {
        if (m_creature->IsInEvadeMode() || !m_creature->isAlive())
            return;

        Unit *owner = m_creature->GetCharmerOrOwner();

        m_creature->AttackStop();
        m_creature->CombatStop(true);
        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
        }
    }

    void AttackStart(Unit* pWho)
    {
        // mini-pet don't attack anything
        return;
    }
    
    void UpdateAI(const uint32 diff)
    {
        if (!isLove)
        {
            if (Check_timer <= diff)
            {
                if (rand()%2)
                {
                    std::list<Unit *> targets;
                    CellPair p(MaNGOS::ComputeCellPair(m_creature->GetPositionX(), m_creature->GetPositionY()));
                    Cell cell(p);
                    cell.data.Part.reserved = ALL_DISTRICT;
                    cell.SetNoCreate();

                    MaNGOS::AnyFriendlyUnitInObjectRangeCheck u_check(m_creature, 8.0f);
                    MaNGOS::UnitListSearcher<MaNGOS::AnyFriendlyUnitInObjectRangeCheck> searcher(m_creature, targets, u_check);
                    Cell::VisitAllObjects(m_creature, searcher, 8.0f);

                    for(std::list<Unit *>::iterator tIter = targets.begin(); tIter != targets.end(); tIter++)
                    {
                        if ((*tIter)->GetTypeId() == TYPEID_UNIT && (*tIter)->GetEntry() == 32791 && (*tIter)->GetGUID() != m_creature->GetGUID())
                        {
                            DoCast(*tIter,61728);
                            if (Unit *owner = m_creature->GetOwner())
                                owner->CastSpell(owner,61875,true);                            
                            isLove = true;
                            break;
                        }
                    }
                }
                Check_timer = 5000;
            }else Check_timer -= diff;
        } else
        {
            if (Unlove_timer <= diff)
            {            
                m_creature->InterruptNonMeleeSpells(false);
                isLove = false;
                Unlove_timer = 4000;
            } else Unlove_timer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_spring_rabbit(Creature* pCreature)
{
    return new npc_spring_rabbitAI(pCreature);
}

/*######
## npc_orphant
######*/

struct MANGOS_DLL_DECL npc_orphantAI : public ScriptedAI
{
    npc_orphantAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Check_timer;
    uint32 Unlove_timer;
    bool isLove;


    void Reset()
    {
        Unit *owner = m_creature->GetOwner();
        if (!owner)
            return;

        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        Check_timer = 5000;
        Unlove_timer = 4000;
        isLove = false;
    }

    void EnterEvadeMode()
    {
        if (m_creature->IsInEvadeMode() || !m_creature->isAlive())
            return;

        Unit *owner = m_creature->GetCharmerOrOwner();

        m_creature->AttackStop();
        m_creature->CombatStop(true);
        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
        }
    }

    void AttackStart(Unit* pWho)
    {
        // mini-pet don't attack anything
        return;
    }
    
    void UpdateAI(const uint32 diff)
    {
        Unit *owner = m_creature->GetCharmerOrOwner();
        if (!owner)
            return;

        if (Check_timer <= diff)
        {
            switch(m_creature->GetAreaId())
            {
            case 1599: owner->CastSpell(owner,65058,true); break; // Mor'shan Rampart Credit
            case 1497: owner->CastSpell(owner,65059,true); break; // Lordaeron Throne Room Credit
            case 1657: owner->CastSpell(owner,65056,true); break; // Bough of the Eternals Credit
            case 146: owner->CastSpell(owner,65055,true); break; // Stonewrought Dam Credit
            case 115: owner->CastSpell(owner,65054,true); break; // Lighthouse Credit
            case 392: owner->CastSpell(owner,65057,true); break; // Ratchet Docks Credit
            default: break;
            }
        }
        else Check_timer -= diff;
    }
};

CreatureAI* GetAI_npc_orphant(Creature* pCreature)
{
    return new npc_orphantAI(pCreature);
}

std::string GenerateCode(int type)
{
    // generate code
    std::string Code;    
    // check if db gone away
    QueryResult *result = SD2Database.PQuery("SELECT 1 FROM sd2_db_version");
    if (!result)
    {
        Config SD2Config;
        SD2Config.SetSource("/home/mangos/bin/wolk/etc/scriptdev2.conf");
        std::string strSD2DBinfo = SD2Config.GetStringDefault("ScriptDev2DatabaseInfo", "");
        SD2Database.Initialize(strSD2DBinfo.c_str());
    }
    delete result;

    QueryResult* pResult = SD2Database.PQuery("SELECT substr(MD5(%u),27)", GetGameTime());
    if (pResult)
    {
        Field* pFields = pResult->Fetch();
        Code = pFields[0].GetCppString();
        SD2Database.PExecute("INSERT INTO raffle_code VALUES ('%s', %i)", Code.c_str(), type);
    }
    delete pResult;
    return Code;
}

void NotifyPlayer(Player* pPlayer, Creature* pCreature, const char* item, std::string Code)
{
	const char* name = pPlayer->GetName();
    // server announce
    SendWorldText(11900, name, item);
    // send mail
    // msgSubject, msgText isn't NUL after prev. check
    std::string subject = "Поздравляем!";
    std::string text    = "Поздравляем, Вы выиграли в лотерее! Ваш приз - ";
    text = text + item;
	std::string tail = ". Ваш код для получения приза - ";
	text = text + tail + Code;

    MailDraft(subject, text)
        .SendMailTo(MailReceiver(pPlayer,GUID_LOPART(pPlayer->GetGUID())),MailSender(MAIL_CREATURE, pCreature->GetEntry()));
}

void DoDrawing(Player *pPlayer, Creature *pCreature, uint32 Price, int Chance, const char* Item, int type, int b)
{
    if (pPlayer->GetMoney() < Price)
    {
        pPlayer->SendBuyError( BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else
    {
        pPlayer->ModifyMoney(-int(Price));
        if (roll_chance_f(Chance))
        {
            std::string Code;
            Code = GenerateCode(type);
            NotifyPlayer(pPlayer, pCreature, Item, Code);
            pPlayer->SEND_GOSSIP_MENU(122992, pCreature->GetGUID());
        }
        else
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ещё раз?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +b);
            pPlayer->SEND_GOSSIP_MENU(122991, pCreature->GetGUID());
        }
    }
}

#define GOSSIP_GO_SWIFT_SPECTRAL        "Я хочу сыграть на Резвого Призрачного Тигра"
#define GOSSIP_GO_NETHER_ROCKET         "Я хочу сыграть на ИКС-ключительную ракету Пустоты X-51"
#define GOSSIP_WEATHER_MACHINE          "Я хочу сыграть на Гоблинскую метеоустановку - прототип 01-B"
#define GOSSIP_HIPPOGRYPH_HATCHLING     "Я хочу сыграть на Детеныша гиппогрифа"
#define GOSSIP_BANANA_CHARM             "Я хочу сыграть на Банановый оберег"
#define GOSSIP_CELESTIAL_STEED          "Я хочу сыграть на Небесного скакуна"
bool GossipHello_npc_raffle(Player* pPlayer, Creature* pCreature)
{
    // Reins of the Swift Spectral Tiger
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GO_SWIFT_SPECTRAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +1);
    // X-51 Nether-Rocket X-TREME 
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GO_NETHER_ROCKET, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +3);
    // Goblin Weather Machine
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WEATHER_MACHINE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +5);
    // Hippogryph Hatchling
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HIPPOGRYPH_HATCHLING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +7);
    // Banana Charm
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BANANA_CHARM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +9);
    // Celestial Steed
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CELESTIAL_STEED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +11);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_raffle(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Испытать удачу...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(122990, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Испытать удачу...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
        pPlayer->SEND_GOSSIP_MENU(122993, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Испытать удачу...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
        pPlayer->SEND_GOSSIP_MENU(122994, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+7:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Испытать удачу...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+8);
        pPlayer->SEND_GOSSIP_MENU(122995, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+9:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Испытать удачу...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+10);
        pPlayer->SEND_GOSSIP_MENU(122996, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+11:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Испытать удачу...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+12);
        pPlayer->SEND_GOSSIP_MENU(122997, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        DoDrawing(pPlayer, pCreature, 10000000, 1, "Swift Spectral Tiger", 1, 1);
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        DoDrawing(pPlayer, pCreature, 7000000, 1, "X-51 Nether-Rocket X-TREME", 2, 3);
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        DoDrawing(pPlayer, pCreature, 1500000, 1, "Goblin Weather Machine - Prototype 01-B", 3, 5);
        break;
    case GOSSIP_ACTION_INFO_DEF+8:
        DoDrawing(pPlayer, pCreature, 3000000, 1, "Hippogryph Hatchling", 4, 7);
        break;
    case GOSSIP_ACTION_INFO_DEF+10:
        DoDrawing(pPlayer, pCreature, 3000000, 1, "Banana Charm", 5, 9);
        break;
    case GOSSIP_ACTION_INFO_DEF+12:
        DoDrawing(pPlayer, pCreature, 12000000, 1, "Celestial Steed", 6, 11);
        break;
    }
    return true;
}

void AddSC_npcs_special()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_air_force_bots";
    newscript->GetAI = &GetAI_npc_air_force_bots;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_chicken_cluck";
    newscript->GetAI = &GetAI_npc_chicken_cluck;
    newscript->pQuestAccept =   &QuestAccept_npc_chicken_cluck;
    newscript->pQuestComplete = &QuestComplete_npc_chicken_cluck;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dancing_flames";
    newscript->GetAI = &GetAI_npc_dancing_flames;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lake_frog";
    newscript->GetAI = &GetAI_npc_lake_frog;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_injured_patient";
    newscript->GetAI = &GetAI_npc_injured_patient;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_doctor";
    newscript->GetAI = &GetAI_npc_doctor;
    newscript->pQuestAccept = &QuestAccept_npc_doctor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_garments_of_quests";
    newscript->GetAI = &GetAI_npc_garments_of_quests;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_guardian";
    newscript->GetAI = &GetAI_npc_guardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_innkeeper";
    newscript->pGossipHello = &GossipHello_npc_innkeeper;
    newscript->pGossipSelect = &GossipSelect_npc_innkeeper;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kingdom_of_dalaran_quests";
    newscript->pGossipHello =  &GossipHello_npc_kingdom_of_dalaran_quests;
    newscript->pGossipSelect = &GossipSelect_npc_kingdom_of_dalaran_quests;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lunaclaw_spirit";
    newscript->pGossipHello =  &GossipHello_npc_lunaclaw_spirit;
    newscript->pGossipSelect = &GossipSelect_npc_lunaclaw_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mount_vendor";
    newscript->pGossipHello =  &GossipHello_npc_mount_vendor;
    newscript->pGossipSelect = &GossipSelect_npc_mount_vendor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rogue_trainer";
    newscript->pGossipHello =  &GossipHello_npc_rogue_trainer;
    newscript->pGossipSelect = &GossipSelect_npc_rogue_trainer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sayge";
    newscript->pGossipHello = &GossipHello_npc_sayge;
    newscript->pGossipSelect = &GossipSelect_npc_sayge;
    newscript->RegisterSelf();
	
	newscript = new Script;
    newscript->Name = "npc_bathandler_camille";
    newscript->pGossipHello = &GossipHello_Bat_Handler_Camille;
    newscript->pGossipSelect = &GossipSelect_Bat_Handler_Camille;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_greer_orehammer";
    newscript->pGossipHello = &GossipHello_Greer_OreHammer;
    newscript->pGossipSelect = &GossipSelect_Greer_OreHammer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_wild_wolpertinger";
    newscript->GetAI = &GetAI_npc_wild_wolpertinger;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_mirror_image";
	newscript->GetAI = &GetAI_npc_mirror_image;
	newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_winter_reveler";
    newscript->GetAI = &GetAI_npc_winter_reveler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tabard_vendor";
    newscript->pGossipHello =  &GossipHello_npc_tabard_vendor;
    newscript->pGossipSelect = &GossipSelect_npc_tabard_vendor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_locksmith";
    newscript->pGossipHello =  &GossipHello_npc_locksmith;
    newscript->pGossipSelect = &GossipSelect_npc_locksmith;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_love_air";
    newscript->GetAI = &GetAI_npc_love_air;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_fjord_turkey";
    newscript->GetAI = &GetAI_npc_fjord_turkey;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_spring_rabbit";
    newscript->GetAI = &GetAI_npc_spring_rabbit;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_orphant";
    newscript->GetAI = &GetAI_npc_orphant;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_raffle";
    newscript->pGossipHello =  &GossipHello_npc_raffle;
    newscript->pGossipSelect = &GossipSelect_npc_raffle;
    newscript->RegisterSelf();
}
