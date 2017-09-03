#pragma once

#include <Utility/AutoEnum.h>
#include <Actor/Actor.h>
#include <Actor/Movement/StateMachine/ActorStateEvents.h>


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

class IActorComponent;
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



class CActorStateLadder : IPlayerInputCancelHandler
{
public:
	CActorStateLadder();

	bool OnPrePhysicsUpdate(IActorComponent& actorComponent, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(IActorComponent& actorComponent);

	void OnUseLadder(IActorComponent& actorComponent, IEntity* pLadder);
	void LeaveLadder(IActorComponent& actorComponent, ELadderLeaveLocation leaveLocation);
	void SetHeightFrac(IActorComponent& actorComponent, float heightFrac);
	void InformLadderAnimEnter(IActorComponent& actorComponent, CLadderAction* thisAction);
	void InformLadderAnimIsDone(IActorComponent& actorComponent, CLadderAction* thisAction);
	EntityId GetLadderId() { return m_ladderEntityId; };

	static bool IsUsableLadder(IActorComponent& actorComponent, IEntity* pLadder, const SmartScriptTable& ladderProperties);

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
	void QueueLadderAction(IActorComponent& actorComponent, CLadderAction * action);
	void SetMostRecentlyEnteredAction(CLadderAction * thisAction);
	void InterruptCurrentAnimation();

	static void SendLadderFlowgraphEvent(IActorComponent& actorComponent, IEntity * pLadderEntity, const char * eventName);

	// ICharacterInputCancelHandler
	DEFINE_CANCEL_HANDLER_NAME("CPlayerStateLadder")
		virtual bool HandleCancelInput(CPlayerComponent & player, TCancelButtonBitfield cancelButtonPressed);
	// ~ICharacterInputCancelHandler

#ifndef _RELEASE
	int m_dbgNumActions;
#endif
};
}