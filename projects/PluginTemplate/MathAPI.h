/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"

class MathAPI : public FB::JSAPIAuto
{
public:
    virtual ~MathAPI();

    MathAPI();

    std::string returnString(const std::string& s);

    std::string intToString(int i);

    int sumOf(int a, int b);

    std::string concatenate(const std::string& a, const std::string& b, const std::string& c);

    std::string concatenate2(const std::string& a, const FB::VariantList& list);

    void set_Message(const std::string& s);

    std::string get_Message();

    std::string get_ReadOnlyMessage();

    std::string getType(const FB::variant a);

private:
    std::string m_message;
    const std::string m_readOnlyMessage;
};
