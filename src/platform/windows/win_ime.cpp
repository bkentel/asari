#include "win_ime.h"

#include "common.h"

namespace asr::win
{

HRESULT __stdcall text_input_manager_old::QueryInterface(GUID const& riid, void** ppvObject) noexcept
{
    return m_manager.QueryInterface(riid, ppvObject);
}

ULONG __stdcall text_input_manager_old::AddRef() noexcept
{
    return m_manager.AddRef();
}

ULONG __stdcall text_input_manager_old::Release() noexcept
{
    return m_manager.Release();
}

HRESULT __stdcall text_input_manager_old::AdviseSink(REFIID riid, IUnknown* punk, DWORD dwMask) noexcept
{
    return m_manager.AdviseSink(riid, punk, dwMask);
}

HRESULT __stdcall text_input_manager_old::UnadviseSink(IUnknown* punk) noexcept
{
    return m_manager.UnadviseSink(punk);
}

HRESULT __stdcall text_input_manager_old::RequestLock(DWORD dwLockFlags, HRESULT* phrSession) noexcept
{
    return m_manager.RequestLock(dwLockFlags, phrSession);
}

HRESULT __stdcall text_input_manager_old::GetStatus(TS_STATUS* pdcs) noexcept
{
    return m_manager.GetStatus(pdcs);
}

HRESULT __stdcall text_input_manager_old::QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG* pacpResultStart, LONG* pacpResultEnd) noexcept
{
    return m_manager.QueryInsert(acpTestStart, acpTestEnd, cch, pacpResultStart, pacpResultEnd);
}

HRESULT __stdcall text_input_manager_old::GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP* pSelection, ULONG* pcFetched) noexcept
{
    return m_manager.GetSelection(ulIndex, ulCount, pSelection, pcFetched);
}

HRESULT __stdcall text_input_manager_old::SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) noexcept
{
    return m_manager.SetSelection(ulCount, pSelection);
}

HRESULT __stdcall text_input_manager_old::GetText(LONG acpStart, LONG acpEnd, WCHAR* pchPlain, ULONG cchPlainReq, ULONG* pcchPlainRet, TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfoRet, LONG* pacpNext) noexcept
{
    return m_manager.GetText(acpStart, acpEnd, pchPlain, cchPlainReq, pcchPlainRet, prgRunInfo, cRunInfoReq, pcRunInfoRet, pacpNext);
}

HRESULT __stdcall text_input_manager_old::SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange) noexcept
{
    return m_manager.SetText(dwFlags, acpStart, acpEnd, pchText, cch, pChange);
}

HRESULT __stdcall text_input_manager_old::GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject** ppDataObject) noexcept
{
    return m_manager.GetFormattedText(acpStart, acpEnd, ppDataObject);
}

HRESULT __stdcall text_input_manager_old::GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown** ppunk) noexcept
{
    return m_manager.GetEmbedded(acpPos, rguidService, riid, ppunk);
}

HRESULT __stdcall text_input_manager_old::QueryInsertEmbedded(const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable) noexcept
{
    return m_manager.QueryInsertEmbedded(pguidService, pFormatEtc, pfInsertable);
}

HRESULT __stdcall text_input_manager_old::InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange) noexcept
{
    return m_manager.InsertEmbedded(dwFlags, acpStart, acpEnd, pDataObject, pChange);
}

HRESULT __stdcall text_input_manager_old::InsertTextAtSelection(DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept
{
    return m_manager.InsertTextAtSelection(dwFlags, pchText, cch, pacpStart, pacpEnd, pChange);
}

HRESULT __stdcall text_input_manager_old::InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept
{
    return m_manager.InsertEmbeddedAtSelection(dwFlags, pDataObject, pacpStart, pacpEnd, pChange);
}

HRESULT __stdcall text_input_manager_old::RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs) noexcept
{
    return m_manager.RequestSupportedAttrs(dwFlags, cFilterAttrs, paFilterAttrs);
}

HRESULT __stdcall text_input_manager_old::RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept
{
    return m_manager.RequestAttrsAtPosition(acpPos, cFilterAttrs, paFilterAttrs, dwFlags);
}

HRESULT __stdcall text_input_manager_old::RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept
{
    return m_manager.RequestAttrsTransitioningAtPosition(acpPos, cFilterAttrs, paFilterAttrs, dwFlags);
}

HRESULT __stdcall text_input_manager_old::FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset) noexcept
{
    return m_manager.FindNextAttrTransition(acpStart, acpHalt, cFilterAttrs, paFilterAttrs, dwFlags, pacpNext, pfFound, plFoundOffset);
}

