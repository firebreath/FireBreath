#ifndef H_FB_BROWSERHOSTWRAPPER
#define H_FB_BROWSERHOSTWRAPPER

#include "APITypes.h"

namespace FB
{
	class BrowserHostWrapper
	{
	public:
		BrowserHostWrapper() { }
		virtual ~BrowserHostWrapper() { }

	public:
        virtual bool FireEvent(std::string name, variant &target, variant *args, int argCount) = 0;
	};
}

#endif