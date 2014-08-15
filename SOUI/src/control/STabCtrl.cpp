#include "souistd.h"
#include "control/Stabctrl.h"

namespace SOUI
{
    class STabSlider : public SWindow
    {
        SOUI_CLASS_NAME(STabSlider, L"tabslider")

    public:
        STabSlider( STabCtrl *pTabCtrl =NULL)
        {
            if(pTabCtrl)
            {
                CRect rcPage=pTabCtrl->GetChildrenLayoutRect();
                pTabCtrl->InsertChild(this);
                Move(rcPage);
                SetVisible(FALSE);
                GETRENDERFACTORY->CreateRenderTarget(&m_memRT,rcPage.Width()*2,rcPage.Height());
            }
        }

        virtual ~STabSlider()
        {
            if(GetParent())
            {
                GetParent()->RemoveChild(this);
            }
        }

        void Slide(SLIDEDIR sd,int nSteps=20)
        {
            CRect rcPage=m_rcWindow;

            SetVisible(TRUE);

            BringWindowToTop();
            for(int i=0; i<nSteps; i++)
            {
                CAutoRefPtr<IRenderTarget> pRTPage=GetRenderTarget(NULL,OLEDC_OFFSCREEN);
                switch(sd)
                {
                case SD_LEFTRIGHT:
                    BitBlt(pRTPage,rcPage.left,rcPage.top,rcPage.Width()*(i+1)/nSteps,rcPage.Height(),m_memRT,rcPage.Width()+rcPage.Width()*(nSteps-i-1)/nSteps,0,SRCCOPY);
                    BitBlt(pRTPage,rcPage.left+rcPage.Width()*(i+1)/nSteps,rcPage.top,rcPage.Width()*(nSteps-i-1)/nSteps,rcPage.Height(),m_memRT,0,0,SRCCOPY);
                    break;
                case SD_RIGHTLEFT:
                    BitBlt(pRTPage,rcPage.left,rcPage.top,rcPage.Width(),rcPage.Height(),m_memRT,rcPage.Width()*(i+1)/nSteps,0,SRCCOPY);
                    break;
                case SD_TOPBOTTOM:
                    BitBlt(pRTPage,rcPage.left,rcPage.top+rcPage.Height()*(nSteps-i-1)/nSteps,rcPage.Width(),rcPage.Height()*(i+1)/nSteps,
                        m_memRT,rcPage.Width(),0,SRCCOPY);//new
                    BitBlt(pRTPage,rcPage.left,rcPage.top,rcPage.Width(),rcPage.Height()*(nSteps-i-1)/nSteps,
                        m_memRT,0,rcPage.Height()*(i+1)/nSteps,SRCCOPY);//old
                    break;
                case SD_BOTTOMTOP:
                    BitBlt(pRTPage,rcPage.left,rcPage.top,rcPage.Width(),rcPage.Height()*(i+1)/nSteps,
                        m_memRT,rcPage.Width(),rcPage.Height()*(nSteps-i-1)/nSteps,SRCCOPY);//new
                    BitBlt(pRTPage,rcPage.left,rcPage.top+rcPage.Height()*(i+1)/nSteps,rcPage.Width(),rcPage.Height()*(nSteps-i-1)/nSteps,
                        m_memRT,0,0,SRCCOPY);//old
                    break;
                }
                PaintForeground(pRTPage,&rcPage);
                ReleaseRenderTarget(pRTPage);
                Sleep(10);
            }

            SetVisible(FALSE);
        }

        void InitPage(BOOL bPage1)
        {
            CRect rcPage;
            GetWindowRect(&rcPage);
            CAutoRefPtr<IRenderTarget> pRTPage=GetRenderTarget(&rcPage,OLEDC_NODRAW);
            PaintBackground(pRTPage,&rcPage);
            BitBlt(m_memRT,bPage1?0:rcPage.Width(),0,rcPage.Width(),rcPage.Height(),pRTPage,rcPage.left,rcPage.top,SRCCOPY);
            ReleaseRenderTarget(pRTPage);
        }
    protected:

