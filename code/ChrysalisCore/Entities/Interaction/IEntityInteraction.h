#pragma once

#include <CryCore/Platform/platform.h>


namespace Chrysalis
{
struct IInteraction
{
	/** Called at the start of an interaction. */
	virtual void OnInteractionStart() {};

	/** Called each game frame an interaction is ongoing. */
	virtual void OnInteractionTick() {};

	/** Called when an interaction is completed normally. */
	virtual void OnInteractionComplete() {};

	/** Called if an interaction is cancelled early. */
	virtual void OnInteractionCancel() {};

	bool IsUseable() const { return true; };
	virtual const string GetVerb() const { return "interaction_interact"; };
	virtual const string GetVerbUI() const { return "@" + GetVerb(); };

	bool IsEnabled() const { return m_isEnabled; };
	void SetEnabled(bool isEnabled) { m_isEnabled = isEnabled; };
	bool IsHidden() const { return m_isHidden; };
	void SetHidden(bool isHidden) { m_isHidden = isHidden; };

protected:
	bool m_isEnabled { true };
	bool m_isHidden { false };
};
DECLARE_SHARED_POINTERS(IInteraction);


// ***
// *** Zoom in to an entity and switch to an examine camera.
// ***

struct IInteractionExamine
{
	virtual void OnInteractionExamineStart() = 0;
	virtual void OnInteractionExamineComplete() = 0;
	virtual void OnInteractionExamineCancel() = 0;
};


class CInteractionExamine : public IInteraction
{
public:
	CInteractionExamine(IInteractionExamine* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_examine"; };
	void OnInteractionStart() override { m_subject->OnInteractionExamineStart(); };
	void OnInteractionComplete() override { m_subject->OnInteractionExamineComplete(); };
	void OnInteractionCancel() override { m_subject->OnInteractionExamineCancel(); };

private:
	IInteractionExamine* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionExamine);


// ***
// *** Generic interactions e.g. "use", "interact". Good for when then is really only one
// *** option for interacting with an entity.
// ***

struct IInteractionInteract
{
	virtual void OnInteractionInteractStart() = 0;
	virtual void OnInteractionInteractTick() = 0;
	virtual void OnInteractionInteractComplete() = 0;
	virtual void OnInteractionInteractCancel() = 0;
};


class CInteractionInteract : public IInteraction
{
public:
	CInteractionInteract(IInteractionInteract* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_interact"; };
	void OnInteractionStart() override { m_subject->OnInteractionInteractStart(); };
	void OnInteractionTick() override { m_subject->OnInteractionInteractTick(); };
	void OnInteractionComplete() override { m_subject->OnInteractionInteractComplete(); };
	void OnInteractionCancel() override { m_subject->OnInteractionInteractCancel(); };

private:
	IInteractionInteract* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionInteract);


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
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_drs"; };
	void OnInteractionStart() override { m_subject->OnInteractionDRS(); };

private:
	IInteractionDRS* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionDRS);


// ***
// *** Switches
// ***

struct IInteractionSwitch
{
	virtual void OnInteractionSwitchToggle() = 0;
	virtual void OnInteractionSwitchOff() = 0;
	virtual void OnInteractionSwitchOn() = 0;
};


class CInteractionSwitchToggle : public IInteraction
{
public:
	CInteractionSwitchToggle(IInteractionSwitch* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_switch_toggle"; };
	void OnInteractionStart() override { m_subject->OnInteractionSwitchToggle(); };

private:
	IInteractionSwitch* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionSwitchToggle);


class CInteractionSwitchOn : public IInteraction
{
public:
	CInteractionSwitchOn(IInteractionSwitch* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_switch_on"; };
	void OnInteractionStart() override { m_subject->OnInteractionSwitchOn(); };

private:
	IInteractionSwitch* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionSwitchOn);


class CInteractionSwitchOff : public IInteraction
{
public:
	CInteractionSwitchOff(IInteractionSwitch* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_switch_off"; };
	void OnInteractionStart() override { m_subject->OnInteractionSwitchOff(); };

private:
	IInteractionSwitch* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionSwitchOff);


// ***
// *** Items which can be pickup up, inspected, dropped, etc.
// ***


struct IInteractionItem
{
	virtual void OnInteractionItemInspect() = 0;
	virtual void OnInteractionItemPickup() = 0;
	virtual void OnInteractionItemDrop() = 0;
	virtual void OnInteractionItemToss() = 0;
};


class CInteractionItemInspect : public IInteraction
{
public:
	CInteractionItemInspect(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_inspect"; };
	void OnInteractionStart() override { m_subject->OnInteractionItemInspect(); };

private:
	IInteractionItem* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionItemInspect);


class CInteractionItemPickup : public IInteraction
{
public:
	CInteractionItemPickup(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_pickup"; };
	void OnInteractionStart() override { m_subject->OnInteractionItemPickup(); };

private:
	IInteractionItem* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionItemPickup);


class CInteractionItemDrop : public IInteraction
{
public:
	CInteractionItemDrop(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_drop"; };
	void OnInteractionStart() override { m_subject->OnInteractionItemDrop(); };

private:
	IInteractionItem* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionItemDrop);


class CInteractionItemToss : public IInteraction
{
public:
	CInteractionItemToss(IInteractionItem* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_toss"; };
	void OnInteractionStart() override { m_subject->OnInteractionItemToss(); };

private:
	IInteractionItem* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionItemToss);


// ***
// *** Generic open.
// ***


struct IInteractionOpenable
{
	virtual void OnInteractionOpenableOpen() = 0;
	virtual void OnInteractionOpenableClose() = 0;
};


class CInteractionOpenableOpen : public IInteraction
{
public:
	CInteractionOpenableOpen(IInteractionOpenable* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_openable_open"; };
	void OnInteractionStart() override { m_subject->OnInteractionOpenableOpen(); };

private:
	IInteractionOpenable* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionOpenableOpen);


class CInteractionOpenableClose : public IInteraction
{
public:
	CInteractionOpenableClose(IInteractionOpenable* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_openable_close"; };
	void OnInteractionStart() override { m_subject->OnInteractionOpenableClose(); };

private:
	IInteractionOpenable* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionOpenableClose);


// ***
// *** Lockable.
// ***


struct IInteractionLockable
{
	virtual void OnInteractionLockableLock() = 0;
	virtual void OnInteractionLockableUnlock() = 0;
};


class CInteractionLockableLock : public IInteraction
{
public:
	CInteractionLockableLock(IInteractionLockable* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_lockable_lock"; };
	void OnInteractionStart() override { m_subject->OnInteractionLockableLock(); };

private:
	IInteractionLockable* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionLockableLock);


class CInteractionLockableUnlock : public IInteraction
{
public:
	CInteractionLockableUnlock(IInteractionLockable* subject, bool isEnabled = true, bool isHidden = false)
	{
		m_subject = subject;
		m_isEnabled = isEnabled;
		m_isHidden = isHidden;
	};

