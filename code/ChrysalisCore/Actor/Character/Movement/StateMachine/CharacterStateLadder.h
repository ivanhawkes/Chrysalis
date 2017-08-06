#pragma once

#include <Utility/AutoEnum.h>
#include <Actor/Character/Character.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateEvents.h>


struct SActorMovementRequest;
class SmartScriptTable;

namespace Chrysalis
{
class CPlayerComponent;


#define ladderAnimTypeList(func)             \
	func(kLadderAnimType_none)                 \
	func(kLadderAnimType_atTopLeftFoot)        \
	func(kLadderAnimType_atTopRightFoot)       \
	func(kLadderAnimType_atBottom)             \
	func(kLadderAnimType_upLeftFoot)           \
	func(kLadderAnimType_upLoop)               \
	func(kLadderAnimType_upRightFoot)          \
	func(kLadderAnimType_downLeftFoot)         \
	func(kLadderAnimType_downRightFoot)        \
	func(kLadderAnimType_midAirGrab)           \

class CCharacterComponent;
class CLadderAction;


typedef TBitfield TCancelButtonBitfield;

#ifndef _RELEASE
#define DEFINE_CANCEL_HANDLER_NAME(...) string DbgGetCancelHandlerName() const { return string().Format(__VA_ARGS__); }
#else
#define DEFINE_CANCEL_HANDLER_NAME(...)
#endif

struct IPlayerInputCancelHandler
{
public:
	virtual ~IPlayerInputCancelHandler() {}
	virtual bool HandleCancelInput(CPlayerComponent & player, TCancelButtonBitfield cancelButtonPressed) = 0;

#ifndef _RELEASE
	virtual string DbgGetCancelHandlerName() const = 0;
#endif
};



class CCharacterStateLadder : IPlayerInputCancelHandler
{
public:
	CCharacterStateLadder();

	bool OnPrePhysicsUpdate(CCharacterComponent& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(CCharacterComponent& Character);

	void OnUseLadder(CCharacterComponent& Character, IEntity* pLadder);
	void LeaveLadder(CCharacterComponent& Character, ELadderLeaveLocation leaveLocation);
	void SetHeightFrac(CCharacterComponent& Character, float heightFrac);
	void InformLadderAnimEnter(CCharacterComponent & Character, CLadderAction * thisAction);
	void InformLadderAnimIsDone(CCharacterComponent & Character, CLadderAction * thisAction);
	EntityId GetLadderId() { return m_ladderEntityId; };

	static bool IsUsableLadder(CCharacterComponent& Character, IEntity* pLadder, const SmartScriptTable& ladderProperties);

	AUTOENUM_BUILDENUMWITHTYPE_WITHNUM(ELadderAnimType, ladderAnimTypeList, kLadderAnimType_num);

#ifndef _RELEASE
	void UpdateNumActions(int change);
#endif

private:
	bool m_CharacterIsThirdPerson;
	EntityId m_ladderEntityId;
	Vec3 m_ladderBottom;
	float m_offsetFromAnimToRung;
	float m_climbInertia;
	float m_scaleSettle;
	uint32 m_numRungsFromBottomPosition;
	uint32 m_topRungNumber;
	float m_fractionBetweenRungs;
	ELadderAnimType m_playGetOffAnim;
	ELadderAnimType m_playGetOnAnim;
	CLadderAction * m_mostRecentlyEnteredAction;

	void SetClientCharacterOnLadder(IEntity * pLadder, bool onOff);
	void QueueLadderAction(CCharacterComponent& Character, CLadderAction * action);
	void SetMostRecentlyEnteredAction(CLadderAction * thisAction);
	void InterruptCurrentAnimation();

	static void SendLadderFlowgraphEvent(CCharacterComponent & Character, IEntity * pLadderEntity, const char * eventName);

	// ICharacterInputCancelHandler
	DEFINE_CANCEL_HANDLER_NAME("CPlayerStateLadder")
		virtual bool HandleCancelInput(CPlayerComponent & player, TCancelButtonBitfield cancelButtonPressed);
	// ~ICharacterInputCancelHandler

#ifndef _RELEASE
	int m_dbgNumActions;
#endif
};
}