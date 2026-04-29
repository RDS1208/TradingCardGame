cmake_minimum_required(VERSION 4.2)
project(TradingCardGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(TradingCardGame
    src/main.cpp
    src/Card.cpp
    src/UnitCard.cpp
    src/SpellCard.cpp
    src/StructureCard.cpp
    src/EffectRegistry.cpp
    src/Player.cpp
    src/Game.cpp
)

# Makes #include "Card.h" work from any .cpp file without needing path prefixes.
target_include_directories(TradingCardGame PRIVATE include)

# Pune .exe-ul direct în folderul principal al proiectului (nu în build/Release/).
set_target_properties(TradingCardGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)

# Copy deck.txt next to the executable after each build so the game finds it at runtime.
add_custom_command(TARGET TradingCardGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/data/deck.txt
        $<TARGET_FILE_DIR:TradingCardGame>/deck.txt
)
