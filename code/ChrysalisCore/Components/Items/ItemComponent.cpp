#include <StdAfx.h>

#include "ItemComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <Components/Snaplocks/SnaplockComponent.h>


namespace Chrysalis
{
static void RegisterItemComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CItemComponent));
		// Functions
		{
		}
	}
}


void CItemComponent::ReflectType(Schematyc::CTypeDesc<CItemComponent>& desc)
{
	desc.SetGUID(CItemComponent::IID());
	desc.SetEditorCategory("Items");
	desc.SetLabel("Item");
	desc.SetDescription("Base functionality for an item.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::None});

	desc.AddMember(&CItemComponent::m_item, 'item', "ItemProperties", "Item Properties", "desc", IItem());
	desc.AddMember(&CItemComponent::m_itemClassCollection, 'clsc', "ItemClassCollection", "ItemClassCollection", "A collection of item classes.", IItemClassCollection());
}


void CItemComponent::Initialize()
{
	// Manage our snaplocks.
	m_pSnaplockComponent = m_pEntity->GetOrCreateComponent<CSnaplockComponent>();

	// Provide them with an effects controller for this entity.
	m_effectsController.Init(GetEntityId());

	// Get it into a known state.
	OnResetState();
}


void CItemComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case EEntityEvent::LevelStarted:

		// Editor specific, physicalize on reset, property change or transform change
		case EEntityEvent::Reset:
		case EEntityEvent::EditorPropertyChanged:
		case EEntityEvent::TransformChangeFinishedInEditor:
			OnResetState();
			break;
	}
}


void CItemComponent::OnResetState()
{
}


// ***
// *** OWNERSHIP
// ***



// #TODO: IMPORTANT - Go through all the code to handle ownership of items.


// ***
// *** HOLDING
// ***


void CItemComponent::EnablePicking(bool enable, bool dropped)
{
	if (enable)
	{
		m_itemStatus.flying = dropped;
		m_itemStatus.dropped = true;
		m_itemStatus.pickable = true;

		//if (dropped)
		//{
		//	GetEntity()->KillTimer(eIT_Flying);
		//	GetEntity()->SetTimer(eIT_Flying, 750);
		//}

		//if (GetEntity()->IsSlotValid(eIGS_Aux0))
		//{
		//	DrawSlot(eIGS_Aux0, true);
		//	DrawSlot(eIGS_FirstPerson, false);
		//	DrawSlot(eIGS_ThirdPerson, false);
		//}
	}
	else
	{
		//if (GetEntity()->IsSlotValid(eIGS_Aux0))
		//{
		//	DrawSlot(eIGS_Aux0, false);
		//}

		m_itemStatus.flying = false;
		m_itemStatus.pickable = false;
	}
}


