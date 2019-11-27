#pragma once

#include "win_common.h"

#include "platform/ime.h"

#include <combaseapi.h>
#include <cguid.h>
#include <msctf.h>
#include <comdef.h>

#include <atomic>
#include <array>
#include <mutex>

namespace asr::win
{

class text_input_manager;

class text_input_manager_old
    : public ITextStoreACP
{
public:
    explicit text_input_manager_old(text_input_manager& manager) noexcept
        : m_manager(manager)
    {
    }
public:
    // IUnknown

    HRESULT __stdcall QueryInterface(GUID const& riid, void** ppvObject) noexcept override;
    ULONG __stdcall AddRef() noexcept override;
    ULONG __stdcall Release() noexcept override;
public:
    // ITextStoreACP

    HRESULT __stdcall AdviseSink(REFIID riid, IUnknown* punk, DWORD dwMask) noexcept override;
    HRESULT __stdcall UnadviseSink(IUnknown* punk) noexcept override;
    HRESULT __stdcall RequestLock(DWORD dwLockFlags, HRESULT* phrSession) noexcept override;
    HRESULT __stdcall GetStatus(TS_STATUS* pdcs) noexcept override;
    HRESULT __stdcall QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG* pacpResultStart, LONG* pacpResultEnd) noexcept override;
    HRESULT __stdcall GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP* pSelection, ULONG* pcFetched) noexcept override;
    HRESULT __stdcall SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) noexcept override;
    HRESULT __stdcall GetText(LONG acpStart, LONG acpEnd, WCHAR* pchPlain, ULONG cchPlainReq, ULONG* pcchPlainRet, TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfoRet, LONG* pacpNext) noexcept override;
    HRESULT __stdcall SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange) noexcept override;
    HRESULT __stdcall GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject** ppDataObject) noexcept override;
    HRESULT __stdcall GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown** ppunk) noexcept override;
    HRESULT __stdcall QueryInsertEmbedded(const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable) noexcept override;
    HRESULT __stdcall InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange) noexcept override;
    HRESULT __stdcall InsertTextAtSelection(DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept override;
    HRESULT __stdcall InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept override;
    HRESULT __stdcall RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs) noexcept override;
    HRESULT __stdcall RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept override;
    HRESULT __stdcall RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept override;
    HRESULT __stdcall FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset) noexcept override;
    HRESULT __stdcall RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL* paAttrVals, ULONG* pcFetched) noexcept override;
    HRESULT __stdcall GetEndACP(LONG* pacp) noexcept override;
    HRESULT __stdcall GetActiveView(TsViewCookie* pvcView) noexcept override;
    HRESULT __stdcall GetACPFromPoint(TsViewCookie vcView, const POINT* ptScreen, DWORD dwFlags, LONG* pacp) noexcept override;
    HRESULT __stdcall GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) noexcept override;
    HRESULT __stdcall GetScreenExt(TsViewCookie vcView, RECT* prc) noexcept override;
    HRESULT __stdcall GetWnd(TsViewCookie vcView, HWND* phwnd) noexcept override;
private:
    text_input_manager& m_manager;
};

