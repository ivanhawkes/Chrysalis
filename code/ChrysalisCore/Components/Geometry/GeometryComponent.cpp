#include "StdAfx.h"

#include "GeometryComponent.h"
#include <CryPhysics/physinterface.h>
#include <CryAnimation/ICryAnimation.h>
#include <CrySerialization/Decorators/Resources.h>
#include <CrySerialization/Enum.h>


class CDRSInteractionEntityRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CGeometryComponent>("GeometryComponent", "Geometry", "physicsobject.bmp", true);
	}
};

CDRSInteractionEntityRegistrator g_geomEntityRegistrator;

YASLI_ENUM_BEGIN_NESTED(CGeometryComponent, EPhysicalizationType, "PhysicalizationType")
YASLI_ENUM_VALUE_NESTED(CGeometryComponent, ePhysicalizationType_None, "None")
YASLI_ENUM_VALUE_NESTED(CGeometryComponent, ePhysicalizationType_Static, "Static")
YASLI_ENUM_VALUE_NESTED(CGeometryComponent, ePhysicalizationType_Rigid, "Rigid")
YASLI_ENUM_END()

CRYREGISTER_CLASS(CGeometryComponent);


void CGeometryComponent::Initialize()
{
	GetEntity()->SetFlags(GetEntity()->GetFlags() | ENTITY_FLAG_CASTSHADOW);

	CDesignerEntityComponent::Initialize();
}


void CGeometryComponent::ProcessEvent(SEntityEvent& event)
{
	CDesignerEntityComponent::ProcessEvent(event);

	switch (event.event)
	{
		case ENTITY_EVENT_HIDE:
			break;

		case ENTITY_EVENT_UNHIDE:
			break;

		case ENTITY_EVENT_COLLISION:
		{
			// Collision info can be retrieved using the event pointer.
			EventPhysCollision *physCollision = reinterpret_cast<EventPhysCollision *>(event.nParam [0]);

			ISurfaceTypeManager* pSurfaceTypeManager = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager();
			if (ISurfaceType* pSurfaceType = pSurfaceTypeManager->GetSurfaceType(physCollision->idmat [1]))
			{
				string surfaceTypeName = pSurfaceType->GetName();
				//ActivateFlowNodeOutput(eOutputPort_CollisionSurfaceName, TFlowInputData(surfaceTypeName));
			}

			if (IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity [1]))
			{
				//ActivateFlowNodeOutput(eOutputPort_OnCollision, TFlowInputData(pOtherEntity->GetId()));
			}
			else
			{
				//ActivateFlowNodeOutput(eOutputPort_OnCollision, TFlowInputData());
			}
		}
		break;
	}
}


void CGeometryComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(Serialization::ModelFilename(m_model), "Geometry", "Geometry");
	archive(m_physicalizationType, "Physicalize", "Physicalize");
	archive(m_bReceiveCollisionEvents, "ReceiveCollisionEvents", "ReceiveCollisionEvents");
	archive(m_mass, "Mass", "Mass");

	if (archive.isInput())
	{
		OnResetState();
	}
}


void CGeometryComponent::SetGeometry(const char* szFilePath)
{
	m_model = szFilePath;
	OnResetState();
}


void CGeometryComponent::OnResetState()
{
	if (m_model.size() > 0)
	{
		// Load the model.
		m_slotId = LoadMesh(m_model, m_slotId);

		// Physicalise it if needed.
		SEntityPhysicalizeParams physicalizationParams;
		switch (m_physicalizationType)
		{
			case ePhysicalizationType_None:
				physicalizationParams.type = PE_NONE;
				break;

			case ePhysicalizationType_Static:
				physicalizationParams.type = PE_STATIC;
				break;

			case ePhysicalizationType_Rigid:
				physicalizationParams.type = PE_RIGID;
				break;
		}

		physicalizationParams.mass = m_mass;
		GetEntity()->Physicalize(physicalizationParams);
	}

	// Notify listeners.
	for (auto& pListener : m_ListenersList)
	{
		pListener->OnGeometryResetState();
	}
}
