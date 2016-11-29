// IniDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "thbgmChg2.h"
#include "IniDlg.h"


// CIniDlg ダイアログ

IMPLEMENT_DYNAMIC(CIniDlg, CDialog)

CIniDlg::CIniDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIniDlg::IDD, pParent)
	, m_strIni(_T(""))
	, m_strIni1(_T(""))
{

}

CIniDlg::~CIniDlg()
{
}

void CIniDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INI, m_strIni);
	DDX_Text(pDX, IDC_EDIT_INI1, m_strIni1);
}


BEGIN_MESSAGE_MAP(CIniDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CIniDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CIniDlg メッセージ ハンドラ

BOOL CIniDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	m_strIni1 = L"※注意※";
	m_strIni = L"このツールは上海アリス幻樂団（神主様）とは一切関係ありません。\r\nこのツールに関することを上海アリス幻樂団（神主様）に問い合わせることは一切行わないでください。\r\n\r\n利用は自己責任でお願いします。";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CIniDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	OnOK();
}
