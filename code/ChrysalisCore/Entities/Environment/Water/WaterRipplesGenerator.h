/**
\file	Environment\Water\WaterRipplesGenerator.h

Generates water ripples when moving across a water surface
*/
#pragma once

#include <IGameObject.h>


#define WATER_RIPPLES_EDITING_ENABLED 1


class CWaterRipplesGenerator : public CGameObjectExtensionHelper < CWaterRipplesGenerator, IGameObjectExtension >
{
public:
	CWaterRipplesGenerator();
	virtual ~CWaterRipplesGenerator();

	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const  override { pSizer->Add(*this); };
	bool Init(IGameObject *pGameObject) override;
	void PostInit(IGameObject *pGameObject) override;
	void InitClient(int channelId) override {};
	void PostInitClient(int channelId) override {};
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override {};
	bool GetEntityPoolSignature(TSerialize signature) override;
	void Release() override;
	void FullSerialize(TSerialize ser) override;
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) { return true; }
	void PostSerialize()  override {};
	void SerializeSpawnInfo(TSerialize ser)  override {};
	ISerializableInfoPtr GetSpawnInfo() override { return 0; }
	void Update(SEntityUpdateContext &ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent &gameObjectEvent) override;
	void ProcessEvent(SEntityEvent &) override;
	void SetChannelId(uint16 id) override {};
	// FIX: 5.4
	//void SetAuthority(bool auth) override {};
	void PostUpdate(float frameTime) override {};
	void PostRemoteSpawn() override {};

	// ***
	// *** ~IGameObjectExtension
	// ***

	virtual void ProcessHit(bool isMoving);

private:
	void RegisterEvents(IGameObjectExtension& goExt, IGameObject& gameObject);
	void Reset();
	void ActivateGeneration(const bool activate);

	float m_scale;
	float m_strength;
	float m_frequency;
	float m_randFrequency;
	float m_randScale;
	float m_randStrength;
	bool m_enabled;
	bool m_autoSpawn;
	bool m_spawnOnMovement;
	Vec2 m_randomOffset;

	// TODO: Update the way time is handled.
	float m_lastSpawnTime;

#if WATER_RIPPLES_EDITING_ENABLED
	bool TestLocation(const Vec3& testPosition);
	bool m_currentLocationOk;
#endif
};
