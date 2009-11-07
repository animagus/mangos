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
SDName: Howling_Fjord
SD%Complete: ?
SDComment: Quest support: 11221
SDCategory: Howling Fjord
EndScriptData */

#include "precompiled.h"

/*#######################
## Deathstalker Razael ##
#######################*/

#define GOSSIP_ITEM_DEATHSTALKER_RAZAEL "High Executor Anselm requests your report."
enum
{
   QUEST_REPORTS_FROM_THE_FIELD       = 11221,
   SPELL_RAZAEL_KILL_CREDIT           = 42756,
   GOSSIP_TEXTID_DEATHSTALKER_RAZAEL1 = 11562,
   GOSSIP_TEXTID_DEATHSTALKER_RAZAEL2 = 11564
};
bool GossipHello_npc_deathstalker_razael(Player* pPlayer, Creature* pCreature)
{
   if (pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu(pCreature->GetGUID());

   if (pPlayer->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
   {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEATHSTALKER_RAZAEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEATHSTALKER_RAZAEL1, pCreature->GetGUID());
   }
   else
       pPlayer->SEND_GOSSIP_MENU(pCreature->GetNpcTextId(), pCreature->GetGUID());
   return true;
}

bool GossipSelect_npc_deathstalker_razael(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
   switch(uiAction)
   {
       case GOSSIP_ACTION_INFO_DEF+1:
           pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEATHSTALKER_RAZAEL2, pCreature->GetGUID());
           pCreature->CastSpell(pPlayer, SPELL_RAZAEL_KILL_CREDIT, true);
            break;
   }
   return true;
}

/*#####################
## Dark Ranger Lyana ##
#####################*/

#define GOSSIP_ITEM_DARK_RANGER_LYANA "High Executor Anselm requests your report."

enum
{
   GOSSIP_TEXTID_DARK_RANGER_LYANA1    = 11586,
   GOSSIP_TEXTID_DARK_RANGER_LYANA2    = 11588,
   SPELL_DARK_RANGER_LYANA_KILL_CREDIT = 42799
};

bool GossipHello_npc_dark_ranger_lyana(Player* pPlayer, Creature* pCreature)
{
   if (pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu(pCreature->GetGUID());

   if (pPlayer->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
   {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DARK_RANGER_LYANA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DARK_RANGER_LYANA1, pCreature->GetGUID());
   }
   else
       pPlayer->SEND_GOSSIP_MENU(pCreature->GetNpcTextId(), pCreature->GetGUID());
   return true;
}

bool GossipSelect_npc_dark_ranger_lyana(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
   switch(uiAction)
   {
       case GOSSIP_ACTION_INFO_DEF+1:
           pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DARK_RANGER_LYANA2, pCreature->GetGUID());
           pCreature->CastSpell(pPlayer, SPELL_DARK_RANGER_LYANA_KILL_CREDIT, true);
            break;
   }
   return true;
}

struct  MANGOS_DLL_DECL mob_protodrakeAI : public ScriptedAI
{
  mob_protodrakeAI( Creature* pCreature ) : ScriptedAI( pCreature ) { Reset(); } 

  uint64 PlayerGUID;
  bool canEat;
  bool canFly;
  uint32 eatTimer;
  uint32 flyTimer;
  float x,y,z;

  void Reset()
  {
    PlayerGUID = 0;
    canEat = false;
    canFly = false;
    eatTimer = 5000;
    flyTimer = 5000;
    x = 0;
    y = 0;
    z = 0;
  }

  void BeginEvent(Player* pPlayer)
  {
    PlayerGUID = pPlayer->GetGUID();
    x = pPlayer->GetPositionX();
    y = pPlayer->GetPositionY();
    z = pPlayer->GetPositionZ();

    canFly = true;
  } 

  void MovementInform(uint32 type, uint32 id)
  {
    if(type != POINT_MOTION_TYPE)
      return;

    if(id == 1)
	{
		canEat = true;
		m_creature->HandleEmoteCommand( EMOTE_ONESHOT_ATTACKUNARMED );
	}
  }


  void UpdateAI(const uint32 diff)
  {

    if(canFly)
      {
	if(flyTimer < diff)
	  {
	    m_creature->GetMotionMaster()->MovePoint(1, x,y,z);
	    if(canEat)
	      {
		  if( eatTimer < diff )
		  {
			  if( Player* plr = (Player*)Unit::GetUnit((*m_creature), PlayerGUID)) 
				  plr->KilledMonsterCredit( 24170,  m_creature->GetGUID() );
		  }else eatTimer -= diff;
	      }
	  }else flyTimer -= diff;
      }
	
	if ( !m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

	DoMeleeAttackIfReady();
  }

};

CreatureAI* GetAI_mob_protodrakeAI( Creature* pCreature )
{
  return new mob_protodrakeAI( pCreature );
}

bool ItemUse_item_Tillinghast_Plagued_Meat(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{

  if(pPlayer->GetQuestStatus(11280) == QUEST_STATUS_INCOMPLETE)
  {
	  Creature* pCreature =  pPlayer->SummonCreature(23689, pPlayer->GetPositionX(), pPlayer->GetPositionY() + 10, pPlayer->GetPositionZ() + 20 , 0, TEMPSUMMON_TIMED_DESPAWN, 180000);
	  ((mob_protodrakeAI*)pCreature->AI())->BeginEvent(pPlayer);
  }

  return true;
}

void AddSC_howling_fjord()
{
   Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_deathstalker_razael";
    newscript->pGossipHello = &GossipHello_npc_deathstalker_razael;
    newscript->pGossipSelect = &GossipSelect_npc_deathstalker_razael;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dark_ranger_lyana";
    newscript->pGossipHello = &GossipHello_npc_dark_ranger_lyana;
    newscript->pGossipSelect = &GossipSelect_npc_dark_ranger_lyana;
    newscript->RegisterSelf();
	
	newscript = new Script;
    newscript->Name = "mob_protodrake";
    newscript->GetAI = &GetAI_mob_protodrakeAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_tillinghast_plagued_meat";
    newscript->pItemUse = &ItemUse_item_Tillinghast_Plagued_Meat;
    newscript->RegisterSelf();

}
