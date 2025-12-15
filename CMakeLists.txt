cmake_minimum_required(VERSION 3.22)
project(card_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции сборки
option(BUILD_TESTS "Build tests" ON)

# Включаем директории
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

set (COMMON_SOURCES 
    src/common/common.h
    src/common/message.cpp
    src/common/message.h
)

# server.cpp is excluded from SERVER_SOURCES because we don't want to include the main() method
set (SERVER_SOURCES 
    src/server/accept_handler.cpp
    src/server/accept_handler.h
    src/server/game_room.cpp
    src/server/game_room.h
    src/server/game_server.cpp
    src/server/game_server.h
    src/server/pipe_wrapper.h
    src/server/player_manager.cpp    
    src/server/player_manager.h    
    src/server/command.cpp
    src/server/command.h
    src/server/actor/actor.cpp
    src/server/actor/actor.h
    src/server/actor/strategy.cpp
    src/server/actor/strategy.h
    src/server/durak/card.cpp
    src/server/durak/card.h
    src/server/durak/deck.cpp
    src/server/durak/deck.h
    src/server/durak/durak.cpp
    src/server/durak/durak.h
    src/server/durak/hand.h
)

# set (CLIENT_SOURCES ...)

add_executable(${PROJECT_NAME}_client src/client/client.cpp ${COMMON_SOURCES})
add_executable(${PROJECT_NAME}_server src/server/server.cpp ${SERVER_SOURCES} ${COMMON_SOURCES})

target_compile_features(${PROJECT_NAME}_client PUBLIC cxx_std_23)
target_compile_features(${PROJECT_NAME}_server PUBLIC cxx_std_23)

# Тесты
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    file(GLOB TEST_SOURCES "src/test/*.cpp")

    add_executable(test_card_game ${TEST_SOURCES} ${SERVER_SOURCES} ${COMMON_SOURCES})
    target_link_libraries(test_card_game GTest::gtest_main)
    

    include(GoogleTest)
    gtest_discover_tests(test_card_game)


endif()

# Установка (опционально)
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(DIRECTORY include/ DESTINATION include)
