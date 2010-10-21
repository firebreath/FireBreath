#/***********************************************************\
#Created:    Mar 10, 2010
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#
#Copyright 2010 Firebreath development team
#\**********************************************************/


if (APPLE)
    set (FBMAC_USE_CARBON        1)
    set (FBMAC_USE_COCOA         0)
    set (FBMAC_USE_QUICKDRAW     1)
    set (FBMAC_USE_COREGRAPHICS  0)
    set (FBMAC_USE_COREANIMATION 0)
endif(APPLE)

if (WIN32)
    set (FBWIN_WINDOWLESS 0)
endif(WIN32)

