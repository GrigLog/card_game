cmake_minimum_required(VERSION 3.22)
project(card_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции сборки
option(BUILD_TESTS "Build tests" OFF)

# Включаем директории
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/common)

set (COMMON_SOURCES 
    src/common/common.cpp
    src/common/message.cpp
)

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
    src/server/server.cpp
)

set (CLIENT_SOURCES src/client/client.cpp)

add_executable(${PROJECT_NAME}_client ${CLIENT_SOURCES} ${COMMON_SOURCES})
add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES} ${COMMON_SOURCES})

target_compile_features(${PROJECT_NAME}_client PUBLIC cxx_std_23)
target_compile_features(${PROJECT_NAME}_server PUBLIC cxx_std_23)

# Тесты
# if(BUILD_TESTS)
#     enable_testing()
    
#     # Поиск Google Test
#     include(FetchContent)
#     FetchContent_Declare(
#         googletest
#         GIT_REPOSITORY https://github.com/google/googletest.git
#         GIT_TAG v1.14.0
#     )
    
#     # Для Windows: отключаем установку googletest
#     set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
#     FetchContent_MakeAvailable(googletest)
    
#     # Файлы тестов
#     file(GLOB TEST_SOURCES "test/*.cpp")
    
#     foreach(TEST_SOURCE ${TEST_SOURCES})
#         get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
#         add_executable(${TEST_NAME} ${TEST_SOURCE} ${SOURCES})
#         target_link_libraries(${TEST_NAME} 
#             PRIVATE 
#             GTest::gtest 
#             GTest::gtest_main
#             GTest::gmock
#         )
#         target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
#         add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
#     endforeach()
# endif()

# Проверка стиля кодирования с помощью clang-format
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    # Получаем все исходные файлы C++
    file(GLOB_RECURSE ALL_SOURCES 
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/include/*.h
        ${CMAKE_SOURCE_DIR}/test/*.cpp
    )
    
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_SOURCES}
        COMMENT "Checking code style with clang-format"
    )
    
    add_custom_target(format-fix
        COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCES}
        COMMENT "Fixing code style with clang-format"
    )
else()
    message(WARNING "clang-format not found. Code style checking will be skipped.")
endif()

# Установка (опционально)
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(DIRECTORY include/ DESTINATION include)
