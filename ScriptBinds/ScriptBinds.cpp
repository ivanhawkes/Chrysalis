#include <StdAfx.h>

#include "ScriptBinds.h"
#include "Game/ScriptBindGame.h"
#include "Boids/ScriptBindBoids.h"
#include "Item/ScriptBindItem.h"


CScriptBinds::CScriptBinds()
{}


CScriptBinds::~CScriptBinds()
{
}


void CScriptBinds::Init()
{
	m_pScriptBindGame = new CScriptBindGame();
	m_pScriptBindBoids = new CScriptBindBoids();
	m_pScriptBindItem = new CScriptBindItem();	
}


void CScriptBinds::Release()
{
	// Delete the game script bind.
	SAFE_DELETE(m_pScriptBindGame);
	SAFE_DELETE(m_pScriptBindBoids);
	SAFE_DELETE(m_pScriptBindItem);
}


void CScriptBinds::GetMemoryUsage(ICrySizer * s)
{
	s->Add(*this);
	s->Add(*m_pScriptBindGame);
	s->Add(*m_pScriptBindBoids);
	s->Add(*m_pScriptBindItem);
}
