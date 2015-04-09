
function (configure_crx_template TEMPLATE_DIR EXT_DIR)
    file(GLOB TEMPLATELIST ${TEMPLATE_DIR}/[^.]*)
    foreach(TemplateFile ${TEMPLATELIST})
        get_filename_component(CURFILE ${TemplateFile} NAME)
        get_filename_component(CUREXT ${TemplateFile} EXT)
        set (SOURCE_FILE "${TEMPLATE_DIR}/${CURFILE}")
        if (CUREXT STREQUAL ".png" OR CUREXT STREQUAL ".ico" OR CUREXT STREQUAL ".jpg")
            if (VERBOSE)
                message(STATUS "Copying Chrome extension template file ${CURFILE}")
            endif()
            file(COPY "${SOURCE_FILE}" DESTINATION "${EXT_DIR}/")
        else()
            if (VERBOSE)
                message("Configuring Chrome extension template file ${CURFILE}")
            endif()
            configure_template("${SOURCE_FILE}" "${EXT_DIR}/${CURFILE}")
        endif()
    endforeach()
endfunction(configure_crx_template)

function (configure_chrome_wyrmhole)

    set (EXT_DIR "${CMAKE_CURRENT_BINARY_DIR}/NativeMessageExtension")
    message(STATUS "Chrome extension (unpacked) will be built in ${EXT_DIR}")
    if (NOT EXISTS ${EXT_DIR})
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/NativeMessageExtension")
    endif()
    set (TEMPLATE_DIR "${FB_ROOT}/crx_template")
    set (TEMPLATE_OVERRIDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/crx_template")

    configure_crx_template("${TEMPLATE_DIR}" "${EXT_DIR}")

    if (EXISTS "${TEMPLATE_OVERRIDE_DIR}")
        message(STATUS "${TEMPLATE_OVERRIDE_DIR} found; using custom files to override default crx files")
        configure_crx_template("${TEMPLATE_OVERRIDE_DIR}" "${EXT_DIR}")
    else()
        message(STATUS "${TEMPLATE_OVERRIDE_DIR} does not exist; using default crx files")
    endif()

endfunction(configure_chrome_wyrmhole)

function (get_plugin_path OUTPUT_PATH TARGETNAME)

    cmake_policy(SET CMP0026 OLD)
    if (APPLE)
        firebreath_find_plugin_path(OUTPATH ${TARGETNAME})
    else()
        get_property(OUTPATH TARGET ${TARGETNAME} PROPERTY LOCATION)
    endif()
    set(${OUTPUT_PATH} ${OUTPATH} PARENT_SCOPE)

endfunction(get_plugin_path)

function (add_native_message_host TARGETNAME)

    get_plugin_path(PLUGIN_PATH ${TARGETNAME})
    set(CFGFILE_SCRIPT "${FB_ROOT}/cmake/GenericConfigureFileScript.cmake")
    message(STATUS "Native message host to be built in $<TARGET_FILE:FireWyrmNativeMessageHost>")
    message(STATUS "Plugin to be built in $<TARGET_FILE_DIR:${TARGETNAME}>")

    add_custom_command(
        TARGET ${TARGETNAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:FireWyrmNativeMessageHost>" "$<TARGET_FILE_DIR:${TARGETNAME}>/"
        COMMAND ${CMAKE_COMMAND} -D INFILE="${CMAKE_CURRENT_BINARY_DIR}/gen/fwh-chrome-manifest.json"
                                 -D OUTFILE="$<TARGET_FILE_DIR:${TARGETNAME}>/${PLUGIN_CRX_NATIVEHOST_NAME}.json"
                                 -D NAMESHOST="$<TARGET_FILE_DIR:${TARGETNAME}>/$<TARGET_FILE_NAME:FireWyrmNativeMessageHost>"
                                 -P ${CFGFILE_SCRIPT}
        )
    if (UNIX)
        if (APPLE)
            set(HOST_REGISTER_DIR "~/Library/Application Support/Google/Chrome/NativeMessagingHosts")
        else()
            set(HOST_REGISTER_DIR "~/.config/google-chrome/NativeMessagingHosts")
        endif()
        add_custom_target(
            ${TARGETNAME}_install
            COMMAND ${CMAKE_COMMAND} -E make_directory ${HOST_REGISTER_DIR}
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                                        "$<TARGET_FILE_DIR:${TARGETNAME}>/${PLUGIN_CRX_NATIVEHOST_NAME}.json"
                                        "${HOST_REGISTER_DIR}/${PLUGIN_CRX_NATIVEHOST_NAME}.json"
            COMMAND ${CMAKE_COMMAND} -E remove -f "~/Library/Internet Plug-Ins/$<TARGET_FILE_NAME:${TARGETNAME}>.plugin"
            COMMAND ${CMAKE_COMMAND} -E remove_directory "~/Library/Internet Plug-Ins/$<TARGET_FILE_NAME:${TARGETNAME}>.plugin"
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                                        "${PLUGIN_PATH}"
                                        "~/Library/Internet Plug-Ins/$<TARGET_FILE_NAME:${TARGETNAME}>.plugin"
            DEPENDS ${TARGETNAME}
            )
    elseif(WIN32)
        add_custom_target(
            ${TARGETNAME}_install
            COMMAND ${CMAKE_COMMAND} -E write_regv "HKCU\\Software\\Google\\Chrome\\NativeMessagingHosts\\${PLUGIN_CRX_NATIVEHOST_NAME}"
                                                   "$<TARGET_FILE_DIR:${TARGETNAME}>/${PLUGIN_CRX_NATIVEHOST_NAME}.json"
            COMMAND regsvr32 /s "${PLUGIN_PATH}"
            DEPENDS ${TARGETNAME}
            )
    endif()

endfunction()
