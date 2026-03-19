
project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
List(PREPEND CMAKE_PREFIX_PATH
    "D:/C++Library/spdlog-1.17.0/build/install/lib/cmake"
)
find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2 Network)
find_package(spdlog CONFIG REQUIRED)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(client
    src/main.cpp src/singleton.h
    src/pages/login_page.cpp     src/pages/login_page.h
    src/pages/register_page.cpp  src/pages/register_page.h
    src/pages/reset_page.cpp     src/pages/reset_page.h
    src/http_mgr.cpp            src/http_mgr.h
    src/tcp_mgr.cpp             src/tcp_mgr.h
    src/user_mgr.cpp            src/user_mgr.h
    src/pages/chat_page.cpp     src/pages/chat_page.h
    src/models/chat_message_model.cpp src/models/chat_message_model.h
    src/models/chat_list_model.cpp src/models/chat_list_model.h
    src/models/user_list_model.cpp src/models/user_list_model.h
    src/models/user_item.h
)

qt_add_resources(client "resources"
    PREFIX "/"
    FILES
        assets/chat.png assets/kejianxing-bukejian.png assets/kejianxing-kejian.png
        assets/duihuaxinxi.png assets/fenxiang.png assets/kaixin.png
        assets/shangchuan.png assets/sousuo.png assets/tianjiayonghu.png
        assets/tongxunlu.png assets/agree.png assets/disagree.png
        assets/avatars/Artboard_0.png
        assets/avatars/Artboard_1.png assets/avatars/Artboard_2.png assets/avatars/Artboard_3.png
        assets/avatars/Artboard_4.png assets/avatars/Artboard_5.png assets/avatars/Artboard_6.png
        assets/avatars/Artboard_7.png assets/avatars/Artboard_8.png assets/avatars/Artboard_9.png
        assets/avatars/Artboard_10.png assets/avatars/Artboard_11.png assets/avatars/Artboard_12.png
        assets/avatars/Artboard_13.png assets/avatars/Artboard_14.png assets/avatars/Artboard_15.png
        assets/avatars/Artboard_16.png assets/avatars/Artboard_17.png assets/avatars/Artboard_18.png
        assets/avatars/Artboard_19.png assets/avatars/Artboard_20.png assets/avatars/Artboard_21.png
        assets/avatars/Artboard_22.png assets/avatars/Artboard_23.png assets/avatars/Artboard_24.png
        assets/avatars/Artboard_25.png assets/avatars/Artboard_26.png assets/avatars/Artboard_27.png
        assets/avatars/Artboard_28.png assets/avatars/Artboard_29.png assets/avatars/Artboard_30.png
        assets/avatars/Artboard_31.png assets/avatars/Artboard_32.png assets/avatars/Artboard_33.png
        assets/avatars/Artboard_34.png assets/avatars/Artboard_35.png
)

set_source_files_properties(src/pages/Common.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(client
    URI client
    VERSION 1.0
    QML_FILES
        src/base_components/TextFieldComp.qml src/base_components/ButtonComp.qml src/base_components/ToastComp.qml
        src/base_components/TimerButtonComp.qml src/base_components/ToolButtonComp.qml src/base_components/ChatHeaderComp.qml
        src/base_components/ChatListItem.qml src/base_components/ContactListItem.qml src/base_components/TextEditComp.qml
        src/base_components/ChatBubbleComp.qml src/base_components/FriendRequestListItem.qml
        
        src/chat_components/CompoundList.qml          src/chat_components/ChatList.qml 
        src/chat_components/MessageWindowList.qml  src/chat_components/SideBarComp.qml
           src/chat_components/ChatPageItem0.qml
        src/chat_components/ContactDialog.qml  src/chat_components/FriendRequestList.qml
        src/chat_components/AddUserDialog.qml src/chat_components/ContactList.qml
        src/chat_components/SearchList.qml 
        
        src/pages/Main.qml         src/pages/Common.qml src/pages/ChatPageView.qml
        src/pages/LoginPageView.qml         src/pages/RegisterPageView.qml         src/pages/ResetPageView.qml

                
)

set_target_properties(client PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE 
)

target_include_directories(client PRIVATE src/pages)

target_link_libraries(client
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Network
    PRIVATE spdlog::spdlog
)

if(WIN32)
    # 使用 windeployqt 自动部署所有 DLL + QML 插件
    find_program(WINDEPLOYQT windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT)
        add_custom_command(TARGET client POST_BUILD
            COMMAND "${WINDEPLOYQT}"
                    --no-compiler-runtime
                    --no-translations
                    --no-system-d3d-compiler
                    --no-system-dxc-compiler
                    --no-opengl-sw
                    --no-ffmpeg
                    --skip-plugin-types qmltooling,generic
                    --qmldir "${CMAKE_CURRENT_SOURCE_DIR}"
                    "$<TARGET_FILE:client>"
            COMMENT "Running windeployqt..."
        )
    endif()

    configure_file(data/config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)
endif()

include(GNUInstallDirs)
install(TARGETS client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
