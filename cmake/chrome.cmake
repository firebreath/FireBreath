# Build a signed Chrome package to distribute the plugin
# 2012/01 Geoffroy Couprie geoffroy.couprie@loginpeople.com

function (add_chrome_package PROJNAME CRX_OUTDIR DLLFILE KEYFILE CRX_PROJDEP)
		message(STATUS "CONFIGURING CRX PACKAGING")
        set (CRX_SOURCES
                ${FB_ROOT}/cmake/dummy.cpp
            )
        if (NOT FB_CRX_SUFFIX)
             set (FB_CRX_SUFFIX _CRX)
        endif()
        ADD_LIBRARY(${PROJNAME}${FB_CRX_SUFFIX} STATIC ${CRX_SOURCES})

		if (NOT "${PASSFILE}" STREQUAL "")
			file(STRINGS "${PASSFILE}" PASSPHRASE LIMIT_COUNT 1)
        endif()

		add_custom_command(
			TARGET ${PROJNAME}${FB_CRX_SUFFIX}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory "\"${CMAKE_CURRENT_SOURCE_DIR}/Chrome/chromepackage\"" "\"${CRX_OUTDIR}/${FBSTRING_PluginFileName}/\""
			COMMAND ${CMAKE_COMMAND} -E copy ${DLLFILE} "\"${CRX_OUTDIR}/${FBSTRING_PluginFileName}/\""
			COMMAND $ENV{LOCALAPPDATA}\\Google\\Chrome\\Application\\chrome.exe --pack-extension="\"${CRX_OUTDIR}/${FBSTRING_PluginFileName}\"" --pack-extension-key="\"${KEYFILE}\"" --no-message-box
			COMMAND popd
			COMMAND ${CMAKE_COMMAND} -E remove_directory "${CRX_OUTDIR}/${FBSTRING_PluginFileName}"
		 )
        ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_SUFFIX} ${CRX_PROJDEP})

endfunction(add_chrome_package)
