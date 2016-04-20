set(INSTALLER_NAME "@{PLUGIN_name}.pkg")

FIREBREATH_FIND_COMMANDS()

message(STATUS "Adding PKG installer for ${PROJECT_NAME}")

set(PLG_CRX_NAME ${PLUGIN_CRX_NATIVEHOST_NAME})
set(BNDL_NAME "@{PLUGIN_name}.plugin")
set(TMPL_RPATH "Contents/Resources/fwh-chrome-manifest.template")
set(NVH_RPATH "Contents/Resources/FireWyrmNativeMessageHost")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/scripts/postinstall ${CMAKE_CURRENT_BINARY_DIR}/scripts_tmp/postinstall @@ONLY)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/scripts_tmp/postinstall DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/scripts FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

set(PROD_TITLE ${FBSTRING_ProductName})
string(TOLOWER "@{COMPANY_revdomain}.@{PLUGIN_name}.pkg" PKG_ID)
string(REPLACE " " "_" PKG_ID ${PKG_ID})
set(PKG_NAME "@{PLUGIN_name}Res.pkg")
string(REPLACE " " "_" PKG_NAME ${PKG_NAME})
set(PLG_VERSION ${FBSTRING_PLUGIN_VERSION})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/distribution.xml ${CMAKE_CURRENT_BINARY_DIR}/distribution.xml @@ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/bundle_template/remove.sh ${CMAKE_CURRENT_BINARY_DIR}/remove.sh @@ONLY)

#To sign the package installer set variable 'SIGN_CERT_CN' to the code signing certificate's CN.
#The code signing certificate and the corresponding private key must be already in the keychain.
#If CN contains parentheses they must be escaped by "\\".
if(SIGN_CERT_CN)
    message(STATUS "Adding signature on ${SIGN_CERT_CN} certificate")
    set(PKGBUILD_SIGN_PARAM --sign)
else()
    message(STATUS "Package without signature")
    set(PKGBUILD_SIGN_PARAM "")
endif()

message(STATUS "PKGBUILD_SIGN_PARAM: ${PKGBUILD_SIGN_PARAM} ${SIGN_CERT_CN}")

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMENT "------------ CREATE PKG INSTALLER"
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/remove.sh ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.plugin/Contents
    COMMAND ${CMD_PKGBUILD} --component ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.plugin --identifier "${PKG_ID}" --version "${FBSTRING_PLUGIN_VERSION}" --install-location "/Library/Internet Plug-Ins" --scripts ${CMAKE_CURRENT_BINARY_DIR}/scripts ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PKG_NAME}
    COMMAND ${CMD_PRODUCTBUILD} --distribution=${CMAKE_CURRENT_BINARY_DIR}/distribution.xml --resources ${CMAKE_CURRENT_BINARY_DIR} --package-path ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR} ${PKGBUILD_SIGN_PARAM} ${SIGN_CERT_CN} "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/@{PLUGIN_name}.pkg"
    COMMAND ${CMD_RM} ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${PKG_NAME}
)
