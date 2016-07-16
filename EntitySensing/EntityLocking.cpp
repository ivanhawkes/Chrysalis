#include <StdAfx.h>

#include "EntityLocking.h"
#include "EntityAwareness.h"
#include <IActorSystem.h>
#include <IGameRulesSystem.h>
#include <CryRenderer/IRenderAuxGeom.h>


//static const float ProximityRadius = 2.0f;

// Magic number to identify which items are pickable.
// TODO: Re-implement.
const int isPickableMagicNumber = 725725;


// ***
// *** IGameObjectExtension
// ***

void CEntityLocking::GetMemoryUsage(ICrySizer * pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_queryMethods);
	pSizer->AddObject(m_pEntityAwareness);
}


bool CEntityLocking::Init(IGameObject * pGameObject)
{
	SetGameObject(pGameObject);

	// Aquire any needed extensions, if we don't have them already.
	if (!m_pEntityAwareness)
	{
		m_pEntityAwareness = static_cast <IEntityAwareness*> (GetGameObject()->AcquireExtension("EntityAwareness"));
		if (!m_pEntityAwareness)
			return false;
	}

	//m_pEntityAwareness->SetProximityRadius(ProximityRadius);

	// TODO: is there a better place we can store these useable queries? They are on GameRules so each game type can have
	// different rules, but does that apply to us, or in fact any useful situation?
	m_pGameRules = gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->GetCurrentGameRulesEntity()->GetScriptTable();
	m_pGameRules->GetValue("IsUsable", m_funcIsUsable);
	m_pGameRules->GetValue("AreUsable", m_funcAreUsable);
	m_pGameRules->GetValue("OnNewUsable", m_funcOnNewUsable);
	m_pGameRules->GetValue("OnUsableMessage", m_funcOnUsableMessage);
	m_pGameRules->GetValue("OnLongHover", m_funcOnLongHover);

	m_areUsableEntityList.Create(gEnv->pScriptSystem);

	// Should be more than enough for average situation.
	m_frameQueryVector.reserve(16);

	//m is optimized for Crysis2, "rb" is Crysis1 compatible
	//m_queryMethods = (m_funcAreUsable) ? "m" : "rb";

	// TEST: Allow me to test each method as required.
	m_queryMethods = "rb";

	return true;
}


void CEntityLocking::PostInit(IGameObject * pGameObject)
{
	pGameObject->EnableUpdateSlot(this, 0);
}


bool CEntityLocking::ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params)
{
	ResetGameObject();

	CRY_ASSERT_MESSAGE(false, "CEntityLocking::ReloadExtension not implemented");

	return false;
}


void CEntityLocking::Release()
{
	delete this;
}


void CEntityLocking::FullSerialize(TSerialize ser)
{
	ser.Value("useHoverTime", m_useHoverTime);
	ser.Value("unUseHoverTime", m_unUseHoverTime);
	ser.Value("messageHoverTime", m_messageHoverTime);
	ser.Value("longHoverTime", m_longHoverTime);
	ser.Value("queryMethods", m_queryMethods);
	ser.Value("lastRadius", m_lastRadius);

	if (ser.GetSerializationTarget() == eST_Script && ser.IsReading())
	{
		EntityId lockedById = INVALID_ENTITYID, lockId = INVALID_ENTITYID;
		int lockIdx = 0;
		ser.Value("locker", lockedById);
		if (lockedById)
		{
			if (m_lockedByEntityId && lockedById != m_lockedByEntityId)
			{
				GameWarning("Attempt to change lock status by an entity that did not lock us originally");
			}
			else
			{
				ser.Value("lockId", lockId);
				ser.Value("lockIdx", lockIdx);
				if (lockId && lockIdx)
				{
					m_lockedByEntityId = lockedById;
					m_lockEntityId = lockId;
					m_lockIdx = lockIdx;
				}
				else
				{
					m_lockedByEntityId = INVALID_ENTITYID;
					m_lockEntityId = INVALID_ENTITYID;
					m_lockIdx = 0;
				}
			}
		}
	}

	// the following should not be used as parameters
	if (ser.GetSerializationTarget() != eST_Script)
	{
		ser.Value("nextOverId", m_nextOverId);
		ser.Value("nextOverTime", m_nextOverTime);
		ser.Value("overId", m_overId);
		ser.Value("overTime", m_overTime);
		ser.Value("sentMessageHover", m_sentMessageHover);
		ser.Value("sentLongHover", m_sentLongHover);

		//don't serialize lockEntity, it will lock on usage
		if (ser.IsReading())
		{
			m_lockedByEntityId = INVALID_ENTITYID;
			m_lockEntityId = INVALID_ENTITYID;
			m_lockIdx = 0;
		}
	}
}


