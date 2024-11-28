set(EXEC_NAME Beadler)

# Mark as WIN32 executable so when compiling on Windows, this is flagged as 
# a GUI application with a WinMain() entry point instead of just main().
add_executable(
    ${EXEC_NAME}
    WIN32
    main.cpp
    MainFrame.cpp
    ../include/MainFrame.hpp
    App.cpp
    ../include/App.hpp
    SpritePanel.cpp
    ../include/SpritePanel.hpp
    SpriteScrolledPanel.cpp
    ../include/SpriteScrolledPanel.hpp
    ScrolledWindow.cpp
    ../include/ScrolledWindow.hpp
    ColorDifference.cpp
    ../include/ColorDifference.hpp
    ../include/ColorData.hpp
    JobsFrame.cpp
    ../include/JobsFrame.hpp
    JobManager.cpp
    ../include/JobManager.hpp
    ../include/Dispatcher.hpp
)

target_include_directories(
    ${EXEC_NAME}
    PUBLIC
    ../include)

target_link_libraries( 
    ${EXEC_NAME}
    PRIVATE
    bfg::lyra
    spdlog::spdlog
    ${PROJ_NAME}_project_warnings
    ${PROJ_NAME}_project_libraries
    ${PROJ_NAME}_project_options
)

################################################################################
# Installation
################################################################################
include(GNUInstallDirs)

install(
   TARGETS             ${EXEC_NAME}
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)