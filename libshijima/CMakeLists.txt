add_library(shijima STATIC
    shijima/action/animate.cc
    shijima/action/animation.cc
    shijima/action/base.cc
    shijima/action/breed.cc
    shijima/action/breedjump.cc
    shijima/action/breedmove.cc
    shijima/action/complexjump.cc
    shijima/action/complexmove.cc
    shijima/action/dragged.cc
    shijima/action/fall.cc
    shijima/action/instant.cc
    shijima/action/interact.cc
    shijima/action/jump.cc
    shijima/action/look.cc
    shijima/action/move.cc
    shijima/action/movewithturn.cc
    shijima/action/mute.cc
    shijima/action/offset.cc
    shijima/action/reference.cc
    shijima/action/resist.cc
    shijima/action/scaninteract.cc
    shijima/action/scanjump.cc
    shijima/action/scanmove.cc
    shijima/action/select.cc
    shijima/action/selfdestruct.cc
    shijima/action/sequence.cc
    shijima/action/stay.cc
    shijima/action/transform.cc
    shijima/action/turn.cc
    shijima/behavior/base.cc
    shijima/behavior/list.cc
    shijima/behavior/manager.cc
    shijima/broadcast/client.cc
    shijima/broadcast/interaction.cc
    shijima/broadcast/manager.cc
    shijima/broadcast/server.cc
    shijima/mascot/factory.cc
    shijima/mascot/manager.cc
    shijima/mascot/state.cc
    shijima/scripting/duktape/duktape.cc
    shijima/scripting/condition.cc
    shijima/scripting/context.cc
    shijima/scripting/variables.cc
    shijima/animation.cc
    shijima/hotspot.cc
    shijima/log.cc
    shijima/parser.cc
    shijima/parser_error.cc
    shijima/translator.cc
)

target_include_directories(shijima PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shijima")
target_link_libraries(shijima PUBLIC pugixml::pugixml)
target_compile_definitions(shijima PRIVATE SHIJIMA_DUK_STATIC_BUILD)
if(MSVC)
    target_compile_options(shijima PRIVATE /utf-8)
    target_compile_definitions(shijima PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
else()
    target_compile_options(shijima PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()
