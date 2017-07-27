# Factions

We will need to build a generic system for handling character faction. Ideally, the same system will support both player and non-player styles of factions.

## Reputation

Characters will be able to join only one faction at any given time. They can earn points towards their primary faction through several mechanisms:

-   faction quests
-   killing enemies of their faction (perhaps)

Each character will maintain a list of factions, and their relative ranking with that faction. Rank is determined by XP e.g.

-   Mortal Enemies -15,000
-   Despised -5000
-   Loathed -1000
-   Hostile -200
-   Unfriendly -50
-   Neutral 0
-   Friendly 200
-   Decorated 1,000
-   Honoured 5,000
-   Revered 20,000
-   Exalted 120,000

You should start out with 0 points of reputation against nearly all factions, except ones that are meant to be hostile right from the start.

Actions taken against other players will result in a reduction of your standing with their faction e.g.

-   /slap (10 points)
-   /spit (20 points)
-   theft (50 points)
-   murder (500 points)

You may be able to undertake quests that are against another faction which also award points. These should award XP from the quest giver's faction, and remove it from the targetted faction.

Other player factions shouldn't be able to initiate combat against each other until they are hostile.

Non-lethal combat (e.g. boxing) should be allowed to start off the fighting when the game first releases. On FFA and FFA FF servers they can kick off any time they want.

## Rewards

Increasing your faction should open up access to rewards. This is true of characters and factions themselves. Implement an XP driven reward system which can provide the usual incentives.

-   faction banners / tabards
-   improved XP gains for characters in the faction
-   building plans
-   ability to own land (serious chance for things to go bad if this is done wrong)
-   access to defenses for land
-   ability to create faction owned characters, such as crafters, guards, and other NPC like characters - though all would be able to be controlled by a player of high enough faction rank (rank up in craft, leadership, military, etc all separately? Split out according to broad interests?)

It might be interesting if factions can spend points similar to a skill point system. Eventually, they could open up large amounts of the rewards, but never everything at once. A PVP faction could favour PVP rewards, a leveling factions - ones that speed up leveling, and PVE ones that help down bosses in dungeons.

There's scope to mess things up here with rewards that tilt the power balance but I'm not sure that time saving and vanity only rewards are quite right either. It must have a sense of fairness and balance, though not neccessarily equality.

## City Building

Factions will grow and eventually build houses and towns. As they grow they should invest some points (or resource of some sort) into creating new characters who will work as guards, crafters, or basic NPCs.

e.g. They position a pair of guards at each gate, and have a few who patrol the buildings. They also build a post office and staff it so mail can be delivered. Local merchants could sell items made by their craftsmen (only available in their own towns) and merchants could travel to other towns in order to trade.

A player might direct a merchant to go to another town and receive a notice when he is there. When he arrives, the player takes over, initiates any wanted trades, and sends him back again. This reduces the lost time for players, but still keeps the world feeling move alive.

## Storage / Banks

Many people clutter up MMO games with bank alts due to the inadequacies of the banking and inventory systems. A character might be given 50 slots of bank space on creation, but have to spend a day's worth of income at latter levels just to open up 10 more slots.

Players park characters right outside the bank, auction house, and mailbox, running a trianle between them just trying to get basic busy work completed in game.

While some developers believe this helps create a feeling of a living city, it's a false illusion since everyone is AFK while scanning the auction house or busy reading their mail.

Alternatives are to provide these features at a character's base, but that can lead to an empty world with everyone inside their own base a good portion of the day.

It would be better to give player's much better accesss to their bank and mail and allow the interactions to occur out in the field instead. It's also preferable to not have people creating spare characters just to store a bunch of materials for crafting. TESO is perhaps the worst offender in this case, needing something like five alts just to hold my crafting materials. It's like an inventory management game with some combat features added. Guild Wars 2 however, was much better - allowing you to read mail in the field, auction items in the field, and a slot for every single material you might harvest - separate to your quest and gear slots.

One proposal is to not provide any form of guild bank or character bank for new characters, and given them very few initial item slots in their inventory. You can unlock new slots over the course of a few hours questing. Make them have to invest time in before they can get something for free. At the same time, lower the reasons to do this by making sure they have more than adequate storage available at reasonable cost on their main character(s).

### Bank Space

A faction or character can purchase some storage space at a registered bank. These can be staffed by non-players, using pre-built scripts, or realms can supply their own.

### Mail

We should implement a complete and fairly standard mail and parcel service for realms.

On top of this, I would like to create a unique, player run version using pony express style delivery - requiring guards to ward off bandits.

Mail could be implemented as a massive, no limit (or no reasonable limit) storage bag capable of holding both messages and items / currency. Mail items could be swept by an offline process to archive and bounce them.

Could we just run this on Postfix / IMAP?

### Auction House

Games live and die based on their auctions houses. Although I would prefer to try without one initially, most realms would doubtless prefer a standard auction house. This is a big feature, and will require large amounts of forethought and planning.

## Taxes

A faction is able to levy a tax against any characters who belong to that faction. Tax rates are set by the faction.

While it might be possible to simply deduct a taxable amount from all the currency a character earns, it could open up more options for conflict if we allow them to track what is owed - and send debt collectors out for characters who haven't remitted their taxes.

As usual, it's best to start with the simple option and go from there.

## Damage Calculations

-   IsHostile - Hostile status
-   IsAttackable - Attackable status

Calculations need to account for friendly fire as well as faction and free for all status of location / zone / realm.

e.g. Gurubashi Arena is FFA FF on entering, but flag wears off after you leave it for five minutes

Implement FFA servers with and without friendly fire on (faction / party / raid).

Try and use the in-built faction variable if possible

e.g. Faction_[factionID]

## Ownership

Each realm should be capable of having it's own in-built factions, provided by the realm owner or system. These would still need a character proxy to attach to for simplicity sake. The realm admins should be free to create one or more characters / faction, in advance of opening the realm.

Faction ownership must be transferrable between characters, although there need to be checks and balances on this and cooldown timers.

Factions might be as small as just one single character, or as large as is practical for administration. Realm administration should be able to set the maximum faction size, but be aware this can lead to "sister factions" to get around the limits.


## Bosses

Factions can have their own boss level leaders who can be attacked by other factions and possibly farmed each week :D

It would be like a PVP raid, perhaps with sub bosses and trash, which plays out in your home town or better yet, your rival's town. There would need to be some level of control to prevent player harrassment by massive factions against smaller groups. Remember the lessons from AoC where the new PVP system caused nearly all the players to move their characters to just three massive PVP guilds.
