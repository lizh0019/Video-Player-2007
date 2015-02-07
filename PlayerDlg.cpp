// PlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
#define ONE_SECOND 10000000 
#define IDT_TIMER1 WM_APP+1  
static	long rtNow1=0;



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerDlg dialog

CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlayerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pGraph = NULL;
    pControl = NULL;
    pEvent = NULL;
    pBA   = NULL;
	vol=-10000L;
	g_pWc = NULL;
    pVW   = NULL;
	pStep=NULL;
	pseek=NULL;
	nTimerID=0;

}


HRESULT CPlayerDlg::InitWindowlessVMR( 
										   HWND hwndApp,                  // Window to hold the video. 
										   IGraphBuilder* pGraph,         // Pointer to the Filter Graph Manager. 
										   IVMRWindowlessControl** ppWc  // Receives a pointer to the VMR.
										   ) 
{ 
	if (!pGraph || !ppWc) return E_POINTER;
    IBaseFilter* pVmr = NULL; 
    IVMRWindowlessControl* pWc = NULL; 
    // Create the VMR. 
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, 
        CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr); 
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Add the VMR to the filter graph.
    hr = pGraph->AddFilter(pVmr, L"qqVideo Mixing Renderer"); 
    if (FAILED(hr)) 
    {
        pVmr->Release();
        return hr;
    }
    // Set the rendering mode.  
    IVMRFilterConfig* pConfig; 
    hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
    if (SUCCEEDED(hr)) 
    { 
        hr = pConfig->SetRenderingMode(VMRMode_Windowless); 
        pConfig->Release(); 
    }
    if (SUCCEEDED(hr))
    {
        // Set the window. 
        hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);
        if( SUCCEEDED(hr)) 
        { 
            hr = pWc->SetVideoClippingWindow(hwndApp); 
            if (SUCCEEDED(hr))
            {
				*ppWc = pWc; // Return this as an AddRef'd pointer. 
            }
            else
            {
                // An error occurred, so release the interface.
                pWc->Release();
            }
        } 
    } 
 	

    pVmr->Release(); 
    return hr; 
} 

void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlayerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CPlayerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnOpen)
	ON_BN_CLICKED(IDC_BUTTON4, OnClose)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUTTON5, OnContinue)
	ON_BN_CLICKED(IDC_BUTTON6, OnPause)
	ON_BN_CLICKED(IDC_BUTTON7, OnVOLP)
	ON_BN_CLICKED(IDC_BUTTON8, OnVOLM)
	ON_BN_CLICKED(IDC_BUTTON3, OnFull)
	ON_WM_SIZE()
    ON_MESSAGE(WM_GRAPHNOTIFY,HandleGraphEvent)
	ON_BN_CLICKED(IDC_BUTTON1, OnEmDev)
	ON_BN_CLICKED(IDC_BUTTON9, OnGrab)
	ON_BN_CLICKED(IDC_BUTTON10, OnEmuGraphyFilter)
	ON_BN_CLICKED(IDC_BUTTON11, OnSeek)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerDlg message handlers

BOOL CPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// Initialize the trackbar range, but disable the 
    // control until the user opens a file.
    hScroll = (CSliderCtrl *)GetDlgItem(IDC_SLIDER1);


    hScroll->EnableWindow(FALSE);
    hScroll->SendMessage(TBM_SETRANGE, TRUE, MAKELONG(0, 100));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlayerDlg::OnPaint() 
{
	HDC         hdc; 
    RECT        rcClient; 
    GetClientRect(&rcClient); 
	CClientDC dc(this);
    if (g_pWc != NULL) 
    {   CPaintDC dc(this); 
	
	hdc=dc.GetSafeHdc();
	CRgn   rgn,rgn2,rgn3;   
	rgn.CreateRectRgnIndirect(&rcClient);   
	rgn2.CreateRectRgnIndirect(&rcDest);   
	rgn3.CreateRectRgn(0,0,1,1);   
	rgn3.CombineRgn(&rgn,&rgn2,RGN_DIFF); 
	HBRUSH hbr = GetSysColorBrush(15); 
	FillRgn(hdc,rgn3, hbr); 
	// Request the VMR to paint the video.
	HRESULT hr = g_pWc->RepaintVideo(m_hWnd, hdc);  
    } 
    else  // There is no video, so paint the whole client area. 
    {  
		CDialog::OnPaint();
    } 
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPlayerDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	
	
   

	if (g_pWc!=NULL)
	{
	pControl->Stop();
	pControl->Release();
    pEvent->Release();
    pGraph->Release();
	g_pWc->Release();
	pBA->Release();
	pVW->Release();
	g_pWc=NULL;
	pseek->Release();
	pStep->Release();
    CoUninitialize();

	CFileDialog dlg(TRUE);
	if (dlg.DoModal()==IDOK)
    {   p=dlg.GetPathName();
	OnPlay();
    }
	}
else
{CFileDialog dlg(TRUE);
if (dlg.DoModal()==IDOK)
{   p=dlg.GetPathName();
OnPlay();
}
	}


	
}