HRESULT __stdcall text_input_manager_old::RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL* paAttrVals, ULONG* pcFetched) noexcept
{
    return m_manager.RetrieveRequestedAttrs(ulCount, paAttrVals, pcFetched);
}

HRESULT __stdcall text_input_manager_old::GetEndACP(LONG* pacp) noexcept
{
    return m_manager.GetEndACP(pacp);
}

HRESULT __stdcall text_input_manager_old::GetActiveView(TsViewCookie* pvcView) noexcept
{
    return m_manager.GetActiveView(pvcView);
}

HRESULT __stdcall text_input_manager_old::GetACPFromPoint(TsViewCookie vcView, const POINT* ptScreen, DWORD dwFlags, LONG* pacp) noexcept
{
    return m_manager.GetACPFromPoint(vcView, ptScreen, dwFlags, pacp);
}

HRESULT __stdcall text_input_manager_old::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) noexcept
{
    return m_manager.GetTextExt(vcView, acpStart, acpEnd, prc, pfClipped);
}

HRESULT __stdcall text_input_manager_old::GetScreenExt(TsViewCookie vcView, RECT* prc) noexcept
{
    return m_manager.GetScreenExt(vcView, prc);
}

HRESULT __stdcall text_input_manager_old::GetWnd(TsViewCookie vcView, HWND* phwnd) noexcept
{
    return m_manager.GetWnd(vcView, phwnd);
}

text_input_manager::text_input_manager()
    : m_old(*this)
{
    ::ImmDisableIME(-1);

    if (auto const hr = ::CoCreateInstance(
              CLSID_TF_ThreadMgr
            , nullptr
            , CLSCTX_INPROC_SERVER
            , IID_PPV_ARGS(&m_thread_manager_ex));
        FAILED(hr))
    {
        ASR_ABORT_UNREACHABLE();
    }

    ////////////////////////////////////////////////////////////////////////////

    if (auto const hr = m_thread_manager_ex->QueryInterface(&m_source);
        FAILED(hr))
    {
        ASR_ABORT_UNREACHABLE();
    }

    auto const unknown = get_unknown();

    install_sink<ITfDisplayAttributeNotifySink>();
    install_sink<ITfActiveLanguageProfileNotifySink>();
    install_sink<ITfThreadFocusSink>();
    install_sink<ITfPreservedKeyNotifySink>();
    install_sink<ITfThreadMgrEventSink>();
    install_sink<ITfKeyTraceEventSink>();
    install_sink<ITfUIElementSink>();
    install_sink<ITfInputProcessorProfileActivationSink>();

    if (auto const hr = m_thread_manager_ex->ActivateEx(&m_client_id, TF_TMAE_UIELEMENTENABLEDONLY);
        FAILED(hr))
    {
        ASR_ABORT_UNREACHABLE();
    }

    if (auto const hr = m_thread_manager_ex->CreateDocumentMgr(&m_document_manager);
        FAILED(hr))
    {
    }


    if (auto const hr = m_document_manager->CreateContext(m_client_id, 0, unknown, &m_context, &m_edit_cookie);
        FAILED(hr))
    {
    }

    //if (auto const hr = m_document_manager->Push(m_context);
    //    FAILED(hr))
    //{
    //}
}

void text_input_manager::set_window(HWND const window) noexcept
{
    std::lock_guard const lock {m_mutex};

    m_window = window;
    m_thread_manager_ex->AssociateFocus(m_window, m_document_manager, &m_previous_document_manager);
}

void text_input_manager::set_focus(bool const state) noexcept
{
    if (m_advise_sink)
    {
        m_advise_sink->OnSelectionChange();
    }

    //if (state)
    //{
    //    if (auto const hr = m_thread_manager_ex->GetFocus(&m_previous_document_manager);
    //        FAILED(hr))
    //    {
    //    }

    //    if (auto const hr = m_thread_manager_ex->SetFocus(m_document_manager);
    //        FAILED(hr))
    //    {
    //    }

    //    return;
    //}

    //if (auto const hr = m_thread_manager_ex->SetFocus(m_previous_document_manager);
    //    FAILED(hr))
    //{
    //}

    //m_previous_document_manager = nullptr;
}

