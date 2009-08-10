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
SDName: Boss_Archavon_The_Stone_Watcher
SD%Complete: 0%
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "def_vault_of_archavon.h"

enum
{
    SPELL_ROCK_SHARDS_N                     = 58695,
    SPELL_ROCK_SHARDS_H                     = 60884,
    SPELL_CRUSHING_LEAP                     = 58960,
    SPELL_STOMP                             = 58663,
    SPELL_IMPALE                            = 58666,

    SPELL_BERSERK                           = 47008,
    H_SPELL_CRUSHING_LEAP                   = 60894,
    H_SPELL_STOMP                           = 60880,
    H_SPELL_IMPALE                          = 60882
};

struct MANGOS_DLL_DECL boss_archavonAI : public ScriptedAI
{
    boss_archavonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    bool m_bIsHeroicMode;
    uint32 m_uiBerserkTimer;

    void Reset()
    {
        m_uiBerserkTimer = 5000; //5 minutes
    }

    void Aggro(Unit *pWho) {}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserkTimer = 60000;
        }else m_uiBerserkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_archavon(Creature *pCreature)
{
    return new boss_archavonAI (pCreature);
};

void AddSC_boss_archavon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_archavon";
    newscript->GetAI = &GetAI_boss_archavon;
    newscript->RegisterSelf();
};
