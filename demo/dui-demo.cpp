// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "DuiSystem.h" 

#ifdef _DEBUG
#include <vld.h>//使用Vitural Leaker Detector来检测内存泄漏，可以从http://vld.codeplex.com/ 下载
#endif

#include "MainDlg.h"


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	DUIASSERT(SUCCEEDED(hRes));

#ifdef _DEBUG
    HMODULE hImgDecoder = LoadLibrary(_T("imgdecoder-wic_d.dll"));
    HMODULE hRenderSkia = LoadLibrary(_T("render-skia_d.dll"));
#else
    HMODULE hImgDecoder = LoadLibrary(_T("imgdecoder-wic.dll"));
    HMODULE hRenderSkia = LoadLibrary(_T("render-skia.dll"));
#endif
    typedef BOOL (*fnCreateImgDecoderFactory)(SOUI::IImgDecoderFactory**);
    fnCreateImgDecoderFactory funImg = (fnCreateImgDecoderFactory)GetProcAddress(hImgDecoder,"CreateImgDecoderFactory");
    CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
    funImg(&pImgDecoderFactory);
    
    typedef BOOL (*fnCreateRenderFactory)(SOUI::IRenderFactory **,SOUI::IImgDecoderFactory *);
    fnCreateRenderFactory funRender = (fnCreateRenderFactory)GetProcAddress(hRenderSkia,"CreateRenderFactory");
    CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
    funRender(&pRenderFactory,pImgDecoderFactory);
    pImgDecoderFactory=NULL;
    
	DuiSystem *pDuiSystem=new DuiSystem(pRenderFactory,hInstance);

#if 1
    TCHAR szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
    GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
    LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
    *lpInsertPos = _T('\0');   
    _tcscat( szCurrentDir, _T("\\..\\demo\\skin") );

    DuiResProviderFiles *pResProvider=new DuiResProviderFiles;
    if(!pResProvider->Init(szCurrentDir))
    {
        DUIASSERT(0);
        return 1;
    }
#else
    DuiResProviderPE *pResProvider = new DuiResProviderPE(hInstance);
#endif
    
    pDuiSystem->AddResProvider(pResProvider);

	BOOL bOK=pDuiSystem->Init(_T("IDR_DUI_INIT")); //初始化DUI系统,原来的系统初始化方式依然可以使用。
	pDuiSystem->SetMsgBoxTemplate(_T("IDR_DUI_MSGBOX"));

	int nRet = 0; 
	// BLOCK: Run application
	{
		CMainDlg dlgMain;  
		nRet = dlgMain.DoModal();  
	}



	delete pDuiSystem;
    
    delete pResProvider;
    
    pRenderFactory=NULL;
    
	OleUninitialize();
	return nRet;
}