        void OnPaint(IRenderTarget *pRT)
        {
        }
        
        void BitBlt(IRenderTarget *pRTDest,int xDest,int yDest,int nWid,int nHei,IRenderTarget *pRTSrc,int xSrc,int ySrc,DWORD rop)
        {
            CRect rcDest(xDest,yDest,xDest+nWid,yDest+nHei);
            pRTDest->BitBlt(&rcDest,pRTSrc,xSrc,ySrc,rop);
        }
        
        CAutoRefPtr<IRenderTarget> m_memRT;
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
        SOUI_MSG_MAP_END()
    };


//////////////////////////////////////////////////////////////////////////
// STabCtrl

STabCtrl::STabCtrl() : m_nCurrentPage(0)
    , m_pSkinTab(GETBUILTINSKIN(SKIN_SYS_TAB_PAGE))
    , m_pSkinIcon(NULL)
    , m_pSkinTabInter(NULL)
    , m_pSkinFrame(NULL)
    , m_nTabInterSize(0)
    , m_nTabWidth(0)
    , m_nTabHeight(0)
    , m_nTabPos(0)
    , m_nFramePos(0)
    , m_nHoverTabItem(-1)
    , m_nTabAlign(AlignTop)
    , m_nAnimateSteps(0)
    , m_ptText(-1,-1)
{
    m_bFocusable=TRUE;
    m_evtSet.addEvent(EventTabSelChanging::EventID);
    m_evtSet.addEvent(EventTabSelChanged::EventID);
}

void STabCtrl::OnPaint(IRenderTarget *pRT)
{
    SPainter painter;
    BeforePaint(pRT,painter);
    
    CRect rcTabs;
    CRect rcItem,rcItemPrev;
    CRect rcSplit;
    DWORD dwState;
    
    GetClientRect(&rcTabs);
    if(m_nTabAlign==AlignLeft)
        rcTabs.right=rcTabs.left+m_nTabWidth;
    else
        rcTabs.bottom=rcTabs.top+m_nTabHeight;

    if (m_pSkinFrame)
    {
        CRect rcFrame;
        GetClientRect(rcFrame);
        pRT->PushClipRect(&rcFrame,RGN_AND);

        switch (m_nTabAlign)
        {
        case AlignTop:
            rcFrame.top += m_nTabHeight + m_nFramePos;
            break;
        case AlignLeft:
            rcFrame.left += m_nTabWidth + m_nFramePos;
            break;
        }
        m_pSkinFrame->Draw(pRT, rcFrame, WndState_Normal);
    }
    else
    {
        pRT->PushClipRect(&rcTabs,RGN_AND);
    }

    for(size_t i=0; i<GetItemCount(); i++)
    {
        dwState=WndState_Normal;
        if(i == m_nCurrentPage) dwState=WndState_PushDown;
        else if(i== m_nHoverTabItem) dwState=WndState_Hover;

        GetItemRect(i,rcItem);
        //���ָ���
        if(i>0 && m_pSkinTabInter)
        {
            rcSplit=rcItem;
            if(m_nTabAlign==AlignLeft)
            {
                rcSplit.top=rcItemPrev.bottom;
                rcSplit.bottom = rcSplit.top + m_nTabInterSize;
            }
            else
            {
                rcSplit.left=rcItemPrev.right;
                rcSplit.right=rcSplit.left + m_nTabInterSize;
            }
            m_pSkinTabInter->Draw(pRT,rcSplit,0);
        }

        DrawItem(pRT,rcItem,i,dwState);
        rcItemPrev=rcItem;
    }
    
    if(GetContainer()->SwndGetFocus()==m_hSWnd && IsFocusable())
    {
        CRect rc;
        GetItemRect(m_nCurrentPage,rc);
        rc.DeflateRect(2,2);
        DrawDefFocusRect(pRT,&rc);
    }
    
    pRT->PopClip();

    AfterPaint(pRT,painter);
}

CRect STabCtrl::GetChildrenLayoutRect()
{
    CRect rcRet=__super::GetChildrenLayoutRect();
    if(m_nTabAlign==AlignLeft)
        rcRet.left+= (m_nTabWidth+m_nFramePos);
    else
        rcRet.top+= (m_nTabHeight+m_nFramePos);
    return rcRet;
}

void STabCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
    CRect rcItem;
    BOOL bClickMove = TRUE;
    int nTabCount=GetItemCount();
    for (int i = 0; i < nTabCount; i ++)
    {
        GetItemRect(i, rcItem);

        if (rcItem.PtInRect(point))
        {
            bClickMove = FALSE;
            if (i == m_nCurrentPage)
                continue;

            SetCurSel(i);
            break;
        }
    }
    if (bClickMove)
    {
        __super::OnLButtonDown(nFlags,point);
    }else
    {
        SetFocus();
    }
}

