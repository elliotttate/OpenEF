#============================================================================
# OpenEF - Elite Force SP on the OpenJK engine
#
# This file is part of the OpenJK source code.
#
# OpenJK is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#============================================================================

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(EFSPGameDefines ${SharedDefines} "EF_MODE" "SP_GAME" "_ENFORCE_MATCHING_ALLOCATORS=0")

set(EFSPGameIncludeDirectories
	"${EFSPDir}"
	"${EFSPDir}/game"
	"${EFSPDir}/renderer"
	"${CMAKE_BINARY_DIR}/shared"
	)
if(WIN32)
	set(EFSPGameLibraries "winmm")
endif(WIN32)

# Game module files
set(EFSPGameGameFiles
	"${EFSPDir}/game/AI_Avatar.cpp"
	"${EFSPDir}/game/AI_Borg.cpp"
	"${EFSPDir}/game/AI_Etherian.cpp"
	"${EFSPDir}/game/AI_Harvester.cpp"
	"${EFSPDir}/game/AI_HeadBot.cpp"
	"${EFSPDir}/game/AI_HirogenAlpha.cpp"
	"${EFSPDir}/game/AI_HunterSeeker.cpp"
	"${EFSPDir}/game/AI_Parasite.cpp"
	"${EFSPDir}/game/AI_Reaver.cpp"
	"${EFSPDir}/game/AI_Scavenger.cpp"
	"${EFSPDir}/game/AI_ScoutBot.cpp"
	"${EFSPDir}/game/AI_Species8472.cpp"
	"${EFSPDir}/game/AI_Utils.cpp"
	"${EFSPDir}/game/AI_Vohrsoth.cpp"
	"${EFSPDir}/game/AI_WarriorBot.cpp"
	"${EFSPDir}/game/G_Timer.cpp"
	"${EFSPDir}/game/NPC.cpp"
	"${EFSPDir}/game/NPC_behavior.cpp"
	"${EFSPDir}/game/NPC_combat.cpp"
	"${EFSPDir}/game/NPC_formation.cpp"
	"${EFSPDir}/game/NPC_goal.cpp"
	"${EFSPDir}/game/NPC_misc.cpp"
	"${EFSPDir}/game/NPC_move.cpp"
	"${EFSPDir}/game/NPC_reactions.cpp"
	"${EFSPDir}/game/NPC_senses.cpp"
	"${EFSPDir}/game/NPC_sounds.cpp"
	"${EFSPDir}/game/NPC_spawn.cpp"
	"${EFSPDir}/game/NPC_stats.cpp"
	"${EFSPDir}/game/NPC_utils.cpp"
	"${EFSPDir}/game/Q3_Interface.cpp"
	"${EFSPDir}/game/Q3_Registers.cpp"
	"${EFSPDir}/game/bg_misc.cpp"
	"${EFSPDir}/game/bg_pangles.cpp"
	"${EFSPDir}/game/bg_panimate.cpp"
	"${EFSPDir}/game/bg_pmove.cpp"
	"${EFSPDir}/game/bg_slidemove.cpp"
	"${EFSPDir}/game/g_ICARUS.cpp"
	"${EFSPDir}/game/g_active.cpp"
	"${EFSPDir}/game/g_ambients.cpp"
	"${EFSPDir}/game/g_boltons.cpp"
	"${EFSPDir}/game/g_breakable.cpp"
	"${EFSPDir}/game/g_camera.cpp"
	"${EFSPDir}/game/g_client.cpp"
	"${EFSPDir}/game/g_cmds.cpp"
	"${EFSPDir}/game/g_combat.cpp"
	"${EFSPDir}/game/g_functions.cpp"
	"${EFSPDir}/game/g_fx.cpp"
	"${EFSPDir}/game/g_infostringLoad.cpp"
	"${EFSPDir}/game/g_itemLoad.cpp"
	"${EFSPDir}/game/g_items.cpp"
	"${EFSPDir}/game/g_main.cpp"
	"${EFSPDir}/game/g_mem.cpp"
	"${EFSPDir}/game/g_misc.cpp"
	"${EFSPDir}/game/g_misc_model.cpp"
	"${EFSPDir}/game/g_missile.cpp"
	"${EFSPDir}/game/g_mover.cpp"
	"${EFSPDir}/game/g_nav.cpp"
	"${EFSPDir}/game/g_navigator.cpp"
	"${EFSPDir}/game/g_object.cpp"
	"${EFSPDir}/game/g_objectives.cpp"
	"${EFSPDir}/game/g_ref.cpp"
	"${EFSPDir}/game/g_roff.cpp"
	"${EFSPDir}/game/g_savegame.cpp"
	"${EFSPDir}/game/g_session.cpp"
	"${EFSPDir}/game/g_spawn.cpp"
	"${EFSPDir}/game/g_squad.cpp"
	"${EFSPDir}/game/g_svcmds.cpp"
	"${EFSPDir}/game/g_target.cpp"
	"${EFSPDir}/game/g_trigger.cpp"
	"${EFSPDir}/game/g_turret.cpp"
	"${EFSPDir}/game/g_usable.cpp"
	"${EFSPDir}/game/g_utils.cpp"
	"${EFSPDir}/game/g_weapon.cpp"
	"${EFSPDir}/game/g_weaponLoad.cpp"
	# Headers
	"${EFSPDir}/game/AI.h"
	"${EFSPDir}/game/Q3_Interface.h"
	"${EFSPDir}/game/Q3_Registers.h"
	"${EFSPDir}/game/anims.h"
	"${EFSPDir}/game/b_local.h"
	"${EFSPDir}/game/b_public.h"
	"${EFSPDir}/game/bg_local.h"
	"${EFSPDir}/game/bg_public.h"
	"${EFSPDir}/game/boltons.h"
	"${EFSPDir}/game/bset.h"
	"${EFSPDir}/game/bstate.h"
	"${EFSPDir}/game/channels.h"
	"${EFSPDir}/game/characters.h"
	"${EFSPDir}/game/events.h"
	"${EFSPDir}/game/fields.h"
	"${EFSPDir}/game/g_functions.h"
	"${EFSPDir}/game/g_infostrings.h"
	"${EFSPDir}/game/g_items.h"
	"${EFSPDir}/game/g_local.h"
	"${EFSPDir}/game/g_nav.h"
	"${EFSPDir}/game/g_navigator.h"
	"${EFSPDir}/game/g_public.h"
	"${EFSPDir}/game/g_roff.h"
	"${EFSPDir}/game/g_shared.h"
	"${EFSPDir}/game/g_squad.h"
	"${EFSPDir}/game/objectives.h"
	"${EFSPDir}/game/say.h"
	"${EFSPDir}/game/speakers.h"
	"${EFSPDir}/game/statindex.h"
	"${EFSPDir}/game/surfaceflags.h"
	"${EFSPDir}/game/teams.h"
	"${EFSPDir}/game/weapons.h"
	)
