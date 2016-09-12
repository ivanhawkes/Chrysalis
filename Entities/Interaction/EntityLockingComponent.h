/**
\file d:\Chrysalis\Code\ChrysalisSDK\GameObjects\EntityLocking.h

Declares the entity locking class. Refactored from CryEngine\CryAction\GameObjects\Interactor.h

This utilises an EntityAwareness extension to perform the core work of detecting nearby entities and raycasting.
**/
#pragma once

#include <IGameObject.h>
#include <Entities/Interaction/IEntityLockingComponent.h>


struct IEntityAwarenessComponent;
struct IEntitySystem;


class CEntityLockingComponent : public CGameObjectExtensionHelper<CEntityLockingComponent, IEntityLockingComponent>
{
public:
	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const override;
	void PostInit(IGameObject * pGameObject) override;
	void FullSerialize(TSerialize ser) override;
	void PostSerialize() override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;


	// ***
	// *** IEntityLockingComponent
	// ***

	virtual bool IsUsable(EntityId entityId) const;
	virtual bool IsLocked() const { return m_lockEntityId != 0; };
	virtual int GetLockedEntityId() const { return m_lockEntityId; };
	virtual void SetQueryMethods(char* pMethods) { m_queryMethods = pMethods; };
	virtual int GetOverEntityId() const { return m_overId; };


	// ***
	// *** CEntityLockingComponent
	// ***

public:

	CEntityLockingComponent() {};
	virtual ~CEntityLockingComponent();

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

private:

	// Bitset representing the different ways an entity might be used.
	enum EEntityUseFlags
	{
		// This is not useable.
		NotUseable = 0,

		// Generic 'use' available (Use). Examples might be a vista, teleport, switches, door, etc.
		Useable = BIT(0),

		// Is able to be picked up (Pick Up).
		Pickable = BIT(1),
		
		// Is able to be looted / auto-looted / area looted (Loot).
		Lootable = BIT(2),
		
		// Is able to perform a trade (Trade).
		Tradeable = BIT(3),
		
		// Is able to work as a container (Open).
		Container = BIT(4),
		
		// Is a resource that can be collected (Collect).
		Resource = BIT(5),
	};


	struct SQueryResult
	{
		EntityId entityId { INVALID_ENTITYID };

		// Flag to indicate possible modes of use, if any.
		int useFlags { NotUseable };
		
		// Position of ray-trace hit, if applicable.
		Vec3 hitPosition { 0, 0, 0 };

		// Distance from the actor.
		float hitDistance { 0.0f };
	};


	typedef std::pair<IEntity*, SQueryResult> TQueryElement;
	typedef std::vector<TQueryElement> TQueryVector;
	typedef std::map<IEntityClass*, bool>	TUsableClassesMap;

	/** Used for performing the low-level queries for entities in proxmity to the actor. */
	IEntityAwarenessComponent * m_pEntityAwareness = nullptr;

	float m_useHoverTime = 0.05f;
	float m_unUseHoverTime = 0.2f;
	float m_messageHoverTime = 0.05f;
	float m_longHoverTime = 5.0f;

	ITimer * m_pTimer = gEnv->pTimer;
	IEntitySystem * m_pEntitySystem = gEnv->pEntitySystem;

	string m_queryMethods;

	//SmartScriptTable m_pGameRules = 0;
	//SmartScriptTable m_areUsableEntityList = 0;

	//HSCRIPTFUNCTION m_funcIsUsable = 0;
	//HSCRIPTFUNCTION m_funcOnNewUsable = 0;
	//HSCRIPTFUNCTION m_funcOnUsableMessage = 0;
	//HSCRIPTFUNCTION m_funcOnLongHover = 0;
	//HSCRIPTFUNCTION m_funcAreUsable = 0;

	//static ScriptAnyValue EntityIdToScript(EntityId id);

	TQueryVector m_frameQueryVector;
	TUsableClassesMap m_usableEntityClasses;

	EntityId m_lockedByEntityId = INVALID_ENTITYID;
	EntityId m_lockEntityId = INVALID_ENTITYID;
	int m_lockIdx = 0;

	EntityId m_nextOverId = INVALID_ENTITYID;
	int m_nextOverIdx = -100;
	CTimeValue m_nextOverTime = 0.0f;

	EntityId m_overId = INVALID_ENTITYID;
	int m_overIdx = -100;
	CTimeValue m_overTime = 0.0f;
	bool m_sentMessageHover = false;
	bool m_sentLongHover = false;

	float m_lastRadius = 10.0f;


	/**
	Determines which queries to run and then executes them.
	
	\param [in,out]	entityId The identifier for the entity most likely to be of interest to the actor.
	\param [in,out]	useFlags The use flags.
	**/
	void PerformQueries(EntityId& entityId, int& useFlags);

	bool PerformRayCastQuery(SQueryResult& result);
	bool PerformViewCenterQuery(SQueryResult& result);
	
	// TODO: This would give poor results in cases where entities that are not useable are closest to the centre.
	bool PerformDotFilteredProximityQuery(SQueryResult& result, float minDot);
	
	
	bool PerformMergedQuery(SQueryResult& result, float minDot);
	bool PerformUsableTestAndCompleteIds(IEntity * pEntity, SQueryResult& result) const;
	int PerformUsableTest(IEntity * pEntity) const;
	void PerformUsableTestOnEntities(TQueryVector& query);
	void UpdateTimers(EntityId entityId, int useFlags);
	float LinePointDistanceSqr(const Line& line, const Vec3& point);
	bool IsEntityUsable(const IEntity* pEntity);
};