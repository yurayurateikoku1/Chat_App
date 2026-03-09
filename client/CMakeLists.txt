
project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
List(PREPEND CMAKE_PREFIX_PATH
    "D:/C++Library/spdlog-1.17.0/build/install/lib/cmake"
)
find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2 Network)
find_package(spdlog CONFIG REQUIRED)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appclient
    src/main.cpp src/singleton.h
    src/pages/login_page.cpp     src/pages/login_page.h
    src/pages/register_page.cpp  src/pages/register_page.h
    src/http_mgr.cpp            src/http_mgr.h
)

qt_add_resources(appclient "resources"
    PREFIX "/"
    FILES
        assets/chat.png
)

set_source_files_properties(src/pages/Common.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(appclient
    URI client
    VERSION 1.0
    QML_FILES
        src/pages/Main.qml         src/pages/Common.qml 
        src/pages/LoginPageView.qml
        src/pages/RegisterPageView.qml
        src/components/TextFieldComp.qml src/components/ButtonComp.qml src/components/ToastComp.qml

)

set_target_properties(appclient PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE 
)

target_include_directories(appclient PRIVATE src/pages)

target_link_libraries(appclient
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Network
    PRIVATE spdlog::spdlog
)

if(WIN32)
    # 使用 windeployqt 自动部署所有 DLL + QML 插件
    find_program(WINDEPLOYQT windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT)
        add_custom_command(TARGET appclient POST_BUILD
            COMMAND "${WINDEPLOYQT}"
                    --no-compiler-runtime
                    --no-translations
                    --no-system-d3d-compiler
                    --no-system-dxc-compiler
                    --no-opengl-sw
                    --no-ffmpeg
                    --skip-plugin-types qmltooling,generic
                    --qmldir "${CMAKE_CURRENT_SOURCE_DIR}"
                    "$<TARGET_FILE:appclient>"
            COMMENT "Running windeployqt..."
        )
    endif()

    configure_file(data/config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)
endif()

include(GNUInstallDirs)
install(TARGETS appclient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
