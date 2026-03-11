project(gate_server LANGUAGES CXX)
List(PREPEND CMAKE_PREFIX_PATH
    "D:/C++Library/mysql-connector-c++-8.3.0-winx64"
    "D:/C++Library/spdlog-1.17.0/build/install/lib/cmake"
    "D:/C++Library/json-3.12.0/build/install/share/cmake"
    "E:/WORKSPACE/Qt/Chat_App/conan_cmake_deps"
)
find_package(Boost CONFIG REQUIRED)

set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)

# find_package(jsoncpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(grpc CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(mysql-concpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(${PROJECT_NAME}  
    src/main.cpp src/common.h src/singleton.h src/utils.h 
    src/iocontext_pool.h src/iocontext_pool.cpp
    src/config_mgr.h src/config_mgr.cpp
    src/cserver.cpp src/cserver.h
    src/csession.cpp src/csession.h
    src/message.grpc.pb.h src/message.grpc.pb.cc
    src/message.pb.h src/message.pb.cc
    src/logic_system.cpp src/logic_system.h
    src/verify_grpc_client.h src/verify_grpc_client.cpp
    src/redis_mgr.h src/redis_mgr.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Boost::headers Boost::boost Boost::filesystem Boost::system 
    nlohmann_json::nlohmann_json  
    gRPC::grpc++ gRPC::grpc 
    hiredis::hiredis 
    mysql::concpp
    D:/C++Library/mysql-connector-c++-8.3.0-winx64/lib64/vs14/mysqlcppconn.lib
    spdlog::spdlog
)

configure_file(data/config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
            -DSRC_DLLS="$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
            -DDEST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/CopyRuntimeDependencies.cmake"
    COMMAND_EXPAND_LISTS
)