void CPlayerDlg::OnPlay() 
{
	HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return;
    }
	
    // Create the filter graph manager and query for interfaces.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
		IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
		printf("ERROR - Could not create the Filter Graph Manager.");
        return;
    }
	
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pEvent);
    hr=pGraph->QueryInterface(IID_IBasicAudio, (void **)&pBA);
	pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	pGraph->QueryInterface(IID_IMediaSeeking,(void**)&pseek);
    

	
	

	
	
	pEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0);
	


	pVW->put_Owner((OAHWND)m_hWnd);
	pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	
	
	hr = InitWindowlessVMR(m_hWnd, pGraph, &g_pWc);
    WCHAR  MediaFile[MAX_PATH];
	
	MultiByteToWideChar(CP_ACP, 0, p, -1, MediaFile, MAX_PATH);

	
	
	if (SUCCEEDED(hr))
	{
		// Build the graph. For example:
		pGraph->RenderFile(MediaFile, 0);
		// Release the VMR interface when you are done.
		
	}
	
	
	long lWidth, lHeight; 
	hr = g_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 
	if (SUCCEEDED(hr))
	{
		; 
		// Set the source rectangle.
		SetRect(&rcSrc, 0, 0, lWidth, lHeight); 
		
		// Get the window client area.
		GetClientRect(&rcDest); 
		// Set the destination rectangle.
		SetRect(&rcDest, 0, 0, 0.8*rcDest.right, 0.6*rcDest.bottom); 
		
		// Set the video position.
		hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest); 
	}

	hr=pGraph->QueryInterface(IID_IVideoFrameStep,(void**)&pStep);
	if (FAILED(hr))
	{
		MessageBox("error",NULL);
	}
	pGraph->QueryInterface(IID_IMediaSeeking,(void**)&pseek);
    bCanSeek = FALSE;
	DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration; 
	bCanSeek = (S_OK == pseek->CheckCapabilities(&caps));
	if (bCanSeek)
	{
		// Enable the trackbar.
		hScroll->EnableWindow(TRUE);
		
		// Find the file duration.
		pseek->GetDuration(&g_rtTotalTime);
	}
	
	
    // Build the graph. IMPORTANT: Change this string to a file on your system.
	
    if (SUCCEEDED(hr))
    {
        // Run the graph.
		pControl->Run();
		if (bCanSeek)
		{
			if (nTimerID != 0)
			{
			KillTimer(nTimerID);
			nTimerID = 0;
			}

		           // Make sure an old timer is not still active.
		    nTimerID = SetTimer(IDT_TIMER1, 1000,(TIMERPROC)NULL);
			if (nTimerID == 0)
			{
				/* Handle Error */
			}
		}

		pBA->put_Volume(-2000);
        if (SUCCEEDED(hr))
        {
            // Wait for completion.
           // long evCode;
			// pEvent->WaitForCompletion(INFINITE, &evCode);
			
            // Note: Do not use INFINITE in a real application, because it
            // can block indefinitely.
        }
    }		
}

