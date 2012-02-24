/**********************************************************\ 
Original Author: Roee Shenberg

Created:    Feb 21, 2012
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

\**********************************************************/

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "Node.h"

using namespace FB::ActiveX::AXDOM;

Node::Node(const FB::JSObjectPtr& element, IWebBrowser *web)
    : m_axNode(FB::ptr_cast<IDispatchAPI>(element)->getIDispatch()),
        m_webBrowser(web), FB::DOM::Node(element)
{
}

Node::~Node() 
{
}

FB::DOM::NodePtr Node::appendChild(FB::DOM::NodePtr node) {
	CComPtr<IHTMLDOMNode> newNode;
	NodePtr actualNode = FB::ptr_cast<Node>(node);
	if (SUCCEEDED(m_axNode->appendChild(actualNode->m_axNode, &newNode))) {
		actualNode->m_axNode = newNode; // set the node to point to the real IHTMLDOMNode
	} else {
		throw std::runtime_error("failed to appendChild!");
	}
	return node;
}

