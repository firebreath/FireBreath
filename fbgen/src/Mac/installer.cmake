set(INSTALLER_NAME "${PLUGIN_NAME} Installer")

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMENT "------------ CREATE DMG INSTALLER"

    #replace the copy with svn/git/whatever export if needed
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/Mac/dmg_template ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template
    COMMAND cp -R ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.plugin ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template

    #Give an icon to your bundle
    #COMMAND sips -i ${CMAKE_CURRENT_SOURCE_DIR}/Mac/icon.png
    #COMMAND /Developer/Tools/DeRez -only icns ${CMAKE_CURRENT_SOURCE_DIR}/Mac/icon.png > ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tempicns.rsrc
    #COMMAND /Developer/Tools/Rez -append ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tempicns.rsrc -o `printf "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template/${PLUGIN_NAME}.plugin/Icon\r"` 
    COMMAND /Developer/Tools/SetFile -a C ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template/${PLUGIN_NAME}.plugin/
    COMMAND ln -s /Library/Internet\ Plug-Ins ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template/
    COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template/Internet\ Plug-Ins ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template/Plugins

    #Create the DMG
    COMMAND /usr/bin/hdiutil create -fs HFS+ -srcfolder ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template/ -volname "${INSTALLER_NAME}" -format UDRW ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}-temp.dmg
    COMMAND hdiutil attach ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}-temp.dmg -noautoopen -quiet

    #Wait for the installer to mount
    COMMAND sleep 2
    COMMAND osascript ${CMAKE_CURRENT_SOURCE_DIR}/Mac/dmgdesign.applescript ${PLUGIN_NAME}.plugin "${INSTALLER_NAME}"
    COMMAND sleep 2
    COMMAND hdiutil attach ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}-temp.dmg -noautoopen -quiet
    #Repeat the commands, as they are not always executed o_O
    COMMAND sleep 2
    COMMAND osascript ${CMAKE_CURRENT_SOURCE_DIR}/Mac/dmgdesign.applescript ${PLUGIN_NAME}.plugin "${INSTALLER_NAME}"
    COMMAND sleep 2

    COMMAND hdiutil convert ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}-temp.dmg -format UDZO -imagekey zlib-level=9 -o ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.dmg

    COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}-temp.dmg
    COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/dmg_template
    #COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tempicns.rsrc
)
