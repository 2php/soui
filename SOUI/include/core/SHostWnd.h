//////////////////////////////////////////////////////////////////////////
//  Class Name: CDuiHostWnd
//    Description: Real Container of DuiWindow
//     Creator: ZhangXiaoxuan
//     Version: 2010.3.12 - 1.0 - Create
//                2011.6.18    1.1   huangjianxiong
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "SThreadActiveWndMgr.h"

#include "SwndContainerImpl.h"
#include "control/Srealwnd.h"

#include "SimpleWnd.h"
#include "SDropTargetDispatcher.h"
#include "event/eventcrack.h"

#pragma warning(disable: 4996)

#define INITCODE_NOTSHOW    100

namespace SOUI
{

    class SHostWnd;
    class SDummyWnd : public CSimpleWnd
    {
    public:
        SDummyWnd(SHostWnd* pOwner):m_pOwner(pOwner)
        {
        }

        void OnPaint(CDCHandle dc);

        BEGIN_MSG_MAP_EX(SDummyWnd)
            MSG_WM_PAINT(OnPaint)
        END_MSG_MAP()
    private:
        SHostWnd *m_pOwner;
    };

    class SHostWndAttr : public SObject
    {
        SOUI_CLASS_NAME(SWindow, L"hostwndattr")
    public:
        SHostWndAttr(void)
            : m_bResizable(FALSE)
            , m_bTranslucent(FALSE)
            , m_bAppWnd(FALSE)
            , m_bToolWnd(FALSE)
            , m_szMin(200, 200)
            , m_szInit(640,480)
            , m_dwStyle(0)
            , m_dwExStyle(0)
        {

        }

        void FreeOwnedSkins()
        {
            if(!m_strName.IsEmpty()) 
                SApplication::getSingleton().FreeSkins(m_strName);    //load skin only used in the host window
        }

        void LoadOwnedSkins()
        {
            if(!m_strName.IsEmpty()) 
                SApplication::getSingleton().LoadSkins(m_strName);    //load skin only used in the host window
        }

        SOUI_ATTRS_BEGIN()
            ATTR_STRINGW(L"name",m_strName,FALSE)
            ATTR_STRINGW(L"title",m_strTitle,FALSE)
            ATTR_SIZE(L"size",m_szInit,FALSE)
            ATTR_INT(L"width",m_szInit.cx,FALSE)
            ATTR_INT(L"height",m_szInit.cy,FALSE)
            ATTR_RECT(L"margin",m_rcMargin,FALSE)
            ATTR_SIZE(L"minsize",m_szMin,FALSE)
            ATTR_DWORD(L"wndstyle",m_dwStyle,FALSE)
            ATTR_DWORD(L"wndstyleex",m_dwExStyle,FALSE)
            ATTR_INT(L"resizable",m_bResizable,FALSE)
            ATTR_INT(L"translucent",m_bTranslucent,FALSE)
            ATTR_INT(L"appwnd",m_bAppWnd,FALSE)
            ATTR_INT(L"toolwindow",m_bToolWnd,FALSE)
        SOUI_ATTRS_END()

        CRect m_rcMargin;
        CSize m_szMin;
        CSize m_szInit;

        BOOL m_bResizable;
        BOOL m_bAppWnd;
        BOOL m_bToolWnd;
        BOOL m_bTranslucent;    //���ڵİ�͸������

        DWORD m_dwStyle;
        DWORD m_dwExStyle;

        SStringW m_strName;
        SStringW m_strTitle;
    };

class STipCtrl;

class SOUI_EXP SHostWnd
    : public CSimpleWnd
    , public SwndContainerImpl
    , public SWindow
    , protected IDuiRealWndHandler
{
    friend class SDummyWnd;
public:
    SHostWnd(LPCTSTR pszResName = NULL);
    virtual ~SHostWnd();

public:
    SWindow * GetRoot(){return this;}
    CSimpleWnd * GetNative(){return this;}

    HWND Create(HWND hWndParent,int x,int y,int nWidth,int nHeight);
    HWND Create(HWND hWndParent,DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight);

    BOOL SetXml(LPCTSTR pszXmlName);

    BOOL SetXml(LPCWSTR lpszXml,int nLen);

    BOOL AnimateHostWindow(DWORD dwTime,DWORD dwFlags);
protected:
    BOOL InitFromXml(pugi::xml_node xmlNode);
    void _Redraw();
    
    SDummyWnd            m_dummyWnd;    //��͸������ʹ�õ�һ����ӦWM_PAINT��Ϣ�Ĵ���
    SHostWndAttr         m_hostAttr;

    SStringT m_strXmlLayout;

    // Tracking flag
    BOOL m_bTrackFlag;


    BOOL m_bCaretShowing;    //��ǰ�в����������ʾ
    CAutoRefPtr<IBitmap>    m_bmpCaret; //��͸�������е�ģ������
    SIZE                    m_szCaret;  //�������С
    BOOL m_bCaretActive;    //ģ������������ʾ��־
    CPoint m_ptCaret;        //�����λ��
    CRect    m_rcValidateCaret;//������ʾ�������λ��

    BOOL m_bNeedRepaint;
    BOOL m_bNeedAllRepaint;

    STipCtrl    * m_pTipCtrl;

    CAutoRefPtr<IRegion>    m_rgnInvalidate;
    CAutoRefPtr<IRenderTarget> m_memRT;
    
    
protected:
    //////////////////////////////////////////////////////////////////////////
    // Message handler

    void OnPrint(CDCHandle dc, UINT uFlags);

    void OnPaint(CDCHandle dc);

    BOOL OnEraseBkgnd(CDCHandle dc);

    int OnCreate(LPCREATESTRUCT lpCreateStruct);

    void OnDestroy();

    void OnSize(UINT nType, CSize size);

    void OnMouseMove(UINT nFlags, CPoint point);

    void OnMouseLeave();

    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);

    BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);

    void OnTimer(UINT_PTR idEvent);

    void OnSwndTimer(char cTimerID);

    void DrawCaret(CPoint pt);

    LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    void OnActivate(UINT nState, BOOL bMinimized, HWND wndOther);

    //////////////////////////////////////////////////////////////////////////
    // CDuiContainer

    /*virtual */
    BOOL OnFireEvent(EventArgs &evt);

    /*virtual */
    CRect GetContainerRect();

    /*virtual */
    HWND GetHostHwnd();

    /*virtual */
    IRenderTarget * OnGetRenderTarget(const CRect & rc,DWORD gdcFlags);

    /*virtual */
    void OnReleaseRenderTarget(IRenderTarget * pRT,const CRect &rc,DWORD gdcFlags);

    /*virtual */
    void OnRedraw(const CRect &rc);

    /*virtual */
    BOOL OnReleaseSwndCapture();

    /*virtual */
    SWND OnSetSwndCapture(SWND hDuiWnd);

    /*virtual */
    BOOL IsTranslucent();

    /*virtual */
    BOOL SwndCreateCaret(HBITMAP hBmp,int nWidth,int nHeight);

    /*virtual */
    BOOL SwndShowCaret(BOOL bShow);

    /*virtual */
    BOOL SwndSetCaretPos(int x,int y);

    /*virtual */
    BOOL SwndUpdateWindow();

    /*virtual */
    BOOL RegisterTimelineHandler(ITimelineHandler *pHandler);

    /*virtual */
    BOOL UnregisterTimelineHandler(ITimelineHandler *pHandler);
    
    //////////////////////////////////////////////////////////////////////////
    // IDuiRealWndHandler
    virtual HWND OnRealWndCreate(SRealWnd *pRealWnd);
    virtual BOOL OnRealWndInit(SRealWnd *pRealWnd);
    virtual void OnRealWndDestroy(SRealWnd *pRealWnd);
    virtual void OnRealWndSize(SRealWnd *pRealWnd);
 
    LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);

    void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);

    BOOL OnNcActivate(BOOL bActive);
    
    UINT OnWndNcHitTest(CPoint point);

    void OnSetFocus(HWND wndOld);
    void OnKillFocus(HWND wndFocus);

    void OnSetCaretValidateRect( LPCRECT lpRect );

    void UpdateHost(CDCHandle dc,const CRect &rc);
    void UpdateLayerFromRenderTarget(IRenderTarget *pRT,BYTE byAlpha);
protected:
    virtual BOOL _HandleEvent(SOUI::EventArgs *pEvt){return FALSE;}

    BEGIN_MSG_MAP_EX(SHostWnd)
        MSG_WM_SIZE(OnSize)
        MSG_WM_PRINT(OnPrint)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_ACTIVATE(OnActivate)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
        MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST, WM_KEYLAST, OnKeyEvent)
        MESSAGE_RANGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,WM_IME_KEYLAST,OnKeyEvent)
        MESSAGE_HANDLER_EX(WM_IME_CHAR, OnKeyEvent)
        MSG_WM_SETCURSOR(OnSetCursor)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_NCACTIVATE(OnNcActivate)
        MSG_WM_NCCALCSIZE(OnNcCalcSize)
        MSG_WM_NCHITTEST(OnWndNcHitTest)
        MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
        REFLECT_NOTIFY_CODE(NM_CUSTOMDRAW)
    END_MSG_MAP()
};

}//namespace SOUI