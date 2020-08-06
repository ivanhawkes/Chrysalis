cmake_minimum_required (VERSION 3.14)
set(CRYENGINE_DIR "D:/CRYENGINE-SOURCE" CACHE STRING "CRYENGINE root directory.")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/../")

include("${TOOLS_CMAKE_DIR}/InitialSetup.cmake")
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
add_sources("Animation_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Animation"
		"Animation/Animation.cpp"
		"Animation/Animation.h"
)
add_sources("ProceduralClip_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Animation\\\\ProceduralClip"
		"Animation/ProceduralClip/ProceduralClipAiming.cpp"
		"Animation/ProceduralClip/ProceduralClipLooking.cpp"
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
add_sources("Components_Actor_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Actor"
		"Components/Actor/ActorComponent.cpp"
		"Components/Actor/ActorControllerComponent.cpp"
		"Components/Actor/ActorComponent.h"
		"Components/Actor/ActorControllerComponent.h"
)
add_sources("Animation_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Animation"
		"Components/Animation/ActorAnimationComponent.cpp"
		"Components/Animation/ControlledAnimationComponent.cpp"
		"Components/Animation/SimpleAnimationComponent.cpp"
		"Components/Animation/ActorAnimationComponent.h"
		"Components/Animation/ControlledAnimationComponent.h"
		"Components/Animation/SimpleAnimationComponent.h"
)
add_sources("Compass_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Compass"
		"Components/Compass/CompassComponent.cpp"
		"Components/Compass/CompassComponent.h"
)
add_sources("Door_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Door"
		"Components/Door/DoorComponent.cpp"
		"Components/Door/DoorComponent.h"
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
		"Components/Interaction/DRSInteractionComponent.h"
		"Components/Interaction/EntityAwarenessComponent.cpp"
		"Components/Interaction/EntityAwarenessComponent.h"
		"Components/Interaction/EntityInteractionComponent.cpp"
		"Components/Interaction/EntityInteractionComponent.h"
		"Components/Interaction/InteractComponent.cpp"
		"Components/Interaction/InteractComponent.h"
		"Components/Interaction/ItemInteractionComponent.cpp"
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
add_sources("Lockable_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Lockable"
		"Components/Lockable/KeyringComponent.cpp"
		"Components/Lockable/LockableComponent.cpp"
		"Components/Lockable/KeyringComponent.h"
		"Components/Lockable/LockableComponent.h"
)
add_sources("Loot_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Loot"
		"Components/Loot/LootableComponent.cpp"
		"Components/Loot/LootableComponent.h"
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
add_sources("SecurityPad_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\SecurityPad"
		"Components/SecurityPad/SecurityPadComponent.cpp"
		"Components/SecurityPad/SecurityPadComponent.h"
)
add_sources("Skeleton_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Skeleton"
		"Components/Skeleton/JointManipulationComponent.cpp"
		"Components/Skeleton/JointManipulationComponent.h"
)
add_sources("Slidable_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Slidable"
		"Components/Slidable/SlidableComponent.cpp"
		"Components/Slidable/SlidableComponent.h"
)
add_sources("Snaplocks_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Snaplocks"
		"Components/Snaplocks/SnaplockComponent.cpp"
		"Components/Snaplocks/SnaplockComponent.h"
)
add_sources("Spells_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Components\\\\Spells"
		"Components/Spells/SpellbookComponent.cpp"
		"Components/Spells/SpellbookComponent.h"
		"Components/Spells/SpellParticipantComponent.cpp"
		"Components/Spells/SpellParticipantComponent.h"
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
add_sources("ECS_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "ECS"
		"ECS/ECS.cpp"
		"ECS/ECS.h"
)
add_sources("Components_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "ECS\\\\Components"
		"ECS/Components/Components.cpp"
		"ECS/Components/Components.h"
		"ECS/Components/Health.cpp"
		"ECS/Components/Health.h"
		"ECS/Components/Inventory.h"
		"ECS/Components/Items.h"
		"ECS/Components/RenderLight.cpp"
		"ECS/Components/RenderLight.h"
		"ECS/Components/Qi.h"
)
add_sources("Components_Spells_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "ECS\\\\Components\\\\Spells"
		"ECS/Components/Spells/Spell.h"
		"ECS/Components/Spells/Spell.cpp"
		"ECS/Components/Spells/SpellActions.h"
		"ECS/Components/Spells/SpellActions.cpp"
)
add_sources("Systems_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "ECS\\\\Systems"
		"ECS/Systems/ItemSystem.cpp"
		"ECS/Systems/ItemSystem.h"
		"ECS/Systems/Simulation.h"
		"ECS/Systems/Simulation.cpp"
		"ECS/Systems/Systems.h"
		"ECS/Systems/Systems.cpp"
		"ECS/Systems/XMLSerializer.h"
		"ECS/Systems/XMLSerializer.cpp"
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
		"Entities/Interaction/IEntityInteraction.cpp"
		"Entities/Interaction/IEntityInteraction.h"
)
add_sources("Ownership_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Entities\\\\Ownership"
		"Entities/Ownership/IEntityOwnership.cpp"
		"Entities/Ownership/IEntityOwnership.h"
)
add_sources("Game_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Game"
)
add_sources("Interfaces_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Interfaces"
		"Interfaces/IActor.h"
		"Interfaces/IEquipment.h"
		"Interfaces/IInventory.h"
		"Interfaces/IItem.h"
		"Interfaces/ISpell.h"
)
add_sources("Item_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Item"
		"Item/ItemAccessory.cpp"
		"Item/ItemSystem.cpp"
		"Item/ItemSystem.h"
)
add_sources("Accessory_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Item\\\\Accessory"
		"Item/Accessory/Accessory.cpp"
		"Item/Accessory/Accessory.h"
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
add_sources("Spells_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Spells"
		"Spells/Spells.cpp"
		"Spells/Spells.h"
)
add_sources("SpellActions_uber.cpp"
    PROJECTS Chrysalis
    SOURCE_GROUP "Spells\\\\SpellActions"
		"Spells/SpellActions/SpellActionOpenDoor.cpp"
		"Spells/SpellActions/SpellActionOpenDoor.h"
		"Spells/SpellActions/SpellActionSwitch.cpp"
		"Spells/SpellActions/SpellActionSwitch.h"
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
		"Utility/LocalizeUtility.h"
		"Utility/StringConversions.h"
		"Utility/StringUtils.h"
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CVarOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "CVarOverrides.h"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/EngineDefineOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "EngineDefineOverrides.h"
    )
endif()
end_sources()

CryGameModule(Chrysalis FORCE_SHARED PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
    "${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
    "${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-variable
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-parentheses
        -Wno-switch
        -Wno-format
        -Wno-dynamic-class-memaccess
        -Wno-unused-private-field
        -Wno-unused-value
        -Wno-invalid-offsetof
        -Wno-multichar
        -Wno-char-subscripts
        -Wno-null-conversion
        -Wno-empty-body
        -Wno-unused-lambda-capture
        -Wno-unused-function
        -Wno-tautological-constant-out-of-range-compare
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-function
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-aligned-new
        -Wno-int-in-bool-context
        -Wno-invalid-offsetof
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-but-set-variable
        -Wno-maybe-uninitialized
        -Wno-misleading-indentation
        -Wno-unused-result
        -Wno-unknown-pragmas
        -Wno-multichar
        -Wno-strict-aliasing
        -Wno-char-subscripts
        -Wno-conversion-null
        -Wno-reorder
        -Wno-parentheses
        -Wno-format
        -Wno-switch
    )
endif()


if(OPTION_ENGINE)
    if(NOT EXISTS "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
        add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
        set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
        if (WIN32)
            set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/Chrysalis.cryproject\"")
        endif()
    endif()
else()
    add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/Chrysalis.cryproject\"")
    endif()

    add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/Chrysalis.cryproject\"")
    endif()

    add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"${PROJECT_DIR}/Chrysalis.cryproject\"")
    endif()
endif()

# Set StartUp project in Visual Studio
set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/Chrysalis.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.

# Force the compiler to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("../SDK/EnTT/src")

# I like to know which compiler I am using.
message (STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
#END-CUSTOM