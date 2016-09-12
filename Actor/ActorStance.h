#pragma once

// ***
// *** Character stances
// ***

// TODO: Document all this, re-work it to suit our game. Figure out what it does :D
// TODO: That array needs to be loaded with data at some point.

struct SActorStance
{
	// Dimensions.
	float heightCollider { 0.0f };
	float heightPivot { 0.0f };
	float groundContactEps { 0.025f };

	bool useCapsule = false;

	Vec3 size { 0.5f, 0.5f, 0.5f };

	// View.
	Vec3 viewOffset { ZERO };
	Vec3 leanLeftViewOffset { ZERO };
	Vec3 leanRightViewOffset { ZERO };
	Vec3 whileLeanedLeftViewOffset { ZERO };
	Vec3 whileLeanedRightViewOffset { ZERO };
	float viewDownYMod { 0.0f };

	Vec3 peekOverViewOffset { ZERO };
	Vec3 peekOverWeaponOffset { ZERO };

	// Weapon.
	Vec3 weaponOffset { ZERO };
	Vec3 leanLeftWeaponOffset { ZERO };
	Vec3 leanRightWeaponOffset { ZERO };
	Vec3 whileLeanedLeftWeaponOffset { ZERO };
	Vec3 whileLeanedRightWeaponOffset { ZERO };

	// Movement.
	float normalSpeed { 0.0f };
	float maxSpeed { 0.0f };

	// ???
	Vec3 modelOffset { ZERO };

	// Misc.
	char name [32] { "null" };


	inline Vec3	GetViewOffsetWithLean(float lean, float peekOver, bool useWhileLeanedOffsets = false) const
	{
		float peek = clamp_tpl(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (peekOverViewOffset - viewOffset);

		if (lean < -0.01f)
		{
			float a = clamp_tpl(-lean, 0.0f, 1.0f);
			return viewOffset + a * ((useWhileLeanedOffsets ? whileLeanedLeftViewOffset : leanLeftViewOffset) - viewOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp_tpl(lean, 0.0f, 1.0f);
			return viewOffset + a * ((useWhileLeanedOffsets ? whileLeanedRightViewOffset : leanRightViewOffset) - viewOffset) + peekOffset;
		}

		return viewOffset + peekOffset;
	}


	inline Vec3	GetWeaponOffsetWithLean(float lean, float peekOver, bool useWhileLeanedOffsets = false) const
	{
		float peek = clamp_tpl(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (peekOverWeaponOffset - weaponOffset);

		if (lean < -0.01f)
		{
			float a = clamp_tpl(-lean, 0.0f, 1.0f);
			return weaponOffset + a * ((useWhileLeanedOffsets ? whileLeanedLeftWeaponOffset : leanLeftWeaponOffset) - weaponOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp_tpl(lean, 0.0f, 1.0f);
			return weaponOffset + a * ((useWhileLeanedOffsets ? whileLeanedRightWeaponOffset : leanRightWeaponOffset) - weaponOffset) + peekOffset;
		}

		return weaponOffset + peekOffset;
	}


	static inline Vec3 GetOffsetWithLean(float lean, float peekOver, const Vec3& sOffset, const Vec3& sLeftLean, const Vec3& sRightLean, const Vec3& sPeekOffset)
	{
		float peek = clamp_tpl(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (sPeekOffset - sOffset);

		if (lean < -0.01f)
		{
			float a = clamp_tpl(-lean, 0.0f, 1.0f);
			return sOffset + a * (sLeftLean - sOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp_tpl(lean, 0.0f, 1.0f);
			return sOffset + a * (sRightLean - sOffset) + peekOffset;
		}

		return sOffset + peekOffset;
	}


	// Returns the size of the stance including the collider and the ground location.
	ILINE AABB GetStanceBounds() const
	{
		AABB aabb(-size, size);

		// Compensate for capsules.
		if (useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}

		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;

		// Include ground position.
		aabb.Add(Vec3(0, 0, 0));

		// Make relative to the entity.
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;

		return aabb;
	}


	// Returns the size of the collider of the stance.
	ILINE AABB	GetColliderBounds() const
	{
		AABB aabb(-size, size);

		// Compensate for capsules.
		if (useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}

		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;

		// Make relative to the entity.
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;

		return aabb;
	}
};
