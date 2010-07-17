
#include "precompiled.h"
#include "ulduar.h"

struct MANGOS_DLL_DECL boss_thorimAI : public ScriptedAI
{
    boss_thorimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *pInstance;

    void Reset()
    {
        if(pInstance && !m_creature->isAlive())
            pInstance->SetData(TYPE_THORIM, NOT_STARTED);
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_THORIM, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance)
        {
            pInstance->SetData(TYPE_THORIM, DONE);

            GameObject* pGo = GetClosestGameObjectWithEntry(m_creature,m_bIsRegularMode ? 194312 : 194314,200.0f);
            if (pGo)
                pInstance->DoRespawnGameObject(pGo->GetGUID(),604800);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thorim(Creature* pCreature)
{
    return new boss_thorimAI(pCreature);
}

void AddSC_boss_thorim()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_thorim";
    newscript->GetAI = &GetAI_boss_thorim;
    newscript->RegisterSelf();
}
