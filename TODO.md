TODO List and Implementation Ideas

# Attaching things

See ProceduralClipProps.cpp in CryAction\Mannequin for examples of attaching geometry.



```#include <CryEntitySystem/IEntitySystem.h>
#include <CrySchematyc/CoreAPI.h>
```
into stdafx.h







# Definition

An item is an entity that has one or more of the following:

-   Geometry
-   Attributes
-   Currency / other rewards e.g. gold, XP
-   Interactions e.g. a flashlight has geometry and provides several interactions for the character who possesses it, like switch on and switch off.
-   Can provide all of the following verbs: pickup, drop, throw, inspect
-   Can optionally be equipped / removed from a character

A weapon is an item that provides more functionality

-   TODO: Need details on how much functionality is required

A piece of equipment is an entity that has one or more of the following:

-   Base item features (geometry, attributes, interactions)
-   Can be 'equipped' into an equipment slot on a character e.g. a weapon that can be equipped into left hand / right hand - a piece of chest armor that can be equipped to the chest and provides defensive attributes.

Inventory is a fictitious storage space that is able to contain a set number of entities, in particular items and weapons.

Ideal design will be three of more components that know little or nothing of each other. 

-   ItemComponent: add to entities that wish to work as items.
-   WeaponComponent: add to entities that wish to work as weapons.
-   EquipmentComponent: extra functionality needed to be added to a character in an equipment slot
-   InventoryComponent: functions needed to be added to inventory - this may be potentially moved into item / equipment is there is too little code.
-   InventoryManager, EquipmentManager, ItemManager?, WeaponManager?

There may be specializations of these.

# Premises

-   For an item to exist in the world space, it must be an entity. We cannot interact with static geometry or BasicEntity. An item must provide a minimum contact to be placed int the world.
-   Any item that is in the world space must provide some geometry to represent the item. Is it possible / advisable to use different geometry for the world to that when equipped?
-   Any item that is equippable must provide geometry to display when it is equipped.
-   An item may not lose instance data as it transitions from the world, to character, inventory or back.

# Known types of items

## Items

-   Flashlight / lantern
-   Currency
-   Mission item - not backed by anything except a token
-   Compass
-   Map
-   Spell book (perhaps offer spells not otherwise useable)
-   One off spell item e.g. fireworks, hand grenade

## Weapons

-   Knife
-   Sword
-   Pistol
-   Rifle
-   etc

## Armour

- Head
- Chest
- Legs
- Feet
- etc

# Misc

The recursion isn't needed in the struct. Instead, the UI should recurse any female, following through to it's male, and exposing any female sockets that are available from there.

Structs should be flat, and perhaps static.

Connect structs together with entityId, and some unique way to identify the snaplocks it connects to.

# Snaplock System (Ports)

A clear definition of items is needed, including which accessories can snap into which ports. This should be generic enough to use on weapons and vehicles as well.

Items should be able to present male and female ports, to both snap into other items and be snapped into in turn.

System should use GUIDs or ObjectIds to provide strong names for attachment, with a preference for GUIDs

We can query an object for a snaplock manager, if it has one, it can allow for attachments.

It might be a good idea to define a PORT_ROOT, a single shared name that is defined as being the root object onto which others attach themselves. Simple objects can use the ROOT for their geometry. More complex ones can avail themselves of snaps, which will tie extra geometry / functionality back to the core slots system.

## First thoughts

- IItemManager - handles all management e.g. adding / removing components
- IItem / CItem - the basic item, must be able to work with the ports system so allows snapping into compatible sockets
- IInventory / CInventory - management of items to an from the inventory. Serialize and de-serialise to storage.
- ISnaplockMale, ISnaplockFemale - generic port
- IWeapon / CWeapon - specialized versions of IItem / CItem
- Ammo - not sure
- ISnaplockManager - generic snaplock management, allows for query, attachment, detachment.

*   snaplock manager
*   *   male snaplock (0-x)
*   *   *   supports iteration
*   *   female snaplock (0-x)
*   *   *   supports iteration

Snaplocks should be capable of publishing events when they are utilized.

Is this just a clever way to wrap render slots? Should we allow snapping things that aren't entities e.g. fx. Check the existing entity FX and lighting handling code for ways to wrap that and fold it into this system.

## Combinations

### Characters

All the usual attachment points e.g. head, chest, shoulders, left hand, right hand.

### Items

Gun     male - left hand, right hand, holster
        female - scope, laser, magazine(?) 
Flashlight - same (perhaps only one hand)
All hand held items - same

Scope - Gunscope (male)
Laser target - Laser (male)

Turrets - (female) slot to attach to, male slot for the various weapons.

Offhand weapons (book, lantern) - one male slot for left hand

Generic item...none, unless it can be equipped into a hand.

