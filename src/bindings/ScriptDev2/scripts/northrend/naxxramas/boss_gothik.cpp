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
SDName: Boss_Gothik
SDAuthor: ckegg
SD%Complete: 0
SDComment: Center gate.........?
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_SPEECH                  -1533040
#define SAY_KILL                    -1533041
#define SAY_DEATH                   -1533042
#define SAY_TELEPORT                -1533043

//Gothik
#define SPELL_HARVESTSOUL           28679
#define SPELL_SHADOWBOLT            29317
#define H_SPELL_SHADOWBOLT          56405

//Unrelenting Trainee
#define SPELL_EAGLECLAW             30285
#define SPELL_KNOCKDOWN_PASSIVE     6961

//Unrelenting Deathknight
#define SPELL_CHARGE                22120
#define SPELL_SHADOW_MARK           27825

//Unrelenting Rider
#define SPELL_UNHOLY_AURA           55606
#define H_SPELL_UNHOLY_AURA         55608
#define SPELL_SHADOWBOLT_VOLLEY     27831                   //Search thru targets and find those who have the SHADOW_MARK to cast this on
#define H_SPELL_SHADOWBOLT_VOLLEY   55638

//Spectral Trainee
#define SPELL_ARCANE_EXPLOSION      27989

//Spectral Deathknight
#define SPELL_WHIRLWIND             28334
#define SPELL_SUNDER_ARMOR          25051                   //cannot find sunder that reduces armor by 2950
#define SPELL_CLEAVE                20677
#define SPELL_MANA_BURN             17631

//Spectral Rider
#define SPELL_LIFEDRAIN             24300
//USES SAME UNHOLY AURA AS UNRELENTING RIDER

//Spectral Horse
#define SPELL_STOMP                 27993

#define MOB_LIVE_TRAINEE    16124
#define MOB_LIVE_KNIGHT     16125
#define MOB_LIVE_RIDER      16126
#define MOB_DEAD_TRAINEE    16127
#define MOB_DEAD_KNIGHT     16148
#define MOB_DEAD_RIDER      16150
#define MOB_DEAD_HORSE      16149

#define POS_LIVE 3
#define POS_DEAD 5

const struct Waves { uint32 entry, number, time; }
waves[] =
{
    {MOB_LIVE_TRAINEE, 2, 20000},
    {MOB_LIVE_TRAINEE, 2, 20000},
    {MOB_LIVE_TRAINEE, 2, 10000},
    {MOB_LIVE_KNIGHT,  1, 10000}, // 60
    {MOB_LIVE_TRAINEE, 2, 15000}, 
    {MOB_LIVE_KNIGHT,  1, 10000},
    {MOB_LIVE_TRAINEE, 2, 15000},
    {MOB_LIVE_TRAINEE, 2, 0},
    {MOB_LIVE_KNIGHT,  1, 10000},
    {MOB_LIVE_RIDER,   1, 10000}, // 120
    {MOB_LIVE_TRAINEE, 2, 5000},
    {MOB_LIVE_KNIGHT,  1, 15000},
    {MOB_LIVE_TRAINEE, 2, 0},
    {MOB_LIVE_RIDER,   1, 10000},
    {MOB_LIVE_KNIGHT,  1, 10000},
    {MOB_LIVE_TRAINEE, 2, 10000},
    {MOB_LIVE_RIDER,   1, 5000},
    {MOB_LIVE_KNIGHT,  1, 5000},  // 180
    {MOB_LIVE_TRAINEE, 2, 20000},
    {MOB_LIVE_TRAINEE, 2, 0},
    {MOB_LIVE_KNIGHT,  1, 0},
    {MOB_LIVE_RIDER,   1, 15000},
    {MOB_LIVE_TRAINEE, 2, 29000}, // 244
    {0, 0, 0},
};

const float PosSummonLive[POS_LIVE][3] =
{
    {2669.7f, -3430.9f, 268.56f},
    {2692.0f, -3430.9f, 268.56f},
    {2714.1f, -3430.9f, 268.56f},
};

const float PosSummonDead[POS_DEAD][3] =
{
    {2725.1f, -3310.0f, 268.85f},
    {2699.3f, -3322.8f, 268.60f},
    {2733.1f, -3348.5f, 268.84f},
    {2682.8f, -3304.2f, 268.85f},
    {2664.8f, -3340.7f, 268.23f},
};

const float PosPlatform[4] = {2640.5f, -3360.6f, 285.26f, 0};
const float PosGroundLive[4] = {2692.174f, -3400.963f, 267.680f, 1.7f};
const float PosGroundDeath[4] = {2690.378f, -3328.279f, 267.681f, 1.7f};

struct MANGOS_DLL_DECL boss_gothikAI : public Scripted_NoMovementAI
{
    boss_gothikAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool SummonPhase;
    bool BlinkPhase;

    std::list<ObjectGuid> SummonsList;

    uint32 waveCount;
    uint32 Summon_Timer;
    uint32 SummonDeathCheck_Timer;
    uint32 HarvestSoul_Timer;
    uint32 ShadowBolt_Timer;
    uint32 Blink_Timer;

