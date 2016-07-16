#pragma once

#include <IGameObject.h>


class CLedgeObject : public CGameObjectExtensionHelper < CLedgeObject, IGameObjectExtension >
{
	struct LedgeProperties
	{
		LedgeProperties(const IEntity& entity);

		float ledgeCornerMaxAngle;
		float ledgeCornerEndAdjustAmount;
		bool ledgeFlipped;
		uint16 ledgeFlags_MainSide;
		uint16 ledgeFlags_OppositeSide;

	private:
		LedgeProperties();
	};

public:
	CLedgeObject();
	virtual ~CLedgeObject();

	// IGameObjectExtension
	virtual bool Init(IGameObject * pGameObject);
	virtual void InitClient(int channelId) {};
	virtual void PostInit(IGameObject * pGameObject);
	virtual void PostInitClient(int channelId) {};
	virtual bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params);
	virtual void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) {}
	virtual bool GetEntityPoolSignature(TSerialize signature);
	virtual void Release();
	virtual void FullSerialize(TSerialize ser) {};
	virtual bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags) { return false; };
	virtual void PostSerialize() {};
	virtual void SerializeSpawnInfo(TSerialize ser) {}
	virtual ISerializableInfoPtr GetSpawnInfo() { return 0; }
	virtual void Update(SEntityUpdateContext& ctx, int slot) {};
	virtual void HandleEvent(const SGameObjectEvent& gameObjectEvent);
	virtual void ProcessEvent(SEntityEvent& entityEvent);
	virtual void SetChannelId(uint16 id) {};
	virtual void SetAuthority(bool auth) {};
	virtual void PostUpdate(float frameTime) { CRY_ASSERT(false); }
	virtual void PostRemoteSpawn() {};
	virtual void GetMemoryUsage(ICrySizer *pSizer) const;
	// ~IGameObjectExtension


	// ***
	// *** CLedge
	// ***

protected:
	virtual bool IsStatic() const { return false; }

private:
	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent() calls).
	*/
	void RegisterEvents();


	void UpdateLocation();
	void ComputeLedgeMarkers();

	ILINE bool IsFlipped() const { return m_flipped; };

	bool m_flipped;
};