/*********************************************************************\
    Original Author: Richard Bateman

    Created:    Jan 18, 2015
    License:    Dual license model; choose one of two:
                New BSD License
                http://www.opensource.org/licenses/bsd-license.php
                - or -
                GNU Lesser General Public License, version 2.1
                http://www.gnu.org/licenses/lgpl-2.1.html

    Copyright 2015 Richard Bateman and the Firebreath development team
\*********************************************************************/

#if 1

#include "FireWyrm.h"
#include "WyrmColony.h"
#include "logging.h"

using namespace FB::FireWyrm;

FW_RESULT FireWyrm_Init(FWHostFuncs* hFuncs, FWColonyFuncs* cFuncs) {
    FBLOG_INFO("FireWyrm", "Initializing FireWyrm plugin");
    WyrmColony* mod = WyrmColony::GetColony(0);
    mod->setFuncs(hFuncs);
    mod->populateFuncs(cFuncs);
    return 0;
}
FW_RESULT FireWyrm_Finit() {
    FBLOG_INFO("FireWyrm", "Finalizing FireWyrm plugin");
    return WyrmColony::ReleaseColony(0);
}

#endif
