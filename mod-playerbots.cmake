# mod-playerbots ShatterCore configuration hook
# Included by modules/CMakeLists.txt for every module (even disabled ones),
# so guard on the resolved linkage before touching core targets.

ModuleNameToVariable("mod-playerbots" PLAYERBOTS_LINKAGE_VARIABLE)

if(NOT ${${PLAYERBOTS_LINKAGE_VARIABLE}} STREQUAL "disabled")
  message("  -> mod-playerbots: enabling MOD_PLAYERBOTS core support")

  # WotLK-era raid/dungeon tactic packs are not ported to 4.3.4 yet -
  # keep them out of the build (see AddSC_*BotScripts in Playerbots.cpp).
  GetPathToModuleSource("mod-playerbots" PLAYERBOTS_SOURCE_PATH)
  set(MODULE_MOD_PLAYERBOTS_EXCLUDE_DIRS
    ${PLAYERBOTS_SOURCE_PATH}/Ai/Raid
    ${PLAYERBOTS_SOURCE_PATH}/Ai/Dungeon)

  # The playerbots database pool and the playerbot core patches are guarded
  # with #ifdef MOD_PLAYERBOTS throughout the core.
  target_compile_definitions(database
    PRIVATE
      MOD_PLAYERBOTS)

  # game links game-interface PUBLIC, so this reaches game, scripts,
  # worldserver and the modules library.
  target_compile_definitions(game-interface
    INTERFACE
      MOD_PLAYERBOTS)
endif()
