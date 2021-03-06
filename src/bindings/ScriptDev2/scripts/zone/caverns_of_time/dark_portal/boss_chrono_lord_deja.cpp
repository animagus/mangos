/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Chrono_Lord_Deja
SD%Complete: 65
SDComment: All abilities not implemented
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */

#include "precompiled.h"
#include "def_dark_portal.h"

#define SAY_ENTER                   -1269006
#define SAY_AGGRO                   -1269007
#define SAY_BANISH                  -1269008
#define SAY_SLAY1                   -1269009
#define SAY_SLAY2                   -1269010
#define SAY_DEATH                   -1269011

#define SPELL_ARCANE_BLAST          31457
#define H_SPELL_ARCANE_BLAST        38538
#define SPELL_ARCANE_DISCHARGE      31472
#define H_SPELL_ARCANE_DISCHARGE    38539
#define SPELL_TIME_LAPSE            31467
#define SPELL_ATTRACTION            38540                       //Not Implemented (Heroic mode)

struct MANGOS_DLL_DECL boss_chrono_lord_dejaAI : public ScriptedAI
{
    boss_chrono_lord_dejaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 ArcaneBlast_Timer;
    uint32 TimeLapse_Timer;
    uint32 Attraction_Timer;
    uint32 ArcaneDischarge_Timer;

    void Reset()
    {
        ArcaneBlast_Timer = 18000+rand()%5000;
        TimeLapse_Timer = 10000+rand()%5000;
        ArcaneDischarge_Timer = 20000+rand()%10000;
        Attraction_Timer = 25000+rand()%10000;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit *who)
    {
        //Despawn Time Keeper
        if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_TIME_KEEPER)
        {
            if (m_creature->IsWithinDistInMap(who,20.0f))
            {
                DoScriptText(SAY_BANISH, m_creature);
                m_creature->DealDamage(who, who->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_RIFT,SPECIAL);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //Arcane Blast
        if (ArcaneBlast_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsHeroicMode ? H_SPELL_ARCANE_BLAST : SPELL_ARCANE_BLAST);
            ArcaneBlast_Timer = 15000+rand()%10000;
        }else ArcaneBlast_Timer -= diff;

        //Arcane Discharge
        if (ArcaneDischarge_Timer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
            DoCast(target, m_bIsHeroicMode ? H_SPELL_ARCANE_DISCHARGE : SPELL_ARCANE_DISCHARGE);
            ArcaneDischarge_Timer = 20000+rand()%10000;
        }else ArcaneDischarge_Timer -= diff;

        //Time Lapse
        if (TimeLapse_Timer < diff)
        {
            DoScriptText(SAY_BANISH, m_creature);
            DoCast(m_creature, SPELL_TIME_LAPSE);
            TimeLapse_Timer = 15000+rand()%10000;
        }else TimeLapse_Timer -= diff;

        if (m_bIsHeroicMode)
        {
            if (Attraction_Timer < diff)
            {
                DoCast(m_creature,SPELL_ATTRACTION);
                Attraction_Timer = 25000+rand()%10000;
            }else Attraction_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_chrono_lord_deja(Creature* pCreature)
{
    return new boss_chrono_lord_dejaAI(pCreature);
}

void AddSC_boss_chrono_lord_deja()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_chrono_lord_deja";
    newscript->GetAI = &GetAI_boss_chrono_lord_deja;
    newscript->RegisterSelf();
}
