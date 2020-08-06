#pragma once

#include <entt/entt.hpp>
#include <ECS/Components/RenderLight.h>

#include <CryEntitySystem/IEntity.h>
#include <CryParticleSystem\IParticles.h>


namespace Chrysalis
{
// #TODO: Dig into the code to find out what this does.
enum eGeometrySlot
{
	eIGS_FirstPerson = 0,		// First person character.
	eIGS_ThirdPerson,			// Third person character.
	eIGS_Owner,					// Mostly used in item code. Appears in weapon code.
	eIGS_OwnerAnimGraph,		// Deprecated (probably).
	eIGS_OwnerAnimGraphLooped,	// Deprecated (probably).
	eIGS_Aux0,					// Gun turrets and items.
	eIGS_Destroyed,				// Gun turret and item resource.
	eIGS_Aux1,					// Gun turret, item, laser.
	eIGS_ThirdPersonAux,		// Item, JAW, laser.
	eIGS_Last,					// End of reserved slot list.
};


typedef uint32 TAttachedEffectId;
const TAttachedEffectId EFFECTID_INVALID = 0;

/** An simple struct to help pass around parameters required to spawn a new effect. */
struct SEffectSpawnParams
{
	SEffectSpawnParams(const Vec3& _position = Vec3(ZERO), const Vec3& _direction = FORWARD_DIRECTION, 
		const float _scale = 1.0f, const float _speed = -1.0, const bool _isPrime = false)
		: position(_position), direction(_direction), scale(_scale), speed(_speed), isPrime(_isPrime)
	{
	}

	Vec3 position = ZERO;
	Vec3 direction = FORWARD_DIRECTION;
	float scale = 1.0f;
	bool isPrime = false;
	float speed = -1.0f;
};


/** A convenience struct used when attaching effects. It provides for translations to the position,
setting of 'isPrime' and the first safe slot. */
struct SEffectAttachParams
{
	SEffectAttachParams(const Vec3& _offset = Vec3(ZERO), const Vec3& _direction = FORWARD_DIRECTION,
		const float _scale = 1.0f, const bool _isPrime = false, const int _firstSafeSlot = -1)
		: offset(_offset), direction(_direction), scale(_scale), isPrime(_isPrime), firstSafeSlot(_firstSafeSlot)
	{
	}


	Vec3 offset = ZERO;
	Vec3 direction = FORWARD_DIRECTION;
	float scale = 1.0f;
	bool isPrime = false;
	int firstSafeSlot = -1;
};


// ***
// *** Particle effects.
// ***


IParticleEmitter* SpawnParticleFX(const char* effectName, const SEffectSpawnParams& spawnParams, const char* requester = nullptr);
IParticleEmitter* SpawnParticleFX(IParticleEffect* pParticleEffect, const SEffectSpawnParams& spawnParams);

void SpawnParticleWithEntity(const IEntity* pTargetEntity, const int targetSlot, const char* effectName, const char* helperName, const SEffectSpawnParams& spawnParams);
void SpawnParticleWithEntity(const IEntity* pTargetEntity, const int targetSlot, IParticleEffect* pParticleEffect, const char* helperName, const SEffectSpawnParams& spawnParams);


// ***
// *** An effects controller to control the lifetime of effects.
// ***

class CEffectsController
{
public:
	/** Information about a given effect. */
	struct SEffectInfo
	{
		SEffectInfo()
		{
		}


		bool operator == (const TAttachedEffectId& rhs) const
		{
			return (id == rhs);
		};


		TAttachedEffectId id = 0;
		int entityEffectSlot = -1;
		int characterEffectSlot = -1;
		string helperName;
	};


	CEffectsController();

	void Init(EntityId entityId);
	void FreeAllEffects();

	TAttachedEffectId AttachParticleEffect(IParticleEffect* pParticleEffect, const SEffectAttachParams& attachParams);
	TAttachedEffectId AttachParticleEffect(const char* effectName, const SEffectAttachParams& attachParams);
	TAttachedEffectId AttachParticleEffect(IParticleEffect* pParticleEffect, const int targetSlot, const char* helperName, const SEffectAttachParams& attachParams);
	TAttachedEffectId AttachParticleEffect(const char* effectName, const int targetSlot, const char* helperName, const SEffectAttachParams& attachParams);


	/**
	 Attach a light to an entity.

	 \param	targetSlot   	Target slot.
	 \param	helperName   	Name of the helper.
	 \param	offset		 	The offset.
	 \param	direction	 	The direction.
	 \param	firstSafeSlot	The first safe slot.
	 \param	attachParams 	Options for controlling the attachment.

	 \return	A TAttachedEffectId.
	 */

	TAttachedEffectId AttachLight(const int targetSlot, const char* helperName, Vec3 offset, Vec3 direction, eGeometrySlot firstSafeSlot, const ECS::RenderLight& renderLight);


	/**
	 Detach an effect.

	 \param	effectId	Identifier for the effect.
	 */

	void DetachEffect(const TAttachedEffectId effectId);


	IParticleEmitter* GetEffectEmitter(const TAttachedEffectId effectId) const;
	ILightSource* GetLightSource(const TAttachedEffectId effectId) const;
	void SetEffectWorldTM(const TAttachedEffectId effectId, const Matrix34& effectWorldTM);

	/** Updates the TM for entitiy effects, working from the (new) helper positions. */
	void UpdateEntitySlotEffectLocationsFromHelpers();

	void GetMemoryStatistics(ICrySizer* pSizer) const;

private:
	int FindSafeSlot(int firstSafe);

	EntityId m_ownerEntityId = INVALID_ENTITYID;
	typedef std::vector<SEffectInfo> TAttachedEffects;
	TAttachedEffects m_attachedEffects;
	TAttachedEffectId m_effectGeneratorId;
};
}