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

		#Replace the plugin version in the install.rdf template
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi)
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/xpi/content/chrome.manifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi)
		configure_template(${CMAKE_CURRENT_SOURCE_DIR}/xpi/content/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi/install.rdf)

		add_custom_command(
			TARGET ${PROJNAME}${FB_XPI_SUFFIX}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory "\"${CMAKE_CURRENT_BINARY_DIR}/gen/${FBSTRING_PluginFileName}-xpi\"" "\"${XPI_OUTDIR}/${FBSTRING_PluginFileName}/\""
			COMMAND mkdir "\"${XPI_OUTDIR}/${FBSTRING_PluginFileName}/plugins/\""
			COMMAND ${CMAKE_COMMAND} -E copy "${DLLFILE}" "\"${XPI_OUTDIR}/${FBSTRING_PluginFileName}/plugins/\""
			COMMAND java -jar ${XPISIGNERPATH} ${PFXFILE} ${PASSPHRASE} "${XPI_OUTDIR}/${FBSTRING_PluginFileName}" "${XPI_OUTDIR}/${FBSTRING_PluginFileName}.xpi"
			COMMAND popd
			COMMAND ${CMAKE_COMMAND} -E remove_directory "${XPI_OUTDIR}/${FBSTRING_PluginFileName}"
		 )
        ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_SUFFIX} ${XPI_PROJDEP})

endfunction(add_signed_xpi_installer)
