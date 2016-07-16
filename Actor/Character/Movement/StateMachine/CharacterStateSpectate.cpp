
#include <StdAfx.h>
//
//#include <Actor/Character/Character.h>
//#include "CharacterStateFly.h"
//#include "CharacterStateSpectate.h"
//#include "CharacterStateUtil.h"
//#include "CharacterInput.h"
//#include "UI/UIManager.h"
//#include "UI/Utils/ScreenLayoutManager.h"
//#include "Graphics/2DRenderUtils.h"
//
//
//#define INVERSE_SPECTATE_FADE_OVER_TIME 1.f / 1.5f
//
//CCharacterStateSpectate::CCharacterStateSpectate ()
//	: m_fFadeOutAmount (0.0f)
//	, m_fFadeForTime (0.0f)
//{
//}
//
//
//void CCharacterStateSpectate::OnEnter (CCharacter& Character)
//{
//	if (Character.IsClient ())
//	{
//		m_fFadeOutAmount = 1.0f;
//		m_fFadeForTime = STAY_FADED_TIME;
//	}
//
//	CHUDEventDispatcher::AddHUDEventListener (this, "OnPostHUDDraw");
//
//	inherited::OnEnter (Character);
//}
//
//
//void CCharacterStateSpectate::UpdateFade (float frameTime)
//{
//	if (m_fFadeForTime > 0.f)
//	{
//		m_fFadeForTime -= gEnv->pTimer->GetFrameTime (ITimer::ETIMER_UI);
//	}
//	else
//	{
//		m_fFadeOutAmount = max (m_fFadeOutAmount - frameTime * INVERSE_SPECTATE_FADE_OVER_TIME, 0.0f);
//	}
//}
//
//
//void CCharacterStateSpectate::OnHUDEvent (const SHUDEvent& event)
//{
//	switch (event.eventType)
//	{
//		case eHUDEvent_OnPostHUDDraw:
//		{
//			if (m_fFadeOutAmount > 0.0f)
//				DrawSpectatorFade ();
//			break;
//		}
//	}
//}
//
//
//void CCharacterStateSpectate::DrawSpectatorFade ()
//{
//	// If we're in a host migration we don't get ticked since the game is paused, still need to update the 
//	// fade though otherwise Characters end up on a black screen for several seconds
//	if (g_pGame->GetHostMigrationState () != CGame::eHMS_NotMigrating)
//	{
//		m_fFadeOutAmount = max (m_fFadeOutAmount - gEnv->pTimer->GetFrameTime (ITimer::ETIMER_UI), 0.f);
//	}
//
//	const float fCurrentFadeOut = m_fFadeOutAmount;
//	C2DRenderUtils* pRenderUtils = g_pGame->GetUI ()->Get2DRenderUtils ();
//	ScreenLayoutManager* pLayoutManager = g_pGame->GetUI ()->GetLayoutManager ();
//
//	ScreenLayoutStates prevLayoutState = pLayoutManager->GetState ();
//	pLayoutManager->SetState (eSLO_DoNotAdaptToSafeArea | eSLO_ScaleMethod_None);
//
//	ColorF color (0.f, 0.f, 0.f, fCurrentFadeOut);
//	float width = pLayoutManager->GetVirtualWidth ();
//	float height = pLayoutManager->GetVirtualHeight ();
//
//	gEnv->pRenderer->SetState (GS_NODEPTHTEST);
//	pRenderUtils->DrawQuad (0, 0, width, height, color);
//
//	pLayoutManager->SetState (prevLayoutState);
//}
//
//
//void CCharacterStateSpectate::OnExit (CCharacter& Character)
//{
//	if (Character.IsClient ())
//	{
//		m_fFadeOutAmount = 0.0f;
//	}
//
//	CHUDEventDispatcher::RemoveHUDEventListener (this);
//
//	inherited::OnExit (Character);
//}