bool CEntityLocking::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags)
{
	return true;
}


void CEntityLocking::PostSerialize()
{
	if (m_funcOnNewUsable)
		Script::CallMethod(m_pGameRules, m_funcOnNewUsable, EntityIdToScript(GetEntityId()), EntityIdToScript(m_overId));
}


bool CEntityLocking::GetEntityPoolSignature(TSerialize signature)
{
	CRY_ASSERT_MESSAGE(false, "CEntityLocking::GetEntityPoolSignature not implemented");

	return true;
}


void CEntityLocking::Update(SEntityUpdateContext&, int)
{
	FUNCTION_PROFILER(GetISystem(), PROFILE_ACTION);

	EntityId newOverId = INVALID_ENTITYID;
	int useFlags = 0;

	if (m_lockedByEntityId)
	{
		newOverId = m_lockEntityId;
		useFlags = m_lockIdx;
	}
	else
	{
		PerformQueries(newOverId, useFlags);
	}

	UpdateTimers(newOverId, useFlags);
}


// ***
// *** IInteractor
// ***

bool CEntityLocking::IsUsable(EntityId entityId) const
{
	IEntity *pEntity = m_pEntitySystem->GetEntity(entityId);

	static SQueryResult dummy;
	return PerformUsableTestAndCompleteIds(pEntity, dummy);
}


// ***
// *** CEntityLocking
// ***


CEntityLocking::~CEntityLocking()
{
	if (m_pEntityAwareness)
		GetGameObject()->ReleaseExtension("EntityAwareness");

	gEnv->pScriptSystem->ReleaseFunc(m_funcIsUsable);
	gEnv->pScriptSystem->ReleaseFunc(m_funcAreUsable);
	gEnv->pScriptSystem->ReleaseFunc(m_funcOnNewUsable);
	gEnv->pScriptSystem->ReleaseFunc(m_funcOnUsableMessage);
	gEnv->pScriptSystem->ReleaseFunc(m_funcOnLongHover);
}


ScriptAnyValue CEntityLocking::EntityIdToScript(EntityId id)
{
	if (id)
	{
		ScriptHandle hdl;
		hdl.n = id;
		return ScriptAnyValue(hdl);
	}
	else
	{
		return ScriptAnyValue();
	}
}


void CEntityLocking::PerformQueries(EntityId& entityId, int& useFlags)
{
	SQueryResult bestResult;
	bestResult.hitDistance = FLT_MAX;

	for (string::const_iterator iter = m_queryMethods.begin(); iter != m_queryMethods.end(); ++iter)
	{
		bool found;
		SQueryResult result;
		switch (*iter)
		{
			case 'r':
				found = PerformRayCastQuery(result);
				break;
			case 'b':
				found = PerformViewCenterQuery(result);
				break;
			case 'd':
				found = PerformDotFilteredProximityQuery(result, 0.8f);
				break;
			case 'm':
				found = PerformMergedQuery(result, 0.8f);
				break;
			default:
				GameWarning("Interactor: Invalid query mode '%c'", *iter);
				found = false;
		}

		if (found && result.hitDistance < bestResult.hitDistance)
		{
			if (bestResult.entityId && !result.entityId)
				;
			else
				bestResult = result;
		}
	}

	entityId = bestResult.entityId;
	useFlags = bestResult.useFlags;
}


