#include "StdAfx.h"
#include "ImageMerger.h"
#include <controls.extend/FileHelper.h>

#include <atl.mini/SComHelper.h>

class CDropTarget:public SUnknownImpl<IDropTarget>
{
public:
    CDropTarget()
    {
    }

    virtual ~CDropTarget(){}

    COM_INTERFACE_BEGIN()
        COM_INTERFACE(IDropTarget)
    COM_INTERFACE_END()
    
    //////////////////////////////////////////////////////////////////////////
    // IDropTarget

    virtual HRESULT STDMETHODCALLTYPE DragEnter( 
        /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect)
    {
        *pdwEffect=DROPEFFECT_LINK;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect)
    {
        *pdwEffect=DROPEFFECT_LINK;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragLeave( void)
    {
        return S_OK;
    }
};

class CDropTarget_Canvas : public CDropTarget
{
protected:
    CImageMerger *m_pImgMerge;
public:
    CDropTarget_Canvas(CImageMerger *pImgMerge):m_pImgMerge(pImgMerge)
    {
    }
    ~CDropTarget_Canvas()
    {
    }
public:
    virtual HRESULT STDMETHODCALLTYPE Drop( 
        /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect)
    {
        FORMATETC format =
        {
            CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL
        };
        STGMEDIUM medium;
        if(FAILED(pDataObj->GetData(&format, &medium)))
        {
            return S_FALSE;
        }

        HDROP hdrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));

        if(!hdrop)
        {
            return S_FALSE;
        }

        bool success = false;
        TCHAR filename[MAX_PATH];
        UINT files = DragQueryFile(hdrop,-1,NULL,0);
        for(UINT i=0;i<files;i++)
        {
            success=!!DragQueryFile(hdrop, i, filename, MAX_PATH);
            if(success)
            {
                m_pImgMerge->AddFile(filename);
            }
        }
        DragFinish(hdrop);
        GlobalUnlock(medium.hGlobal);


        *pdwEffect=DROPEFFECT_LINK;
        return S_OK;
    }
};

CImageMerger::CImageMerger(void):m_pPageRoot(NULL),m_pImgCanvas(NULL)
{
}

CImageMerger::~CImageMerger(void)
{
}

void CImageMerger::OnInit(SWindow *pRoot)
{
    m_pPageRoot = pRoot->FindChildByName(L"page_imagemerge");
    m_pImgCanvas = m_pPageRoot->FindChildByName2<SImgCanvas>(L"wnd_canvas");//CDropTarget_Canvas
    IDropTarget *pDT = new CDropTarget_Canvas(this);
    pRoot->GetContainer()->RegisterDragDrop(m_pImgCanvas->GetSwnd(),pDT);
    pDT->Release();
}

void CImageMerger::AddFile(LPCWSTR pszFileName)
{
    m_pImgCanvas->AddFile(pszFileName);
}

void CImageMerger::OnSave()
{
    CFileDialogEx dlgSave(FALSE,_T("png"),0,6,_T("png files(*.png)\0*.png\0All files (*.*)\0*.*\0\0"));
    if(dlgSave.DoModal()==IDOK)
    {
        m_pImgCanvas->Save2File(dlgSave.m_szFileName);
    }
}

void CImageMerger::OnClear()
{
    m_pImgCanvas->Clear();
}

void CImageMerger::OnModeHorz()
{
    if(!m_pImgCanvas) return;
    m_pImgCanvas->SetVertical(FALSE);
}

void CImageMerger::OnModeVert()
{
    if(!m_pImgCanvas) return;
    m_pImgCanvas->SetVertical(TRUE);
}