source_group("game" FILES ${EFSPGameGameFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameGameFiles})

# Cgame module files
set(EFSPGameCGameFiles
	"${EFSPDir}/cgame/FX_Compression.cpp"
	"${EFSPDir}/cgame/FX_Grenade_Launcher.cpp"
	"${EFSPDir}/cgame/FX_Memory.cpp"
	"${EFSPDir}/cgame/FX_Phaser.cpp"
	"${EFSPDir}/cgame/FX_Stasis.cpp"
	"${EFSPDir}/cgame/FX_Util.cpp"
	"${EFSPDir}/cgame/FX_Vohrsoth.cpp"
	"${EFSPDir}/cgame/FX_avatar.cpp"
	"${EFSPDir}/cgame/cg_camera.cpp"
	"${EFSPDir}/cgame/cg_consolecmds.cpp"
	"${EFSPDir}/cgame/cg_draw.cpp"
	"${EFSPDir}/cgame/cg_drawtools.cpp"
	"${EFSPDir}/cgame/cg_effects.cpp"
	"${EFSPDir}/cgame/cg_ents.cpp"
	"${EFSPDir}/cgame/cg_env.cpp"
	"${EFSPDir}/cgame/cg_event.cpp"
	"${EFSPDir}/cgame/cg_gi_compat.cpp"
	"${EFSPDir}/cgame/cg_info.cpp"
	"${EFSPDir}/cgame/cg_localents.cpp"
	"${EFSPDir}/cgame/cg_main.cpp"
	"${EFSPDir}/cgame/cg_marks.cpp"
	"${EFSPDir}/cgame/cg_players.cpp"
	"${EFSPDir}/cgame/cg_playerstate.cpp"
	"${EFSPDir}/cgame/cg_predict.cpp"
	"${EFSPDir}/cgame/cg_scoreboard.cpp"
	"${EFSPDir}/cgame/cg_servercmds.cpp"
	"${EFSPDir}/cgame/cg_snapshot.cpp"
	"${EFSPDir}/cgame/cg_syscalls.cpp"
	"${EFSPDir}/cgame/cg_text.cpp"
	"${EFSPDir}/cgame/cg_view.cpp"
	"${EFSPDir}/cgame/cg_weapons.cpp"
	"${EFSPDir}/cgame/fx_borg.cpp"
	"${EFSPDir}/cgame/fx_dreadnought.cpp"
	"${EFSPDir}/cgame/fx_hunter_seeker.cpp"
	"${EFSPDir}/cgame/fx_imod.cpp"
	"${EFSPDir}/cgame/fx_lib.cpp"
	"${EFSPDir}/cgame/fx_parasite.cpp"
	"${EFSPDir}/cgame/fx_primitive.cpp"
	"${EFSPDir}/cgame/fx_proton.cpp"
	"${EFSPDir}/cgame/fx_quantum.cpp"
	"${EFSPDir}/cgame/fx_reaver.cpp"
	"${EFSPDir}/cgame/fx_scavenger.cpp"
	"${EFSPDir}/cgame/fx_scout_bot.cpp"
	"${EFSPDir}/cgame/fx_tetrion.cpp"
	"${EFSPDir}/cgame/fx_transporter.cpp"
	# Headers
	"${EFSPDir}/cgame/FX_Public.h"
	"${EFSPDir}/cgame/cg_camera.h"
	"${EFSPDir}/cgame/cg_local.h"
	"${EFSPDir}/cgame/cg_media.h"
	"${EFSPDir}/cgame/cg_public.h"
	"${EFSPDir}/cgame/cg_text.h"
	)