void CPlayerDlg::OnClose() 
{
	if (g_pWc!=NULL)
	{
		pControl->Stop();
		pControl->Release();
		pEvent->Release();
		pGraph->Release();
		g_pWc->Release();
		pBA->Release();
		pVW->Release();
        pseek->Release();
		pStep->Release();
		g_pWc=NULL;
		CoUninitialize();
		OnOK();
	
	}
	else
		OnOK();
}

void CPlayerDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	HDC hdc;
	CClientDC dc(this);
	RECT        rcClient; 
    GetClientRect(&rcClient); 
	
    if (g_pWc != NULL) 
    { 
		
		hdc=dc.GetSafeHdc();
		CRgn   rgn,rgn2,rgn3;   
		rcClient.bottom=rcClient.bottom;
		rcClient.right=0.9*rcClient.right;
		rgn.CreateRectRgnIndirect(&rcClient);   
		rgn2.CreateRectRgnIndirect(&rcDest);   
		rgn3.CreateRectRgn(0,0,1,1);   
		rgn3.CombineRgn(&rgn,&rgn2,RGN_DIFF); 
		HBRUSH hbr = GetSysColorBrush(15); 
	FillRgn(hdc,rgn3, hbr); }
	
}

void CPlayerDlg::OnContinue() 
{
	// TODO: Add your control notification handler code here
	pControl->Run();
}

void CPlayerDlg::OnPause() 
{
pControl->Pause();	
}

void CPlayerDlg::OnVOLP() 
{   //Read current volume
// 	HRESULT hr;
//     hr = pBA->get_Volume(&g_lVolume);
//     if (hr == E_NOTIMPL)
//     {
//         // Fail quietly if this is a video-only media file
//     
//     }
//     else if (FAILED(hr))
//     {
//        
//      
//     }
// 	
//     // Switch volume levels
//     if (g_lVolume == 0l)
//         g_lVolume = -10000l;
//     else
//         g_lVolume = 0;
// 	
//     // Set new volume
	vol=vol/5;
	if(vol==0)
		vol=-1;
    pBA->put_Volume(vol);
	
    
  
	
}

void CPlayerDlg::OnVOLM() 
{
	// TODO: Add your control notification handler code here
	vol=vol*5;
	if(vol<-10000)
		vol=-10000;
    pBA->put_Volume(vol);
}

void CPlayerDlg::OnFull() 
{

	HRESULT hr=S_OK;
    LONG lMode;
    static HWND hDrain=0;
	
   
	
    // Read current state
    pVW->get_FullScreenMode(&lMode);
	
    if (lMode == OAFALSE)
    {
        // Save current message drain
       
       pVW->put_FullScreenMode(TRUE);
       
    }
    else
    {
       
       pVW->put_FullScreenMode(lMode);
		
       
        pVW->SetWindowForeground(-1);
		
      
        UpdateWindow();
      
    }
	
    
}

void CPlayerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void CPlayerDlg:: HandleGraphEvent()
{
	// Disregard if we don't have an IMediaEventEx pointer.
    if (pEvent == NULL)
    {
        return;
    }
    // Get all the events
    long evCode, param1, param2;
    HRESULT hr;
    while (SUCCEEDED(pEvent->GetEvent(&evCode, &param1, &param2, 0)))
    {
        pEvent->FreeEventParams(evCode, param1, param2);
        switch (evCode)
        {
		case EC_STEP_COMPLETE:
			{MessageBox("aa",NULL);
			break;}
        case EC_COMPLETE:  // Fall through.
        case EC_USERABORT: // Fall through.
        case EC_ERRORABORT:
			
			pControl->Stop();
			pControl->Release();
			pEvent->Release();
			pGraph->Release();
			g_pWc->Release();
			pBA->Release();
			pVW->Release();
			g_pWc=NULL;
			pStep->Release();
			pseek->Release();
            CoUninitialize();
	     	
           
            return;
        }
    } 

}

void CPlayerDlg::EmDEvice(REFCLSID clsid)
{	
HRESULT hr = CoInitialize(NULL);
ICreateDevEnum *pSysDevEnum = NULL;
 hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL, CLSCTX_INPROC_SERVER,
					  IID_ICreateDevEnum, (void **)&pSysDevEnum);