BOOL STabCtrl::RemoveItem( int nIndex , int nSelPage/*=0*/)
{
    STabPage * pTab = GetItem(nIndex);

    DestroyChild(pTab);
    m_lstPages.RemoveAt(nIndex);

    if (m_nCurrentPage == nIndex)
    {
        if(nSelPage<0) nSelPage=0;
        if(nSelPage>=(int)GetItemCount()) nSelPage=GetItemCount()-1;
        m_nCurrentPage=-1;
        SetCurSel(nSelPage);
    }else
    {
        if(m_nCurrentPage>nIndex) m_nCurrentPage--;

        CRect rcTitle;
        GetClientRect(rcTitle);
        if(m_nTabAlign==AlignLeft)
            rcTitle.right = rcTitle.left + (m_nTabWidth+m_nFramePos);
        else
            rcTitle.bottom = rcTitle.top + (m_nTabHeight+m_nFramePos);
        
        InvalidateRect(rcTitle);
    }
    return TRUE;
}

void STabCtrl::RemoveAllItems( void )
{
    for (int i = GetItemCount()-1; i >= 0; i--)
    {
        STabPage * pTab = GetItem(i);
        DestroyChild(pTab);
        m_lstPages.RemoveAt(i);
    }
    Invalidate();
}

void STabCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
    CRect rcItem;
    int nOldHover=m_nHoverTabItem;
    m_nHoverTabItem=-1;
    int nTabCount=GetItemCount();
    for (int i = 0; i < nTabCount; i ++)
    {
        GetItemRect(i, rcItem);

        if (rcItem.PtInRect(point))
        {
            m_nHoverTabItem=i;
            break;
        }
    }
    if (m_nHoverTabItem != nOldHover)
    {
        if(nOldHover!=-1 && nOldHover!=m_nCurrentPage)
        {
            GetItemRect(nOldHover, rcItem);
            InvalidateRect(rcItem);
        }
        if(m_nHoverTabItem!=-1 && m_nHoverTabItem != m_nCurrentPage)
        {
            GetItemRect(m_nHoverTabItem, rcItem);
            InvalidateRect(rcItem);
        }
    }
}

void STabCtrl::OnDestroy()
{
    for(int i=GetItemCount()-1; i>=0; i--)
    {
        DestroyChild(m_lstPages[i]);
    }
    m_lstPages.RemoveAll();
}