int text_input_manager::pump()
{
    MSG msg {};

    for (;;)
    {
        BOOL result = FALSE;
        auto const hr = m_message_pump->GetMessageW(&msg, nullptr, 0, 0, &result);

        if (FAILED(hr))
        {
            continue;
        }

        if (msg.message == WM_KEYDOWN)
        {
            BOOL eaten = FALSE;

            if (m_key_stroke_manager->TestKeyDown(msg.wParam, msg.lParam, &eaten) == S_OK && eaten
                && m_key_stroke_manager->KeyDown(msg.wParam, msg.lParam, &eaten) && eaten)
            {
                continue;
            }
        }
        else if (msg.message == WM_KEYUP)
        {
            BOOL eaten = FALSE;

            if (m_key_stroke_manager->TestKeyUp(msg.wParam, msg.lParam, &eaten) == S_OK && eaten
                && m_key_stroke_manager->KeyUp(msg.wParam, msg.lParam, &eaten) && eaten)
            {
                continue;
            }
        }

        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);

        if (msg.message == WM_QUIT)
        {
            return static_cast<int>(msg.wParam);
        }
    }
}

#pragma region IUnknown
// IUnknown

HRESULT __stdcall text_input_manager::QueryInterface(
      GUID   const& riid
    , void** const  ppvObject) noexcept
{
    if (!ppvObject)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    auto const match =
           qi<IUnknown, ITextStoreACP2>               (riid, ppvObject)
        || qi<ITextStoreACP>                          (riid, ppvObject, &m_old)
        || qi<ITextStoreACP2>                         (riid, ppvObject)
        || qi<ITfContextOwnerCompositionSink>         (riid, ppvObject)
        || qi<ITfDisplayAttributeNotifySink>          (riid, ppvObject)
        || qi<ITfActiveLanguageProfileNotifySink>     (riid, ppvObject)
        || qi<ITfThreadFocusSink>                     (riid, ppvObject)
        || qi<ITfPreservedKeyNotifySink>              (riid, ppvObject)
        || qi<ITfThreadMgrEventSink>                  (riid, ppvObject)
        || qi<ITfKeyTraceEventSink>                   (riid, ppvObject)
        || qi<ITfUIElementSink>                       (riid, ppvObject)
        || qi<ITfInputProcessorProfileActivationSink> (riid, ppvObject);

    if (!match)
    {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG __stdcall text_input_manager::AddRef() noexcept
{
    return ++m_ref_count;
}

ULONG __stdcall text_input_manager::Release() noexcept
{
    auto const n = --m_ref_count;
    if (n)
    {
        return n;
    }

    delete this;

    return 0;
}

#pragma endregion IUnknown

#pragma region ITextStoreACP2
// ITextStoreACP2

HRESULT __stdcall text_input_manager::AdviseSink(GUID const& riid, IUnknown* const punk, DWORD const dwMask) noexcept
{
    if (!punk)
    {
        return E_INVALIDARG;
    }

    std::scoped_lock const lock {m_mutex};

    if (m_advise_sink_id)
    {
        if (m_advise_sink_id != punk)
        {
            return CONNECT_E_ADVISELIMIT;
        }

        m_advise_sink_mask = dwMask;
    }

    if (auto const hr = punk->QueryInterface<ITextStoreACPSink>(&m_advise_sink);
        FAILED(hr))
    {
        return E_UNEXPECTED;
    }

    m_advise_sink_mask = dwMask;

    return S_OK;
}

HRESULT __stdcall text_input_manager::UnadviseSink(IUnknown* punk) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::RequestLock(DWORD const dwLockFlags, HRESULT* const phrSession) noexcept
{
    if (!phrSession)
    {
        return E_INVALIDARG;
    }

    //if (!(dwLockFlags & TS_LF_SYNC))
    {
        *phrSession = m_advise_sink->OnLockGranted(dwLockFlags);
        return S_OK;
    }

    //return E_FAIL;
}

HRESULT __stdcall text_input_manager::GetStatus(TS_STATUS* const pdcs) noexcept
{
    if (!pdcs)
    {
        return E_INVALIDARG;
    }

    pdcs->dwDynamicFlags = 0;
    pdcs->dwStaticFlags  = TS_SS_NOHIDDENTEXT;

    return S_OK;
}

HRESULT __stdcall text_input_manager::QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG* pacpResultStart, LONG* pacpResultEnd) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP* pSelection, ULONG* pcFetched) noexcept
{
    if (!pcFetched)
    {
        return E_INVALIDARG;
    }

    if (!ulCount)
    {
        *pcFetched = 0;
        return S_OK;
    }

    if (ulIndex == TF_DEFAULT_SELECTION)
    {
        *pcFetched = 1;
        *pSelection = TS_SELECTION_ACP {};
        pSelection->style.ase = TS_AE_END;

    }

    return S_OK;
}

