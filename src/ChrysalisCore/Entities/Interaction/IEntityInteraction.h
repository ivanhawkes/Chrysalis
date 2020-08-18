#pragma once


namespace Chrysalis
{
struct IActor;


/** An generic definition for interactions fired off by the player during gameplay. */
struct IInteraction
{
	/**
	Called at the start of an interaction. Generally called on a downward keypress.

	\param [in,out]	actor The actor who triggered this interaction.
	**/
	virtual void OnInteractionStart(IActor& actor) {};


	/**
	Called each game frame an interaction is ongoing. This will be called multiple times, as long as the player is
	holding down the interaction key / button.

	\param [in,out]	actor The actor who triggered this interaction.
	**/
	virtual void OnInteractionTick(IActor& actor) {};


	/**
	Called when an interaction is completed normally. Generally called on an upward keypress.

	\param [in,out]	actor The actor who triggered this interaction.
	**/
	virtual void OnInteractionComplete(IActor& actor) {};


	virtual const string GetVerb() const { return "interaction_interact"; };
	virtual const string GetVerbUI() const { return "@" + GetVerb(); };

	bool IsEnabled() const { return m_isEnabled; };
	void SetEnabled(bool isEnabled) { m_isEnabled = isEnabled; };
	bool IsHidden() const { return m_isHidden; };
	void SetHidden(bool isHidden) { m_isHidden = isHidden; };

protected:
	bool m_isEnabled {true};
	bool m_isHidden {false};
};
DECLARE_SHARED_POINTERS(IInteraction);


// ***
// *** Generic interactions e.g. "use", "interact". Good for when there is really only one
// *** option for interacting with an entity.
// ***

struct IInteractionInteract
{
	virtual void OnInteractionInteractStart(IInteraction& pInteraction, IActor& actor) = 0;
	virtual void OnInteractionInteractTick(IInteraction& pInteraction, IActor& actor) = 0;
	virtual void OnInteractionInteractComplete(IInteraction& pInteraction, IActor& actor) = 0;
};


class CInteractionInteract : public IInteraction
{
public:
	CInteractionInteract(IInteractionInteract* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_interact"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionInteractStart(*this, actor); };
	void OnInteractionTick(IActor& actor) override { m_subject->OnInteractionInteractTick(*this, actor); };
	void OnInteractionComplete(IActor& actor) override { m_subject->OnInteractionInteractComplete(*this, actor); };

private:
	IInteractionInteract* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionInteract);


// ***
// *** Switches
// ***

struct IInteractionSwitch
{
	virtual void OnInteractionSwitchToggle(IInteraction& pInteraction, IActor& actor) = 0;
	virtual void OnInteractionSwitchOff(IInteraction& pInteraction, IActor& actor) = 0;
	virtual void OnInteractionSwitchOn(IInteraction& pInteraction, IActor& actor) = 0;
};


class CInteractionSwitchToggle : public IInteraction
{
public:
	CInteractionSwitchToggle(IInteractionSwitch* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_switch_toggle"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionSwitchToggle(*this, actor); };

private:
	IInteractionSwitch* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionSwitchToggle);


class CInteractionSwitchOn : public IInteraction
{
public:
	CInteractionSwitchOn(IInteractionSwitch* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_switch_on"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionSwitchOn(*this, actor); };

private:
	IInteractionSwitch* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionSwitchOn);


class CInteractionSwitchOff : public IInteraction
{
public:
	CInteractionSwitchOff(IInteractionSwitch* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_switch_off"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionSwitchOff(*this, actor); };

private:
	IInteractionSwitch* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionSwitchOff);


// ***
// *** Open / close.
// ***


struct IInteractionOpenable
{
	virtual void OnInteractionOpenableOpen(IActor& actor) = 0;
	virtual void OnInteractionOpenableClose(IActor& actor) = 0;
};


class CInteractionOpenableOpen : public IInteraction
{
public:
	CInteractionOpenableOpen(IInteractionOpenable* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_openable_open"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionOpenableOpen(actor); };

private:
	IInteractionOpenable* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionOpenableOpen);


class CInteractionOpenableClose : public IInteraction
{
public:
	CInteractionOpenableClose(IInteractionOpenable* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_openable_close"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionOpenableClose(actor); };

private:
	IInteractionOpenable* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionOpenableClose);


// ***
// *** Lock / unlock.
// ***


struct IInteractionLockable
{
	virtual void OnInteractionLockableLock(IActor& actor) = 0;
	virtual void OnInteractionLockableUnlock(IActor& actor) = 0;
};


class CInteractionLockableLock : public IInteraction
{
public:
	CInteractionLockableLock(IInteractionLockable* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_lockable_lock"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionLockableLock(actor); };

private:
	IInteractionLockable* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionLockableLock);


class CInteractionLockableUnlock : public IInteraction
{
public:
	CInteractionLockableUnlock(IInteractionLockable* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_lockable_unlock"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionLockableUnlock(actor); };

private:
	IInteractionLockable* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionLockableUnlock);


// ***
// *** Items which can be pickup up, inspected, dropped, etc.
// ***


struct IInteractionItem
{
	virtual void OnInteractionItemInspect(IActor& actor) = 0;
	virtual void OnInteractionItemPickup(IActor& actor) = 0;
	virtual void OnInteractionItemDrop(IActor& actor) = 0;
	virtual void OnInteractionItemToss(IActor& actor) = 0;
};


class CInteractionItemInspect : public IInteraction
{
public:
	CInteractionItemInspect(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_inspect"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionItemInspect(actor); };

private:
	IInteractionItem* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionItemInspect);


class CInteractionItemPickup : public IInteraction
{
public:
	CInteractionItemPickup(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_pickup"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionItemPickup(actor); };

private:
	IInteractionItem* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionItemPickup);


class CInteractionItemDrop : public IInteraction
{
public:
	CInteractionItemDrop(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_drop"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionItemDrop(actor); };

private:
	IInteractionItem* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionItemDrop);


class CInteractionItemToss : public IInteraction
{
public:
	CInteractionItemToss(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_toss"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionItemToss(actor); };

private:
	IInteractionItem* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionItemToss);


// ***
// *** Zoom in to an entity and switch to an examine camera.
// ***

struct IInteractionExamine
{
	virtual void OnInteractionExamineStart(IActor& actor) = 0;
	virtual void OnInteractionExamineComplete(IActor& actor) = 0;
};


class CInteractionExamine : public IInteraction
{
public:
	CInteractionExamine(IInteractionExamine* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_examine"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionExamineStart(actor); };
	void OnInteractionComplete(IActor& actor) override { m_subject->OnInteractionExamineComplete(actor); };

private:
	IInteractionExamine* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionExamine);


// ***
// *** Trigger a DRS operation with variable passed in from the component.
// ***

struct IInteractionDRS
{
	virtual void OnInteractionDRS() = 0;
};


class CInteractionDRS : public IInteraction
{
public:
	CInteractionDRS(IInteractionDRS* subject, bool isEnabled = true, bool isHidden = false)
		: m_subject(subject)
	{
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_drs"; };
	void OnInteractionStart(IActor& actor) override { m_subject->OnInteractionDRS(); };

private:
	IInteractionDRS* m_subject {nullptr};
};
DECLARE_SHARED_POINTERS(CInteractionDRS);
}