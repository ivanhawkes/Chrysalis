cmake_minimum_required (VERSION 3.6.0)
project (Chrysalis VERSION 1.0 LANGUAGES CXX)

if (${CMAKE_GENERATOR} MATCHES "^Visual Studio")
	ADD_DEFINITIONS("/ZI")
	configure_file ("${PROJECT_NAME}.vcxproj.user.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

find_package (CryCommon REQUIRED)
find_package (CryAction REQUIRED)

#START-FILE-LIST

set (OPTION_UNITY_BUILD ON CACHE BOOL "Enable Unity builds")
set (OPTION_PCH ON CACHE BOOL "Enable Precompiled headers")


#
# Mostly generic game code.
#

set (SourceGroup_Main
	"StdAfx.cpp"
	"StdAfx.h"
)
source_group("Main" FILES ${SourceGroup_Main})

set (SourceGroup_Plugin
	"Plugin/ChrysalisCore.cpp"
	"Plugin/ChrysalisCore.h"
	"Plugin/ChrysalisCorePlugin.cpp"
	"Plugin/ChrysalisCorePlugin.h"
)
source_group("Plugin" FILES ${SourceGroup_Plugin})

set (SourceGroup_Game
	"Game/GameRules.cpp"
	"Game/GameRules.h"
)
source_group("Game" FILES ${SourceGroup_Game})

#set (SourceGroup_WeaponEntities
#	"Entities/Gameplay/Weapons/Bullet.cpp"
#	"Entities/Gameplay/Weapons/Bullet.h"
#	"Entities/Gameplay/Weapons/ISimpleWeapon.h"
#	"Entities/Gameplay/Weapons/Rifle.cpp"
#	"Entities/Gameplay/Weapons/Rifle.h"
#)
#source_group("Entities\\Gameplay\\Weapons" FILES ${SourceGroup_WeaponEntities})

#set (SourceGroup_GameplayEntities
#	"Entities/Gameplay/SpawnPoint.cpp"
#	"Entities/Gameplay/SpawnPoint.h"
#)
#source_group("Entities\\Gameplay" FILES ${SourceGroup_GameplayEntities})

set (SourceGroup_EntityHelpers
	"Entities/Helpers/ISimpleExtension.h"
)
source_group("Entities\\Helpers" FILES ${SourceGroup_EntityHelpers})

set (SourceGroup_Player
	"Player/Player.cpp"
	"Player/Player.h"
)
source_group("Player" FILES ${SourceGroup_Player})


#
# Chrysalis specific code.
#

set (SourceGroup_Actor
	"Actor/Actor.cpp"
	"Actor/Actor.h"
	"Actor/ActorPhysics.h"
	"Actor/ActorStance.h"
	"Actor/ActorState.h"
	"Actor/Fate.h"
	"Actor/IActorEventListener.h"
	"Actor/ISimpleActor.h"
)
source_group("Actor" FILES ${SourceGroup_Actor})

set (SourceGroup_Actor_Animation
	"Actor/Animation/ActorAnimation.cpp"
	"Actor/Animation/ActorAnimation.h"
	"Actor/Animation/ActorAnimationEvents.cpp"
	"Actor/Animation/ActorAnimationEvents.h"
)
source_group("Actor\\Animation" FILES ${SourceGroup_Actor_Animation})

set (SourceGroup_Actor_Animation_Actions
	"Actor/Animation/Actions/ActorAnimationActionAiming.cpp"
	"Actor/Animation/Actions/ActorAnimationActionAiming.h"
	"Actor/Animation/Actions/ActorAnimationActionAimPose.cpp"
	"Actor/Animation/Actions/ActorAnimationActionAimPose.h"
	"Actor/Animation/Actions/ActorAnimationActionCooperative.cpp"
	"Actor/Animation/Actions/ActorAnimationActionCooperative.h"
	"Actor/Animation/Actions/ActorAnimationActionEmote.cpp"
	"Actor/Animation/Actions/ActorAnimationActionEmote.h"
	"Actor/Animation/Actions/ActorAnimationActionInteration.cpp"
	"Actor/Animation/Actions/ActorAnimationActionInteration.h"
	"Actor/Animation/Actions/ActorAnimationActionLocomotion.cpp"
	"Actor/Animation/Actions/ActorAnimationActionLocomotion.h"
	"Actor/Animation/Actions/ActorAnimationActionLooking.cpp"
	"Actor/Animation/Actions/ActorAnimationActionLooking.h"
	"Actor/Animation/Actions/ActorAnimationActionLookPose.cpp"
	"Actor/Animation/Actions/ActorAnimationActionLookPose.h"
)
source_group("Actor\\Animation\\Actions" FILES ${SourceGroup_Actor_Animation_Actions})

set (SourceGroup_Actor_Character
	"Actor/Character/Character.cpp"
	"Actor/Character/Character.h"
	"Actor/Character/CharacterAttributes.cpp"
	"Actor/Character/CharacterAttributes.h"
	"Actor/Character/CharacterAttributesComponent.cpp"
	"Actor/Character/CharacterAttributesComponent.h"
)
source_group("Actor\\Character" FILES ${SourceGroup_Actor_Character})

set (SourceGroup_Actor_Character_Movement
	"Actor/Character/Movement/CharacterMovementController.h"
	"Actor/Character/Movement/CharacterMovementController.cpp"
	"Actor/Character/Movement/CharacterRotation.cpp"
	"Actor/Character/Movement/CharacterRotation.h"
)
source_group("Actor\\Character\\Movement" FILES ${SourceGroup_Actor_Character_Movement})

set (SourceGroup_Actor_Character_Movement_StateMachine
	"Actor/Character/Movement/StateMachine/CharacterStateAIMovement.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateAnimationControlled.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateDead.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateDead.h"
	"Actor/Character/Movement/StateMachine/CharacterStateEntry.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateEvents.h"
	"Actor/Character/Movement/StateMachine/CharacterStateFly.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateFly.h"
	"Actor/Character/Movement/StateMachine/CharacterStateGround.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateGround.h"
	"Actor/Character/Movement/StateMachine/CharacterStateJump.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateJump.h"
	"Actor/Character/Movement/StateMachine/CharacterStateLadder.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateLadder.h"
	"Actor/Character/Movement/StateMachine/CharacterStateLedge.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateLedge.h"
	"Actor/Character/Movement/StateMachine/CharacterStateLinked.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateMovement.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateSpectate.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateSpectate.h"
	"Actor/Character/Movement/StateMachine/CharacterStateSwim.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateSwim.h"
	"Actor/Character/Movement/StateMachine/CharacterStateSwimWaterTestProxy.cpp"
	"Actor/Character/Movement/StateMachine/CharacterStateSwimWaterTestProxy.h"
)
source_group("Actor\\Character\\Movement\\StateMachine" FILES ${SourceGroup_Actor_Character_Movement_StateMachine})

set (SourceGroup_Actor_Inventory
	"Actor/Inventory/EntityInventory.cpp"
	"Actor/Inventory/EntityInventory.h"
)
source_group("Actor\\Inventory" FILES ${SourceGroup_Actor_Inventory})

#set (SourceGroup_Actor_Mount
#	"Actor/Mount/Mount.cpp"
#	"Actor/Mount/Mount.h"
#)
#source_group("Actor\\Mount" FILES ${SourceGroup_Actor_Mount})

set (SourceGroup_Actor_Movement
	"Actor/Movement/ActorMovementController.h"
	"Actor/Movement/ActorMovementController.cpp"
)
source_group("Actor\\Movement" FILES ${SourceGroup_Actor_Movement})

set (SourceGroup_Actor_Movement_StateMachine
	"Actor/Movement/StateMachine/ActorStateEvents.h"
	"Actor/Movement/StateMachine/ActorStateUtility.cpp"
	"Actor/Movement/StateMachine/ActorStateUtility.h"
)
source_group("Actor\\Movement\\StateMachine" FILES ${SourceGroup_Actor_Movement_StateMachine})

#set (SourceGroup_Actor_Pet
#	"Actor/Pet/Pet.cpp"
#	"Actor/Pet/Pet.h"
#)
#source_group("Actor\\Pet" FILES ${SourceGroup_Actor_Pet})

set (SourceGroup_Animation
	"Animation/Animation.cpp"
	"Animation/Animation.h"
)
source_group("Animation" FILES ${SourceGroup_Animation})

#set (SourceGroup_Animation_ProceduralContext
#	"Animation/ProceduralContext/ProceduralContextAim.cpp"
#	"Animation/ProceduralContext/ProceduralContextAim.h"
#	"Animation/ProceduralContext/ProceduralContextColliderMode.cpp"
#	"Animation/ProceduralContext/ProceduralContextColliderMode.h"
#	"Animation/ProceduralContext/ProceduralContextHelpers.h"
#	"Animation/ProceduralContext/ProceduralContextLook.cpp"
#	"Animation/ProceduralContext/ProceduralContextLook.h"
#	"Animation/ProceduralContext/ProceduralContextMovementControlMethod.cpp"
#	"Animation/ProceduralContext/ProceduralContextMovementControlMethod.h"
#	"Animation/ProceduralContext/ProceduralContextRagdoll.cpp"
#	"Animation/ProceduralContext/ProceduralContextRagdoll.h"
#)
#source_group("Animation\\ProceduralContext" FILES ${SourceGroup_Animation_ProceduralContext})

#set (SourceGroup_Boids
#	"Boids/BoidCollision.cpp"
#	"Boids/BoidCollision.h"
#	"Boids/BoidObject.cpp"
#	"Boids/BoidObject.h"
#	"Boids/BoidsProxy.cpp"
#	"Boids/BoidsProxy.h"
#	"Boids/Flock.cpp"
#	"Boids/Flock.h"
#)
#source_group("Boids" FILES ${SourceGroup_Boids})

#set (SourceGroup_Boids_Amphibian
#	"Boids/Amphibian/FrogBoids.cpp"
#	"Boids/Amphibian/FrogBoids.h"
#	"Boids/Amphibian/TurtleBoid.cpp"
#	"Boids/Amphibian/TurtleBoid.h"
#)
#source_group("Boids\\Amphibian" FILES ${SourceGroup_Boids_Amphibian})

#set (SourceGroup_Boids_Aquatic
#	"Boids/Aquatic/BoidFish.cpp"
#	"Boids/Aquatic/BoidFish.h"
#	"Boids/Aquatic/FishFlock.cpp"
#	"Boids/Aquatic/FishFlock.h"
#)
#source_group("Boids\\Aquatic" FILES ${SourceGroup_Boids_Aquatic})

#set (SourceGroup_Boids_Avian
#	"Boids/Avian/AvianEnum.h"
#	"Boids/Avian/AvianFlock.cpp"
#	"Boids/Avian/AvianFlock.h"
#	"Boids/Avian/BoidAvian.cpp"
#	"Boids/Avian/BoidAvian.h"
#	"Boids/Avian/ChickenBoids.cpp"
#	"Boids/Avian/ChickenBoids.h"
#)
#source_group("Boids\\Avian" FILES ${SourceGroup_Boids_Avian})

#set (SourceGroup_Boids_Terran
#	"Boids/Terran/BugsFlock.cpp"
#	"Boids/Terran/BugsFlock.h"
#)
#source_group("Boids\\Terran" FILES ${SourceGroup_Boids_Terran})

set (SourceGroup_Components_Animation
	"Components/Animation/ControlledAnimationComponent.cpp"
	"Components/Animation/ControlledAnimationComponent.h"
	"Components/Animation/SimpleAnimationComponent.cpp"
	"Components/Animation/SimpleAnimationComponent.h"
)
source_group("Components\\Animation" FILES ${SourceGroup_ComponentsAnimation})

set (SourceGroup_Components_Equipment
	"Components/Equipment/EquipmentComponent.cpp"
	"Components/Equipment/EquipmentComponent.h"
)
source_group("Components\\Equipment" FILES ${SourceGroup_Components_Equipment})

set (SourceGroup_Components_Geometry
	"Components/Geometry/GeometryComponent.cpp"
	"Components/Geometry/GeometryComponent.h"
)
source_group("Components\\Geometry" FILES ${SourceGroup_Components_Geometry})

set (SourceGroup_Components_Interaction
	"Components/Interaction/DRSInteractionComponent.cpp"
	"Components/Interaction/DRSInteractionComponent.h"
	"Components/Interaction/EntityAwarenessComponent.cpp"
	"Components/Interaction/EntityAwarenessComponent.h"
	"Components/Interaction/EntityInteractionComponent.cpp"
	"Components/Interaction/EntityInteractionComponent.h"
	"Components/Interaction/ItemInteractionComponent.cpp"
	"Components/Interaction/ItemInteractionComponent.h"
)
source_group("Components\\Interaction" FILES ${SourceGroup_Components_Interaction})

set (SourceGroup_Components_Inventory
	"Components/Inventory/InventoryComponent.cpp"
	"Components/Inventory/InventoryComponent.h"
)
source_group("Components\\Inventory" FILES ${SourceGroup_Components_Inventory})

set (SourceGroup_Components_Lights
	"Components/Lights/DynamicLightComponent.cpp"
	"Components/Lights/DynamicLightComponent.h"
)
source_group("Components\\Lights" FILES ${SourceGroup_Components_Lights})

set (SourceGroup_Components_Lockable
	"Components/Lockable/KeyringComponent.cpp"
	"Components/Lockable/KeyringComponent.h"
	"Components/Lockable/LockableComponent.cpp"
	"Components/Lockable/LockableComponent.h"
)
source_group("Components\\Lockable" FILES ${SourceGroup_Components_Lockable})

set (SourceGroup_Components_Openable
	"Components/Openable/ContainerComponent.cpp"
	"Components/Openable/ContainerComponent.h"
)
source_group("Components\\Openable" FILES ${SourceGroup_Components_Openable})

set (SourceGroup_Components_Particles
	"Components/Particles/ParticleComponent.cpp"
	"Components/Particles/ParticleComponent.h"
)
source_group("Components\\Particles" FILES ${SourceGroup_Components_Particles})

set (SourceGroup_Components_Snaplocks
	"Components/Snaplocks/SnaplockComponent.h"
	"Components/Snaplocks/SnaplockComponent.cpp"
)
source_group("Components\\Snaplocks" FILES ${SourceGroup_Components_Snaplocks})

set (SourceGroup_Components_Switchable
#	"Components/Switchable/SwitchableComponent.cpp"
#	"Components/Switchable/SwitchableComponent.h"
	"Components/Switchable/SwitchComponent.cpp"
	"Components/Switchable/SwitchComponent.h"
)
source_group("Components\\Switchable" FILES ${SourceGroup_Components_Switchable})

set (SourceGroup_ConsoleCommands
	"ConsoleCommands/ConsoleCommands.h"
	"ConsoleCommands/ConsoleCommands.cpp"
)
source_group("ConsoleCommands" FILES ${SourceGroup_ConsoleCommands})

set (SourceGroup_DynamicResponseSystem
	"DynamicResponseSystem/ActionClose.h"
	"DynamicResponseSystem/ActionClose.cpp"
	"DynamicResponseSystem/ActionLock.h"
	"DynamicResponseSystem/ActionLock.cpp"
	"DynamicResponseSystem/ActionOpen.h"
	"DynamicResponseSystem/ActionOpen.cpp"
	"DynamicResponseSystem/ActionPlayAnimation.h"
	"DynamicResponseSystem/ActionPlayAnimation.cpp"
	"DynamicResponseSystem/ActionSwitch.h"
	"DynamicResponseSystem/ActionSwitch.cpp"
	"DynamicResponseSystem/ActionUnlock.h"
	"DynamicResponseSystem/ActionUnlock.cpp"
	"DynamicResponseSystem/ConditionDistanceToEntity.h"
	"DynamicResponseSystem/ConditionDistanceToEntity.cpp"
)
source_group("DynamicResponseSystem" FILES ${SourceGroup_DynamicResponseSystem})

set (SourceGroup_Entities
	"Entities/EntityEffects.h"
	"Entities/EntityEffects.cpp"
)
source_group("Entities" FILES ${SourceGroup_Entities})

set (SourceGroup_Entities_Compass
	"Entities/Compass/CompassComponent.cpp"
	"Entities/Compass/CompassComponent.h"
)
source_group("Entities\\Compass" FILES ${SourceGroup_Entities_Compass})

set (SourceGroup_Entities_Doors
	"Entities/Doors/AnimatedDoorComponent.cpp"
	"Entities/Doors/AnimatedDoorComponent.h"
)
source_group("Entities\\Doors" FILES ${SourceGroup_Entities_Doors})

set (SourceGroup_Entities_Flashlight
	"Entities/Flashlight/FlashlightComponent.cpp"
	"Entities/Flashlight/FlashlightComponent.h"
)
source_group("Entities\\Flashlight" FILES ${SourceGroup_Entities_Flashlight})

#set (SourceGroup_Entities_Environment_Ledge
#	"Entities/Environment/Ledge/Ledge.h"
#	"Entities/Environment/Ledge/Ledge.cpp"
#	"Entities/Environment/Ledge/LedgeManager.h"
#	"Entities/Environment/Ledge/LedgeManager.cpp"
#	"Entities/Environment/Ledge/LedgeStatic.h"
#	"Entities/Environment/Ledge/LedgeStatic.cpp"
#)
#source_group("Entities\\Environment\\Ledge" FILES ${SourceGroup_Entities_Environment_Ledge})

#set (SourceGroup_Entities_Environment_Water
#	"Entities/Environment/Water/WaterRipplesGenerator.h"
#	"Entities/Environment/Water/WaterRipplesGenerator.cpp"
#)
#source_group("Entities\\Environment\\Water" FILES ${SourceGroup_Entities_Environment_Water})

set (SourceGroup_Entities_Interaction
	"Entities/Interaction/DRSInteractionEntity.cpp"
	"Entities/Interaction/DRSInteractionEntity.h"
	"Entities/Interaction/IEntityInteraction.h"
)
source_group("Entities\\Interaction" FILES ${SourceGroup_Entities_Interaction})

#set (SourceGroup_Entities_Ownership
#	"Entities/Ownership/IEntityOwnership.h"
#	"Entities/Ownership/IEntityOwnership.cpp"
#)
#source_group("Entities\\Ownership" FILES ${SourceGroup_Entities_Ownership})

set (SourceGroup_Entities_SecurityPad
	"Entities/SecurityPad/SecurityPadComponent.cpp"
	"Entities/SecurityPad/SecurityPadComponent.h"
)
source_group("Entities\\SecurityPad" FILES ${SourceGroup_Entities_SecurityPad})

#set (SourceGroup_Game_Cache
#	"Game/Cache/GameCache.cpp"
#	"Game/Cache/GameCache.h"
#)
#source_group("Game\\Cache" FILES ${SourceGroup_Game_Cache})

#set (SourceGroup_Game_Physics
#	"Game/Physics/GamePhysicsSettings.cpp"
#	"Game/Physics/GamePhysicsSettings.h"
#)
#source_group("Game\\Physics" FILES ${SourceGroup_Game_Physics})

#set (SourceGroup_Game_Registration
#	"Game/Registration/GameRegistration.cpp"
#	"Game/Registration/GameRegistration.h"
#)
#source_group("Game\\Registration" FILES ${SourceGroup_Game_Registration})

set (SourceGroup_Helpers
	"Helpers/DesignerEntityComponent.cpp"
	"Helpers/DesignerEntityComponent.h"
)
source_group("Helpers" FILES ${SourceGroup_Helpers})

set (SourceGroup_Item
	"Item/Item.cpp"
	"Item/Item.h"
	"Item/ItemAccessory.cpp"
	"Item/ItemEffect.cpp"
	"Item/ISimpleItem.h"
)
source_group("Item" FILES ${SourceGroup_Item})

set (SourceGroup_Item_Accessory
	"Item/Accessory/Accessory.cpp"
	"Item/Accessory/Accessory.h"
)
source_group("Item\\Accessory" FILES ${SourceGroup_Item_Accessory})

set (SourceGroup_Item_Flashlight
	"Item/Flashlight/ItemFlashlightParameter.cpp"
	"Item/Flashlight/ItemFlashlightParameter.h"
	"Item/Flashlight/ItemFlashlightParameterShared.cpp"
	"Item/Flashlight/ItemFlashlightParameterShared.h"
)
source_group("Item\\Flashlight" FILES ${SourceGroup_Item_Flashlight})

set (SourceGroup_Item_Parameters
	"Item/Parameters/ItemAccessoryParameter.cpp"
	"Item/Parameters/ItemAccessoryParameter.h"
	"Item/Parameters/ItemBaseParameter.cpp"
	"Item/Parameters/ItemBaseParameter.h"
	"Item/Parameters/ItemGeometryParameter.cpp"
	"Item/Parameters/ItemGeometryParameter.h"
	"Item/Parameters/ItemLaserParameter.cpp"
	"Item/Parameters/ItemLaserParameter.h"
)
source_group("Item\\Parameters" FILES ${SourceGroup_Item_Parameters})

set (SourceGroup_Player_Camera
	"Player/Camera/ActionRPGCameraComponent.cpp"
	"Player/Camera/ActionRPGCameraComponent.h"
	"Player/Camera/CameraManagerComponent.cpp"
	"Player/Camera/CameraManagerComponent.h"
	"Player/Camera/ExamineCameraComponent.cpp"
	"Player/Camera/ExamineCameraComponent.h"
	"Player/Camera/FirstPersonCameraComponent.cpp"
	"Player/Camera/FirstPersonCameraComponent.h"
	"Player/Camera/ICameraComponent.cpp"
	"Player/Camera/ICameraComponent.h"
)
source_group("Player\\Camera" FILES ${SourceGroup_Player_Camera})

set (SourceGroup_PlayerInput
	"Player/Input/IPlayerInputComponent.h"
	"Player/Input/PlayerInputComponent.cpp"
	"Player/Input/PlayerInputComponent.h"
)
source_group("Player\\Input" FILES ${SourceGroup_PlayerInput})

set (SourceGroup_ObjectID
	"ObjectID/ObjectId.cpp"
	"ObjectID/ObjectId.h"
	"ObjectID/ObjectIdMasterFactory.cpp"
	"ObjectID/ObjectIdMasterFactory.h"
)
source_group("ObjectID" FILES ${SourceGroup_ObjectID})

set (SourceGroup_SharedParameters
	"SharedParameters/DynamicLight.h"
	"SharedParameters/DynamicLight.cpp"
	"SharedParameters/FogVolume.h"
	"SharedParameters/FogVolume.cpp"
	"SharedParameters/SharedParameters.h"
)
source_group("SharedParameters" FILES ${SourceGroup_SharedParameters})

set (SourceGroup_Snaplocks
	"Snaplocks/Snaplock.h"
	"Snaplocks/Snaplock.cpp"
)
source_group("Snaplocks" FILES ${SourceGroup_Snaplocks})

set (SourceGroup_StateMachine
	"StateMachine/StateMachine.h"
)
source_group("StateMachine" FILES ${SourceGroup_StateMachine})

set (SourceGroup_Utility
	"Utility/AutoEnum.cpp"
	"Utility/AutoEnum.h"
	"Utility/CryHash.h"
	"Utility/CryWatch.cpp"
	"Utility/CryWatch.h"
	"Utility/DesignerWarning.cpp"
	"Utility/DesignerWarning.h"
	"Utility/DRS.cpp"
	"Utility/DRS.h"
	"Utility/ItemString.h"
	"Utility/LocalizeUtility.cpp"
	"Utility/LocalizeUtility.h"
	"Utility/StringConversions.h"
	"Utility/StringUtils.cpp"
	"Utility/StringUtils.h"
)
source_group("Utility" FILES ${SourceGroup_Utility})


set (SOURCE
	${SourceGroup_Main}
	${SourceGroup_Plugin}
	${SourceGroup_Game}
#	${SourceGroup_WeaponEntities}
#	${SourceGroup_GameplayEntities}
	${SourceGroup_EntityHelpers}
	${SourceGroup_Player}
	${SourceGroup_PlayerInput}

	${SourceGroup_Actor}
	${SourceGroup_Actor_Animation}
	${SourceGroup_Actor_Animation_Actions}
	${SourceGroup_Actor_Character}
	${SourceGroup_Actor_Character_Movement}
	${SourceGroup_Actor_Character_Movement_StateMachine}
	${SourceGroup_Actor_Inventory}
#	${SourceGroup_Actor_Mount}
	${SourceGroup_Actor_Movement}
	${SourceGroup_Actor_Movement_StateMachine}
#	${SourceGroup_Actor_Pet}

	${SourceGroup_Animation}
#	${SourceGroup_Animation_ProceduralContext}
#	${SourceGroup_Boids}
#	${SourceGroup_Boids_Amphibian}
#	${SourceGroup_Boids_Aquatic}
#	${SourceGroup_Boids_Avian}
#	${SourceGroup_Boids_Terran}
	${SourceGroup_Player_Camera}
	
	${SourceGroup_Components}
	${SourceGroup_Components_Animation}
	${SourceGroup_Components_Equipment}
	${SourceGroup_Components_Geometry}
	${SourceGroup_Components_Interaction}
	${SourceGroup_Components_Inventory}
	${SourceGroup_Components_Lights}
	${SourceGroup_Components_Lockable}
	${SourceGroup_Components_Openable}
	${SourceGroup_Components_Particles}
	${SourceGroup_Components_Snaplocks}
	${SourceGroup_Components_Switchable}

	${SourceGroup_ConsoleCommands}
	${SourceGroup_DynamicResponseSystem}

	${SourceGroup_Entities}
	${SourceGroup_Entities_Compass}
	${SourceGroup_Entities_Doors}
	${SourceGroup_Entities_Flashlight}
	${SourceGroup_Entities_Interaction}
#	${SourceGroup_Entities_Ownership}
#	${SourceGroup_Entities_Environment_Ledge}
#	${SourceGroup_Entities_Environment_Water}
	${SourceGroup_Entities_SecurityPad}

#	${SourceGroup_Game_Cache}
#	${SourceGroup_Game_Physics}
#	${SourceGroup_Game_Registration}
	${SourceGroup_Helpers}
	${SourceGroup_Item}
	${SourceGroup_Item_Accessory}
	${SourceGroup_Item_Flashlight}
	${SourceGroup_Item_Parameters}
	${SourceGroup_ObjectID}
	${SourceGroup_SharedParameters}
	${SourceGroup_Snaplocks}
	${SourceGroup_StateMachine}
	${SourceGroup_Utility}
)

if(OPTION_PCH AND MSVC)	
	set (PRECOMPILED_SOURCE ${SOURCE})
	list(REMOVE_ITEM PRECOMPILED_SOURCE "StdAfx.cpp")
	set (PRECOMPILED_BINARY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pch)
	set_source_files_properties (${PRECOMPILED_SOURCE} PROPERTIES COMPILE_FLAGS "/Yu\"StdAfx.h\" /Fp\"${PRECOMPILED_BINARY}\"" OBJECT_DEPENDS "${PRECOMPILED_BINARY}")
	set_source_files_properties (StdAfx.cpp PROPERTIES COMPILE_FLAGS "/Yc\"StdAfx.h\" /Fp\"${PRECOMPILED_BINARY}\"" OBJECT_OUTPUTS "${PRECOMPILED_BINARY}")
endif (OPTION_PCH AND MSVC)

add_library(${PROJECT_NAME} MODULE ${SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE CryCommon CryAction)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE Ntdll)
endif(WIN32)
