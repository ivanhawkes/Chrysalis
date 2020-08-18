#pragma once

#include "ICryMannequin.h"
#include <Actor/Animation/ActorAnimation.h>


namespace Chrysalis
{
#define EMOTE_FRAGMENTS( x ) \
	x( Emote )

#define EMOTE_TAGS( x ) \
 x( Anger ) \
 x( Annoyance ) \
 x( Anxiety ) \
 x( Apathy ) \
 x( Awe ) \
 x( Boredom ) \
 x( Confidence ) \
 x( Contempt ) \
 x( Contentment ) \
 x( Courage ) \
 x( Curiosity ) \
 x( Depression ) \
 x( Despair ) \
 x( Disappointment ) \
 x( Disgust ) \
 x( Dread ) \
 x( Embarrassment ) \
 x( Excitement ) \
 x( Fear ) \
 x( Frustration ) \
 x( Gratitude ) \
 x( Grief ) \
 x( Guilt ) \
 x( Happiness ) \
 x( Hatred ) \
 x( Hope ) \
 x( Horror ) \
 x( Hostility ) \
 x( Hurt ) \
 x( Hysteria ) \
 x( Indifference ) \
 x( Interest ) \
 x( Joy ) \
 x( Loathing ) \
 x( Loneliness ) \
 x( Outrage ) \
 x( Panic ) \
 x( Pity ) \
 x( Pleasure ) \
 x( Pride ) \
 x( Rage ) \
 x( Regret ) \
 x( Relief ) \
 x( Remorse ) \
 x( Sadness ) \
 x( Satisfaction ) \
 x( Shame ) \
 x( Shock ) \
 x( Shyness ) \
 x( Sorrow ) \
 x( Surprise ) \
 x( Terror ) \
 x( Trust ) \
 x( Wonder ) \
 x( Worry ) \
 x( Zeal ) \
 x( Zest ) \

#define EMOTE_TAGGROUPS( x ) \
 x( Emote ) \

#define EMOTE_SCOPES( x )

#define EMOTE_CONTEXTS( x )

#define EMOTE_CHANGEFRAGMENT_FRAGMENT_TAGS( x )

#define EMOTE_FRAGMENT_TAGS( x )

MANNEQUIN_USER_PARAMS(SMannequinEmoteParams, EMOTE_FRAGMENTS, EMOTE_TAGS, EMOTE_TAGGROUPS, EMOTE_SCOPES, EMOTE_CONTEXTS, EMOTE_FRAGMENT_TAGS);


class CActorAnimationActionEmote : public CAnimationAction
{
public:
	DEFINE_ACTION("Emote");

	CActorAnimationActionEmote(TagID emoteTagId);

	// IAction
	virtual void OnInitialise() override;
	virtual EStatus Update(float timePassed) override;
	void Enter() override;
	void Exit() override;
	// ~IAction

	static bool IsSupported(const SAnimationContext& context);

private:
	static FragmentID FindFragmentId(const SAnimationContext& context);

    const struct SMannequinEmoteParams* m_emoteParams {nullptr};

	TagID m_emoteTagId;
};

extern SMannequinEmoteParams g_emoteMannequinParams;
}