cmake_minimum_required(VERSION 3.20)
project(TradingCardGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GAME_SOURCES
    src/Card.cpp
    src/UnitCard.cpp
    src/SpellCard.cpp
    src/StructureCard.cpp
    src/TrapCard.cpp
    src/EffectRegistry.cpp
    src/Player.cpp
    src/Game.cpp
    src/CardFactory.cpp
)

add_library(TradingCardGameCore ${GAME_SOURCES})
target_include_directories(TradingCardGameCore PUBLIC include)

add_executable(TradingCardGame src/main.cpp)
target_link_libraries(TradingCardGame PRIVATE TradingCardGameCore)

enable_testing()
add_executable(TradingCardGameTests tests/TradingCardGameTests.cpp)
target_link_libraries(TradingCardGameTests PRIVATE TradingCardGameCore)
add_test(NAME TradingCardGameTests COMMAND TradingCardGameTests)
set_tests_properties(TradingCardGameTests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(EdgeCaseTests tests/EdgeCaseTests.cpp)
target_link_libraries(EdgeCaseTests PRIVATE TradingCardGameCore)
add_test(NAME EdgeCaseTests COMMAND EdgeCaseTests)
set_tests_properties(TradingCardGameTests EdgeCaseTests PROPERTIES TIMEOUT 5)

# Copy deck.txt next to the executable after each build so the game finds it at runtime.
add_custom_command(TARGET TradingCardGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/data/deck.txt
        $<TARGET_FILE_DIR:TradingCardGame>/deck.txt
)

