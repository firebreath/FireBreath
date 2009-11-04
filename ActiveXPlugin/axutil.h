
#include "win_common.h"

class FbPerUserRegistration
{
public:
	FbPerUserRegistration(bool perUser = true);
	~FbPerUserRegistration();

private:
#if MSC_VER < 9
	HKEY m_hkey;
#endif
};