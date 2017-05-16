#include "StdAfx.h"

#include "ParticleComponent.h"
#include <Cry3DEngine/I3DEngine.h>
#include <CryParticleSystem/IParticlesPfx2.h>
#include <CrySerialization/Decorators/Resources.h>


class CParticleRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CParticleComponent>("ParticleComponent", "Particles", "Particles.bmp");
	}
};

CParticleRegistrator g_particleRegistrator;

CRYREGISTER_CLASS(CParticleComponent);


void CParticleComponent::SetParticleEffectName(cstr effectName)
{
	m_particleEffectPath = effectName;
	OnResetState();
}


void CParticleComponent::OnResetState()
{
	IEntity& entity = *GetEntity();

	// Check if we have to unload first
	if (m_particleSlot != -1)
	{
		entity.FreeSlot(m_particleSlot);
		m_particleSlot = -1;
	}

	// We can skip this if the particle isn't active.
	if (m_bActive)
	{
		if (IParticleEffect* pEffect = gEnv->pParticleManager->FindEffect(m_particleEffectPath, "ParticleEntity"))
		{
			m_particleSlot = entity.LoadParticleEmitter(-1, pEffect);
		}
	}
}


void CParticleComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_bActive, "Active", "Active");
	archive(Serialization::ParticleName(m_particleEffectPath), "ParticleEffect", "ParticleEffect");

	if (archive.isInput())
	{
		OnResetState();
	}
}
