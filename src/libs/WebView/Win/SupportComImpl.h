/**********************************************************\
Original Author: Vijaye Raji

Created:    Jun 23, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/

#pragma once
#include <Windows.h>
#include <exdisp.h>
#include <mshtml.h>
#include <mshtmhst.h>

class DocHostUIHandler : public IDocHostUIHandler {
public:
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) {
        if (riid == IID_IUnknown || riid == IID_IDocHostUIHandler) {
            *ppvObject = this;
            return S_OK;
        }
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() { return 1; }
    ULONG __stdcall Release() { return 1; }

    HRESULT __stdcall ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved) { return S_FALSE; }
    HRESULT __stdcall GetHostInfo(DOCHOSTUIINFO *pInfo) {
        pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
        return S_OK;
    }

    HRESULT __stdcall ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) { return E_NOTIMPL; }
    HRESULT __stdcall HideUI() { return E_NOTIMPL; }
    HRESULT __stdcall UpdateUI() { return E_NOTIMPL; }
    HRESULT __stdcall EnableModeless(BOOL fEnable) { return E_NOTIMPL; }
    HRESULT __stdcall OnDocWindowActivate(BOOL fActivate) { return E_NOTIMPL; }
    HRESULT __stdcall OnFrameWindowActivate(BOOL fActivate) { return E_NOTIMPL; }
    HRESULT __stdcall ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) { return E_NOTIMPL; }
    HRESULT __stdcall TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) { return E_NOTIMPL; }
    HRESULT __stdcall GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw) { return E_NOTIMPL; }
    HRESULT __stdcall GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) { return E_NOTIMPL; }
    HRESULT __stdcall GetExternal(IDispatch **ppDispatch) { return E_NOTIMPL; }
    HRESULT __stdcall TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) { return E_NOTIMPL; }
    HRESULT __stdcall FilterDataObject(IDataObject *pDO, IDataObject **ppDORet) { return E_NOTIMPL; }
};

class OleFrame : public IOleInPlaceFrame {
private:
    HWND hostWindow;
public:
    OleFrame(HWND hostWindow) {
        this->hostWindow = hostWindow;
    }
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) { return E_NOINTERFACE; }
    ULONG __stdcall AddRef() { return 1; }
    ULONG __stdcall Release() { return 1; }

    HRESULT __stdcall GetWindow(HWND *phwnd) {
        phwnd = &this->hostWindow;
        return S_OK;
    }
    HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }
    HRESULT __stdcall GetBorder(LPRECT lprectBorder) { return E_NOTIMPL; }
    HRESULT __stdcall RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
    HRESULT __stdcall SetBorderSpace(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
    HRESULT __stdcall SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) { return S_OK; }
    HRESULT __stdcall InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) { return E_NOTIMPL; }
    HRESULT __stdcall SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) { return S_OK; }
    HRESULT __stdcall RemoveMenus(HMENU hmenuShared) { return E_NOTIMPL; }
    HRESULT __stdcall SetStatusText(LPCOLESTR pszStatusText) { return S_OK; }
    HRESULT __stdcall EnableModeless(BOOL fEnable) { return S_OK; }
    HRESULT __stdcall TranslateAccelerator(LPMSG lpmsg, WORD wID) { return E_NOTIMPL; }
};

class OleInPlaceSite : IOleInPlaceSite {
private:
    OleFrame* oleFrame;
    HWND hostWindow;

public:
    OleInPlaceSite() { hostWindow = NULL; }
    HWND GetHostWindow() { return this->hostWindow; }

    void SetHostWindow(HWND hostWindow) {
        this->oleFrame = new OleFrame(hostWindow);
        this->hostWindow = hostWindow;
    }

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) { return E_NOINTERFACE; }
    ULONG __stdcall AddRef() { return 1; }
    ULONG __stdcall Release() { return 1; }
    HRESULT __stdcall GetWindow(HWND *phwnd) {
        *phwnd = this->hostWindow;
        return S_OK;
    }
    HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }
    HRESULT __stdcall CanInPlaceActivate() { return S_OK; }
    HRESULT __stdcall OnInPlaceActivate() { return S_OK; }
    HRESULT __stdcall OnUIActivate() { return S_OK; }
    HRESULT __stdcall GetWindowContext(
        IOleInPlaceFrame **ppFrame,
        IOleInPlaceUIWindow **ppDoc,
        LPRECT lprcPosRect,
        LPRECT lprcClipRect,
        LPOLEINPLACEFRAMEINFO lpFrameInfo)
    {
        *ppFrame = this->oleFrame;

        *ppDoc = NULL;
        lpFrameInfo->fMDIApp = FALSE;
        lpFrameInfo->hwndFrame = this->hostWindow;
        lpFrameInfo->haccel = NULL;
        lpFrameInfo->cAccelEntries = NULL;

        GetClientRect(this->hostWindow, lprcPosRect);
        GetClientRect(this->hostWindow, lprcClipRect);

        return S_OK;
    }
    HRESULT __stdcall Scroll(SIZE scrollExtant) { return E_NOTIMPL; }
    HRESULT __stdcall OnUIDeactivate(BOOL fUndoable) { return S_OK; }
    HRESULT __stdcall OnInPlaceDeactivate() { return S_OK; }
    HRESULT __stdcall DiscardUndoState() { return E_NOTIMPL; }
    HRESULT __stdcall DeactivateAndUndo() { return E_NOTIMPL; }
    HRESULT __stdcall OnPosRectChange(LPCRECT lprcPosRect) { return S_OK; }
};

class OleClientSite : public IOleClientSite
{
public:
    OleInPlaceSite *inplaceSite;

    OleClientSite() {
        this->inplaceSite = new OleInPlaceSite();
    }

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) {
        if (riid == IID_IUnknown || riid == IID_IOleClientSite) {
            *ppvObject = this;
            return S_OK;
        } else if (riid == IID_IOleInPlaceSite) {
            *ppvObject = this->inplaceSite;
            return S_OK;
        }

        *ppvObject = 0;
		return E_NOINTERFACE;
    }

    ULONG __stdcall AddRef() { return 1; }
    ULONG __stdcall Release() { return 1; }
    HRESULT __stdcall SaveObject() { return E_NOTIMPL; }
    HRESULT __stdcall GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) { return E_NOTIMPL; }
    HRESULT __stdcall GetContainer(IOleContainer **ppContainer) {
        *ppContainer = 0;
        return E_NOINTERFACE;
    }
    HRESULT __stdcall ShowObject() { return NOERROR; }
    HRESULT __stdcall OnShowWindow(BOOL fShow) { return E_NOTIMPL; }
    HRESULT __stdcall RequestNewObjectLayout() { return E_NOTIMPL; }
};

class OleStorage : public IStorage {
public:
    OleStorage() {}

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) { return E_NOTIMPL; }
    ULONG __stdcall AddRef() { return 1; }
    ULONG __stdcall Release() { return 1; }
    HRESULT __stdcall CreateStream(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
    HRESULT __stdcall OpenStream(const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
    HRESULT __stdcall CreateStorage(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) { return E_NOTIMPL; }
    HRESULT __stdcall OpenStorage(const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, __RPC__deref_opt_in_opt SNB snbExclude, DWORD reserved, IStorage **ppstg) { return E_NOTIMPL; }
    HRESULT __stdcall CopyTo(DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest) { return E_NOTIMPL; }
    HRESULT __stdcall MoveElementTo(const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) { return E_NOTIMPL; }
    HRESULT __stdcall Commit(DWORD grfCommitFlags) { return E_NOTIMPL; }
    HRESULT __stdcall Revert() { return E_NOTIMPL; }
    HRESULT __stdcall EnumElements(DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum) { return E_NOTIMPL; }
    HRESULT __stdcall DestroyElement(const OLECHAR *pwcsName) { return E_NOTIMPL; }
    HRESULT __stdcall RenameElement(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName) { return E_NOTIMPL; }
    HRESULT __stdcall SetElementTimes(const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime) { return E_NOTIMPL; }
    HRESULT __stdcall SetClass(__RPC__in REFCLSID clsid) { return S_OK; }
    HRESULT __stdcall SetStateBits(DWORD grfStateBits, DWORD grfMask) { return E_NOTIMPL; }
    HRESULT __stdcall Stat(STATSTG *pstatstg, DWORD grfStatFlag) { return E_NOTIMPL; }
};



