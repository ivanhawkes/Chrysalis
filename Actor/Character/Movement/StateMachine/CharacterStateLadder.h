#pragma once

#include <Utility/AutoEnum.h>
#include <Actor/Character/Character.h>
#include <Actor/Player/PlayerInput/IPlayerInput.h>
#include <Actor/Character/Movement/StateMachine/CharacterStateEvents.h>


class CPlayer;


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

class CCharacter;
struct SActorMovementRequest;
class SmartScriptTable;
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
	virtual bool HandleCancelInput(CPlayer & player, TCancelButtonBitfield cancelButtonPressed) = 0;

#ifndef _RELEASE
	virtual string DbgGetCancelHandlerName() const = 0;
#endif
};



class CCharacterStateLadder : IPlayerInputCancelHandler
{
public:
	CCharacterStateLadder();

	bool OnPrePhysicsUpdate(CCharacter& Character, const SActorMovementRequest& movementRequest, float frameTime);
	void OnExit(CCharacter& Character);

	void OnUseLadder(CCharacter& Character, IEntity* pLadder);
	void LeaveLadder(CCharacter& Character, ELadderLeaveLocation leaveLocation);
	void SetHeightFrac(CCharacter& Character, float heightFrac);
	void InformLadderAnimEnter(CCharacter & Character, CLadderAction * thisAction);
	void InformLadderAnimIsDone(CCharacter & Character, CLadderAction * thisAction);
	EntityId GetLadderId() { return m_ladderEntityId; };

	static bool IsUsableLadder(CCharacter& Character, IEntity* pLadder, const SmartScriptTable& ladderProperties);

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
	void QueueLadderAction(CCharacter& Character, CLadderAction * action);
	void SetMostRecentlyEnteredAction(CLadderAction * thisAction);
	void InterruptCurrentAnimation();

	static void SendLadderFlowgraphEvent(CCharacter & Character, IEntity * pLadderEntity, const char * eventName);

	// ICharacterInputCancelHandler
	DEFINE_CANCEL_HANDLER_NAME("CPlayerStateLadder")
		virtual bool HandleCancelInput(CPlayer & player, TCancelButtonBitfield cancelButtonPressed);
	// ~ICharacterInputCancelHandler

#ifndef _RELEASE
	int m_dbgNumActions;
#endif
};
