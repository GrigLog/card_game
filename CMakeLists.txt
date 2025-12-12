cmake_minimum_required(VERSION 3.22)
project(card_game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Кросс-платформенные настройки
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Опции сборки
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Включаем директории
include_directories(${CMAKE_SOURCE_DIR}/include)

# Исходные файлы
set(SOURCES
    src/Card.cpp
    src/Deck.cpp
    src/GameEvent.cpp
    src/Action.cpp
    src/BotStrategy.cpp
    src/BotPlayer.cpp
    src/HumanPlayer.cpp
    src/GameSession.cpp
    src/GameServer.cpp
)

# Основной исполняемый файл
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    src/main.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Тесты
if(BUILD_TESTS)
    enable_testing()
    
    # Поиск Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    # Для Windows: отключаем установку googletest
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Файлы тестов
    file(GLOB TEST_SOURCES "test/*.cpp")
    
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE} ${SOURCES})
        target_link_libraries(${TEST_NAME} 
            PRIVATE 
            GTest::gtest 
            GTest::gtest_main
            GTest::gmock
        )
        target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()

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
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