bool CEntityLocking::PerformDotFilteredProximityQuery(SQueryResult& result, float minDot)
{
	float minDistanceToCenterSq(FLT_MAX);
	IEntity* pNearestEntityToViewCenter(nullptr);
	float maxDot = -1.0f;
	Vec3 viewDirection = m_pEntityAwareness->GetDir();
	Vec3 viewPos = m_pEntityAwareness->GetPos();

	auto entities = m_pEntityAwareness->NearQuery();
	for (auto entityId : entities)
	{
		IEntity *pEntity = m_pEntitySystem->GetEntity(entityId);
		if (!pEntity)
			continue;

		AABB bbox;
		pEntity->GetWorldBounds(bbox);

		Vec3 itemPos = bbox.GetCenter();
		Vec3 diff = itemPos - viewPos;
		Vec3 dirToItem = diff.GetNormalized();
		float dotToItem = dirToItem.dot(viewDirection);

		if (dotToItem > maxDot)
		{
			maxDot = dotToItem;
			pNearestEntityToViewCenter = pEntity;
			result.hitPosition = itemPos;
			result.hitDistance = (itemPos - viewPos).GetLengthSquared();
		}
	}

	if (pNearestEntityToViewCenter && maxDot > minDot)
	{
		result.hitDistance = sqrt_tpl(result.hitDistance);
		PerformUsableTestAndCompleteIds(pNearestEntityToViewCenter, result);
		return true;
	}
	else
	{
		return false;
	}
}


bool CEntityLocking::PerformRayCastQuery(SQueryResult& result)
{
	result.entityId = INVALID_ENTITYID;

	if (const ray_hit * pHit = m_pEntityAwareness->RaycastQuery())
	{
		EntityId targetId = m_pEntityAwareness->GetLookAtEntityId();
		IEntity* pTargetEntity = m_pEntitySystem->GetEntity(targetId);
		if (pTargetEntity)
		{
			result.hitPosition = pHit->pt;
			result.hitDistance = pHit->dist;

			return PerformUsableTestAndCompleteIds(pTargetEntity, result);
		}
	}

	return false;
}


bool CEntityLocking::PerformViewCenterQuery(SQueryResult& result)
{
	float minDistanceToCenterSq(FLT_MAX);
	const Vec3 viewDirection = m_pEntityAwareness->GetDir() * m_lastRadius;
	const Vec3 viewPos = m_pEntityAwareness->GetPos();
	Line viewLine(viewPos, viewDirection);

	// DEBUG: get the lineseg to show up.	
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(viewLine.pointonline, ColorB(0, 64, 0), viewLine.pointonline + viewDirection, ColorB(0, 128, 0));
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(viewLine.pointonline, 0.05f, ColorB(64, 64, 64), false);
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(viewLine.pointonline + viewDirection, 0.05f, ColorB(0, 128, 0), false);

	IEntity* pNearestEntityToViewCenter { nullptr };

	auto entities = m_pEntityAwareness->GetEntitiesInFrontOf();
	for (auto entityId : entities)
	{
		IEntity* pEntity = m_pEntitySystem->GetEntity(entityId);
		if (!pEntity)
			continue;

		AABB bbox;
		pEntity->GetWorldBounds(bbox);
		Vec3 itemPos = bbox.GetCenter();

		// DEBUG: May as well see which are being considered.
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, false, ColorB(0, 64, 64, 64), EBoundingBoxDrawStyle::eBBD_Faceted);

		// DEBUG:
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(viewLine.pointonline, ColorB(64, 64, 0, 255), itemPos, ColorB(128, 128, 0, 255));

		float dstSqr = LinePointDistanceSqr(viewLine, itemPos);
		if ((dstSqr < minDistanceToCenterSq))
		{
			result.hitPosition = itemPos;
			result.hitDistance = (itemPos - viewPos).GetLengthSquared();
			minDistanceToCenterSq = dstSqr;
			pNearestEntityToViewCenter = pEntity;

			// DEBUG: Highlight the entity.
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawAABB(bbox, false, ColorB(0, 196, 0, 128), EBoundingBoxDrawStyle::eBBD_Faceted);
		}
	}

	if (pNearestEntityToViewCenter && PerformUsableTestAndCompleteIds(pNearestEntityToViewCenter, result))
	{
		return true;
	}

	return false;
}


