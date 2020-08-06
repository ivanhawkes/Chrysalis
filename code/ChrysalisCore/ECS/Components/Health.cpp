#include <StdAfx.h>

#include "Health.h"
#include "CrySerialization/Enum.h"


namespace Chrysalis::ECS
{
SERIALIZATION_ENUM_BEGIN(DamageType, "Damage Type")
SERIALIZATION_ENUM(DamageType::acid, "acid", "Acid")
SERIALIZATION_ENUM(DamageType::bleed, "bleed", "Bleed")
SERIALIZATION_ENUM(DamageType::cold, "cold", "cold")
SERIALIZATION_ENUM(DamageType::collision, "collision", "collision")
SERIALIZATION_ENUM(DamageType::cleave, "cleave", "cleave")
SERIALIZATION_ENUM(DamageType::crush, "crush", "crush")
SERIALIZATION_ENUM(DamageType::decay, "decay", "decay")
SERIALIZATION_ENUM(DamageType::disease, "disease", "disease")
SERIALIZATION_ENUM(DamageType::electricity, "electricity", "electricity")
SERIALIZATION_ENUM(DamageType::energy, "energy", "energy")
SERIALIZATION_ENUM(DamageType::entropy, "entropy", "entropy")
SERIALIZATION_ENUM(DamageType::explosion, "explosion", "explosion")
SERIALIZATION_ENUM(DamageType::fire, "fire", "fire")
SERIALIZATION_ENUM(DamageType::holy, "holy", "holy")
SERIALIZATION_ENUM(DamageType::ice, "ice", "ice")
SERIALIZATION_ENUM(DamageType::nature, "nature", "nature")
SERIALIZATION_ENUM(DamageType::pierce, "pierce", "pierce")
SERIALIZATION_ENUM(DamageType::plasma, "plasma", "plasma")
SERIALIZATION_ENUM(DamageType::poison, "poison", "poison")
SERIALIZATION_ENUM(DamageType::qi, "qi", "qi")
SERIALIZATION_ENUM(DamageType::radiation, "radiation", "radiation")
SERIALIZATION_ENUM(DamageType::slash, "slash", "slash")
SERIALIZATION_ENUM(DamageType::tear, "tear", "tear")
SERIALIZATION_ENUM(DamageType::unholy, "unholy", "unholy")
SERIALIZATION_ENUM_END()
}
