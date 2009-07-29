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
SDName: Borean_Tundra
SD%Complete: 100
SDComment:
SDCategory: Borean Tundra
EndScriptData */

/* ContentData
npc_tiare
npc_surristrasz
EndContentData */

#include "precompiled.h"

/*######
## npc_tiare
######*/

#define GOSSIP_ITEM_TELEPORT    "Teleport me to Amber Ledge, please."

bool GossipHello_npc_tiare(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_GOSSIP);
    pPlayer->SEND_GOSSIP_MENU(pCreature->GetNpcTextId(), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_tiare(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_OPTION_GOSSIP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,50135,true);
    }
    return true;
}

/*######
## npc_surristrasz
######*/

#define GOSSIP_ITEM_FREE_FLIGHT "I'd like passage to the Transitus Shield."
#define GOSSIP_ITEM_FLIGHT      "May I use a drake to fly elsewhere?"

bool GossipHello_npc_surristrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isTaxi())
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FREE_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_GOSSIP);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_ITEM_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_TAXIVENDOR);
    }

    pPlayer->SEND_GOSSIP_MENU(pCreature->GetNpcTextId(), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_surristrasz(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_OPTION_GOSSIP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,46064,true);               //TaxiPath 795 (amber to coldarra)
    }
    if (action == GOSSIP_OPTION_TAXIVENDOR)
    {
        pPlayer->GetSession()->SendTaxiMenu(pCreature);
    }
    return true;
}

// quest 11608

struct  MANGOS_DLL_DECL mob_seaforiumdepthchargeAI : public ScriptedAI
{
	mob_seaforiumdepthchargeAI( Creature* pCreature ) : ScriptedAI( pCreature ) { Reset(); } 

	enum
	{
		SPELL_BOOM          = 45502,
	};

	uint32 m_boomTimer;
	uint32 m_killcredit;

	void Reset()
	{
		m_boomTimer = 6000;
		m_killcredit = 0;
		//m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		//m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
	}

	void UpdateAI(const uint32 diff)
	{
		// todo: need text
		if( m_boomTimer < diff )
		{
			m_creature->CastSpell(m_creature,SPELL_BOOM,true);

			for (int i = 0; i < 4; i++)
			{
				if (GetClosestCreatureWithEntry(m_creature,25402+i,10))
				{
					m_killcredit = 25402+i;
					break;
				}
			}

			Unit *player = m_creature->GetOwner() ;
			if (player && m_killcredit)
				((Player*)player)->KilledMonsterCredit(m_killcredit,m_creature->GetGUID());

			((TemporarySummon *)m_creature)->UnSummon();

		}else m_boomTimer -= diff;
	}

};

CreatureAI* GetAI_mob_seaforiumdepthcharge( Creature* pCreature )
{
	return new mob_seaforiumdepthchargeAI( pCreature );
}

void AddSC_borean_tundra()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_tiare";
    newscript->pGossipHello =  &GossipHello_npc_tiare;
    newscript->pGossipSelect = &GossipSelect_npc_tiare;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_surristrasz";
    newscript->pGossipHello =  &GossipHello_npc_surristrasz;
    newscript->pGossipSelect = &GossipSelect_npc_surristrasz;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_seaforiumdepthcharge";
	newscript->GetAI = &GetAI_mob_seaforiumdepthcharge;
	newscript->RegisterSelf();
}