BOOL STabCtrl::SetCurSel( int nIndex )
{
    if( nIndex < 0 || nIndex> (int)GetItemCount()-1 || (m_nCurrentPage == nIndex)) return FALSE;
    int nOldPage = m_nCurrentPage;
    
    EventTabSelChanging evt(this);
    evt.uOldSel=nOldPage;
    evt.uNewSel=nIndex;

    FireEvent(evt);

    if (evt.bCancel)
        return FALSE;

    STabPage *pTab=GetItem(nIndex);

    CRect rcItem;

    GetItemRect(m_nCurrentPage, rcItem);
    InvalidateRect(rcItem);
    GetItemRect(nIndex, rcItem);
    InvalidateRect(rcItem);

    STabSlider *pTabSlider=NULL;

    if(m_nAnimateSteps && IsVisible(TRUE) && nOldPage!=-1)
    {
        pTabSlider=new STabSlider(this);
        pTabSlider->InitPage(TRUE);
    }

    if(nOldPage!=-1)
    {
        pTab = GetItem(nOldPage);
        if( pTab) pTab->SSendMessage(WM_SHOWWINDOW,FALSE);
    }

    m_nCurrentPage = nIndex;
    if(nIndex!=-1)
    {
        pTab = GetItem(m_nCurrentPage);
        if( pTab) pTab->SSendMessage(WM_SHOWWINDOW,TRUE);
    }
    
    EventTabSelChanged evt2(this);
    evt2.uNewSel=nIndex;
    evt2.uOldSel=nOldPage;

    if(pTabSlider)
    {
        SLIDEDIR sd=SD_RIGHTLEFT;
        if(m_nTabAlign==AlignTop)
        {
            if(nOldPage<nIndex) sd=SD_RIGHTLEFT;
            else sd=SD_LEFTRIGHT;
        }
        else
        {
            if(nOldPage<nIndex) sd=SD_BOTTOMTOP;
            else sd=SD_TOPBOTTOM;
        }
        pTabSlider->InitPage(FALSE);

        pTabSlider->Slide(sd,m_nAnimateSteps);

        delete pTabSlider;
    }
    FireEvent(evt2);
    
    if(IsVisible(TRUE))
    {
        Invalidate();
    }

    return TRUE;
}

BOOL STabCtrl::SetCurSel( LPCTSTR pszTitle )
{
    for(UINT i=0;i<m_lstPages.GetCount();i++)
    {
        if(_tcscmp(m_lstPages[i]->GetTitle(),pszTitle)==0)
            return SetCurSel(i);
    }
    return FALSE;
}

BOOL STabCtrl::SetItemTitle( int nIndex, LPCTSTR lpszTitle )
{
    STabPage* pTab = GetItem(nIndex);
    if (pTab)
    {
        pTab->SetTitle(lpszTitle);

        CRect rcTabs;
        GetClientRect(&rcTabs);
        if(m_nTabAlign==AlignLeft)
            rcTabs.right=rcTabs.left+m_nTabWidth;
        else
            rcTabs.bottom=rcTabs.top+m_nTabHeight;
        InvalidateRect(rcTabs);
        return TRUE;
    }

    return FALSE;
}

BOOL STabCtrl::CreateChildren( pugi::xml_node xmlNode )
{
    for ( pugi::xml_node xmlChild = xmlNode.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
    {
        InsertItem(xmlChild,-1,TRUE);
    }
    
    if(m_nCurrentPage==-1 || m_nCurrentPage>=(int)m_lstPages.GetCount())
    {
        m_nCurrentPage=0;
    }
    if(m_lstPages.GetCount()==0)
    {
        m_nCurrentPage=-1;
    }
    
    if(m_nCurrentPage!=-1)
    {
        GetItem(m_nCurrentPage)->SSendMessage(WM_SHOWWINDOW,TRUE);
    }
    return TRUE;
}

BOOL STabCtrl::InsertItem( LPCWSTR lpContent ,int iInsert/*=-1*/)
{
    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_buffer(lpContent,wcslen(lpContent)*sizeof(wchar_t),pugi::parse_default,pugi::encoding_utf16)) return FALSE;

    pugi::xml_node xmlTab=xmlDoc.child(L"page");

    return InsertItem(xmlTab,iInsert)!=-1;
}

int STabCtrl::InsertItem( pugi::xml_node xmlNode,int iInsert/*=-1*/,BOOL bLoading/*=FALSE*/ )
{
    STabPage *pChild=NULL;
    if (!STabPage::CheckAndNew(xmlNode.name(),(void**)&pChild)) return -1;

    if(iInsert==-1) iInsert=m_lstPages.GetCount();
    InsertChild(pChild);

    m_lstPages.InsertAt(iInsert,pChild);

    pChild->InitFromXml(xmlNode);
    pChild->SetPositionType(SizeX_FitParent|SizeY_FitParent);

    if(!bLoading && m_nCurrentPage>=iInsert) m_nCurrentPage++;

    if(!bLoading)
    {
        CRect rcContainer=GetChildrenLayoutRect();
        pChild->SSendMessage(WM_WINDOWPOSCHANGED,0,(LPARAM)&rcContainer);
        Invalidate();
    }

    return iInsert;
}

