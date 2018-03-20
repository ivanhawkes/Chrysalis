cmake_minimum_required (VERSION 3.6.2)
set(CRYENGINE_DIR "D:/CRYENGINE-SOURCE")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "D:/Chrysalis")

include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("ChrysalisCore_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Root"
		"StdAfx.cpp"
		"StdAfx.h"
)
add_sources("Actor_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor"
		"Actor/ActorComponent.cpp"
		"Actor/ActorControllerComponent.cpp"
		"Actor/ActorComponent.h"
		"Actor/ActorControllerComponent.h"
		"Actor/Fate.h"
)
add_sources("Animation_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Animation"
		"Actor/Animation/ActorAnimation.cpp"
		"Actor/Animation/ActorAnimationEvents.cpp"
		"Actor/Animation/ActorAnimation.h"
		"Actor/Animation/ActorAnimationEvents.h"
)
add_sources("Actions_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Animation\\\\Actions"
		"Actor/Animation/Actions/ActorAnimationActionAiming.cpp"
		"Actor/Animation/Actions/ActorAnimationActionAimPose.cpp"
		"Actor/Animation/Actions/ActorAnimationActionCooperative.cpp"
		"Actor/Animation/Actions/ActorAnimationActionEmote.cpp"
		"Actor/Animation/Actions/ActorAnimationActionInteration.cpp"
		"Actor/Animation/Actions/ActorAnimationActionLocomotion.cpp"
		"Actor/Animation/Actions/ActorAnimationActionLooking.cpp"
		"Actor/Animation/Actions/ActorAnimationActionLookPose.cpp"
		"Actor/Animation/Actions/ActorAnimationActionAiming.h"
		"Actor/Animation/Actions/ActorAnimationActionAimPose.h"
		"Actor/Animation/Actions/ActorAnimationActionCooperative.h"
		"Actor/Animation/Actions/ActorAnimationActionEmote.h"
		"Actor/Animation/Actions/ActorAnimationActionInteration.h"
		"Actor/Animation/Actions/ActorAnimationActionLocomotion.h"
		"Actor/Animation/Actions/ActorAnimationActionLooking.h"
		"Actor/Animation/Actions/ActorAnimationActionLookPose.h"
)
add_sources("Character_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Character"
		"Actor/Character/CharacterAttributes.cpp"
		"Actor/Character/CharacterAttributesComponent.cpp"
		"Actor/Character/CharacterComponent.cpp"
		"Actor/Character/CharacterAttributes.h"
		"Actor/Character/CharacterAttributesComponent.h"
		"Actor/Character/CharacterComponent.h"
)
add_sources("Inventory_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Inventory"
		"Actor/Inventory/EntityInventory.cpp"
		"Actor/Inventory/EntityInventory.h"
)
add_sources("Mount_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Mount"
		"Actor/Mount/Mount.cpp"
		"Actor/Mount/Mount.h"
)
add_sources("Movement_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Movement"
)
add_sources("StateMachine_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Movement\\\\StateMachine"
		"Actor/Movement/StateMachine/ActorStateAnimationControlled.cpp"
		"Actor/Movement/StateMachine/ActorStateDead.cpp"
		"Actor/Movement/StateMachine/ActorStateEntry.cpp"
		"Actor/Movement/StateMachine/ActorStateFly.cpp"
		"Actor/Movement/StateMachine/ActorStateGround.cpp"
		"Actor/Movement/StateMachine/ActorStateJump.cpp"
		"Actor/Movement/StateMachine/ActorStateLadder.cpp"
		"Actor/Movement/StateMachine/ActorStateLinked.cpp"
		"Actor/Movement/StateMachine/ActorStateMovement.cpp"
		"Actor/Movement/StateMachine/ActorStateSwim.cpp"
		"Actor/Movement/StateMachine/ActorStateSwimWaterTestProxy.cpp"
		"Actor/Movement/StateMachine/ActorStateUtility.cpp"
		"Actor/Movement/StateMachine/ActorStateDead.h"
		"Actor/Movement/StateMachine/ActorStateEvents.h"
		"Actor/Movement/StateMachine/ActorStateFly.h"
		"Actor/Movement/StateMachine/ActorStateGround.h"
		"Actor/Movement/StateMachine/ActorStateJump.h"
		"Actor/Movement/StateMachine/ActorStateLadder.h"
		"Actor/Movement/StateMachine/ActorStateSwim.h"
		"Actor/Movement/StateMachine/ActorStateSwimWaterTestProxy.h"
		"Actor/Movement/StateMachine/ActorStateUtility.h"
)
add_sources("Pet_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Actor\\\\Pet"
		"Actor/Pet/Pet.cpp"
		"Actor/Pet/Pet.h"
)
add_sources("Animation_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Animation"
		"Animation/Animation.cpp"
		"Animation/Animation.h"
)
add_sources("ProceduralContext_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Animation\\\\ProceduralContext"
		"Animation/ProceduralContext/ProceduralContextAim.cpp"
		"Animation/ProceduralContext/ProceduralContextColliderMode.cpp"
		"Animation/ProceduralContext/ProceduralContextLook.cpp"
		"Animation/ProceduralContext/ProceduralContextMovementControlMethod.cpp"
		"Animation/ProceduralContext/ProceduralContextRagdoll.cpp"
		"Animation/ProceduralContext/ProceduralContextAim.h"
		"Animation/ProceduralContext/ProceduralContextColliderMode.h"
		"Animation/ProceduralContext/ProceduralContextHelpers.h"
		"Animation/ProceduralContext/ProceduralContextLook.h"
		"Animation/ProceduralContext/ProceduralContextMovementControlMethod.h"
		"Animation/ProceduralContext/ProceduralContextRagdoll.h"
)
add_sources("Components_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components"
)
add_sources("Animation_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Animation"
		"Components/Animation/ControlledAnimationComponent.cpp"
		"Components/Animation/SimpleAnimationComponent.cpp"
		"Components/Animation/ControlledAnimationComponent.h"
		"Components/Animation/SimpleAnimationComponent.h"
)
add_sources("Compass_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Compass"
		"Components/Compass/CompassComponent.cpp"
		"Components/Compass/CompassComponent.h"
)
add_sources("Equipment_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Equipment"
		"Components/Equipment/EquipmentComponent.cpp"
		"Components/Equipment/EquipmentComponent.h"
)
add_sources("Gauge_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Gauge"
		"Components/Gauge/GaugeComponent.cpp"
		"Components/Gauge/GaugeComponent.h"
)
add_sources("Interaction_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Interaction"
		"Components/Interaction/DRSInteractionComponent.cpp"
		"Components/Interaction/EntityAwarenessComponent.cpp"
		"Components/Interaction/EntityInteractionComponent.cpp"
		"Components/Interaction/InteractComponent.cpp"
		"Components/Interaction/ItemInteractionComponent.cpp"
		"Components/Interaction/DRSInteractionComponent.h"
		"Components/Interaction/EntityAwarenessComponent.h"
		"Components/Interaction/EntityInteractionComponent.h"
		"Components/Interaction/InteractComponent.h"
		"Components/Interaction/ItemInteractionComponent.h"
)
add_sources("Inventory_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Inventory"
		"Components/Inventory/InventoryComponent.cpp"
		"Components/Inventory/InventoryComponent.h"
)
add_sources("Items_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Items"
		"Components/Items/ItemComponent.cpp"
		"Components/Items/ItemComponent.h"
)
add_sources("Lights_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Lights"
		"Components/Lights/DynamicLightComponent.cpp"
		"Components/Lights/DynamicLightComponent.h"
)
add_sources("Lockable_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Lockable"
		"Components/Lockable/KeyringComponent.cpp"
		"Components/Lockable/LockableComponent.cpp"
		"Components/Lockable/KeyringComponent.h"
		"Components/Lockable/LockableComponent.h"
)
add_sources("Openable_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Openable"
		"Components/Openable/ContainerComponent.cpp"
		"Components/Openable/ContainerComponent.h"
)
add_sources("Player_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Player"
		"Components/Player/PlayerComponent.cpp"
		"Components/Player/PlayerComponent.h"
)
add_sources("Camera_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Player\\\\Camera"
		"Components/Player/Camera/ActionRPGCameraComponent.cpp"
		"Components/Player/Camera/CameraManagerComponent.cpp"
		"Components/Player/Camera/ExamineCameraComponent.cpp"
		"Components/Player/Camera/FirstPersonCameraComponent.cpp"
		"Components/Player/Camera/ICameraComponent.cpp"
		"Components/Player/Camera/ActionRPGCameraComponent.h"
		"Components/Player/Camera/CameraManagerComponent.h"
		"Components/Player/Camera/ExamineCameraComponent.h"
		"Components/Player/Camera/FirstPersonCameraComponent.h"
		"Components/Player/Camera/ICameraComponent.h"
)
add_sources("Input_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Player\\\\Input"
		"Components/Player/Input/PlayerInputComponent.cpp"
		"Components/Player/Input/PlayerInputComponent.h"
)
add_sources("Snaplocks_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Snaplocks"
		"Components/Snaplocks/SnaplockComponent.cpp"
		"Components/Snaplocks/SnaplockComponent.h"
)
add_sources("Switchable_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Switchable"
		"Components/Switchable/SwitchComponent.cpp"
		"Components/Switchable/SwitchComponent.h"
)
add_sources("TimePiece_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\TimePiece"
		"Components/TimePiece/TimePieceComponent.cpp"
		"Components/TimePiece/TimePieceComponent.h"
)
add_sources("Console_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Console"
		"Console/CVars.cpp"
		"Console/CVars.h"
)
add_sources("DynamicResponseSystem_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "DynamicResponseSystem"
		"DynamicResponseSystem/ActionClose.cpp"
		"DynamicResponseSystem/ActionLock.cpp"
		"DynamicResponseSystem/ActionOpen.cpp"
		"DynamicResponseSystem/ActionPlayAnimation.cpp"
		"DynamicResponseSystem/ActionSwitch.cpp"
		"DynamicResponseSystem/ActionUnlock.cpp"
		"DynamicResponseSystem/ConditionDistanceToEntity.cpp"
		"DynamicResponseSystem/ActionClose.h"
		"DynamicResponseSystem/ActionLock.h"
		"DynamicResponseSystem/ActionOpen.h"
		"DynamicResponseSystem/ActionPlayAnimation.h"
		"DynamicResponseSystem/ActionSwitch.h"
		"DynamicResponseSystem/ActionUnlock.h"
		"DynamicResponseSystem/ConditionDistanceToEntity.h"
)
add_sources("Entities_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Entities"
		"Entities/EntityEffects.cpp"
		"Entities/EntityEffects.h"
		"Entities/EntityScriptCalls.h"
)
add_sources("Interaction_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Entities\\\\Interaction"
		"Entities/Interaction/DRSInteractionEntity.cpp"
		"Entities/Interaction/IEntityInteraction.cpp"
		"Entities/Interaction/DRSInteractionEntity.h"
		"Entities/Interaction/IEntityInteraction.h"
)
add_sources("Ownership_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Entities\\\\Ownership"
		"Entities/Ownership/IEntityOwnership.cpp"
		"Entities/Ownership/IEntityOwnership.h"
)
add_sources("SecurityPad_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Entities\\\\SecurityPad"
		"Entities/SecurityPad/SecurityPadComponent.cpp"
		"Entities/SecurityPad/SecurityPadComponent.h"
)
add_sources("Game_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Game"
)
add_sources("Cache_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Game\\\\Cache"
		"Game/Cache/GameCache.cpp"
		"Game/Cache/GameCache.h"
)
add_sources("Item_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Item"
		"Item/ItemAccessory.cpp"
		"Item/ItemEffect.cpp"
)
add_sources("Accessory_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Item\\\\Accessory"
		"Item/Accessory/Accessory.cpp"
		"Item/Accessory/Accessory.h"
)
add_sources("Parameters_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Item\\\\Parameters"
		"Item/Parameters/ItemAccessoryParameter.cpp"
		"Item/Parameters/ItemBaseParameter.cpp"
		"Item/Parameters/ItemGeometryParameter.cpp"
		"Item/Parameters/ItemLaserParameter.cpp"
		"Item/Parameters/ItemParameter.cpp"
		"Item/Parameters/ItemAccessoryParameter.h"
		"Item/Parameters/ItemBaseParameter.h"
		"Item/Parameters/ItemGeometryParameter.h"
		"Item/Parameters/ItemLaserParameter.h"
		"Item/Parameters/ItemParameter.h"
)
add_sources("Weapon_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Item\\\\Weapon"
		"Item/Weapon/Weapon.cpp"
		"Item/Weapon/Weapon.h"
)
add_sources("ObjectID_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "ObjectID"
		"ObjectID/ObjectId.cpp"
		"ObjectID/ObjectIdMasterFactory.cpp"
		"ObjectID/ObjectId.h"
		"ObjectID/ObjectIdMasterFactory.h"
)
add_sources("Plugin_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Plugin"
		"Plugin/ChrysalisCorePlugin.cpp"
		"Plugin/ChrysalisCorePlugin.h"
)
add_sources("Schematyc_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Schematyc"
		"Schematyc/CoreEnv.cpp"
		"Schematyc/General.cpp"
		"Schematyc/CoreEnv.h"
)
add_sources("SharedParameters_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "SharedParameters"
		"SharedParameters/DynamicLight.cpp"
		"SharedParameters/FogVolume.cpp"
		"SharedParameters/DynamicLight.h"
		"SharedParameters/FogVolume.h"
		"SharedParameters/SharedParameters.h"
)
add_sources("Snaplocks_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Snaplocks"
		"Snaplocks/Snaplock.cpp"
		"Snaplocks/Snaplock.h"
)
add_sources("StateMachine_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "StateMachine"
		"StateMachine/StateMachine.h"
)
add_sources("Utility_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Utility"
		"Utility/AutoEnum.cpp"
		"Utility/CryHash.cpp"
		"Utility/CryWatch.cpp"
		"Utility/DRS.cpp"
		"Utility/LocalizeUtility.cpp"
		"Utility/StringUtils.cpp"
		"Utility/AutoEnum.h"
		"Utility/CryHash.h"
		"Utility/CryWatch.h"
		"Utility/DRS.h"
		"Utility/ItemString.h"
		"Utility/Listener.h"
		"Utility/LocalizeUtility.h"
		"Utility/StringConversions.h"
		"Utility/StringUtils.h"
)

end_sources()

CryEngineModule(Chrysalis PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE 
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
	"${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
	"${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

# Set StartUp project in Visual Studio

add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"D:/Chrysalis/Chrysalis.cryproject\"")
endif()

add_library(Sandbox STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(Sandbox "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"D:/Chrysalis/Chrysalis.cryproject\"")
endif()

add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"D:/Chrysalis/Chrysalis.cryproject\"")
endif()

set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"D:/Chrysalis/Chrysalis.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
#END-CUSTOM