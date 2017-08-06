#pragma once

namespace Chrysalis
{
// ***
// *** Character stances
// ***


/** All valid actor stances. */

enum EActorStance
{
	eAS_Standing,
	eAS_Crouching,
	eAS_Crawling,
	eAS_Falling,
	eAS_Landing,
	eAS_Swimming,
	eAS_Flying,
	eAS_Spellcasting,
	eAS_SittingChair,
	eAS_SittingFloor,
	eAS_Kneeling
};


/**
The actor's posture extends the stance, giving more precise control over animating their state of mind. In
general, these are applied to eAS_Standing, but some may also have application to sitting or other stances.
**/
enum EActorPosture
{
	// Alertness.
	eAP_Unaware,
	eAP_Distracted,
	eAP_Suspicious,
	eAP_Alerted,

	// Daze / sap effect on them.
	eAP_Dazed,

	// Everyday postures.
	eAP_Neutral,
	eAP_Passive,
	eAP_Aggressive,
	eAP_Interested,
	eAP_Bored,
	eAP_Excited,
	eAP_Depressed,
};


/** Keeps track of the stance the actor is in. */

struct CActorStance
{
public:
	EActorStance GetStance() const { return m_stance; }
	void SetStance(EActorStance stance) { m_stance = stance; }
	EActorPosture GetPosture() const { return m_posture; }
	void SetPosture(EActorPosture posture) { m_posture = posture; }

private:

	/** The actor is currently in this stance, or moving into this stance. */
	EActorStance m_stance { eAS_Standing };

	/** The actor's posture, which should indicate their state of mind, or game conditions they presently have e.g. sapped. */
	EActorPosture m_posture { eAP_Neutral };
};
}