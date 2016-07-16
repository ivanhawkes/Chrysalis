#pragma once

//#include "UI/HUD/HUDEventDispatcher.h"
//
//class CCharacter;
//class CCharacterStateFly;
//struct SActorMovementRequest;
//
//
//class CCharacterStateSpectate : public CCharacterStateFly, public IHUDEventListener
//{
//private:
//#define STAY_FADED_TIME 0.5f
//
//	typedef CCharacterStateFly inherited;
//
//	float m_fFadeOutAmount;
//	float m_fFadeForTime;
//
//public:
//	CCharacterStateSpectate ();
//	void ResetFadeParameters () { m_fFadeOutAmount = 1.0f; m_fFadeForTime = STAY_FADED_TIME; }
//
//	virtual void OnHUDEvent (const SHUDEvent& event);
//
//	void OnEnter (CCharacter& Character);
//	void UpdateFade (float frameTime);
//	void OnExit (CCharacter& Character);
//
//protected:
//	void DrawSpectatorFade ();
//};
