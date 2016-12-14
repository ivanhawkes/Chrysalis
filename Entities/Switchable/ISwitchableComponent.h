#pragma once

#include <Entities/Helpers/NativeEntityBase.h>
#include <Utility/IPublisher.h>


struct ISwitchableListener
{
	virtual ~ISwitchableListener() {};

	virtual void OnSwitchOn() = 0;
	virtual void OnSwitchOff() = 0;
};


struct ISwitchableComponent : public CNativeEntityBase, public ISwitchableListener, public IPublisher<ISwitchableListener*>
{
	void OnSwitchOn() override 
	{
		for (auto& it : m_listeners)
			it->OnSwitchOn();
	}

	void OnSwitchOff() override 
	{
		for (auto& it : m_listeners)
			it->OnSwitchOff();
	}
};