// Obtain a class enumerator for the video compressor category.
IEnumMoniker *pEnumCat = NULL;
hr = pSysDevEnum->CreateClassEnumerator(clsid,&pEnumCat, 0);

if (hr == S_OK) 
{
    // Enumerate the monikers.
    IMoniker *pMoniker = NULL;
    ULONG cFetched;
    while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
    {
        IPropertyBag *pPropBag;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
            (void **)&pPropBag);
        if (SUCCEEDED(hr))
        {
            // To retrieve the filter's friendly name, do the following:
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"FriendlyName", &varName,0);
            if (SUCCEEDED(hr))
            {   char szName[MAX_FILTER_NAME];
		     	int cch = WideCharToMultiByte(CP_ACP, 0,varName.bstrVal,
				MAX_FILTER_NAME, szName, MAX_FILTER_NAME, 0, 0);
				
				
			
				MessageBox(szName,NULL,MB_OK);
				LPOLESTR strName = NULL;
				
				hr = pMoniker->GetDisplayName(NULL, NULL, &strName);
				cch = WideCharToMultiByte(CP_ACP, 0,strName,
				MAX_FILTER_NAME, szName, MAX_FILTER_NAME, 0, 0);
				MessageBox(szName,NULL,MB_OK);



                // Display the name in your UI somehow.
            }
            VariantClear(&varName);
// 			
//            
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnumCat->Release();
}
pSysDevEnum->Release();
CoUninitialize();


}

void CPlayerDlg::OnEmDev() 
{
	// TODO: Add your control notification handler code here
	EmDEvice(CLSID_AudioInputDeviceCategory);

// 	    Audio Capture Sources CLSID_AudioInputDeviceCategory MERIT_DO_NOT_USE 
// 		Audio Compressors CLSID_AudioCompressorCategory MERIT_DO_NOT_USE 
// 		Audio Renderers CLSID_AudioRendererCategory MERIT_NORMAL 
// 		Device Control Filters CLSID_DeviceControlCategory MERIT_DO_NOT_USE 
// 		DirectShow Filters CLSID_LegacyAmFilterCategory MERIT_NORMAL 
// 		External Renderers CLSID_TransmitCategory MERIT_DO_NOT_USE 
// 		Midi Renderers CLSID_MidiRendererCategory MERIT_NORMAL 
// 		Video Capture Sources CLSID_VideoInputDeviceCategory MERIT_DO_NOT_USE 
// 		Video Compressors CLSID_VideoCompressorCategory MERIT_DO_NOT_USE 
// 		Video Effects (1 input) CLSID_VideoEffects1Category MERIT_DO_NOT_USE 
// 		Video Effects (2 inputs) CLSID_VideoEffects2Category MERIT_DO_NOT_USE 
// 		WDM Streaming Capture Devices AM_KSCATEGORY_CAPTURE MERIT_DO_NOT_USE 
// 		WDM Streaming Crossbar Devices AM_KSCATEGORY_CROSSBAR MERIT_DO_NOT_USE 
// 		WDM Streaming Rendering Devices AM_KSCATEGORY_RENDER MERIT_DO_NOT_USE 
// 		WDM Streaming Tee/Splitter Devices AM_KSCATEGORY_SPLITTER MERIT_DO_NOT_USE 
// 		WDM Streaming TV Audio Devices AM_KSCATEGORY_TVAUDIO MERIT_DO_NOT_USE 
// 		WDM Streaming TV Tuner Devices AM_KSCATEGORY_TVTUNER MERIT_DO_NOT_USE 
// 		WDM Streaming VBI Codecs AM_KSCATEGORY_VBICODEC MERIT_DO_NOT_USE 
// 		ActiveMovie Filter Categories CLSID_ActiveMovieCategories Not applicable 

}

