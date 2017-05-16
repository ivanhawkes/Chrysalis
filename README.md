# Chrysalis

Chrysalis is a plugin for CRYENGINE 5, written in C++11, that provides a foundation of features required to create action RPG style games. It is under constant development, moving towards a set of final goals.

Currently, most of the work needed for a walking simulator is complete or underway.

## Goals

The primary goal is to provide the base functionality required by several types of games. The main focus is on making an action RPG toolkit, but there are several goals and steps along the way.

*   walking simulator
    *   first and third person cameras
    *   player input handling
    *   actors / characters
    *   locomotion
    *   interactive entities (doors, switches, etc)
    *   item and equipment systems
*   action RPG
    *   weapons
    *   combat
    *   vehicles
*   stretch goals
    *   ladders
    *   swimming
    *   climbing
    *   networked gameplay

At present, I am not expecting to create a UI for any of the game features. Any teams looking to use this code will need to make their own UI, since game features tend to vary widely.

UI is a [CRYENGINE](https://www.cryengine.com/) feature in flux. The Scaleform implementation is getting too old to be useful, while the new UI elements aren't quite ready yet.

In time I will ensure the base code has all the features needed to support a rich client UI, and will probably provide a simple example implementation for the most useful features.

## Gettting Started

The best way is to use Git to clone this entire project to your local hard drive.

The project contains both the code to build, and the project files needed by CRYENGINE to run the example levels.

Don't expect any flash looking level design or grahpics, I'm a programmer, so any models and textures are done as quickly and easily as I can to test out the features.

## Important Information

Chrysalis is very much a work in progress. I try not to check in broken builds, and generally would only do so on a feature branch I am working on, but...things can slip my notice. If you run into any show-stopper problems preventing you from trying out the code, contact me, and I'll see if I can help you sort it out. Contacts details are below.

With that said, I expect you to be competant and comfortable working with CRYENGINE before you consider setting up a test environment for Chrysalis. You can ask for general help on the CryTek forums or within our Slack group.

## Code Quality

This code is considered pre-alpha, under heavy development. That's not a reflection on the quality of the code, but on the degree of features completed and the level of polish for those features.

## My First Level

Chrysalis has some code features which are specific to a long-term goal I have, though most of these will not affect you. One that does affect everyone is the way you play test a level.

Most CRYENGINE games are designed with a player who is also the actor, and that player is spawned into the game as an actor when you connect to the game.

I have worked hard to refactor the code so that the concept of player and actor / character are now completely separate. Code relating to the player is kept to a minimum, in particular:

*   camera management
*   input management

All the other code is split off into an actor class. Actors can have lifespans totally indepedant of the player. A player is able to 'attach' themselves to an actor, the camera and input are then directed to that actor - who acts accordingly.

While I expect to add some code in the near future to handle spawning a new actor when the player enters game mode, for now, it does not.

When you first create a level, be sure to add a new 'Character' entity to that level. Name that entity 'Hero', and run a console command to attach the player camera and input to that character:

```
attach Hero
```

This will set up the player to attach to the 'Hero' character - something that is done automatically when the level is loaded.

You will not be able to enter game mode to test a level without at least one character in the level, and attaching the player to that character.

I will sort out something more convenient in time, as I get close to making a release of this code.

# Contact

If you want to contact me, you can often find me on the CRYENGINE community Slack channel under the name 'Ivan Hawkes'.

[https://crycommunity.slack.com](https://crycommunity.slack.com)

You can contact me directly by emailing ivan.hawkes@gmail.com