source_group("cgame" FILES ${EFSPGameCGameFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameCGameFiles})

# UI module files
set(EFSPGameUIFiles
	"${EFSPDir}/ui/gameinfo.cpp"
	"${EFSPDir}/ui/gameinfo.h"
	"${EFSPDir}/ui/ui_atoms.cpp"
	"${EFSPDir}/ui/ui_cdkey.cpp"
	"${EFSPDir}/ui/ui_confirm.cpp"
	"${EFSPDir}/ui/ui_connect.cpp"
	"${EFSPDir}/ui/ui_controls2.cpp"
	"${EFSPDir}/ui/ui_credits.cpp"
	"${EFSPDir}/ui/ui_crew.cpp"
	"${EFSPDir}/ui/ui_cvars.cpp"
	"${EFSPDir}/ui/ui_field.cpp"
	"${EFSPDir}/ui/ui_fonts.cpp"
	"${EFSPDir}/ui/ui_game.cpp"
	"${EFSPDir}/ui/ui_menu.cpp"
	"${EFSPDir}/ui/ui_mods.cpp"
	"${EFSPDir}/ui/ui_players.cpp"
	"${EFSPDir}/ui/ui_preferences.cpp"
	"${EFSPDir}/ui/ui_qmenu.cpp"
	"${EFSPDir}/ui/ui_sound.cpp"
	"${EFSPDir}/ui/ui_turbolift.cpp"
	"${EFSPDir}/ui/ui_video.cpp"
	# Headers
	"${EFSPDir}/ui/ui_local.h"
	"${EFSPDir}/ui/ui_playerinfo.h"
	"${EFSPDir}/ui/ui_public.h"
	)
source_group("ui" FILES ${EFSPGameUIFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameUIFiles})

# Common files - EF has its own q_shared/q_math, do NOT use OpenJK's SharedCommonFiles
# as they define conflicting types (qboolean, vec3_t etc.)
set(EFSPGameCommonFiles
	"${EFSPDir}/game/q_math.cpp"
	"${EFSPDir}/game/q_shared.cpp"
	"${EFSPDir}/game/q_shared.h"
	"${CMAKE_BINARY_DIR}/shared/qcommon/q_version.h"
	)
source_group("common" FILES ${EFSPGameCommonFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameCommonFiles})

# Skip SharedCommonSafeFiles - they depend on OpenJK's type system which conflicts with EF

