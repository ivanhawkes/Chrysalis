# Requirements Specification


## Movement System

Provide code which can animate humanoid characters performing the following forms of movement:

* locomotion on flat or sloped terrain
** walk
** jog
** run
** crouch
** crawl
* jump
* swim
* climb fixed handholds
* ladders


## Interaction System

Interactions range from simple triggers, to more complex interactive flows e.g. entering a pin number while the camera is zoomed in to the device. Simple interactions can take away player control for very short periods of time to ensure animations play correctly. Longer interactions need to respond to player movement controls and potentially disengage from the interaction.

All interactions should have the possibility to play an animation for the player character which interacts with other characters or simple objects e.g. a plinth that triggers a trackview animation fly around the area as a reward for exploring to a difficult zone.

Provide code which allows the following forms of interaction to occur:

Trigger and play:

* (pull / throw) (switch / lever) (on / off)
* pick up / drop / toss / inspect object (interactive)
* loot container
* open / close
* lock / unlock
* trigger DRS

Long interactions with ticks:

* examine - allow camera change
* keypad entry
* push / pull heavy object

## Item System

## Missions

Provide a means of distributing missions, having the player complete objectives, and gaining rewards from these completions.

## Cinematics

* trigger cinematics

## RPG Elements

The focus for now is fixed upon interactive story-telling. In future it will expand to add RPG elements.

### Experience System

### Loot System

### Gear System

### Skill System

