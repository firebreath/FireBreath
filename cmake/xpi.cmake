# Build a signed XPI package to distribute the plugin
# 2012/01 Geoffroy Couprie geoffroy.couprie@loginpeople.com

function (add_signed_xpi_installer PROJNAME XPI_OUTDIR DLLFILE XPISIGNERPATH PFXFILE PASSFILE XPI_PROJDEP)
		message(STATUS "CONFIGURING XPI PACKAGING")
        set (XPI_SOURCES
                ${FB_ROOT}/cmake/dummy.cpp
            )
        if (NOT FB_XPI_SUFFIX)
             set (FB_XPI_SUFFIX _XPI)
        endif()
        ADD_LIBRARY(${PROJNAME}${FB_XPI_SUFFIX} STATIC ${XPI_SOURCES})

		if (NOT "${PASSFILE}" STREQUAL "")
			file(STRINGS "${PASSFILE}" PASSPHRASE LIMIT_COUNT 1)
        endif()

			if ("${XPISIGNERPATH}" STREQUAL "")
			  set(SIGN_COMMAND cd "${XPI_OUTDIR}/${FBSTRING_PluginFileName}" && rm -f "../${FBSTRING_PluginFileName}.xpi" && zip -r "../${FBSTRING_PluginFileName}.xpi" *)
			else()	  
			  set(SIGN_COMMAND java -jar ${XPISIGNERPATH} ${PFXFILE} ${PASSPHRASE} "${XPI_OUTDIR}/${FBSTRING_PluginFileName}" "${XPI_OUTDIR}/${FBSTRING_PluginFileName}.xpi")
			endif()

		#Replace the plugin version in the install.rdf template
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi)
		if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/xpi/content/chrome.manifest)
		  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/xpi/content/chrome.manifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi)
		endif()
		configure_template(${CMAKE_CURRENT_SOURCE_DIR}/xpi/content/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi/install.rdf)

		add_custom_command(
			TARGET ${PROJNAME}${FB_XPI_SUFFIX}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E remove_directory "${XPI_OUTDIR}/${FBSTRING_PluginFileName}"
			COMMAND ${CMAKE_COMMAND} -E copy_directory "\"${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi\"" "\"${XPI_OUTDIR}/${FBSTRING_PluginFileName}/\""
			COMMAND mkdir "\"${XPI_OUTDIR}/${FBSTRING_PluginFileName}/plugins/\""
			COMMAND ${CMAKE_COMMAND} -E copy "${DLLFILE}" "\"${XPI_OUTDIR}/${FBSTRING_PluginFileName}/plugins/\""
			COMMAND ${SIGN_COMMAND}
			COMMAND ${CMAKE_COMMAND} -E remove_directory "${XPI_OUTDIR}/${FBSTRING_PluginFileName}"
		 )
        ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_SUFFIX} ${XPI_PROJDEP})

endfunction(add_signed_xpi_installer)