# ICARUS scripting files
set(EFSPGameIcarusFiles
	"${EFSPDir}/icarus/BlockStream.cpp"
	"${EFSPDir}/icarus/Instance.cpp"
	# Interface.cpp stub excluded -- real Interface_Init is in game/Q3_Interface.cpp
	"${EFSPDir}/icarus/Interpreter.cpp"
	"${EFSPDir}/icarus/Sequence.cpp"
	"${EFSPDir}/icarus/Sequencer.cpp"
	"${EFSPDir}/icarus/TaskManager.cpp"
	"${EFSPDir}/icarus/Tokenizer.cpp"
	# Headers
	"${EFSPDir}/icarus/blockstream.h"
	"${EFSPDir}/icarus/icarus.h"
	"${EFSPDir}/icarus/instance.h"
	"${EFSPDir}/icarus/interface.h"
	"${EFSPDir}/icarus/interpreter.h"
	"${EFSPDir}/icarus/module.h"
	"${EFSPDir}/icarus/sequence.h"
	"${EFSPDir}/icarus/sequencer.h"
	"${EFSPDir}/icarus/taskmanager.h"
	"${EFSPDir}/icarus/tokenizer.h"
	)
source_group("icarus" FILES ${EFSPGameIcarusFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameIcarusFiles})

# Speedrun tools
set(EFSPGameSpeedrunFiles
	"${EFSPDir}/speedrun/PlayerOverbouncePrediction.cpp"
	"${EFSPDir}/speedrun/overbounce_prediction/OverbouncePrediction.cpp"
	"${EFSPDir}/speedrun/sound_skipping.cpp"
	"${EFSPDir}/speedrun/strafe_helper/strafe_helper.c"
	"${EFSPDir}/speedrun/strafe_helper_customization.cpp"
	# Headers
	"${EFSPDir}/speedrun/strafe_helper/strafe_helper.h"
	"${EFSPDir}/speedrun/strafe_helper/strafe_helper_customization.h"
	"${EFSPDir}/speedrun/strafe_helper_includes.h"
	)
source_group("speedrun" FILES ${EFSPGameSpeedrunFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameSpeedrunFiles})

# Rufl (OpenJK's hstring) - not used by EF, depends on OpenJK's q_shared.h type system

# qcommon headers from EF
set(EFSPGameQcommonFiles
	"${EFSPDir}/qcommon/cm_public.h"
	"${EFSPDir}/qcommon/qcommon.h"
	"${EFSPDir}/qcommon/qfiles.h"
	"${EFSPDir}/qcommon/stv_version.h"
	)
source_group("qcommon" FILES ${EFSPGameQcommonFiles})
set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameQcommonFiles})

if(WIN32)
	set(EFSPGameWin32Files
		"${EFSPDir}/win32/game.rc")
	source_group("win32" FILES ${EFSPGameWin32Files})
	set(EFSPGameFiles ${EFSPGameFiles} ${EFSPGameWin32Files})
endif(WIN32)

add_library(${EFSPGame} SHARED ${EFSPGameFiles})

# EF speedrun tools require C++20 (string_view, filesystem, contains)
target_compile_features(${EFSPGame} PRIVATE cxx_std_20)

# EF's original code freely assigns string literals to char* (C-era convention).
# Disable the conversion error for now; these can be fixed incrementally.
if(MSVC)
	target_compile_options(${EFSPGame} PRIVATE /Zc:strictStrings-)
endif()

if(NOT MSVC)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${EFSPGame} PROPERTIES PREFIX "")
endif()

if(WIN32)
	install(TARGETS ${EFSPGame} RUNTIME DESTINATION "${EFInstallDir}/OpenJK")
else(WIN32)
	if(MakeApplicationBundles AND BuildEFSPEngine)
		install(TARGETS ${EFSPGame} LIBRARY DESTINATION "${EFInstallDir}/${EFSPEngine}.app/Contents/MacOS/OpenJK")
	else(MakeApplicationBundles AND BuildEFSPEngine)
		install(TARGETS ${EFSPGame} LIBRARY DESTINATION "${EFInstallDir}/OpenJK")
	endif(MakeApplicationBundles AND BuildEFSPEngine)
endif(WIN32)

set_target_properties(${EFSPGame} PROPERTIES COMPILE_DEFINITIONS "${EFSPGameDefines}")

# Hide symbols not explicitly marked public.
set_property(TARGET ${EFSPGame} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

set_target_properties(${EFSPGame} PROPERTIES INCLUDE_DIRECTORIES "${EFSPGameIncludeDirectories}")
set_target_properties(${EFSPGame} PROPERTIES PROJECT_LABEL "EF SP Game Library")
# Link libraries
if(EFSPGameLibraries)
	target_link_libraries(${EFSPGame} ${EFSPGameLibraries})
endif(EFSPGameLibraries)

# EF's original build has game/cgame/ui as separate DLLs. When combined into one
# DLL (OpenJK SP pattern), some symbols were multiply defined. Fixed by making
# duplicates static or removing redundant definitions.
