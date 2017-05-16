#pragma once

#include <SharedParameters/SharedParameters.h>


class XmlNodeRef;


struct SItemBaseParameter : public ISharedParams
{
	SHARED_PARAMS_BODY(SItemBaseParameter);

	SItemBaseParameter();
	~SItemBaseParameter();


	// ***
	// *** Base Parameters - common for most items.
	// ***

	/** Used by Mannequin as an additional tag "weaponType". */
	string itemClass;

	/** The name to display in the UI. This should be a localised string. */
	string displayName;

	/** True if this entity can be selected in the player's inventory. */
	bool isSelectable { true };

	/** Is this entity droppable? */
	bool isDroppable { true };

	/** Is this entity droppable automatically? */
	bool isAutoDroppable { false };

	/** true if this object is able to be picked up. */
	bool isPickable { true };

	/** true if this object is automatically able to be picked up. */
	bool isAutoPickable { true };

	/** true if this object can be mounted. */
	bool isMountable { true };

	/** true if this object is usable. */
	bool isUsable { true };

	/** Is this entity able to be bought / picked up / given / traded to a character? */
	bool isGiveable { true };

	/** true if this object is usable under water. */
	bool isUsableUnderWater { false };

	/** Is this entity consumable? */
	bool isConsumable { false };

	/** The weight of the item when dropped. */
	float mass { 1.0f };

	/** The impulse to apply when the item is dropped. */
	float dropImpulse;

	/** Should the entity be removed from the game when it's dropped. */
	bool shouldRemoveOnDrop { false };

	/** Multiplier for how long it takes to select a given item. */
	float selectTimeMultiplier { 1.0f };


	// ***
	// *** Weapon Related
	// ***

	/** Is this entity a weapon? */
	bool isWeapon { false };

	/** Is this object a heavy weapon? */
	bool isHeavyWeapon { false };

	/** Can this object overcharge / overheat through use? */
	bool canOvercharge { false };

	/** Time delay to automatically reload the weapon on an empty magazine. */
	float autoReloadDelay { 0.5f };

	/** The scope attachment. Assumption that this is the slot in which to add the scope, if it should be one. */
	int scopeAttachment { 0 };

	/** Time delay from when the item is selected to when it is usable. */
	float selectOverride { 1.0f };


	// ***
	// *** Accesory Related
	// ***


	/** true if this item is unique. */
	bool isUnique { true };

	/** true if the attachment gives ammo. */
	bool doesAttachmentGiveAmmo { false };


	// ***
	// *** Ledges
	// ***

	/** Can this entity perform a ledge grab? */
	bool canLedgeGrab { false };


	// ***
	// *** Character attachment and animation / sound.
	// ***

	/** A tag(s) to apply to mannequin when this item is in use. */
	string	tag;

	/** Visually attaches the item to the players back in third person view, whenn ot in use. */
	bool isAttachedToBack { false };

	/** Time delay to prevent player from firing after sprinting stopped. */
	float sprintToFireDelay { 0.1f };

	/** Time delay to prevent player from zooming after sprinting stopped. */
	float sprintToZoomDelay { 0.2f };

	/** Time delay to prevent player from melee after sprinting stopped. */
	float sprintToMeleeDelay { 0.4f };

	/** The run to sprint blend time. */
	float runToSprintBlendTime { 0.3f };

	/** The sprint to run blend time. */
	float sprintToRunBlendTime { 0.4f };


	//string	aiAttachment[IItem::eIH_Last];
	//
	//string	attachment[IItem::eIH_Last];

	//string	adbFile;
	//
	//string	soundAdbFile;
	//
	//string  actionControllerFile;


	//Vec3 fpOffset;
	//
	//Quat fpRotationOffset;

	//bool hasAimAnims;
	//
	//float ironsightAimAnimFactor;


	// ***
	// *** Misc
	// ***


	// ***
	// *** Unknown at this time or awaiting implementation.
	// ***

	//SWeaponStatsData weaponStats;
	//SAimAnimsBlock aimAnims;
	//SAimAnimsBlock mountedAimAnims;
	//SAimLookParameters aimLookParams;
	//string mountedTPAimAnims[MountedTPAimAnim::Total];
	//bool has_first_select { false;
	//bool fast_select { false;
	//bool select_delayed_grab_3P { false;
	//bool check_clip_size_after_drop;
	//bool check_bonus_ammo_after_drop;

	//float zoomTimeMultiplier;
	//int hudStyle { 0;
	//string crosshairTexture;


	// ***
	// *** Deprecated or not needed for our project.
	// ***

	//bool can_rip_off { true;


	/** Resets this object to it's default state. */
	void OnResetState();

	/**
	Reads the given node.

	\param	node	The node to read.

	\return	true if it succeeds, false if it fails.
	*/
	bool Read(const XmlNodeRef& node);
};

DECLARE_SHARED_POINTERS(SItemBaseParameter);