	const string GetVerb() const override { return "interaction_lockable_unlock"; };
	void OnInteractionStart() override { m_subject->OnInteractionLockableUnlock(); };

private:
	IInteractionLockable* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionLockableUnlock);


//// ***
//// *** Doors.
//// ***
//
//
//struct IInteractionDoor
//{
//	virtual void OnInteractionDoorOpen() = 0;
//	virtual void OnInteractionDoorClose() = 0;
//};
//
//
//class CInteractionDoorOpen : public IInteraction
//{
//public:
//	CInteractionDoorOpen(IInteractionDoor* subject) { m_subject = subject; };
//
//	const string GetVerb() const override { return "interaction_door_open"; };
//	void OnInteractionStart() override { m_subject->OnInteractionDoorOpen(); };
//
//private:
//	IInteractionDoor* m_subject { nullptr };
//};
//DECLARE_SHARED_POINTERS(CInteractionDoorOpen);
//
//
//class CInteractionDoorClose : public IInteraction
//{
//public:
//	CInteractionDoorClose(IInteractionDoor* subject) { m_subject = subject; };
//
//	const string GetVerb() const override { return "interaction_door_close"; };
//	void OnInteractionStart() override { m_subject->OnInteractionDoorClose(); };
//
//private:
//	IInteractionDoor* m_subject { nullptr };
//};
//DECLARE_SHARED_POINTERS(CInteractionDoorClose);
//
//
//// ***
//// *** Containers.
//// ***
//
//
//struct IInteractionContainer
//{
//	virtual void OnInteractionContainerOpen() = 0;
//	virtual void OnInteractionContainerClose() = 0;
//	virtual void OnInteractionContainerLock() = 0;
//	virtual void OnInteractionContainerUnlock() = 0;
//};
//
//
//class CInteractionContainerOpen : public IInteraction
//{
//public:
//	CInteractionContainerOpen(IInteractionContainer* subject) { m_subject = subject; };
//
//	const string GetVerb() const override { return "interaction_container_open"; };
//	void OnInteractionStart() override { m_subject->OnInteractionContainerOpen(); };
//
//private:
//	IInteractionContainer* m_subject { nullptr };
//};
//DECLARE_SHARED_POINTERS(CInteractionContainerOpen);
//
//
//class CInteractionContainerClose : public IInteraction
//{
//public:
//	CInteractionContainerClose(IInteractionContainer* subject) { m_subject = subject; };
//
//	const string GetVerb() const override { return "interaction_container_close"; };
//	void OnInteractionStart() override { m_subject->OnInteractionContainerClose(); };
//
//private:
//	IInteractionContainer* m_subject { nullptr };
//};
//DECLARE_SHARED_POINTERS(CInteractionContainerClose);
//
//
//class CInteractionContainerLock : public IInteraction
//{
//public:
//	CInteractionContainerLock(IInteractionContainer* subject) { m_subject = subject; };
//
//	const string GetVerb() const override { return "interaction_container_lock"; };
//	void OnInteractionStart() override { m_subject->OnInteractionContainerLock(); };
//
//private:
//	IInteractionContainer* m_subject { nullptr };
//};
//DECLARE_SHARED_POINTERS(CInteractionContainerLock);
//
//
//class CInteractionContainerUnlock : public IInteraction
//{
//public:
//	CInteractionContainerUnlock(IInteractionContainer* subject) { m_subject = subject; };
//
//	const string GetVerb() const override { return "interaction_container_unlock"; };
//	void OnInteractionStart() override { m_subject->OnInteractionContainerUnlock(); };
//
//private:
//	IInteractionContainer* m_subject { nullptr };
//};
//DECLARE_SHARED_POINTERS(CInteractionContainerUnlock);
}