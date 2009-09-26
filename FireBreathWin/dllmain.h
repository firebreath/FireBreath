// dllmain.h : Declaration of module class.

class CFireBreathWinModule : public CAtlDllModuleT< CFireBreathWinModule >
{
public :
    DECLARE_LIBID(LIBID_FireBreathWinLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FIREBREATHWIN, "{B415CD14-B45D-4BCA-B552-B06175C38606}")
};

extern class CFireBreathWinModule _AtlModule;