BOOL STabCtrl::GetItemRect( int nIndex, CRect &rcItem )
{
    if (nIndex < 0 || nIndex >= (int)GetItemCount())
        return FALSE;

    SIZE size = {0, 0};

    if (m_pSkinTab)
        size = m_pSkinTab->GetSkinSize();

    if (0 != m_nTabHeight)
        size.cy = m_nTabHeight;
    if(0 != m_nTabWidth)
        size.cx=m_nTabWidth;

    rcItem.SetRect(m_rcWindow.left, m_rcWindow.top, m_rcWindow.left + size.cx, m_rcWindow.top + size.cy);

    switch (m_nTabAlign)
    {
    case AlignTop:
        rcItem.OffsetRect(m_nTabPos + nIndex * (rcItem.Width()+ m_nTabInterSize),0);
        break;
    case AlignLeft:
        rcItem.OffsetRect(0, m_nTabPos + nIndex * (rcItem.Height()+ m_nTabInterSize));
        break;
    }
    CRect rcClient;
    GetClientRect(&rcClient);
    rcItem.IntersectRect(rcItem,rcClient);
    return TRUE;
}

STabPage* STabCtrl::GetItem( int nIndex )
{
    if(nIndex<0 || nIndex>= (int)GetItemCount()) return NULL;
    return m_lstPages[nIndex];
}

void STabCtrl::DrawItem(IRenderTarget *pRT,const CRect &rcItem,int iItem,DWORD dwState )
{
    if(rcItem.IsRectEmpty()) return;
    if(m_pSkinTab)
        m_pSkinTab->Draw(pRT,rcItem,IIF_STATE3(dwState,WndState_Normal,WndState_Hover,WndState_PushDown),m_byAlpha);

    CRect rcIcon(m_ptIcon+rcItem.TopLeft(),CSize(0,0));
    if(m_pSkinIcon)
    {
        rcIcon.right=rcIcon.left+m_pSkinIcon->GetSkinSize().cx;
        rcIcon.bottom=rcIcon.top+m_pSkinIcon->GetSkinSize().cy;
        m_pSkinIcon->Draw(pRT,rcIcon,iItem);
    }

    if(m_ptText.x!=-1 && m_ptText.y!=-1)
    {//��ָ��λ�ÿ�ʼ��������
        pRT->TextOut(rcItem.left+m_ptText.x,rcItem.top+m_ptText.y,GetItem(iItem)->GetTitle(),-1,m_byAlpha);
    }
    else
    {
        CRect rcText=rcItem;
        UINT alignStyle=m_style.GetTextAlign();
        UINT align=alignStyle;
        if(m_ptText.x==-1 && m_ptText.y!=-1)
        {//ָ����Yƫ�ƣ�X����
            rcText.top+=m_ptText.y;
            align=alignStyle&(DT_CENTER|DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS);
        }
        else if(m_ptText.x!=-1 && m_ptText.y==-1)
        {//ָ����Xƫ�ƣ�Y����
            rcText.left+=m_ptText.x;
            align=alignStyle&(DT_VCENTER|DT_BOTTOM|DT_SINGLELINE|DT_END_ELLIPSIS);
        }
        
        pRT->DrawText(GetItem(iItem)->GetTitle(),-1,&rcText,align,m_byAlpha);
    }
}

void STabCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(nChar==VK_LEFT || nChar==VK_UP)
    {
        if(!SetCurSel(m_nCurrentPage-1))
            SetCurSel(GetItemCount()-1);
    }else if(nChar==VK_RIGHT || nChar==VK_DOWN)
    {
        if(!SetCurSel(m_nCurrentPage+1))
            SetCurSel(0);
    }else if(nChar==VK_HOME)
    {
        SetCurSel(0);
    }else if(nChar==VK_END)
    {
        SetCurSel(GetItemCount()-1);
    }
}

}//namespace SOUI