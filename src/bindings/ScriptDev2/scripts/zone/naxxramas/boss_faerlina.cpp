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
SDName: Boss_Faerlina
SD%Complete: 80
SDComment: Need core support: spell 28732
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SPELL_28732_NOT_YET_IMPLEMENTED

enum
{
    SAY_GREET                 =  -1533009,
    SAY_AGGRO                 =  -1533010,
    SAY_ENRAGE1               =  -1533011,
    SAY_ENRAGE2               =  -1533012,
    SAY_ENRAGE3               =  -1533013,
    SAY_SLAY1                 =  -1533014,
    SAY_SLAY2                 =  -1533015,
    SAY_DEATH                 =  -1533016
};
enum
{
    SPELL_POISONBOLT_VOLLEY   =  28796,
    SPELL_POISONBOLT_VOLLEY_H =  54098,
    SPELL_ENRAGE              =  28798,
    SPELL_RAINOFFIRE          =  39024,                   //Not sure if targeted AoEs work if casted directly upon a player
    SPELL_RAINOFFIRE_H        =  58936,

    SPELL_WIDOWS_EMBRACE      =  28732,					//We will use it, but the real thing will be another
    SPELL_FIREBALL            =  54095,
    SPELL_FIREBALL_H          =  54096
};

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature *c) : ScriptedAI(c)
	{
		pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_bIsHeroicMode = c->GetMap()->IsHeroic();
        HasTaunted = false;
		Reset();
	}

	ScriptedInstance *pInstance;
    bool m_bIsHeroicMode;

	uint32 PoisonBoltVolley_Timer;
    uint32 RainOfFire_Timer;
    uint32 Enrage_Timer;
    uint32 m_uiEvadeCheckCooldown;
    uint64 m_uiworshipperGUID[4];
    bool HasTaunted;

    void Reset()
    {
        memset(&m_uiworshipperGUID, 0, sizeof(m_uiworshipperGUID));
        PoisonBoltVolley_Timer = 8000;
        m_uiEvadeCheckCooldown = 2000;
        RainOfFire_Timer = 16000;
        Enrage_Timer = 60000;
        
        if(pInstance && m_creature->isAlive())
			pInstance->SetData(ENCOUNT_FAERLINA, NOT_STARTED);

        if (pInstance)
        {
            m_uiworshipperGUID[0] = pInstance->GetData64(GUID_WORSHIPPER1);
            m_uiworshipperGUID[1] = pInstance->GetData64(GUID_WORSHIPPER2);
            m_uiworshipperGUID[2] = pInstance->GetData64(GUID_WORSHIPPER3);
            m_uiworshipperGUID[3] = pInstance->GetData64(GUID_WORSHIPPER4);
        }

        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = (Creature*)Unit::GetUnit((*m_creature), m_uiworshipperGUID[i]);
            if (pMinion)
                pMinion->Respawn();
        }
    }

    void Aggro(Unit *who)
    {
		//Close the room for boss fight
		if(pInstance)
            pInstance->SetData(ENCOUNT_FAERLINA, IN_PROGRESS);

        if (pInstance)
        {
            m_uiworshipperGUID[0] = pInstance->GetData64(GUID_WORSHIPPER1);
            m_uiworshipperGUID[1] = pInstance->GetData64(GUID_WORSHIPPER2);
            m_uiworshipperGUID[2] = pInstance->GetData64(GUID_WORSHIPPER3);
            m_uiworshipperGUID[3] = pInstance->GetData64(GUID_WORSHIPPER4);
        }

        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = (Creature*)Unit::GetUnit((*m_creature), m_uiworshipperGUID[i]);
            if (pMinion && !pMinion->getVictim())
                pMinion->AI()->AttackStart(who);
        }

		DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!HasTaunted && m_creature->IsWithinDistInMap(who, 90.0f))
        {
            DoScriptText(SAY_GREET, m_creature);
            HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit* victim)
    {
        switch (rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
		//Faerlina is slayed -> open all doors to Maexxna
		if(pInstance)
            pInstance->SetData(ENCOUNT_FAERLINA, DONE);

		DoScriptText(SAY_DEATH, m_creature);
    }

    void KillWorshipper()
    {
        if (m_creature->HasAura(SPELL_ENRAGE))
            Enrage_Timer = 60000;
        else
            Enrage_Timer = 30000;

        m_creature->RemoveAurasByCasterSpell(SPELL_ENRAGE,m_creature->GetGUID());
        PoisonBoltVolley_Timer = 30000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < diff)
        {
            if (m_creature->GetDistance2d(3353.44f, -3620.44f) > 100.0f)
                EnterEvadeMode();
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= diff;

        //PoisonBoltVolley_Timer
        if (PoisonBoltVolley_Timer < diff)
        {
            DoCast(m_creature->getVictim(),m_bIsHeroicMode?SPELL_POISONBOLT_VOLLEY_H:SPELL_POISONBOLT_VOLLEY);
            PoisonBoltVolley_Timer = 11000;
        }else PoisonBoltVolley_Timer -= diff;

        //RainOfFire_Timer
        if (RainOfFire_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,m_bIsHeroicMode?SPELL_RAINOFFIRE_H:SPELL_RAINOFFIRE);

            RainOfFire_Timer = 16000;
        }else RainOfFire_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
			switch (rand()%3)
			{
				case 0: DoScriptText(SAY_ENRAGE1, m_creature); break;
				case 1: DoScriptText(SAY_ENRAGE2, m_creature); break;
				case 2: DoScriptText(SAY_ENRAGE3, m_creature); break;
			}
            DoCast(m_creature,SPELL_ENRAGE);
            Enrage_Timer = 61000;
        }else Enrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_faerlina(Creature *_Creature)
{
    return new boss_faerlinaAI (_Creature);
}

struct MANGOS_DLL_DECL mob_faerlina_worshipperAI : public ScriptedAI
{
    mob_faerlina_worshipperAI(Creature *c) : ScriptedAI(c)
	{
		pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_bIsHeroic = c->GetMap()->IsHeroic();
		Reset();
	}

	ScriptedInstance *pInstance;
	uint32 fireball_timer;
    bool m_bIsHeroic;

	void Reset()
	{
		fireball_timer = 0;
	}

    void Aggro(Unit *who){}
    void JustDied(Unit* Killer)
    {
     //   DoCast(m_creature,SPELL_WIDOWS_EMBRACE,true);
		Creature* Faerlina = (Creature*)Unit::GetUnit((*m_creature), pInstance->GetData64(GUID_FAERLINA));
		if(Faerlina)
			((boss_faerlinaAI*)Faerlina->AI())->KillWorshipper();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

		//PoisonBoltVolley_Timer
        if (fireball_timer < diff)
        {
            DoCast(m_creature->getVictim(),m_bIsHeroic?SPELL_FIREBALL_H:SPELL_FIREBALL);
            fireball_timer = 3000;
        }else fireball_timer -= diff;

        DoMeleeAttackIfReady();
    }
}; 
CreatureAI* GetAI_mob_faerlina_worshipper(Creature *_Creature)
{
    return new mob_faerlina_worshipperAI (_Creature);
}

void AddSC_boss_faerlina()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_faerlina";
    newscript->GetAI = &GetAI_boss_faerlina;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_faerlina_worshipper";
    newscript->GetAI = &GetAI_mob_faerlina_worshipper;
    newscript->RegisterSelf();
}