// thbgmChg2Dlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

#include "THXReadUtil.h"
#include "afxcmn.h"


// CthbgmChg2Dlg ダイアログ
class CthbgmChg2Dlg : public CDialog
{
// コンストラクション
public:
	CthbgmChg2Dlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_THBGMCHG2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonDatOpen();
	afx_msg void OnBnClickedButtonBgmOpen();
	afx_msg void OnBnClickedButtonDatChk();
	afx_msg void OnBnClickedButtonWavOpen();
	afx_msg void OnEnKillfocusEditLoop();
	afx_msg void OnEnKillfocusEditEnd();
	afx_msg void OnBnClickedButtonSaveOpen();
	afx_msg void OnLbnSelchangeListBgm();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonCan();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void DisableItemIni();
	void EnableItemChk();
	void DisableItemCreate();
	BOOL CheckData();

	static UINT CreateBgmWaitThread(LPVOID pParam);

protected:
	CString m_strDataPath;
	CString m_strBgmPath;
	CString m_strChgWave;
	double m_dLoop;
	double m_dEnd;
	CString m_strSavePath;

	CEdit m_cEditDataPath;
	CEdit m_cEditBgmPath;
	CButton m_cBtnDataOpen;
	CButton m_cBtnBgmOpen;
	CButton m_cBtnDataChk;

	CListBox m_cListBgm;
	CEdit m_cEditChgWav;
	CButton m_cBtnWavOpen;
	CEdit m_cEditLoop;
	CEdit m_cEditEnd;
	CEdit m_cEditSavePath;
	CButton m_cBtnSaveOpen;
	CButton m_cBtnCreate;
	CButton m_cBtnCan;

	int m_iLastIndex;

	CTHXReadUtil m_cRead;
	vector<CTHXReadUtil::BGM_LIST> BgmList;

	CWinThread* m_pThread;
	HANDLE hEvent;

	CProgressCtrl m_cProg;
	BOOL m_bKouma;
	BOOL m_bBgmFile;
	CButton m_cChkBgmFile;
	CButton m_cChkKouma;
public:
	afx_msg void OnBnClickedCheck16();
	BOOL m_bChk16;
	CString m_strLoop2;
	CString m_strEnd2;
};