Flashlight (male) left hand, right hand - (female) dynamic light? Not likely, since every flashlight would require a light.

Perhaps items cannot have a default (root) entry, since they really need it to be male or should we use a male root on these?

# Goals

Game Entry

-   A player enters the game and is attached to a specific character.
-   Optionally, a new character is spawned which the player is then attached onto.
-   The character may have gear on their person already.
-   Some of this gear might be dehydrated down to simple attachments like geometry e.g. a decorative item.
-   Some gear might need to be 'attached' to the character as a complete entity e.g. a weapon, flashlight

As the player plays the character / game, they may acquire new items / gear.

-   A character opens a chest and receives several rewards e.g. weapons, gold, quest items, items.
-   A character is able to equip any item from their inventory onto their equipment slots.
-   When a item is equipped it may make changes to character stats, state, whatever e.g. new chest armor increases / changes damage mitigation. A new weapon might give a bonus to hit / crit.
-   All state changes from item changes will need to be recalculated when any single piece of gear changes.

# Equipping an Item

When an item is equipped it can result in side effects for the entity which equips it.

-   change in primary / secondary attributes e.g. strength, hit and crit
-   access to new spells provided by the item e.g. wand blasts, throw grenade, icy blast every 5 minutes, whatever
-   these may apply to items placed into 'special' slots like quickslots for potions
-   is there a reasonable way to test for equipped items on quests e.g. you must wear a hazmat suit?

# Stories

## InventoryComponent

An inventory component can contain items, materials and weapon components. It should be able to store these in a dehydrated form, allow enumeration of it's contents, and provide facilities to manage (add, remove, delete, equip, de-equip) anything it can store.

Things stored here no longer need resources such as geometry loaded on their behalf. They should not take up an entity slot in the ECS.

Inventory containers should make it simple to transfer stored items between each other, as this is a very common requirement e.g. dumping your haul of loot into the bank.

Each item type must provide an icon to represent the item in storage.

Multiples of stackable items need to stack when added into inventory.

Inventory is sortable, even if only in the UI, though a deep sort of the container might be welcome.

It's unlikely I can implement ACID rules for items in the inventory, but it's worth being aware of the possibilities of errors in handling items leading to dupe exploits and the like.

Inventory provides storage for any currencies you have access to. Each inventory can set limits on the amount of currency it can store - potentially in weight as well as value.

Inventory may allow 0-x storage containers, each of them named, and with all the limits you'd expect e.g. slots, weight, etc.

### Loot (container)

Container is enumerated for items that can be looted by the character.

Optional: lootable items are displayed in a UI.

All lootable items (or selected) or moved from the container into the character inventory, if space permits. Looted item is removed from the container.

Empty loot containers are flagged for removal if required.

### Loot Autoloot (containers)

Perform the same actions as for Loot (container), but on every container in range of the auto-loot.

Optional: perform the loot action on each loot container that drops in the area, without requiring a user action.

### Loot Direct to Character

Any loot is fed directly to the character inventory component, which takes care of placing it safely in storage. This requires we supply an 'overflow' type of storage, for when the character is carrying more than they can handle in their inventory.

For each loot item, place into inventory. Overflow moves to special storage.

**Optional for all types** crafting materials go directly into crafting material storage. Potential for keeping large amounts on the character, or even unlimited amounts - though prefer to store reasonable amounts and have them buy a storage chest for their home.

### Use Item

A UI action indicates the character wishes to run the 'use' verb for an item in the inventory.

Identify the item, hydrate it to some executable form, attempt to execute it's 'use' method if there is one.

On Success: some action occurs

On Fail: notify the UI

### Delete Item

Check if the item can be deleted.

Delete item.

### Inspect

Out of scope of this work.

### Equip

(requires EquipmentComponent)

The item is taken out of inventory and is equipped onto the character, in it's default slot (e.g. chest for chest pieces, right hand for one handed weapons). If the slot is already taken, the old item is moved to the inventory. Character attributes are re-calculated.

### Deposit All (kind, into bank / chest)

Quick method to deposit all the materials, reagents, misc into the large inventory container.

Iterate character inventory for 'kind' and move the items into the large inventory storage.

### Purchase Vendor

Purchase x items from a vendor for y currency.

If the character has the required currency in their inventory, the currency is transferred to the vendor, the items are transferred to the character.

### Buyback Vendor

Keep short list of items that can be returned without penalty to a vendor for a complete refund.


### Iterate Inventory (kind)

Get a list of the inventory items, filtered by kind - where kind might be all sorts of things (tags) like race, class, reputation, fealty.


### Disenchant / etc

Special class specific actions which can be carried out. Needs investigation into how to support this.

### Co-operative actions

Actions that only make sense when in co-operation with another entity, like 'trade', 'forge', 'smelt', 'auction', 'mail', 'repair', 'repair item', etc.

Investigate.