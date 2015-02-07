// PlayerDlg.h : header file
//
#include "Dshow.h"
#if !defined(AFX_PLAYERDLG_H__CE668EF8_76C8_4839_99F0_F01A3FD0BC36__INCLUDED_)
#define AFX_PLAYERDLG_H__CE668EF8_76C8_4839_99F0_F01A3FD0BC36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPlayerDlg dialog
#define WM_GRAPHNOTIFY  WM_APP + 1
class CPlayerDlg : public CDialog
{
// Construction
public:
	BOOL bCanSeek;
	UINT nTimerID;
	LONGLONG g_rtTotalTime;
	CSliderCtrl* hScroll;
	IMediaSeeking * pseek;
	void EmDEvice(REFCLSID);
	long g_lVolume;
long vol;
	CPlayerDlg(CWnd* pParent = NULL);	// standard constructor
	CString p;

	IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEventEx   *pEvent;
	RECT rcSrc, rcDest;
	IBasicAudio   *pBA;
	IVideoWindow  *pVW;
	IVideoFrameStep *pStep;
	
    IVMRWindowlessControl *g_pWc;
	HRESULT InitWindowlessVMR(HWND hwndApp,IGraphBuilder* pGraph,IVMRWindowlessControl** ppWc) ;
	void OnPlay() ;


// Dialog Data
	//{{AFX_DATA(CPlayerDlg)
	enum { IDD = IDD_PLAYER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnContinue();
	afx_msg void OnPause();
	afx_msg void OnVOLP();
	afx_msg void OnVOLM();
	afx_msg void OnFull();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void HandleGraphEvent();
	afx_msg void OnEmDev();
	afx_msg void OnGrab();
	afx_msg void OnEmuGraphyFilter();
	afx_msg void OnSeek();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERDLG_H__CE668EF8_76C8_4839_99F0_F01A3FD0BC36__INCLUDED_)
