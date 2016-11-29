#pragma once


// CIniDlg ダイアログ

class CIniDlg : public CDialog
{
	DECLARE_DYNAMIC(CIniDlg)

public:
	CIniDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CIniDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_INI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strIni;
	CString m_strIni1;
	afx_msg void OnBnClickedOk();
};
