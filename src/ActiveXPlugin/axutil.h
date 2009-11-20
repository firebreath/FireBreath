
#include "Win/win_common.h"

class FbPerUserRegistration
{
public:
	FbPerUserRegistration(bool perUser = true);
	~FbPerUserRegistration();

private:
#if _MSC_VER < 1500
	HKEY m_hkey;
#endif
};