void CPlayerDlg::OnGrab() 
{
	// TODO: Add your control notification handler code here
	BYTE *p=NULL;
//	pControl->Pause();
	g_pWc->GetCurrentImage(&p);
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER *bih2;
    bih2=new BITMAPINFOHEADER;
	bih2=(BITMAPINFOHEADER*)p;
	header.bfType=0x4d42;
	header.bfReserved1=header.bfReserved2=0;
	header.bfOffBits=sizeof(BITMAPINFOHEADER)+sizeof(BITMAPFILEHEADER);
	header.bfSize=bih2->biSizeImage+header.bfOffBits;
	CFile file;
    CString	bmp_path="D:\\temp\\1.bmp";
	file.Open(bmp_path,CFile::modeWrite|CFile::modeCreate);
	file.Write(&header,sizeof(BITMAPFILEHEADER));
	file.Write(p,(header.bfSize-sizeof(BITMAPFILEHEADER)));
	
	file.Close();
//	pControl->Run();
	
}

void CPlayerDlg::OnEmuGraphyFilter() 
{
	// TODO: Add your control notification handler code here
 
	
		IEnumFilters *pEnum = NULL;
		IBaseFilter *pFilter;
		ULONG cFetched;
		
		HRESULT hr = pGraph->EnumFilters(&pEnum);
		
		
		while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
		{
			FILTER_INFO FilterInfo;
			hr = pFilter->QueryFilterInfo(&FilterInfo);
			if (FAILED(hr))
			{
				MessageBox(NULL, TEXT("Could not get the filter info"),
					 MB_OK | MB_ICONERROR);
				continue;  // Maybe the next one will work.
			}
			
#ifdef UNICODE
			MessageBox(NULL, FilterInfo.achName, TEXT("Filter Name"), MB_OK);
#else
			char szName[MAX_FILTER_NAME];
			int cch = WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName,
				MAX_FILTER_NAME, szName, MAX_FILTER_NAME, 0, 0);
			if (cch > 0)
				MessageBox(szName,NULL,MB_OK);
#endif
			
			// The FILTER_INFO structure holds a pointer to the Filter Graph
			// Manager, with a reference count that must be released.
			if (FilterInfo.pGraph != NULL)
			{
				FilterInfo.pGraph->Release();
			}
			pFilter->Release();
		}
		
		pEnum->Release();
	
	
	

}

void CPlayerDlg::OnSeek() 
{


}

void CPlayerDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == IDT_TIMER1)
    {
        // Timer should not be running unless we really can seek.
        ASSERT(bCanSeek == TRUE);
		
        REFERENCE_TIME timeNow;
        if (SUCCEEDED(pseek->GetCurrentPosition(&timeNow)))
        {
            long sliderTick = (long)((timeNow * 100) / g_rtTotalTime);
			hScroll->SetPos(sliderTick);
	//	hScroll->SendMessage(TBM_SETPOS, TRUE, sliderTick );
        }	
    }

//Dialog::OnTimer(nIDEvent);
}

void CPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{static OAFilterState state;
static BOOL bStartOfScroll = TRUE;

	// TODO: Add your message handler code here and/or call default
	short int userReq = nSBCode;

//	if(userReq ==SB_THUMBPOSITION)
//  pScrollBar->SetScrollPos(nPos);

    if (userReq == TB_ENDTRACK || userReq == TB_THUMBTRACK)
    {
        DWORD dwPosition  = hScroll->SendMessage(TBM_GETPOS, 0, 0);
        // Pause when the scroll action begins.
        if (bStartOfScroll) 
        {
            pControl->GetState(10, &state);
            bStartOfScroll = FALSE;
            pControl->Pause();
        }
        // Update the position continuously.
        REFERENCE_TIME newTime = (g_rtTotalTime/100) * dwPosition;
        pseek->SetPositions(&newTime, AM_SEEKING_AbsolutePositioning,
            NULL, AM_SEEKING_NoPositioning);
		
        // Restore the state at the end.
        if (userReq == TB_ENDTRACK)
        {
            if (state == State_Stopped)
                pControl->Stop();
            else if (state == State_Running) 
                pControl->Run();
            bStartOfScroll = TRUE;
        }
    }

//Dialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
