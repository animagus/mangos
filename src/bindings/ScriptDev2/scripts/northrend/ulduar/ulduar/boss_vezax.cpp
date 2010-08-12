
#include "precompiled.h"
#include "ulduar.h"

#define SHADOW_CRASH            62660
#define SEARING_FLAMES          62661
#define SURGE_OF_DARKNESS       62662
#define AURA_OF_DESPIRE         62692
#define MARK_OF_THE_FACELESS    63276
#define SP_ENRAGE               26662
#define SAY_AGGRO               -1999926
#define SAY_KILL                -1999927
#define SAY_KILL2               -1999928
#define SAY_ENRAGE              -1999930
#define SAY_SUMMON              -1999929
#define SAY_DEATH               -1999931

struct MANGOS_DLL_DECL boss_vezaxAI : public ScriptedAI
{
    boss_vezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *pInstance;
    uint32 shadow_crash_timer;
    uint32 searing_flames_timer;
    uint32 surge_of_darkness_timer;
    uint32 mark_of_the_faceless;
    uint32 EnrageTimer;
    bool summon;

    void Reset()
    {
        if(pInstance && m_creature->isAlive())
            pInstance->SetData(TYPE_VEZAX, NOT_STARTED);
        shadow_crash_timer = 10000;
        searing_flames_timer = 21000;
        surge_of_darkness_timer = 49000;
        mark_of_the_faceless = 15500;
        EnrageTimer = 600000;
        summon = false;
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) 
            pInstance->SetData(TYPE_VEZAX, IN_PROGRESS);
        DoCast(m_creature,AURA_OF_DESPIRE,true);
        DoScriptText (SAY_AGGRO, m_creature);
    }


    void JustDied(Unit *killer)
    {
        if(pInstance)
        {
            pInstance->SetData(TYPE_VEZAX, DONE);
            DoScriptText (SAY_DEATH, m_creature);
        }
    }

    void KilledUnit(Unit *who)
    {
        if(irand(0,1))
            DoScriptText(SAY_KILL, m_creature);
        else
            DoScriptText(SAY_KILL2, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        if (shadow_crash_timer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCast(target,SHADOW_CRASH, true);

          shadow_crash_timer=10000;          
        } else shadow_crash_timer -= diff;

        if (searing_flames_timer <= diff)
        {
            DoCast(m_creature,SEARING_FLAMES);
            searing_flames_timer=21000;
        } else searing_flames_timer -= diff;

        if (surge_of_darkness_timer <= diff)
        {
            DoCast(m_creature, SURGE_OF_DARKNESS, true);
            surge_of_darkness_timer=49000;
        } else surge_of_darkness_timer -= diff;

        if (mark_of_the_faceless <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCast(target,MARK_OF_THE_FACELESS, true);

            mark_of_the_faceless=15500;
        } else mark_of_the_faceless -= diff;

        if(EnrageTimer < diff)
        {
            DoCast(m_creature, SP_ENRAGE,true);
            EnrageTimer = 30000;
        }
        else EnrageTimer -= diff;

        if (m_creature->GetHealth() < (m_creature->GetMaxHealth()/100*35) && !summon)       
        {
            summon = true;
            m_creature->SummonCreature(33524, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(),m_creature->GetAngle(m_creature),TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000);
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vezax(Creature* pCreature)
{
    return new boss_vezaxAI(pCreature);
}
struct MANGOS_DLL_DECL mob_saronite_vaporAI : public ScriptedAI
{
    mob_saronite_vaporAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {

    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        
    }

    void AttackStart(Unit* who)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }

    void JustDied(Unit*)
    {
        DoCast(m_creature,63323, true);
    }
};

CreatureAI* GetAI_mob_saronite_vapor(Creature* pCreature)
{
    return new mob_saronite_vaporAI(pCreature);
}

void AddSC_boss_vezax()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_vezax";
    newscript->GetAI = &GetAI_boss_vezax;
    newscript->RegisterSelf();

 
    newscript = new Script;
    newscript->Name = "mob_saronite_vapor";
    newscript->GetAI = &GetAI_mob_saronite_vapor;
    newscript->RegisterSelf();
}