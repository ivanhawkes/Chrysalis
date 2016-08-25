#pragma once

#include <Entities/Helpers/ISimpleExtension.h>


struct IInteraction
{
	virtual void OnInteractionStart() {};
	virtual void OnInteractionComplete() {};
	virtual void OnInteractionCancel() {};

	bool IsUseable() { return true; };
	virtual const string GetVerb() { return "@interact"; };

	bool IsEnabled() { return m_isEnabled; };
	void SetEnabled(bool isEnabled) { m_isEnabled = isEnabled; };
	bool IsHidden() { return m_isHidden; };
	void SetHidden(bool isHidden) { m_isHidden = isHidden; };

private:
	bool m_isEnabled { true };
	bool m_isHidden { false };
};
DECLARE_SHARED_POINTERS(IInteraction);


class IEntityInteraction : public ISimpleExtension
{
public:
	std::vector<string> GetVerbs(bool includeHidden = false);

	void AddInteraction(IInteractionPtr interaction);
	void RemoveInteraction(string verb);
	IInteractionWeakPtr GetInteraction(string verb);
	IInteractionWeakPtr SelectInteractionVerb(string verb);
	void ClearInteractionVerb();

	//void OnInit();
	//void OnReset();
	void OnInteractionStart();
	//void OnUpdate(float deltaX, float deltaY);
	void OnInteractionComplete();
	void OnInteractionCancel();
	//void OnPost();

private:
	std::vector<IInteractionPtr> m_Interactions;
	IInteractionPtr m_selectedInteraction { IInteractionPtr() };
};

// ***
// *** Generic interactions e.g. "use", "interact". Good for when then is really only one
// *** option for interacting with an entity.
// ***

struct IInteractionInteract
{
	virtual void Interact() = 0;
};


class CInteractionInteract : public IInteraction
{
public:
	CInteractionInteract(IInteractionInteract* subject) { m_subject = subject; };

	void OnInteractionStart() override { m_subject->Interact(); };

private:
	IInteractionInteract* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionInteract);


// ***
// *** Switches
// ***

struct IInteractionSwitch
{
	virtual void SwitchOn() = 0;
	virtual void SwitchOff() = 0;
};


class CInteractionSwitchOn : public IInteraction
{
public:
	CInteractionSwitchOn(IInteractionSwitch* subject) { m_subject = subject; };

	const string GetVerb() override { return "@switchon"; };
	void OnInteractionStart() override { m_subject->SwitchOn(); };

private:
	IInteractionSwitch* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionSwitchOn);


class CInteractionSwitchOff : public IInteraction
{
public:
	CInteractionSwitchOff(IInteractionSwitch* subject) { m_subject = subject; };

	const string GetVerb() override { return "@switchoff"; };
	void OnInteractionStart() override { m_subject->SwitchOff(); };

private:
	IInteractionSwitch* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionSwitchOff);


// ***
// *** Items which can be pickup up, inspected, dropped, etc.
// ***


struct IInteractionPickupAndDrop
{
	virtual void Pickup() = 0;
	virtual void Drop() = 0;
	virtual void Inspect() = 0;
};


class CInteractionPickup : public IInteraction
{
public:
	CInteractionPickup(IInteractionPickupAndDrop* subject) { m_subject = subject; };

	const string GetVerb() override { return "@pickup"; };
	void OnInteractionStart() override { m_subject->Pickup(); };

private:
	IInteractionPickupAndDrop* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionPickup);


class CInteractionDrop : public IInteraction
{
public:
	CInteractionDrop(IInteractionPickupAndDrop* subject) { m_subject = subject; };

	const string GetVerb() override { return "@drop"; };
	void OnInteractionStart() override { m_subject->Drop(); };

private:
	IInteractionPickupAndDrop* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionDrop);


class CInteractionInspect : public IInteraction
{
public:
	CInteractionInspect(IInteractionPickupAndDrop* subject) { m_subject = subject; };

	const string GetVerb() override { return "@inspect"; };

	void OnInteractionStart() override { m_subject->Inspect(); };

	//gEnv->pLog->LogAlways("Interation Interact fired.");
private:
	IInteractionPickupAndDrop* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionInspect);


// ***
// *** Containers.
// ***


struct IInteractionContainer
{
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};


class CInteractionOpen : public IInteraction
{
public:
	CInteractionOpen(IInteractionContainer* subject) { m_subject = subject; };

	const string GetVerb() override { return "@open"; };
	void OnInteractionStart() override { m_subject->Open(); };

private:
	IInteractionContainer* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionOpen);


class CInteractionClose : public IInteraction
{
public:
	CInteractionClose(IInteractionContainer* subject) { m_subject = subject; };

	const string GetVerb() override { return "@close"; };
	void OnInteractionStart() override { m_subject->Close(); };

private:
	IInteractionContainer* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionClose);


class CInteractionLock : public IInteraction
{
public:
	CInteractionLock(IInteractionContainer* subject) { m_subject = subject; };

	const string GetVerb() override { return "@lock"; };
	void OnInteractionStart() override { m_subject->Lock(); };

private:
	IInteractionContainer* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionLock);


class CInteractionUnlock : public IInteraction
{
public:
	CInteractionUnlock(IInteractionContainer* subject) { m_subject = subject; };

	const string GetVerb() override { return "@unlock"; };
	void OnInteractionStart() override { m_subject->Unlock(); };

private:
	IInteractionContainer* m_subject { nullptr };
};
DECLARE_SHARED_POINTERS(CInteractionUnlock);
