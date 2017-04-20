#pragma once

#include "Helpers/DesignerEntityComponent.h"


class CParticleComponent final : public CDesignerEntityComponent<IParticleEntityComponent>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_CLASS(CParticleComponent, IParticleEntityComponent, "ParticleComponent", 0x05AC803E3BAB4CD1, 0xB487E73D7C074FA4);

	virtual ~CParticleComponent() {}

public:
	// CDesignerEntityComponent
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	virtual void OnResetState() override;
	// ~CDesignerEntityComponent

	// IParticleEntityComponent
	virtual void SetParticleEffectName(cstr effectName) override;
	// ~IParticleEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "ParticleEffect Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

protected:
	int m_particleSlot { -1 };
	bool m_bActive { true };
	string m_particleEffectPath;
};
