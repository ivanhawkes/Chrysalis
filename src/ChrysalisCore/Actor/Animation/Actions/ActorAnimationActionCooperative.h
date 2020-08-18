#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
struct IActor;


class CActorAnimationActionCooperative : public CAnimationAction, public IEntityEventListener
{
public:

	DEFINE_ACTION("Cooperative")

		explicit CActorAnimationActionCooperative(IActor &sourceActor,
			IActorAnimationControl* pActorAnimationControl,
			EntityId targetEntityId,
			FragmentID fragmentID, TagState tagState, TagID targetTagID,
			const std::vector<string>& tags,
			const IAnimationDatabase* pTargetOptionalDatabase = nullptr);
	~CActorAnimationActionCooperative();


	void AddEventListener(IAnimationEventListener* pListener)
	{
		stl::push_back_unique(m_listeners, pListener);
	}

	void RemoveEventListener(IAnimationEventListener* pListener)
	{
		m_listeners.remove(pListener);
	}

protected:
	// IAction
	void OnInitialise() override;
	void Enter() override;
	void Fail(EActionFailure actionFailure) override;
	void Exit() override;
	IAction::EStatus UpdatePending(float timePassed) override;
	IAction::EStatus Update(float timePassed) override;
	void Install() override;
	virtual void OnAnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event) override;
	// ~IAction

	// IEntityEventListener
	virtual void OnEntityEvent(IEntity* pEntity, const SEntityEvent& event) override;
	// ~IEntityEventListener

	void AddTargetToSlaveContext();
	void RemoveTargetFromSlaveContext();
	void SendStateEventCoopAnim();

	IActor &m_sourceActor;
	EntityId m_targetEntityId { INVALID_ENTITYID };
	IActorAnimationControl* m_pActorAnimationControl;
	TagID m_targetTagID;
	const IAnimationDatabase* m_pTargetOptionalDatabase;
	TagID m_scopeContextId;

	// Tags for controlling the animation.
	std::vector<string> m_tags;

	/** Listeners for animation events. */
	std::list<IAnimationEventListener*> m_listeners;

private:
	// Prevent implementation.
	CActorAnimationActionCooperative();
};
}