void CItemComponent::PickUp(EntityId actorId, bool sound, bool select, bool keepHistory, const char* setup)
{
	//IActor* pActor = CActorComponent::GetActor(actorId);
	//if (!pActor)
	//	return;

	//if (pActor->IsPlayer())
	//	UnlockDefaultAttachments(pActor);

	//CCCPOINT_IF(sound &&  select, Item_PickUpAndSelect_PlaySound);
	//CCCPOINT_IF(sound && !select, Item_PickUpAndStore_PlaySound);
	//CCCPOINT_IF(!sound &&  select, Item_PickUpAndSelect_NoSound);
	//CCCPOINT_IF(!sound && !select, Item_PickUpAndStore_NoSound);

	//IEntity* pPickerEntity = pActor->GetEntity();

	//TriggerRespawn();

	//GetEntity()->EnablePhysics(false);
	//Physicalize(false, false);

	//EnablePicking(false, false);

	//bool soundEnabled = IsSoundEnabled();
	//EnableSound(sound);

	//SetViewMode(0);
	//SetOwnerId(pickerId);

	//CopyRenderFlags(GetOwner());

	//Hide(true);
	//m_itemStatus.dropped = false;
	//m_itemStatus.brandnew = false;
	//m_itemStatus.detached = false;

	//// move the entity to picker position
	//Matrix34 tm(pPickerEntity->GetWorldTM());
	//tm.AddTranslation(Vec3(0, 0, 2));
	//GetEntity()->SetWorldTM(tm);

	//IInventory *pInventory = pActor->GetInventory();
	//if (!pInventory)
	//{
	//	GameWarning("Actor '%s' has no inventory, when trying to pickup '%s'!", pPickerEntity->GetName(), GetEntity()->GetName());
	//	return;
	//}

	//const char* itemClassName = GetEntity()->GetClass()->GetName();
	//bool itemClassNotInInventory = (pInventory->GetCountOfClass(itemClassName) == 0);

	//PickUpAccessories(pActor, pInventory);

	//OnPickedUp(pickerId, m_sharedparams->params.unique && !itemClassNotInInventory);
	//
	//// NOTE: some weapons will get ammo when picked up. This will dictate the result of CanSelect() below so we'll grab
	//// the ammo before trying to select

	//if (itemClassNotInInventory || !m_sharedparams->params.unique)
	//{
	//	// add to inventory
	//	pInventory->AddItem(GetEntity()->GetId());

	//	if (select && itemClassNotInInventory)
	//	{
	//		//Note [Benito]:
	//		// Some of this checks here fix some issues with Save/Load, some code in CItemComponent::PostSerialize messes up 
	//		// the inventory serialization by calling Drop/Pick, because the order in which the items are serialized can be any.
	//		// Not sure behind the reasons for such a code in PostSerialize
	//		const bool hasHeavyWeaponEquipped = (pActor->GetActorClass() == CPlayerComponent::GetActorClassType()) ? static_cast<CPlayerComponent*>(pActor)->HasHeavyWeaponEquipped() : false;

	//		const bool shouldCheck = (pActor->IsClient() || gEnv->bServer);
	//		const bool allowSelectOnPickUp = !pActor->GetLinkedVehicle() && !pActor->IsSwimming() && (pActor->GetGrabbedEntityId() == 0) && !hasHeavyWeaponEquipped;
	//		if (!shouldCheck || allowSelectOnPickUp)
	//		{
	//			if (CanSelect())
	//			{
	//				m_pItemSystem->SetActorItem(pActor, GetEntity()->GetId(), keepHistory);
	//			}
	//			else
	//			{
	//				m_pItemSystem->SetActorAccessory(pActor, GetEntity()->GetId(), keepHistory);
	//			}

	//			CHANGED_NETWORK_STATE(pActor, CPlayerComponent::ASPECT_CURRENT_ITEM);
	//		}
	//	}

	//	EnableSound(soundEnabled);

	//	if (gEnv->pGameFramework->GetGameRules())
	//	{
	//		gEnv->pGameFramework->GetGameRules()->OnItemPickedUp(GetEntity()->GetId(), pickerId);
	//	}

	//	if (select)
	//	{
	//		PlayAction(GetFragmentIds().pickedup);
	//	}

	//	//AI back weapon attachments
	//	if (!IsSelected())
	//	{
	//		AttachToBack(true);
	//	}
	//}
	//else if (m_sharedparams->params.unique && !itemClassNotInInventory)
	//{
	//	if (gEnv->bServer && gEnv->pGameFramework->GetGameRules())
	//	{
	//		gEnv->pGameFramework->GetGameRules()->OnItemPickedUp(GetEntityId(), pickerId);
	//	}

	//	if (gEnv->bServer && !IsDemoPlayback())
	//	{
	//		RemoveEntity();
	//	}

	//	if (select)
	//	{
	//		PlayAction(GetFragmentIds().pickedup);
	//	}
	//}

	//if (gEnv->bServer && pActor)
	//{
	//	//--- We reset the client's inventories at the point where we add the first item to the server inventory
	//	//--- this ensures that they are all kept in synch
	//	GetGameObject()->SetNetworkParent(pickerId);
	//	if ((GetEntity()->GetFlags()&(ENTITY_FLAG_CLIENT_ONLY | ENTITY_FLAG_SERVER_ONLY)) == 0)
	//	{
	//		pActor->GetGameObject()->InvokeRMIWithDependentObject(CActorComponent::ClPickUp(),
	//			CActorComponent::PickItemParams(GetEntity()->GetId(), m_itemStatus.selected, sound, false),
	//			eRMI_ToAllClients | eRMI_NoLocalCalls, GetEntity()->GetId());
	//	}
	//}

	//if (!gEnv->bMultiplayer)
	//{
	//	int numAccessories = m_accessories.size();
	//	for (int i = 0; i < numAccessories; ++i)
	//	{
	//		const SAccessoryParams* pParams = GetAccessoryParams(m_accessories [i].pClass);
	//		if (pParams->extendsMagazine)
	//		{
	//			const char* name = m_accessories [i].pClass->GetName();
	//			AttachAccessory(m_accessories [i].pClass, false, true, true);
	//			DoSwitchAccessory(name);
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	//add any attachemnts on the weapon to the "this life's allowed detachments"
	//	if (pActor->GetEntityId() == gEnv->pGameFramework->GetClientActorId())
	//	{
	//		if (CEquipmentLoadout* pLoadout = gEnv->pGameFramework->GetEquipmentLoadout())
	//		{
	//			int numAccessories = m_accessories.size();
	//			for (int i = 0; i < numAccessories; ++i)
	//			{
	//				pLoadout->UpdateWeaponAttachments(GetEntity()->GetClass(), m_accessories [i].pClass);
	//			}
	//		}
	//	}
	//}
}


// ***
// *** CItemComponent
// ***

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterItemComponent)
}