    void Reset()
    {
        SummonPhase = false;
        BlinkPhase = false;

        SummonsList.clear();

        waveCount = 0;
        Summon_Timer = 10000;
        SummonDeathCheck_Timer = 1000;
        HarvestSoul_Timer = 15000;
        ShadowBolt_Timer = 1000;
        Blink_Timer = 30000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustRespawned()
    {
        JustReachedHome();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GOTHIK, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(SAY_SPEECH, m_creature);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->GetMap()->CreatureRelocation(m_creature, PosPlatform[0], PosPlatform[1], PosPlatform[2], PosPlatform[3]);
        m_creature->SetInCombatWithZone();

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_GOTHIK, IN_PROGRESS);

            /*if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GOTHIK_GATE)))
                pGate->SetGoState(GO_STATE_READY);*/
        }
    }

    void KilledUnit(Unit* victim)
    {
        if(!(rand()%5))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_GOTHIK, DONE);
    }

    void JustSummoned(Creature* pSummon)
    {
        pSummon->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SummonPhase)
        {
            if (HarvestSoul_Timer < diff)
            {
                m_creature->InterruptNonMeleeSpells(false);
                DoCast(m_creature->getVictim(), SPELL_HARVESTSOUL,true);
                HarvestSoul_Timer = 15000 + rand()%1000;
            }else HarvestSoul_Timer -= diff;

            if (ShadowBolt_Timer < diff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? H_SPELL_SHADOWBOLT : SPELL_SHADOWBOLT);
                ShadowBolt_Timer = 1000 + rand()%500;
            }else ShadowBolt_Timer -= diff;

            if (Blink_Timer < diff)
            {
                if (BlinkPhase)
                {
                    m_creature->GetMap()->CreatureRelocation(m_creature, PosGroundLive[0], PosGroundLive[1], PosGroundLive[2], 0.0f);
                    BlinkPhase = false;
                }
                else
                {
                    m_creature->GetMap()->CreatureRelocation(m_creature, PosGroundDeath[0], PosGroundDeath[1], PosGroundDeath[2], 0.0f);
                    BlinkPhase = true;
                }
                DoResetThreat();
                Blink_Timer = 15000;
            }else Blink_Timer -= diff;
        }
        else
        {
            if (Summon_Timer < diff)
            {
                if(waves[waveCount].entry)
                {
                    for(uint32 i = 0; i < waves[waveCount].number; ++i)
                    {
                        uint8 SummonLoc = rand()%POS_LIVE;
                        if (Creature* pTemp = m_creature->SummonCreature(waves[waveCount].entry, PosSummonLive[SummonLoc][0], PosSummonLive[SummonLoc][1], PosSummonLive[SummonLoc][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                            SummonsList.push_back(pTemp->GetObjectGuid());
                    }
                    Summon_Timer = waves[waveCount].time;
                    ++waveCount;
                }
                else
                {
                    DoScriptText(SAY_TELEPORT, m_creature);
                    uint8 SummonLoc = rand()%POS_LIVE;
                    m_creature->GetMap()->CreatureRelocation(m_creature, PosSummonLive[SummonLoc][0], PosSummonLive[SummonLoc][1], PosSummonLive[SummonLoc][2], 0.0f);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    SummonPhase = true;
                }
            }else Summon_Timer -= diff;
        }

        if (SummonDeathCheck_Timer < diff)
        {
            if (!SummonsList.empty())
            {
                for(std::list<ObjectGuid>::iterator itr = SummonsList.begin(); itr != SummonsList.end(); ++itr)
                {
                    if (Creature* pTemp = ((Creature*)m_creature->GetMap()->GetUnit(*itr)))
                    {
                        if (!pTemp->isAlive())
                        {
                            uint8 SummonLoc = rand()%POS_DEAD;
                            if (pTemp->GetEntry() == MOB_LIVE_TRAINEE)
                                m_creature->SummonCreature(MOB_DEAD_TRAINEE, PosSummonDead[SummonLoc][0], PosSummonDead[SummonLoc][1], PosSummonDead[SummonLoc][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                            else if (pTemp->GetEntry() == MOB_LIVE_KNIGHT)
                                m_creature->SummonCreature(MOB_DEAD_KNIGHT, PosSummonDead[SummonLoc][0], PosSummonDead[SummonLoc][1], PosSummonDead[SummonLoc][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                            else if (pTemp->GetEntry() == MOB_LIVE_RIDER)
                            {
                                m_creature->SummonCreature(MOB_DEAD_RIDER, PosSummonDead[SummonLoc][0], PosSummonDead[SummonLoc][1], PosSummonDead[SummonLoc][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                m_creature->SummonCreature(MOB_DEAD_HORSE, PosSummonDead[SummonLoc][0], PosSummonDead[SummonLoc][1], PosSummonDead[SummonLoc][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                            }

                            /*if (m_pInstance)
                                if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GOTHIK_GATE)))
                                    pGate->SetGoState(GO_STATE_ACTIVE);*/
                            SummonsList.remove(pTemp->GetGUID());
                            break;
                        }
                    }
                }
            }
            SummonDeathCheck_Timer = 1000;
        }else SummonDeathCheck_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_gothik(Creature* pCreature)
{
    return new boss_gothikAI(pCreature);
}

void AddSC_boss_gothik()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gothik";
    newscript->GetAI = &GetAI_boss_gothik;
    newscript->RegisterSelf();
}