bool CEntityLocking::PerformMergedQuery(SQueryResult& result, float minDot)
{
	// Clear before re-using it.
	m_frameQueryVector.clear();

	// Check if the owner can interact first.
	IActor* pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(GetEntityId());
	bool canUserInteract = pActor ? (!pActor->IsDead()) && (pActor->GetSpectatorMode() == 0) : false;
	if (!canUserInteract)
		return false;

	// Perform a raycast query, and if it's positive we will use that as the result and early out.
	if (PerformRayCastQuery(result))
		return true;

	// Perform usable test on all entities at once.
	PerformUsableTestOnEntities(m_frameQueryVector);

	float minDistanceToCenterSqr(FLT_MAX);
	float bestDotResult = minDot;

	IEntity* pNearestDotEntity(nullptr);
	IEntity* pNearestBoxEntity(nullptr);
	SQueryResult bestBoxResult;

	const Vec3 viewDirection = m_pEntityAwareness->GetDir() * m_lastRadius;
	const Vec3 viewDirectionNormalized = viewDirection.GetNormalized();
	const Vec3 viewPos = m_pEntityAwareness->GetPos();

	for (auto queryIt : m_frameQueryVector)
	{
		IEntity* pEntity = queryIt.first;
		SQueryResult& currentElement = queryIt.second;
		if (currentElement.useFlags)
		{
			AABB bbox;
			pEntity->GetWorldBounds(bbox);

			const Vec3 entityCenterPos = !bbox.IsEmpty() ? bbox.GetCenter() : pEntity->GetWorldPos();

			const Vec3 dirToEntity = (entityCenterPos - viewPos).GetNormalizedSafe();
			const Line viewLine(viewPos, viewDirection);
			const float linePointDistanceSqr = LinePointDistanceSqr(viewLine, entityCenterPos);

			if (linePointDistanceSqr >= minDistanceToCenterSqr)
				continue;

			const float dotToEntity = viewDirectionNormalized.Dot(dirToEntity);

			// TODO: Improve this simple way of detecting if the entity is an item
			bool isPickableItem = (currentElement.useFlags == isPickableMagicNumber);

			if (dotToEntity > bestDotResult)
			{
				bestDotResult = dotToEntity;
				result.entityId = currentElement.entityId;
				result.useFlags = currentElement.useFlags;
				result.hitPosition = entityCenterPos;
				result.hitDistance = (entityCenterPos - viewPos).GetLength();
				minDistanceToCenterSqr = linePointDistanceSqr;
				pNearestDotEntity = pEntity;
			}
			else if (pNearestDotEntity == nullptr && !bbox.IsEmpty())
			{

				// If fails, bbox overlap.
				const Lineseg viewLineSeg(viewPos, viewPos + viewDirection);
				if (Overlap::Lineseg_AABB(viewLineSeg, bbox) || isPickableItem)
				{
					bestBoxResult.entityId = currentElement.entityId;
					bestBoxResult.useFlags = currentElement.useFlags;
					bestBoxResult.hitPosition = entityCenterPos;
					bestBoxResult.hitDistance = (entityCenterPos - viewPos).GetLength();
					minDistanceToCenterSqr = linePointDistanceSqr;
					pNearestBoxEntity = pEntity;
				}
			}
		}
	}

	if (pNearestBoxEntity || pNearestDotEntity)
	{
		//	CryLog ("[INTERACTION] Dot winner = '%s', box winner = '%s'!", pNearestDotEntity ? pNearestDotEntity->GetClass()->GetName() : "N/A", pNearestBoxEntity ? pNearestBoxEntity->GetClass()->GetName() : "N/A");
		if (pNearestDotEntity == nullptr)
		{
			pNearestDotEntity = pNearestBoxEntity;
			result = bestBoxResult;
		}

		return true;
	}

	return false;
}


float CEntityLocking::LinePointDistanceSqr(const Line& line, const Vec3& point)
{
	Vec3 x0 = point;
	Vec3 x1 = line.pointonline;
	Vec3 x2 = line.pointonline + line.direction;

	return ((x2 - x1).Cross(x1 - x0)).GetLengthSquared() / (x2 - x1).GetLengthSquared();
}


int CEntityLocking::PerformUsableTest(IEntity * pEntity) const
{
	if (pEntity && m_funcIsUsable)
	{
		SmartScriptTable pScriptTable = pEntity->GetScriptTable();
		if (pScriptTable.GetPtr())
		{
			int useFlags;

			bool scriptOk = Script::CallReturn(
				m_pGameRules->GetScriptSystem(),
				m_funcIsUsable,
				m_pGameRules,
				EntityIdToScript(GetEntityId()),
				EntityIdToScript(pEntity->GetId()),
				useFlags);

			// If the script ran and returned a non-zero result, it will be the flags indicating acceptable uses.
			if (scriptOk && useFlags)
				return useFlags;
		}
	}

	return NotUseable;
}