class text_input_manager
    : public input_manager
    , public ITextStoreACP2
    , public ITfContextOwnerCompositionSink
    , public ITfDisplayAttributeNotifySink
    , public ITfActiveLanguageProfileNotifySink
    , public ITfThreadFocusSink
    , public ITfPreservedKeyNotifySink
    , public ITfThreadMgrEventSink
    , public ITfKeyTraceEventSink
    , public ITfUIElementSink
    , public ITfInputProcessorProfileActivationSink
{
public:
    text_input_manager();

    void set_window(HWND window) noexcept;

    void set_focus(bool state) noexcept;

    int pump();
public:
    #pragma region IUnknown
    // IUnknown

    HRESULT __stdcall QueryInterface(
          GUID   const& riid
        , void**        ppvObject) noexcept override;

    ULONG __stdcall AddRef() noexcept override;

    ULONG __stdcall Release() noexcept override;

    #pragma endregion IUnknown
public:
    #pragma region ITextStoreACP2
    // ITextStoreACP2

    HRESULT __stdcall AdviseSink(GUID const& riid, IUnknown* punk, DWORD dwMask) noexcept override;

    HRESULT __stdcall UnadviseSink(IUnknown* punk) noexcept override;

    HRESULT __stdcall RequestLock(DWORD dwLockFlags, HRESULT* phrSession) noexcept override;

    HRESULT __stdcall GetStatus(TS_STATUS* pdcs) noexcept override;

    HRESULT __stdcall QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG* pacpResultStart, LONG* pacpResultEnd) noexcept override;

    HRESULT __stdcall GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP* pSelection, ULONG* pcFetched) noexcept override;

    HRESULT __stdcall SetSelection(ULONG ulCount, const TS_SELECTION_ACP* pSelection) noexcept override;

    HRESULT __stdcall GetText(LONG acpStart, LONG acpEnd, WCHAR* pchPlain, ULONG cchPlainReq, ULONG* pcchPlainRet, TS_RUNINFO* prgRunInfo, ULONG cRunInfoReq, ULONG* pcRunInfoRet, LONG* pacpNext) noexcept override;

    HRESULT __stdcall SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR* pchText, ULONG cch, TS_TEXTCHANGE* pChange) noexcept override;

    HRESULT __stdcall GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject** ppDataObject) noexcept override;

    HRESULT __stdcall GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown** ppunk) noexcept override;

    HRESULT __stdcall QueryInsertEmbedded(const GUID* pguidService, const FORMATETC* pFormatEtc, BOOL* pfInsertable) noexcept override;

    HRESULT __stdcall InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject* pDataObject, TS_TEXTCHANGE* pChange) noexcept override;

    HRESULT __stdcall InsertTextAtSelection(DWORD dwFlags, const WCHAR* pchText, ULONG cch, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept override;

    HRESULT __stdcall InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject* pDataObject, LONG* pacpStart, LONG* pacpEnd, TS_TEXTCHANGE* pChange) noexcept override;

    HRESULT __stdcall RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs) noexcept override;

    HRESULT __stdcall RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept override;

    HRESULT __stdcall RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags) noexcept override;

    HRESULT __stdcall FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, const TS_ATTRID* paFilterAttrs, DWORD dwFlags, LONG* pacpNext, BOOL* pfFound, LONG* plFoundOffset) noexcept override;

    HRESULT __stdcall RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL* paAttrVals, ULONG* pcFetched) noexcept override;

    HRESULT __stdcall GetEndACP(LONG* pacp) noexcept override;

    HRESULT __stdcall GetActiveView(TsViewCookie* const pvcView) noexcept override;

    HRESULT __stdcall GetACPFromPoint(TsViewCookie vcView, const POINT* ptScreen, DWORD dwFlags, LONG* pacp) noexcept override;

    HRESULT __stdcall GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT* prc, BOOL* pfClipped) noexcept override;

    HRESULT __stdcall GetScreenExt(TsViewCookie vcView, RECT* prc) noexcept override;

    HRESULT __stdcall GetWnd(TsViewCookie vcView, HWND* phwnd) noexcept;

    #pragma endregion ITextStoreACP2
public:
    #pragma region ITfContextOwnerCompositionSink
    //ITfContextOwnerCompositionSink

    HRESULT __stdcall OnStartComposition(
        ITfCompositionView *pComposition,
        BOOL *pfOk) noexcept override;

    HRESULT __stdcall OnUpdateComposition(
        ITfCompositionView *pComposition,
        ITfRange *pRangeNew) noexcept override;

    HRESULT __stdcall OnEndComposition(
        ITfCompositionView *pComposition) noexcept override;

    #pragma endregion ITfContextOwnerCompositionSink
public:
    //ITfDisplayAttributeNotifySink

    HRESULT __stdcall OnUpdateInfo() noexcept override;
public:
    // ITfThreadFocusSink

    HRESULT __stdcall OnSetThreadFocus() noexcept override;

    HRESULT __stdcall OnKillThreadFocus() noexcept override;

public:
    //ITfActiveLanguageProfileNotifySink

    HRESULT __stdcall OnActivated(
          REFCLSID clsid
        , REFGUID  guidProfile
        , BOOL     fActivated) noexcept override;
public:
    // ITfPreservedKeyNotifySink

    HRESULT __stdcall OnUpdated(
        const TF_PRESERVEDKEY *pprekey) noexcept override;

