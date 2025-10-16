add_library(CoronaLogger STATIC)
add_library(Corona::Logger ALIAS CoronaLogger)

target_sources(CoronaLogger
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/corona_logger.cpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/corona_logger.h>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/corona_logger.h>
)

target_compile_features(CoronaLogger PUBLIC cxx_std_20)


target_include_directories(CoronaLogger
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CoronaLogger PUBLIC spdlog::spdlog)

if(CORONA_LOGGER_INSTALL)
    install(TARGETS CoronaLogger
        EXPORT CoronaLoggerTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(FILES ${PROJECT_SOURCE_DIR}/include/corona_logger.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT CoronaLoggerTargets
        FILE CoronaLoggerTargets.cmake
        NAMESPACE Corona::
        DESTINATION ${CORONA_LOGGER_INSTALL_CONFIGDIR}
    )
endif()