bool CEntityLocking::PerformUsableTestAndCompleteIds(IEntity * pEntity, SQueryResult& result) const
{
	bool ok = false;

	result.entityId = INVALID_ENTITYID;
	result.useFlags = NotUseable;

	if (int useFlags = PerformUsableTest(pEntity))
	{
		result.entityId = pEntity->GetId();
		result.useFlags = useFlags;
		ok = true;
	}

	return ok;
}


void CEntityLocking::PerformUsableTestOnEntities(TQueryVector& queryList)
{
	// Nothing to do here.
	if (queryList.empty() || !m_funcAreUsable)
		return;

	// Create a list of the scripts which can all be processed in one call. We are re-using the table in the hopes it's faster,
	// or causes less memory thrash but that's really untested.
	m_areUsableEntityList->Clear();
	for (auto element : queryList)
	{
		CRY_ASSERT(element.first);
		CRY_ASSERT(element.first->GetScriptTable());

		m_areUsableEntityList->PushBack(element.first->GetScriptTable());
	}

	// Process all the scripts.
	SmartScriptTable scriptResults;
	bool scriptOk = Script::CallReturn(m_pGameRules->GetScriptSystem(), m_funcAreUsable, m_pGameRules, GetEntity()->GetScriptTable(), m_areUsableEntityList, scriptResults);

	// Pull the results back out.
	if (scriptOk && scriptResults)
	{
		const int resultCount = scriptResults->Count();
		const int queryCount = queryList.size();

		CRY_ASSERT(queryCount <= resultCount);

		if (queryCount <= resultCount)
		{
			for (int i = 0; i < queryCount; ++i)
				scriptResults->GetAt((i + 1), queryList [i].second.useFlags);
		}
	}
}


void CEntityLocking::UpdateTimers(EntityId newOverId, int useFlags)
{
	CTimeValue now = m_pTimer->GetFrameStartTime();

	if (newOverId != m_nextOverId)
	{
		m_nextOverId = newOverId;
		m_nextOverTime = now;
	}

	float compareTime = m_nextOverId ? m_useHoverTime : m_unUseHoverTime;
	if ((m_nextOverId != m_overId) && (now - m_nextOverTime).GetSeconds() > compareTime)
	{
		m_overId = m_nextOverId;
		m_overTime = m_nextOverTime;
		m_sentMessageHover = m_sentLongHover = false;
		if (m_funcOnNewUsable)
			Script::CallMethod(m_pGameRules, m_funcOnNewUsable, EntityIdToScript(GetEntityId()), EntityIdToScript(m_overId));
	}

	if (m_funcOnUsableMessage && !m_sentMessageHover && (now - m_overTime).GetSeconds() > m_messageHoverTime)
	{
		Script::CallMethod(m_pGameRules, m_funcOnUsableMessage, EntityIdToScript(GetEntityId()), EntityIdToScript(m_overId), m_overId);

		m_sentMessageHover = true;
	}

	if (m_funcOnLongHover && !m_sentLongHover && (now - m_overTime).GetSeconds() > m_longHoverTime)
	{
		Script::CallMethod(m_pGameRules, m_funcOnLongHover, EntityIdToScript(GetEntityId()), EntityIdToScript(m_overId));
		m_sentLongHover = true;
	}
}


bool CEntityLocking::IsEntityUsable(const IEntity* pEntity)
{
	CRY_ASSERT(pEntity);

	IEntityClass* pEntityClass = pEntity->GetClass();
	TUsableClassesMap::const_iterator cit = m_usableEntityClasses.find(pEntityClass);
	if (cit != m_usableEntityClasses.end())
	{
		return (cit->second && pEntity->GetScriptTable() && !pEntity->IsHidden() && !pEntity->IsInvisible());
	}
	else
	{
		bool hasIsUsableMethod = false;

		if (IScriptTable* pEntityScript = pEntity->GetScriptTable())
		{
			hasIsUsableMethod = pEntityScript->HaveValue("IsUsable");
		}

		m_usableEntityClasses.insert(TUsableClassesMap::value_type(pEntityClass, hasIsUsableMethod));

		return hasIsUsableMethod && !pEntity->IsHidden() && !pEntity->IsInvisible();
	}
}
