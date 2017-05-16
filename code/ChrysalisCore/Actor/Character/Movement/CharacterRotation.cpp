#include <StdAfx.h>

//#include <Player/Player.h>
//#include <Actor/Character/Character.h>
//#include <Actor/Character/Movement/CharacterRotation.h>
//#include <Actor/Character/Movement/CharacterMovementController.h>
//#include <Utility/CryWatch.h>
//#include <IItemSystem.h>
//#include <Actor/ActorState.h>
//#include <IVehicleSystem.h>
//#include <CryAction.h>
//#include <CryActionPhysicQueues.h>
//
////#include <GameCVars.h>
////#include <GameRules.h>
//
////#include "PlayerVisTable.h"
////#include "AutoAimManager.h"
////#include <Weapon.h>
//
//
//#if !defined(_RELEASE)
//#define ENABLE_NAN_CHECK
//#endif
//
//
//#ifdef ENABLE_NAN_CHECK
//#define PR_CHECKQNAN_FLT(x) \
//	assert(((*(unsigned*)&(x))&0xff000000) != 0xff000000u && (*(unsigned*)&(x) != 0x7fc00000))
//#else
//#define PR_CHECKQNAN_FLT(x) (void*)0
//#endif
//
//
//#define PR_CHECKQNAN_VEC(v) \
//	 PR_CHECKQNAN_FLT(v.x); PR_CHECKQNAN_FLT(v.y); PR_CHECKQNAN_FLT(v.z)
//
//
//#define PR_CHECKQNAN_QUAT(q) \
//	PR_CHECKQNAN_VEC(q.v); \
//	PR_CHECKQNAN_FLT(q.w)
//
//
//CCharacterRotation::CCharacterRotation(const CCharacter& character) :
//	m_character(character),
//	m_baseQuat(IDENTITY),
//	m_deltaAngles(ZERO),
//	m_viewAngles(ZERO),
//	m_externalAngles(ZERO),
//	m_viewRoll(0.0f)
//	//m_peekOverAmount(0.0f),
//	//m_angularImpulseTime(0.0f),
//	//m_angularImpulseDeceleration(0.0f),
//	//m_angularImpulseTimeMax(0.0f),
//	//m_leanAmount(0.0f),
//	//m_angularImpulse(ZERO),
//	//m_angularImpulseDelta(ZERO),
//	//m_viewQuat(IDENTITY),
//	//m_viewQuatFinal(IDENTITY),
//	//m_baseQuatLinked(IDENTITY),
//	//m_viewQuatLinked(IDENTITY),
//	//m_follow_target_id(0),
//	//m_forceLookVector(ZERO),
//	//m_bForcedLookAtBlendingEnabled(true),
//	//m_minAngleTarget(std::numeric_limits<float>::infinity()),
//	//m_minAngleRate(0.0f),
//	//m_minAngle(0.0f),
//	//m_maxAngleTarget(0.0f),
//	//m_maxAngleRate(0.0f),
//	//m_maxAngle(0.0f),
//	//m_follow_target_dir(0, 1, 0),
//	//m_frameViewAnglesOffset(0.0f, 0.0f, 0.0f)
//{
//}
//
//
//void CCharacterRotation::Process(const SActorMovementRequest& movementRequest, float frameTime)
//{
//	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);
//
//	// Add back when we start to get angular impulse sorted.
//	//// Reset to the new impulse.
//	//m_angularImpulseDelta = m_angularImpulse;
//
//	m_deltaAngles = movementRequest.deltaAngles;
//	PR_CHECKQNAN_FLT(m_deltaAngles);
//
//	// Store the previous rotation to get the correct rotation for linked actors.
//	const Quat previousBaseQuat = m_baseQuat;
//	//const Quat previousViewQuat = m_viewQuat;
//
//	//ProcessForcedLookDirection(m_viewQuatFinal, frameTime);
//	//ProcessAngularImpulses(frameTime);
//	//ProcessLeanAndPeek(movementRequest);
//	//ProcessNormalRoll(frameTime);
//
//	//float minAngle, maxAngle;
//	//GetStanceAngleLimits(verticalAcceleration, pCurrentItem, minAngle, maxAngle);
//	//ClampAngles(minAngle, maxAngle);
//
//	ProcessNormal(frameTime);
//
//	//// Update freelook when linked to an entity.
//	//ProcessLinkedState(m_character.m_linkStats, previousBaseQuat, previousViewQuat);
//
//	//// Recoil / Zoom sway offset for local player.
//	//ProcessFinalViewEffects(minAngle, maxAngle);
//
//	//m_frameViewAnglesOffset.Set(0.0f, 0.0f, 0.0f);
//	//m_forceLookVector.zero();
//	//m_externalAngles.Set(0.f, 0.f, 0.f);
//
//	// Normalise the quaternions.
//	m_baseQuat.Normalize();
//	PR_CHECKQNAN_QUAT(m_baseQuat);
//	//m_viewQuat.Normalize();
//	//PR_CHECKQNAN_QUAT(m_viewQuat);
//	//m_viewQuatFinal.Normalize();
//}
//
//
//void CCharacterRotation::FullSerialize(TSerialize ser)
//{
//	ser.BeginGroup("CharacterRotation");
//	//ser.Value("viewAngles", m_viewAngles);
//	//ser.Value("leanAmount", m_leanAmount);
//	//ser.Value("viewRoll", m_viewRoll);
//
//	////[AlexMcC|19.03.10]: #TODO: delete these once we stop reviving players on quick load!
//	//// When we don't revive, these are overwritten by a calculation that reads from m_viewAngles,
//	//// which we serialize above.
//	//ser.Value("viewQuat", m_viewQuat);
//	//ser.Value("viewQuatFinal", m_viewQuatFinal);
//	//ser.Value("baseQuat", m_baseQuat);
//	ser.EndGroup();
//}
//
//
//void CCharacterRotation::ProcessNormal(float frameTime)
//{
//	//#ifdef ENABLE_NAN_CHECK
//	//	//create a matrix perpendicular to the ground
//	//	Vec3 up(0, 0, 1);
//	//	//..or perpendicular to the linked object Z
//	//	SLinkStats *pLinkStats = &m_character.m_linkStats;
//	//	if (pLinkStats->linkID && pLinkStats->flags & SLinkStats::LINKED_FREELOOK)
//	//	{
//	//		IEntity *pLinked = pLinkStats->GetLinked();
//	//		if (pLinked)
//	//			up = pLinked->GetRotation().GetColumn2();
//	//	}
//	//
//	//	const Vec3 right(m_baseQuat.GetColumn0());
//	//	const Vec3 forward((up % right).GetNormalized());
//	//
//	//	PR_CHECKQNAN_VEC(up);
//	//	PR_CHECKQNAN_VEC(right);
//	//#endif //ENABLE_NAN_CHECK
//	//
//	//	const Ang3 vNewDeltaAngles = m_deltaAngles * m_character.m_actorState.flashBangStunMult;
//	//
//	//#ifdef PLAYER_MOVEMENT_DEBUG_ENABLED
//	//	m_character.DebugGraph_AddValue("AimDeltaH", vNewDeltaAngles.z);
//	//	m_character.DebugGraph_AddValue("AimDeltaV", vNewDeltaAngles.x);
//	//#endif
//	//
//	//	Ang3 newViewAngles;
//	//	newViewAngles.Set(m_viewAngles.x + vNewDeltaAngles.x, m_viewAngles.y, m_viewAngles.z + vNewDeltaAngles.z);
//	//	newViewAngles += m_externalAngles;
//	//
//	//	// These values need to be used because the player rotation is a quat and quaternions wrap on 720 degrees.
//	//	newViewAngles.z = (float) __fsel(newViewAngles.z - (2.0f * gf_PI2), newViewAngles.z - (4.0f * gf_PI2), newViewAngles.z);
//	//	newViewAngles.z = (float) __fsel(-(newViewAngles.z + (2.0f * gf_PI2)), newViewAngles.z + (4.0f * gf_PI2), newViewAngles.z);
//	//
//	//	m_viewAngles = newViewAngles;
//	//
//	//	if (m_character.CanTurnBody())
//	//	{
//	//		m_baseQuat = Quat::CreateRotationZ(newViewAngles.z);
//	//	}
//	//
//	//	newViewAngles.y += m_viewRoll;
//	//	m_viewQuat.SetRotationXYZ(newViewAngles);
//	//
//	//	m_deltaAngles = vNewDeltaAngles;
//	//
//	//	// #TODO: Add back in when vehicles are ready.
//	//	if (!m_character.GetLinkedVehicle())
//	//	{
//	//	//	// #TODO: We might need new aspects for characters instead of players.
//	//	//	CHANGED_NETWORK_STATE_REF(m_character, CPlayer::ASPECT_INPUT_CLIENT);
//	//	}
//}
//
//
////IItem* CCharacterRotation::GetCurrentItem(bool includeVehicle)
////{
////	IItem * pCurrentItem = m_character.GetCurrentItem();
////	IVehicle* pVehicle = m_character.GetLinkedVehicle();
////
////	if (pVehicle && includeVehicle)
////	{
////		if (EntityId weaponId = pVehicle->GetCurrentWeaponId(m_character.GetEntity()->GetId()))
////		{
////			if (IItem* pItem = gEnv->pGameFramework->GetIItemSystem()->GetItem(weaponId))
////			{
////				if (IWeapon* pWeapon = pItem->GetIWeapon())
////					pCurrentItem = pItem;
////				else
////					return NULL;
////			}
////		}
////	}
////
////	return pCurrentItem;
////}
////
////
////void CCharacterRotation::AddAngularImpulse(const Ang3 &angular, float deceleration, float duration)
////{
////	m_angularImpulse = angular;
////	m_angularImpulseDelta = angular;
////	m_angularImpulseDeceleration = deceleration;
////	m_angularImpulseTimeMax = m_angularImpulseTime = duration;
////}
////
////
////void CCharacterRotation::SetViewRotation(const Quat &rotation)
////{
////	CRY_ASSERT(rotation.IsValid());
////
////	const Quat normalizedRotation = rotation.GetNormalized();
////
////	m_viewQuat = normalizedRotation;
////	m_viewQuatFinal = normalizedRotation;
////	m_viewAngles.SetAnglesXYZ(normalizedRotation);
////	m_baseQuat = Quat::CreateRotationZ(m_viewAngles.z).GetNormalized();
////}
////
////
////void CCharacterRotation::SetViewRotationAndKeepBaseOrientation(const Quat &rotation)
////{
////	CRY_ASSERT(rotation.IsValid());
////
////	const Quat normalizedRotation = rotation.GetNormalized();
////
////	m_viewQuat = normalizedRotation;
////	m_viewQuatFinal = normalizedRotation;
////	m_viewAngles.SetAnglesXYZ(normalizedRotation);
////}
////
////
////void CCharacterRotation::SetViewRotationOnRevive(const Quat &rotation)
////{
////	SetViewRotation(rotation);
////	m_viewAngles.x = 0.0f;
////	m_viewAngles.y = 0.0f;
////	m_viewRoll = 0.0f;
////
////	// Reset angle transitions
////	m_minAngleTarget = std::numeric_limits<float>::infinity();
////}
////
////
////void CCharacterRotation::ResetLinkedRotation(const Quat& rotation)
////{
////	CRY_ASSERT(rotation.IsValid());
////
////	const Quat normalizedRotation = rotation.GetNormalized();
////
////	m_viewQuatLinked = m_baseQuatLinked = normalizedRotation;
////	m_viewQuatFinal = m_viewQuat = m_baseQuat = normalizedRotation;
////}
////
////
////void CCharacterRotation::GetStanceAngleLimits(const SAimAccelerationParams& verticalAcceleration, IItem* pCurrentPlayerItem, float & minAngle, float & maxAngle)
////{
////	float fNewMinAngle = verticalAcceleration.angle_min;
////	float fNewMaxAngle = verticalAcceleration.angle_max;
////
////	//CWeapon* pWeapon = GetCurrentWeapon(pCurrentPlayerItem);
////	//if (pWeapon)
////	//{
////	//	float weaponMinAngle = fNewMinAngle;
////	//	float weaponMaxAngle = fNewMaxAngle;
////	//	pWeapon->GetAngleLimits(m_character.GetStance(), weaponMinAngle, weaponMaxAngle);
////	//	fNewMinAngle = max(fNewMinAngle, weaponMinAngle);
////	//	fNewMaxAngle = min(fNewMaxAngle, weaponMaxAngle);
////	//}
////
////	// Smooth transition for up/down angle limits
////	if (m_minAngleTarget == std::numeric_limits<float>::infinity())
////	{
////		m_maxAngle = m_maxAngleTarget = fNewMaxAngle;
////		m_minAngle = m_minAngleTarget = fNewMinAngle;
////		m_angleTransitionTimer = gEnv->pTimer->GetFrameStartTime();
////		m_minAngleRate = 0;
////		m_maxAngleRate = 0;
////	}
////	if (fNewMaxAngle != m_maxAngleTarget)
////	{
////		m_maxAngle = m_maxAngleTarget;
////		m_maxAngleTarget = fNewMaxAngle;
////		m_angleTransitionTimer = gEnv->pTimer->GetFrameStartTime();
////		m_maxAngleRate = 0;
////	}
////	if (fNewMinAngle != m_minAngleTarget)
////	{
////		m_minAngle = m_minAngleTarget;
////		m_minAngleTarget = fNewMinAngle;
////		m_angleTransitionTimer = gEnv->pTimer->GetFrameStartTime();
////		m_minAngleRate = 0;
////	}
////	const float transitionTime = g_pGameCVars->cl_angleLimitTransitionTime;
////	float timer = gEnv->pTimer->GetFrameStartTime().GetDifferenceInSeconds(m_angleTransitionTimer);
////	if (timer < transitionTime)
////	{
////		if (m_minAngle < m_minAngleTarget)
////		{
////			SmoothCD(m_minAngle, m_minAngleRate, timer, m_minAngleTarget, transitionTime);
////			fNewMinAngle = m_minAngle;
////		}
////		if (m_maxAngle > m_maxAngleTarget)
////		{
////			SmoothCD(m_maxAngle, m_maxAngleRate, timer, m_maxAngleTarget, transitionTime);
////			fNewMaxAngle = m_maxAngle;
////		}
////	}
////
////	minAngle = DEG2RAD(fNewMinAngle);
////	maxAngle = DEG2RAD(fNewMaxAngle);
////}
////
////
////void CCharacterRotation::ProcessNormalRoll(float frameTime)
////{
////	//apply lean/roll
////	float rollAngleGoal(0);
////	const Vec3 velocity = m_character.GetActorPhysics().velocity;
////	const float speed2(velocity.len2());
////
////	if ((speed2 > 0.01f) && m_character.m_actorState.durationInAir)
////	{
////
////		const float maxSpeed = m_character.GetStanceMaxSpeed(STANCE_STAND);
////		const float maxSpeedInverse = (float) __fsel(-maxSpeed, 1.0f, __fres(maxSpeed + FLT_EPSILON));
////
////		const Vec3 velVec = velocity * maxSpeedInverse;
////
////		const float dotSide(m_viewQuat.GetColumn0() * velVec);
////
////		rollAngleGoal -= DEG2RAD(dotSide * 1.5f);;
////	}
////
////	const float tempLean = m_leanAmount;
////	const float leanAmountMultiplier = 3.0f;
////	const float leanAmount = clamp_tpl(tempLean * leanAmountMultiplier, -1.0f, 1.0f);
////
////	rollAngleGoal += DEG2RAD(leanAmount * m_character.m_params.leanAngle);
////	Interpolate(m_viewRoll, rollAngleGoal, 9.9f, frameTime);
////
////	m_deltaAngles += m_angularImpulseDelta;
////}
////
////
////void CCharacterRotation::ProcessAngularImpulses(float frameTime)
////{
////	//update angular impulse
////	if (m_angularImpulseTime > 0.001f)
////	{
////		m_angularImpulse *= min(m_angularImpulseTime * __fres(m_angularImpulseTimeMax), 1.0f);
////		m_angularImpulseTime -= frameTime;
////	}
////	else if (m_angularImpulseDeceleration > 0.001f)
////	{
////		Interpolate(m_angularImpulse, ZERO, m_angularImpulseDeceleration, frameTime);
////	}
////	m_angularImpulseDelta -= m_angularImpulse;
////}
////
////
////void CCharacterRotation::ClampAngles(float minAngle, float maxAngle)
////{
////	//Cap up/down looking
////	{
////		const float currentViewPitch = GetLocalPitch();
////		const float newPitch = clamp_tpl(currentViewPitch + m_deltaAngles.x, minAngle, maxAngle);
////		m_deltaAngles.x = newPitch - currentViewPitch;
////	}
////
////	//Further limit the view if necessary
////	{
////		const SViewLimitParams& viewLimits = m_character.m_params.viewLimits;
////
////		const Vec3  limitDir = viewLimits.GetViewLimitDir();
////
////		if (limitDir.len2() < 0.1f)
////			return;
////
////		const float limitV = viewLimits.GetViewLimitRangeV();
////		const float limitH = viewLimits.GetViewLimitRangeH();
////		const float limitVUp = viewLimits.GetViewLimitRangeVUp();
////		const float limitVDown = viewLimits.GetViewLimitRangeVDown();
////
////		if ((limitH + limitV + fabsf(limitVUp) + fabsf(limitVDown)) > 0.0f)
////		{
////			//A matrix is built around the view limit, and then the player view angles are checked with it.
////			//Later, if necessary the upVector could be made customizable.
////			const Vec3 forward(limitDir);
////			const Vec3 up(m_baseQuat.GetColumn2());
////			const Vec3 right((-(up % forward)).GetNormalized());
////
////			Matrix33 limitMtx;
////			limitMtx.SetFromVectors(right, forward, right%forward);
////			limitMtx.Invert();
////
////			const Vec3 localDir(limitMtx * m_viewQuat.GetColumn1());
////
////			Ang3 limit;
////
////			if (limitV)
////			{
////				limit.x = asinf(localDir.z) + m_deltaAngles.x;
////
////				const float deltaX(limitV - fabs(limit.x));
////
////				m_deltaAngles.x = m_deltaAngles.x + (float) __fsel(deltaX, 0.0f, deltaX * (float) __fsel(limit.x, 1.0f, -1.0f));
////			}
////
////			if (limitVUp || limitVDown)
////			{
////				limit.x = asinf(localDir.z) + m_deltaAngles.x;
////
////				const float deltaXUp(limitVUp - limit.x);
////				float fNewDeltaX = m_deltaAngles.x;
////
////				const float fDeltaXUpIncrement = (float) __fsel(deltaXUp, 0.0f, deltaXUp);
////				fNewDeltaX = fNewDeltaX + (float) __fsel(-fabsf(limitVUp), 0.0f, fDeltaXUpIncrement);
////
////				const float deltaXDown(limitVDown - limit.x);
////
////				const float fDeltaXDownIncrement = (float) __fsel(deltaXDown, deltaXDown, 0.0f);
////				fNewDeltaX = fNewDeltaX + (float) __fsel(-fabsf(limitVDown), 0.0f, fDeltaXDownIncrement);
////
////				m_deltaAngles.x = fNewDeltaX;
////			}
////
////			if (limitH)
////			{
////				limit.z = atan2_tpl(-localDir.x, localDir.y) + m_deltaAngles.z;
////
////				const float deltaZ(limitH - fabs(limit.z));
////
////				m_deltaAngles.z = m_deltaAngles.z + (float) __fsel(deltaZ, 0.0f, deltaZ * (float) __fsel(limit.z, 1.0f, -1.0f));
////			}
////		}
////	}
////}
////
////
////void CCharacterRotation::ProcessLeanAndPeek(const SActorMovementRequest& movementRequest)
////{
////	const float leanAmt = (float) __fsel(0.01f - fabsf(movementRequest.desiredLean), 0.0f, movementRequest.desiredLean);
////
////	m_leanAmount = leanAmt;
////
////	// Check if its possible.
////	if ((leanAmt * leanAmt) < 0.01f)
////	{
////		// Clear any previous result.
////		m_leanAndPeekInfo.Reset();
////	}
////	else
////	{
////		const EStance stance = m_character.GetStance();
////		const float noLean(0.0f);
////		const Vec3 playerPosition = m_character.GetEntity()->GetWorldPos();
////		const Vec3 headPos(playerPosition + m_baseQuat * m_character.GetStanceViewOffset(stance, &noLean));
////		const Vec3 newPos(playerPosition + m_baseQuat * m_character.GetStanceViewOffset(stance, &m_leanAmount));
////
////		/*gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(headPos, 0.04f, ColorB(0,0,255,255) );
////		gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(newPos, 0.04f, ColorB(0,0,255,255) );
////		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(headPos, ColorB(0,0,255,255), newPos, ColorB(0,0,255,255));*/
////
////		const int rayFlags(rwi_stop_at_pierceable | rwi_colltype_any);
////		IPhysicalEntity *pSkip(m_character.GetEntity()->GetPhysics());
////
////		const float distMult(3.0f);
////		const float distMultInv(1.0f / distMult);
////
////		const Vec3& limitPoint = m_leanAndPeekInfo.GetLeanLimit(headPos + m_viewQuat.GetColumn1() * 0.25f,
////			(newPos - headPos)*distMult, ent_terrain | ent_static | ent_rigid | ent_sleeping_rigid, rayFlags, &pSkip, pSkip ? 1 : 0);
////
////		const bool validHit = (!limitPoint.IsZero());
////		if (validHit)
////		{
////			const float dist((headPos - newPos).len2() * distMult);
////			const float invDist = dist > 0.f ? __fres(dist) : 0.f;
////			m_leanAmount *= ((limitPoint - headPos).len2() * invDist * distMultInv);
////
////			//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(hit.pt, 0.04f, ColorB(0,255,0,255) );
////		}
////	}
////
////	// #TODO: Márcio - Maybe do some checks here!
////	m_peekOverAmount = movementRequest.desiredPeekOver;
////}
////
////
//////CWeapon* CCharacterRotation::GetCurrentWeapon(IItem* pCurrentPlayerItem) const
//////{
//////	return pCurrentPlayerItem ? static_cast<CWeapon*>(pCurrentPlayerItem->GetIWeapon()) : NULL;
//////}
////
////
////void CCharacterRotation::ProcessForcedLookDirection(const Quat& lastViewQuat, float frameTime)
////{
////	const float forceLookLenSqr(m_forceLookVector.len2());
////
////	if (forceLookLenSqr < 0.001f)
////		return;
////
////	const float forceLookLen(sqrt_tpl(forceLookLenSqr));
////	Vec3 forceLook(m_forceLookVector);
////	forceLook *= (float) __fres(forceLookLen);
////	forceLook = lastViewQuat.GetInverted() * forceLook;
////
////	const float smoothSpeed(6.6f * forceLookLen);
////
////	float blendAmount = min(1.0f, frameTime*smoothSpeed);
////	if (!m_bForcedLookAtBlendingEnabled)
////	{
////		blendAmount = 1.0f;
////	}
////
////	m_deltaAngles.x += asinf(forceLook.z) * blendAmount;
////	m_deltaAngles.z += atan2_tpl(-forceLook.x, forceLook.y) * blendAmount;
////
////	PR_CHECKQNAN_VEC(m_deltaAngles);
////}
////
////
////void CCharacterRotation::ProcessLinkedState(SLinkStats& linkStats, const Quat& lastBaseQuat, const Quat& lastViewQuat)
////{
////	if (!linkStats.linkID)
////		return;
////
////	IEntity *pLinked = linkStats.GetLinked();
////	if (pLinked)
////	{
////		//at this point m_baseQuat and m_viewQuat contain the previous frame rotation, I'm using them to calculate the delta rotation.
////		m_baseQuatLinked *= lastBaseQuat.GetInverted() * m_baseQuat;
////		m_viewQuatLinked *= lastViewQuat.GetInverted() * m_viewQuat;
////
////		m_baseQuatLinked.Normalize();
////		m_viewQuatLinked.Normalize();
////
////		const Quat qLinkedRotation = pLinked->GetRotation();
////
////		m_baseQuat = qLinkedRotation * m_baseQuatLinked;
////		m_viewQuat = qLinkedRotation * m_viewQuatLinked;
////	}
////}
////
////void CCharacterRotation::ProcessFinalViewEffects(float minAngle, float maxAngle)
////{
////	if (!m_character.IsClient())
////	{
////		m_viewQuatFinal = m_viewQuat;
////	}
////	else
////	{
////		Ang3 viewAngleOffset = m_frameViewAnglesOffset;
////
////		float currentViewPitch = GetLocalPitch();
////		float newPitch = clamp_tpl(currentViewPitch + viewAngleOffset.x, minAngle, maxAngle);
////		viewAngleOffset.x = newPitch - currentViewPitch;
////
////		m_viewQuatFinal = m_viewQuat * Quat::CreateRotationXYZ(viewAngleOffset);
////	}
////}
////
////
////void CCharacterRotation::AddViewAngles(const Ang3 &angles)
////{
////	m_externalAngles += angles;
////}
//
//
//// ***
//// *** Lean and peak handling.
//// ***
//
//
//void CCharacterRotation::SLeanAndPeekInfo::Reset()
//{
//	// No need to let the last raycast complete.
//	CancelPendingRay();
//
//	m_lastLimit = { 0.0f, 0.0f, 0.0f };
//}
//
//
//const Vec3& CCharacterRotation::SLeanAndPeekInfo::GetLeanLimit(const Vec3 &startPos, const Vec3 &dir, int objTypes, int flags, IPhysicalEntity** pSkipEnts, int nSkipEnts)
//{
//	// Only queue ray cast if we're not waiting for another one.
//	if (m_queuedRayID == 0)
//	{
//		m_queuedRayID = CCryAction::GetCryAction()->GetPhysicQueues().GetRayCaster().Queue(
//			RayCastRequest::MediumPriority,
//			RayCastRequest(startPos, dir,
//				objTypes,
//				flags,
//				pSkipEnts,
//				nSkipEnts),
//			functor(*this, &SLeanAndPeekInfo::OnRayCastDataReceived));
//	}
//
//	return m_lastLimit;
//}
//
//
//void CCharacterRotation::SLeanAndPeekInfo::OnRayCastDataReceived(const QueuedRayID& rayID, const RayCastResult& result)
//{
//	CRY_ASSERT(rayID == m_queuedRayID);
//
//	m_queuedRayID = 0;
//
//	if (result.hitCount > 0)
//		m_lastLimit = result.hits [0].pt;
//	else
//		m_lastLimit.Set(0.0f, 0.0f, 0.0f);
//}
//
//
//void CCharacterRotation::SLeanAndPeekInfo::CancelPendingRay()
//{
//	if (m_queuedRayID != 0)
//		CCryAction::GetCryAction()->GetPhysicQueues().GetRayCaster().Cancel(m_queuedRayID);
//	m_queuedRayID = 0;
//}
