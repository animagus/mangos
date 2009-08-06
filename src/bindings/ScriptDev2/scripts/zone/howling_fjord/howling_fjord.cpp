#include "precompiled.h"
#include "ObjectMgr.h"

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
    eatTimer = 5000;
    flyTimer = 0;
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
    newscript->Name = "mob_protodrake";
    newscript->GetAI = &GetAI_mob_protodrakeAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_tillinghast_plagued_meat";
    newscript->pItemUse = &ItemUse_item_Tillinghast_Plagued_Meat;
    newscript->RegisterSelf();
}