HRESULT __stdcall text_input_manager::SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetText(LONG acpStart, LONG acpEnd, WCHAR* pchPlain, ULONG cchPlainReq, ULONG* pcchPlainRet, TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfoRet, LONG* pacpNext) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject** ppDataObject) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown** ppunk) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::QueryInsertEmbedded(const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::InsertTextAtSelection(DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept
{
    switch (dwFlags)
    {
    case 0 :
        break;
    case TF_IAS_NOQUERY :
        break;
    case TF_IAS_QUERYONLY :
        break;
    default:
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT __stdcall text_input_manager::InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL* paAttrVals, ULONG* pcFetched) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetEndACP(LONG* pacp) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetActiveView(TsViewCookie* const pvcView) noexcept
{
    if (!pvcView)
    {
        return E_INVALIDARG;
    }

    std::scoped_lock const lock {m_mutex};

    *pvcView = m_current_view;

    return S_OK;
}

HRESULT __stdcall text_input_manager::GetACPFromPoint(TsViewCookie vcView, const POINT* ptScreen, DWORD dwFlags, LONG* pacp) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetScreenExt(TsViewCookie vcView, RECT* prc) noexcept
{
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::GetWnd(TsViewCookie vcView, HWND* phwnd) noexcept
{
    if (!phwnd)
    {
        return E_INVALIDARG;
    }

    *phwnd = m_window;

    return S_OK;
}

#pragma endregion ITextStoreACP2

#pragma region ITfContextOwnerCompositionSink
//ITfContextOwnerCompositionSink

HRESULT __stdcall text_input_manager::OnStartComposition(
    ITfCompositionView *pComposition,
    BOOL *pfOk) noexcept {
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::OnUpdateComposition(
    ITfCompositionView *pComposition,
    ITfRange *pRangeNew) noexcept {
    return E_NOTIMPL;
}

HRESULT __stdcall text_input_manager::OnEndComposition(
    ITfCompositionView *pComposition) noexcept {
    return E_NOTIMPL;
}

#pragma endregion ITfContextOwnerCompositionSink

//ITfDisplayAttributeNotifySink

HRESULT __stdcall text_input_manager::OnUpdateInfo() noexcept
{
    return S_OK;
}

// ITfThreadFocusSink

HRESULT __stdcall text_input_manager::OnSetThreadFocus() noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::OnKillThreadFocus() noexcept
{
    return S_OK;
}


//ITfActiveLanguageProfileNotifySink

HRESULT __stdcall text_input_manager::OnActivated(
        REFCLSID clsid
    , REFGUID  guidProfile
    , BOOL     fActivated) noexcept
{
    return S_OK;
}

// ITfPreservedKeyNotifySink

HRESULT __stdcall text_input_manager::OnUpdated(
    const TF_PRESERVEDKEY *pprekey) noexcept
{
    return S_OK;
}


//ITfThreadMgrEventSink

HRESULT __stdcall text_input_manager::OnInitDocumentMgr(
    ITfDocumentMgr *pdim) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::OnUninitDocumentMgr(
    ITfDocumentMgr *pdim) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::OnSetFocus(
    ITfDocumentMgr *pdimFocus,
    ITfDocumentMgr *pdimPrevFocus) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::OnPushContext(
    ITfContext *pic) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::OnPopContext(
    ITfContext *pic) noexcept
{
    return S_OK;
}

//ITfKeyTraceEventSink

HRESULT __stdcall text_input_manager::OnKeyTraceDown(
    WPARAM wParam,
    LPARAM lParam) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::OnKeyTraceUp(
    WPARAM wParam,
    LPARAM lParam) noexcept
{
    return S_OK;
}

#pragma region ITfUIElementSink
// ITfUIElementSink

HRESULT __stdcall text_input_manager::BeginUIElement(DWORD dwUIElementId, BOOL* pbShow) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::UpdateUIElement(DWORD dwUIElementId) noexcept
{
    return S_OK;
}

HRESULT __stdcall text_input_manager::EndUIElement(DWORD dwUIElementId) noexcept
{
    return S_OK;
}

#pragma endregion ITfUIElementSink

HRESULT __stdcall text_input_manager::OnActivated(
        DWORD dwProfileType
    , LANGID langid
    , REFCLSID clsid
    , REFGUID catid
    , REFGUID guidProfile
    , HKL hkl
    , DWORD dwFlags) noexcept
{
    return S_OK;
}

} // namespace asr::win

namespace asr
{

input_manager::~input_manager() = default;

std::unique_ptr<input_manager> make_input_manager()
{
    return std::make_unique<win::text_input_manager>();
}

} // namespace asr