public:
    //ITfThreadMgrEventSink

    HRESULT __stdcall OnInitDocumentMgr(
        ITfDocumentMgr *pdim) noexcept override;

    HRESULT __stdcall OnUninitDocumentMgr(
        ITfDocumentMgr *pdim) noexcept override;

    HRESULT __stdcall OnSetFocus(
        ITfDocumentMgr *pdimFocus,
        ITfDocumentMgr *pdimPrevFocus) noexcept override;

    HRESULT __stdcall OnPushContext(
        ITfContext *pic) noexcept override;

    HRESULT __stdcall OnPopContext(
        ITfContext *pic) noexcept override;

public:
    //ITfKeyTraceEventSink

    HRESULT __stdcall OnKeyTraceDown(
        WPARAM wParam,
        LPARAM lParam) noexcept override;

    HRESULT __stdcall OnKeyTraceUp(
        WPARAM wParam,
        LPARAM lParam) noexcept override;
public:
    #pragma region ITfUIElementSink
    // ITfUIElementSink

    HRESULT __stdcall BeginUIElement(DWORD dwUIElementId, BOOL* pbShow) noexcept override;

    HRESULT __stdcall UpdateUIElement(DWORD dwUIElementId) noexcept override;

    HRESULT __stdcall EndUIElement(DWORD dwUIElementId) noexcept override;

    #pragma endregion ITfUIElementSink
public:

public:
    HRESULT __stdcall OnActivated(
          DWORD dwProfileType
        , LANGID langid
        , REFCLSID clsid
        , REFGUID catid
        , REFGUID guidProfile
        , HKL hkl
        , DWORD dwFlags) noexcept override;
private:
    IUnknown* get_unknown() noexcept
    {
        return static_cast<ITextStoreACP2*>(this);
    }

    template <typename T>
    DWORD& get_sink_cookie() noexcept
    {
        size_t const i =
              std::is_same_v<T, ITfContextOwnerCompositionSink>         ? 1
            : std::is_same_v<T, ITfDisplayAttributeNotifySink>          ? 2
            : std::is_same_v<T, ITfActiveLanguageProfileNotifySink>     ? 3
            : std::is_same_v<T, ITfThreadFocusSink>                     ? 4
            : std::is_same_v<T, ITfPreservedKeyNotifySink>              ? 5
            : std::is_same_v<T, ITfThreadMgrEventSink>                  ? 6
            : std::is_same_v<T, ITfKeyTraceEventSink>                   ? 7
            : std::is_same_v<T, ITfUIElementSink>                       ? 8
            : std::is_same_v<T, ITfInputProcessorProfileActivationSink> ? 9
            : 0;

        return sink_cookies[i];
    }

    template <typename T, typename U = T, typename From>
    bool qi(GUID const& riid, void** const out, From* const from) noexcept
    {
        if (riid != __uuidof(T))
        {
            return false;
        }

        *out = static_cast<U*>(from);
        return true;
    }

    template <typename T, typename U = T>
    bool qi(GUID const& riid, void** const out) noexcept
    {
        return qi<T, U>(riid, out, this);
    }

    template <typename T>
    HRESULT install_sink() noexcept
    {
        return m_source->AdviseSink(
              __uuidof(T)
            , get_unknown()
            , &get_sink_cookie<T>());
    }
private:
    std::atomic<ULONG>     m_ref_count = 0;
    text_input_manager_old m_old;

    mutable std::mutex m_mutex;

    ITfThreadMgr*   m_thread_manager    = nullptr;
    ITfThreadMgr2*  m_thread_manager2   = nullptr;
    ITfThreadMgrEx* m_thread_manager_ex = nullptr;

    ITfSource* m_source = nullptr;

    std::array<DWORD, 10> sink_cookies {};

    TfClientId m_client_id {};

    ITfUIElementMgr* m_ui_element_manager = nullptr;

    ITfKeystrokeMgr* m_key_stroke_manager = nullptr;
    ITfMessagePump*  m_message_pump       = nullptr;

    ITfDocumentMgr* m_document_manager = nullptr;
    ITfContext*     m_context          = nullptr;
    TfEditCookie    m_edit_cookie {};

    ITfDocumentMgr* m_previous_document_manager = nullptr;

    DWORD              m_advise_sink_mask = 0;
    IUnknown const*    m_advise_sink_id   = nullptr;
    ITextStoreACPSink* m_advise_sink      = nullptr;

    TsViewCookie       m_current_view = 1;

    HWND m_window {};
};

} // namespace asr::win
