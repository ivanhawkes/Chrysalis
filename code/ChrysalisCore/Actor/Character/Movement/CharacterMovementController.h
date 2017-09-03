/**
\file d:\CRYENGINE5\Crytek\CRYENGINE_5.1\Code\Chrysalis\Actor\Character\Movement\CharacterMovementController.h

Declares the character movement controller class.

While this class is currently named to imply it requires character class information, it presently
does not, needing only information available on CActorComponent. If possible, I like to preserve this artifical
divide, where information pertinant to moving the actor remains in the CActorComponent base class.

Assuming this is still true when the project is mature - this class can be renamed CActorMovementController instead,
to better illustrate it's use and needs.
**/
#pragma once

#include <IGameObject.h>
#include <IMovementController.h>
#include <Actor/Movement/ActorMovementController.h>


namespace Chrysalis
{
class CActorComponent;
struct SActorMovementRequest;


class CCharacterMovementController : public CActorMovementController
{
	// ***
	// *** IMovementController
	// ***

public:


	// ***
	// *** CCharacterMovementController
	// ***

public:

};
}