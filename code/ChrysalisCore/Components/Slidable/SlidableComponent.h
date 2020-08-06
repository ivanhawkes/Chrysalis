#pragma once


/**
Allows an entity to slide when interacted with.
**/
namespace Chrysalis
{
enum class SlideState
{
	initialPosition,
	inMotion,
	finalPosition,
};


class CSlidableComponent
	: public IEntityComponent
{
public:
	CSlidableComponent() {}
	virtual ~CSlidableComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSlidableComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{64633992-4907-40DA-9467-71791F14D8A9}"_cry_guid;
		return id;
	}


	virtual void Serialize(Serialization::IArchive& ar) override
	{
		Serialization::SContext context(ar, this);
		ar(m_SliderProperties, "slide-properties", "Slide Properties");
	}


	struct SSliderProperties
	{
		inline bool operator==(const SSliderProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }


		static void ReflectType(Schematyc::CTypeDesc<SSliderProperties>& desc)
		{
			desc.SetGUID("{D808A994-FA5A-43B7-BF7D-70C58639C4C0}"_cry_guid);
			desc.SetLabel("Slider Properties");
			desc.SetDescription("Slider Properties.");
			desc.AddMember(&CSlidableComponent::SSliderProperties::rail, 'rail', "Rail", "Rail", "Slide movement will be performed along a virtual rail using objectspace for the vector.", Vec3(0.0f, 0.3f, 0.0f));
		}


		virtual void Serialize(Serialization::IArchive& ar)
		{
			//ICharacterInstance* pCharacterInstance { nullptr };
			// 
			//if (const CSlidableComponent* jointContext = ar.context<CSlidableComponent>())
			//{
			//	pCharacterInstance = jointContext->GetCharacter();
			//}

			//Serialization::SContext characterInstanceContext(ar, pCharacterInstance);
		}

		Vec3 rail{ 0.0f, 0.3f, 0.0f };
		//Schematyc::Range<0, 360> needleValue = 0.0f;
	};

	virtual void Update(SEntityUpdateContext* pCtx);


	/** A signal which is sent when the object has moved to it's initial position. */
	struct SStateInitialSignal
	{
		SStateInitialSignal() = default;
	};


	/** A signal which is sent when the object is in motion. */
	struct SStateInMotionSignal
	{
		SStateInMotionSignal() = default;
	};


	/** A signal which is sent when the object has moved to it's final position. */
	struct SStateFinalSignal
	{
		SStateFinalSignal() = default;
	};



protected:
	SSliderProperties m_SliderProperties;
};
}