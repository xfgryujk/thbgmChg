// thbgmChg2Dlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "thbgmChg2.h"
#include "thbgmChg2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CthbgmChg2Dlg ダイアログ




CthbgmChg2Dlg::CthbgmChg2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CthbgmChg2Dlg::IDD, pParent)
	, m_strDataPath(_T(""))
	, m_strBgmPath(_T(""))
	, m_strChgWave(_T(""))
	, m_dLoop(0)
	, m_dEnd(0)
	, m_strSavePath(_T(""))
	, m_bKouma(FALSE)
	, m_bBgmFile(FALSE)
	, m_bChk16(FALSE)
	, m_strLoop2(_T(""))
	, m_strEnd2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CthbgmChg2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DAT_PATH, m_strDataPath);
	DDX_Text(pDX, IDC_EDIT_BGM_PATH, m_strBgmPath);
	DDX_Text(pDX, IDC_EDIT_CHG_WAV, m_strChgWave);
	DDX_Text(pDX, IDC_EDIT_LOOP, m_dLoop);
	DDX_Text(pDX, IDC_EDIT_END, m_dEnd);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_strSavePath);
	DDX_Control(pDX, IDC_LIST_BGM, m_cListBgm);
	DDX_Control(pDX, IDC_EDIT_CHG_WAV, m_cEditChgWav);
	DDX_Control(pDX, IDC_BUTTON_WAV_OPEN, m_cBtnWavOpen);
	DDX_Control(pDX, IDC_EDIT_LOOP, m_cEditLoop);
	DDX_Control(pDX, IDC_EDIT_END, m_cEditEnd);
	DDX_Control(pDX, IDC_EDIT_SAVE_PATH, m_cEditSavePath);
	DDX_Control(pDX, IDC_BUTTON_SAVE_OPEN, m_cBtnSaveOpen);
	DDX_Control(pDX, IDC_BUTTON_CREATE, m_cBtnCreate);
	DDX_Control(pDX, IDC_BUTTON_CAN, m_cBtnCan);
	DDX_Control(pDX, IDC_EDIT_DAT_PATH, m_cEditDataPath);
	DDX_Control(pDX, IDC_EDIT_BGM_PATH, m_cEditBgmPath);
	DDX_Control(pDX, IDC_BUTTON_DAT_OPEN, m_cBtnDataOpen);
	DDX_Control(pDX, IDC_BUTTON_BGM_OPEN, m_cBtnBgmOpen);
	DDX_Control(pDX, IDC_BUTTON_DAT_CHK, m_cBtnDataChk);
	DDX_Control(pDX, IDC_PROGRESS1, m_cProg);
	DDX_Check(pDX, IDC_CHECK_KOUMA, m_bKouma);
	DDX_Check(pDX, IDC_CHECK_BGMFILE, m_bBgmFile);
	DDX_Control(pDX, IDC_CHECK_BGMFILE, m_cChkBgmFile);
	DDX_Control(pDX, IDC_CHECK_KOUMA, m_cChkKouma);
	DDX_Check(pDX, IDC_CHECK_16, m_bChk16);
	DDX_Text(pDX, IDC_EDIT_LOOP2, m_strLoop2);
	DDX_Text(pDX, IDC_EDIT_END2, m_strEnd2);
}

BEGIN_MESSAGE_MAP(CthbgmChg2Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_DAT_OPEN, &CthbgmChg2Dlg::OnBnClickedButtonDatOpen)
	ON_BN_CLICKED(IDC_BUTTON_BGM_OPEN, &CthbgmChg2Dlg::OnBnClickedButtonBgmOpen)
	ON_BN_CLICKED(IDC_BUTTON_DAT_CHK, &CthbgmChg2Dlg::OnBnClickedButtonDatChk)
	ON_BN_CLICKED(IDC_BUTTON_WAV_OPEN, &CthbgmChg2Dlg::OnBnClickedButtonWavOpen)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP, &CthbgmChg2Dlg::OnEnKillfocusEditLoop)
	ON_EN_KILLFOCUS(IDC_EDIT_END, &CthbgmChg2Dlg::OnEnKillfocusEditEnd)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_OPEN, &CthbgmChg2Dlg::OnBnClickedButtonSaveOpen)
	ON_LBN_SELCHANGE(IDC_LIST_BGM, &CthbgmChg2Dlg::OnLbnSelchangeListBgm)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CthbgmChg2Dlg::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_CAN, &CthbgmChg2Dlg::OnBnClickedButtonCan)
	ON_BN_CLICKED(IDC_CHECK_16, &CthbgmChg2Dlg::OnBnClickedCheck16)
END_MESSAGE_MAP()


// CthbgmChg2Dlg メッセージ ハンドラ

BOOL CthbgmChg2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	DisableItemIni();
	m_iLastIndex = -1;

	m_pThread = NULL;
//	m_pcRead = NULL;


	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CthbgmChg2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CthbgmChg2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CthbgmChg2Dlg::OnBnClickedButtonDatOpen()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	WCHAR strFile[MAX_PATH]=L"";
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof (OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = 	TEXT("dat files (*.dat)\0*.dat\0")
					TEXT("All files (*.*)\0*.*\0\0");
	ofn.lpstrFile = strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == 0) {
		return ;
	}
	m_strDataPath = strFile;
	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnBnClickedButtonBgmOpen()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	if( m_bKouma == FALSE ){
		WCHAR strFile[MAX_PATH]=L"";
		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof (OPENFILENAME);
		ofn.hwndOwner = m_hWnd;
		ofn.lpstrFilter = 	TEXT("dat files (*.dat)\0*.dat\0")
						TEXT("All files (*.*)\0*.*\0\0");
		ofn.lpstrFile = strFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn) == 0) {
			return ;
		}
		m_strBgmPath = strFile;
	}else{
		BROWSEINFO bi;
		LPWSTR lpBuffer;
		LPITEMIDLIST pidlRoot = NULL;
		LPITEMIDLIST pidlBrowse;
		LPMALLOC lpMalloc = NULL;

		HRESULT hr = SHGetMalloc(&lpMalloc);
		if(FAILED(hr)) return;

		if ((lpBuffer = (LPWSTR) lpMalloc->Alloc(_MAX_PATH*2)) == NULL) {
			return;
		}
		if( m_strBgmPath.IsEmpty() != 0 ){
			if (!SUCCEEDED(SHGetSpecialFolderLocation( m_hWnd,CSIDL_DESKTOP,&pidlRoot ) )){ 
				lpMalloc->Free(lpBuffer);
				return;
			}
		}

		bi.hwndOwner = m_hWnd;
		bi.pidlRoot = pidlRoot;
		bi.pszDisplayName = lpBuffer;
		bi.lpszTitle = L"BGMフォルダを選択してください";
		bi.ulFlags = 0;
		bi.lpfn = NULL;
		bi.lParam = (LPARAM)m_strBgmPath.GetBuffer(0);

		pidlBrowse = SHBrowseForFolder(&bi);
		if (pidlBrowse != NULL) {  
			if (SHGetPathFromIDList(pidlBrowse, lpBuffer)) {
				m_strBgmPath = lpBuffer;
			}
			lpMalloc->Free(pidlBrowse);
		}
		if( pidlRoot != NULL ){
			lpMalloc->Free(pidlRoot); 
		}
		lpMalloc->Free(lpBuffer);
		lpMalloc->Release();
	}
	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnBnClickedButtonWavOpen()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	WCHAR strFile[MAX_PATH]=L"";
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof (OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = 	TEXT("wav files (*.wav)\0*.wav\0")
					TEXT("All files (*.*)\0*.*\0\0");
	ofn.lpstrFile = strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == 0) {
		return ;
	}
	m_strChgWave = strFile;
	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnEnKillfocusEditLoop()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	if( m_dLoop < 0 ){
		m_dLoop = 0;
	}
	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnEnKillfocusEditEnd()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	if( m_dEnd < 0 ){
		m_dEnd = 0;
	}
	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnBnClickedButtonSaveOpen()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	BROWSEINFO bi;
	LPWSTR lpBuffer;
	LPITEMIDLIST pidlRoot = NULL;
	LPITEMIDLIST pidlBrowse;
	LPMALLOC lpMalloc = NULL;

	HRESULT hr = SHGetMalloc(&lpMalloc);
	if(FAILED(hr)) return;

	if ((lpBuffer = (LPWSTR) lpMalloc->Alloc(_MAX_PATH*2)) == NULL) {
		return;
	}
	if( m_strSavePath.IsEmpty() != 0 ){
		if (!SUCCEEDED(SHGetSpecialFolderLocation( m_hWnd,CSIDL_DESKTOP,&pidlRoot ) )){ 
			lpMalloc->Free(lpBuffer);
			return;
		}
	}
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = pidlRoot;
	bi.pszDisplayName = lpBuffer;
	bi.lpszTitle = L"データ保存フォルダを選択してください";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = (LPARAM)m_strSavePath.GetBuffer(0);

	pidlBrowse = SHBrowseForFolder(&bi);
	if (pidlBrowse != NULL) {  
		if (SHGetPathFromIDList(pidlBrowse, lpBuffer)) {
			m_strSavePath = lpBuffer;
		}
		lpMalloc->Free(pidlBrowse);
	}
	if( pidlRoot != NULL ){
		lpMalloc->Free(pidlRoot); 
	}
	lpMalloc->Free(lpBuffer);
	lpMalloc->Release();


	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnLbnSelchangeListBgm()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	int iIndex = m_cListBgm.GetCurSel();
	if( iIndex == LB_ERR ){
		m_iLastIndex = -1;
		return ;
	}
	CString strA;
	if( m_iLastIndex != -1 ){
		strA = m_strChgWave;
		BgmList[m_iLastIndex].strWaveFile = strA.GetBuffer(0);
		if( m_bChk16 == TRUE ){
			WCHAR *endstr;
			BgmList[m_iLastIndex].uiLoop = wcstol(m_strLoop2.GetBuffer(0), &endstr, 16);
			BgmList[m_iLastIndex].uiEnd = wcstol(m_strEnd2.GetBuffer(0), &endstr, 16);
		}else{
			BgmList[m_iLastIndex].uiLoop = (ULONG)(m_dLoop*(44100*4));
			BgmList[m_iLastIndex].uiEnd = (ULONG)(m_dEnd*(44100*4));
		}
	}
	m_iLastIndex = iIndex;

	m_strChgWave = BgmList[m_iLastIndex].strWaveFile.c_str();
//	m_dLoop = ((double)BgmList[m_iLastIndex].uiLoop)/(44100*4);
//	m_dEnd = ((double)BgmList[m_iLastIndex].uiEnd)/(44100*4);

	m_dLoop = (BgmList[m_iLastIndex].uiLoop/(44100*4))*100;
	m_dLoop += ((BgmList[m_iLastIndex].uiLoop%(44100*4))/(441*4));
	m_dLoop = m_dLoop/100;
	m_dEnd = (BgmList[m_iLastIndex].uiEnd/(44100*4))*100;
	m_dEnd += ((BgmList[m_iLastIndex].uiEnd%(44100*4))/(441*4));
	m_dEnd = m_dEnd/100;

	m_strLoop2.Format(L"%X",BgmList[m_iLastIndex].uiLoop);
	m_strEnd2.Format(L"%X",BgmList[m_iLastIndex].uiEnd);

	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnBnClickedButtonDatChk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	BgmList.clear();
	m_cListBgm.ResetContent();

	if( m_cRead.LoadDataFile(m_strDataPath.GetBuffer(0)) == FALSE ){
		MessageBox(L"ファイルのロードに失敗しました");
		return ;
	}
	if( m_cRead.LoadBgmData(&BgmList) == FALSE ){
		MessageBox(L"ファイルのロードに失敗しました");
		return ;
	}

	if( BgmList.size() == 0 ){
		MessageBox(L"ファイルのロードに失敗しました");
		return ;
	}

	for( int i=0; i<(int)BgmList.size(); i++ ){
		CStringA strA = BgmList[i].strBgmName.c_str();
		CStringW strW;
		strW= strA;
		m_cListBgm.AddString(strW);
	}
	EnableItemChk();

	m_dLoop = (BgmList[0].uiDefLoop/(44100*4))*100;
	m_dLoop += ((BgmList[0].uiDefLoop%(44100*4))/(441*4));
	m_dLoop = m_dLoop/100;
	m_dEnd = (BgmList[0].uiDefEnd/(44100*4))*100;
	m_dEnd += ((BgmList[0].uiDefEnd%(44100*4))/(441*4));
	m_dEnd = m_dEnd/100;

	m_strLoop2.Format(L"%X",BgmList[0].uiDefLoop);
	m_strEnd2.Format(L"%X",BgmList[0].uiDefEnd);

	m_iLastIndex = 0;
	m_cListBgm.SetCurSel(0);
	m_strChgWave = L"";

	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnBnClickedButtonCreate()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	if( m_strDataPath.IsEmpty() == true || m_strSavePath.IsEmpty() == true || m_strBgmPath.IsEmpty() == true){
		MessageBox(L"保存先が指定されていません");
		return ;
	}

	int iIndex = m_cListBgm.GetCurSel();
	if( iIndex != LB_ERR ){
		CString strA;
		m_iLastIndex = iIndex;

		strA = m_strChgWave;
		BgmList[m_iLastIndex].strWaveFile = strA.GetBuffer(0);
		BgmList[m_iLastIndex].uiLoop = (ULONG)(m_dLoop*(44100*4));
		BgmList[m_iLastIndex].uiEnd = (ULONG)(m_dEnd*(44100*4));
	}

	if( CheckData() == FALSE ){
		EnableItemChk();
		return;
	}

	hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	DisableItemCreate();

	m_pThread = ::AfxBeginThread(CreateBgmWaitThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();

	UpdateData(FALSE);
}

void CthbgmChg2Dlg::OnBnClickedButtonCan()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if( m_pThread != NULL ){
		SetEvent(hEvent);
	}
	EnableItemChk();
}

void CthbgmChg2Dlg::EnableItemChk()
{
	m_cEditDataPath.EnableWindow(TRUE);
	m_cEditBgmPath.EnableWindow(TRUE);
	m_cBtnDataOpen.EnableWindow(TRUE);
	m_cBtnBgmOpen.EnableWindow(TRUE);
	m_cBtnDataChk.EnableWindow(TRUE);
	m_cChkKouma.EnableWindow(TRUE);


	m_cListBgm.EnableWindow(TRUE);
	m_cEditChgWav.EnableWindow(TRUE);
	m_cBtnWavOpen.EnableWindow(TRUE);
	m_cEditLoop.EnableWindow(TRUE);
	m_cEditEnd.EnableWindow(TRUE);
	m_cEditSavePath.EnableWindow(TRUE);
	m_cBtnSaveOpen.EnableWindow(TRUE);
	m_cBtnCreate.EnableWindow(TRUE);
	m_cBtnCan.EnableWindow(FALSE);
	m_cChkBgmFile.EnableWindow(TRUE);

	GetDlgItem(IDC_CHECK_16)->EnableWindow(TRUE);
	OnBnClickedCheck16();
}

void CthbgmChg2Dlg::DisableItemIni()
{
	m_cEditDataPath.EnableWindow(TRUE);
	m_cEditBgmPath.EnableWindow(TRUE);
	m_cBtnDataOpen.EnableWindow(TRUE);
	m_cBtnBgmOpen.EnableWindow(TRUE);
	m_cBtnDataChk.EnableWindow(TRUE);
	m_cChkKouma.EnableWindow(TRUE);

	m_cListBgm.EnableWindow(FALSE);
	m_cEditChgWav.EnableWindow(FALSE);
	m_cBtnWavOpen.EnableWindow(FALSE);
	m_cEditLoop.EnableWindow(FALSE);
	m_cEditEnd.EnableWindow(FALSE);
	m_cEditSavePath.EnableWindow(FALSE);
	m_cBtnSaveOpen.EnableWindow(FALSE);
	m_cBtnCreate.EnableWindow(FALSE);
	m_cBtnCan.EnableWindow(FALSE);
	m_cChkBgmFile.EnableWindow(FALSE);

	GetDlgItem(IDC_CHECK_16)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOOP2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_END2)->EnableWindow(FALSE);
}

void CthbgmChg2Dlg::DisableItemCreate()
{
	m_cEditDataPath.EnableWindow(FALSE);
	m_cEditBgmPath.EnableWindow(FALSE);
	m_cBtnDataOpen.EnableWindow(FALSE);
	m_cBtnBgmOpen.EnableWindow(FALSE);
	m_cBtnDataChk.EnableWindow(FALSE);
	m_cChkKouma.EnableWindow(FALSE);

	m_cListBgm.EnableWindow(FALSE);
	m_cEditChgWav.EnableWindow(FALSE);
	m_cBtnWavOpen.EnableWindow(FALSE);
	m_cEditLoop.EnableWindow(FALSE);
	m_cEditEnd.EnableWindow(FALSE);
	m_cEditSavePath.EnableWindow(FALSE);
	m_cBtnSaveOpen.EnableWindow(FALSE);
	m_cBtnCreate.EnableWindow(FALSE);
	m_cBtnCan.EnableWindow(TRUE);
	m_cChkBgmFile.EnableWindow(FALSE);

	GetDlgItem(IDC_CHECK_16)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOOP2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_END2)->EnableWindow(FALSE);
}

BOOL CthbgmChg2Dlg::CheckData()
{
	for(int i=0; i<(int)BgmList.size(); i++ ){
		if( BgmList[i].strWaveFile.empty() != true ){
			HANDLE hFile = CreateFile( BgmList[i].strWaveFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if( hFile == INVALID_HANDLE_VALUE ){
				MessageBox(L"wavファイルが見つかりませんでした");
				return FALSE;
			}
			DWORD dwL = GetFileSize(hFile,NULL);
			CloseHandle(hFile);
			if( BgmList[i].uiEnd > dwL-0x2C ){
				BgmList[i].uiEnd = dwL;
			}
			if( BgmList[i].uiLoop > dwL-0x2C ){
				BgmList[i].uiLoop = dwL;
			}
		}else{
			if( BgmList[i].uiEnd > BgmList[i].uiDefEnd ){
				BgmList[i].uiEnd = BgmList[i].uiDefEnd;
			}
			if( BgmList[i].uiLoop > BgmList[i].uiDefEnd ){
				BgmList[i].uiLoop = BgmList[i].uiDefEnd;
			}
		}
		if( m_bKouma == TRUE ){
			CString strSrc;
			CStringA strA;
			strA = BgmList[i].strFileName.c_str();
			strSrc = m_strBgmPath;
			strSrc+="\\";
			strSrc+=strA;
			HANDLE hFile = CreateFileW( strSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if( hFile == INVALID_HANDLE_VALUE ){
				MessageBox(L"ファイルが見つかりませんでした");
				return FALSE;
			}
			CloseHandle(hFile);
		}
	}
	return TRUE;
}

UINT CthbgmChg2Dlg::CreateBgmWaitThread(LPVOID pParam)
{
	CthbgmChg2Dlg* pSys = (CthbgmChg2Dlg*)pParam;

	//SetThreadLocale(MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN), SORT_JAPANESE_XJIS));

	pSys->m_cProg.SetPos(0);

	if( pSys->m_bKouma == FALSE ){
		HANDLE hFile = CreateFile( pSys->m_strBgmPath.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ){
			pSys->PostMessageW(WM_USER+101);
			return FALSE;
		}
		CString strBgm;
		strBgm = pSys->m_strSavePath;
		strBgm+="\\thbgm.dat";
		HANDLE hFile2 = CreateFile( strBgm.GetBuffer(0), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile2 == INVALID_HANDLE_VALUE ){
			pSys->PostMessageW(WM_USER+101);
			CloseHandle(hFile);
			return FALSE;
		}
		BYTE ReadBuff[4096];
		DWORD dwRead;
		DWORD dwWrite;

		ReadFile(hFile,&ReadBuff, 0x10, &dwRead, NULL );
		WriteFile(hFile2,&ReadBuff, 0x10, &dwWrite, NULL );

		pSys->m_cProg.SetPos(10);

		DWORD dwOffset = 0x10;

		for( int i=0; i<(int)pSys->BgmList.size(); i++ ){
			if( WaitForSingleObject( pSys->hEvent, 0 ) != WAIT_TIMEOUT ){
				//中止
				pSys->PostMessageW(WM_USER+102);
				CloseHandle(hFile);
				CloseHandle(hFile2);
				return FALSE;
			}
			if( pSys->BgmList[i].strWaveFile.empty() == true ){
				pSys->BgmList[i].uiOffset = dwOffset;
				SetFilePointer(hFile, pSys->BgmList[i].uiDefOffset, NULL, FILE_BEGIN);
				DWORD dwReadSize=0;
				while(dwReadSize<pSys->BgmList[i].uiDefEnd){
					if( WaitForSingleObject( pSys->hEvent, 0 ) != WAIT_TIMEOUT ){
						//中止
						pSys->PostMessageW(WM_USER+102);
						CloseHandle(hFile);
						CloseHandle(hFile2);
						return FALSE;
					}

					DWORD dwSize = 0;
					if( dwReadSize+4096<pSys->BgmList[i].uiDefEnd ){
						dwSize = 4096;
					}else{
						dwSize = pSys->BgmList[i].uiDefEnd - dwReadSize;
					}
					ReadFile(hFile,&ReadBuff, dwSize, &dwRead, NULL );
					WriteFile(hFile2,&ReadBuff, dwSize, &dwWrite, NULL );
					dwReadSize+=4096;
				}
				dwOffset += pSys->BgmList[i].uiDefEnd;
			}else{
				pSys->BgmList[i].uiOffset = dwOffset;
				HANDLE hFile3 = CreateFile( pSys->BgmList[i].strWaveFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile3 == INVALID_HANDLE_VALUE ){
					pSys->PostMessageW(WM_USER+101);
					CloseHandle(hFile);
					CloseHandle(hFile2);
					return FALSE;
				}
				DWORD dwWavSize = GetFileSize(hFile3, NULL);
				dwWavSize-=0x2C;
				SetFilePointer(hFile3, 0x2C, NULL, FILE_BEGIN);
				DWORD dwReadSize=0;

				while(dwReadSize<dwWavSize){
					if( WaitForSingleObject( pSys->hEvent, 0 ) != WAIT_TIMEOUT ){
						//中止
						pSys->PostMessageW(WM_USER+102);
						CloseHandle(hFile);
						CloseHandle(hFile2);
						CloseHandle(hFile3);
						return FALSE;
					}

					DWORD dwSize = 0;
					if( dwReadSize+4096<dwWavSize ){
						dwSize = 4096;
					}else{
						dwSize = dwWavSize - dwReadSize;
					}
					ReadFile(hFile3,&ReadBuff, dwSize, &dwRead, NULL );
					WriteFile(hFile2,&ReadBuff, dwSize, &dwWrite, NULL );
					dwReadSize+=4096;
				}

				dwOffset += dwWavSize;
				CloseHandle(hFile3);
			}
			pSys->m_cProg.SetPos(10+(70/(int)pSys->BgmList.size())*i);
		}

		CloseHandle(hFile);
		CloseHandle(hFile2);
	}else{
		pSys->m_cProg.SetPos(10);
		for( int i=0; i<(int)pSys->BgmList.size(); i++ ){
			if( WaitForSingleObject( pSys->hEvent, 0 ) != WAIT_TIMEOUT ){
				//中止
				pSys->PostMessageW(WM_USER+102);
				return FALSE;
			}
			if( pSys->BgmList[i].strWaveFile.empty() == true ){
				CString strSrc=L"";
				CString strDest=L"";
				CStringA strA;
				strA = pSys->BgmList[i].strFileName.c_str();
				strSrc+= pSys->m_strBgmPath;
				strSrc+="\\";
				strSrc+=strA;

				strA = pSys->BgmList[i].strFileName.c_str();
				strDest+= pSys->m_strSavePath;
				strDest+="\\";
				strDest+=strA;

				CopyFile(strSrc.GetBuffer(0), strDest.GetBuffer(0), FALSE );
			}else{
				CString strSrc=L"";
				CString strDest=L"";
				CStringA strA;
				strA = pSys->BgmList[i].strWaveFile.c_str();
				strSrc+=strA;

				strA = pSys->BgmList[i].strFileName.c_str();
				strDest+= pSys->m_strSavePath;
				strDest+="\\";
				strDest+=strA;

				HANDLE hFile = CreateFileW( strSrc.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile == INVALID_HANDLE_VALUE ){
					return FALSE;
				}
				HANDLE hFile2 = CreateFileW( strDest.GetBuffer(0), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile2 == INVALID_HANDLE_VALUE ){
					CloseHandle(hFile);
					return FALSE;
				}

				DWORD dwWrite;
				DWORD dwRead;
				DWORD dwReadSize=0;
				DWORD dwSize;
				SetFilePointer(hFile, 0x2C, NULL, FILE_BEGIN);

				WriteFile(hFile2, "RIFF", 4, &dwWrite, NULL );
				dwSize = 0x2C+pSys->BgmList[i].uiEnd+pSys->BgmList[i].uiLoop-8;
				WriteFile(hFile2, &dwSize, 4, &dwWrite, NULL );
				WriteFile(hFile2, "WAVEfmt ", 8, &dwWrite, NULL );
				BYTE bfmt[20] = {0x10,0x00,0x00,0x00,
								0x01,0x00,0x02,0x00,
								0x44,0xAC,0x00,0x00,
								0x10,0xB1,0x02,0x00,
								0x04,0x00,0x10,0x00};
				WriteFile(hFile2, bfmt, 20, &dwWrite, NULL );
				WriteFile(hFile2, "data", 4, &dwWrite, NULL );
				dwSize = pSys->BgmList[i].uiEnd+pSys->BgmList[i].uiLoop;
				WriteFile(hFile2, &dwSize, 4, &dwWrite, NULL );

				while(dwReadSize<dwSize){
					DWORD Temp;
					if( dwReadSize+4096<=dwSize ){
						Temp = 4096;
					}else{
						Temp = dwSize-dwReadSize;
					}
					BYTE bBuff[4096];
					ReadFile(hFile, &bBuff,Temp,&dwRead, NULL );
					WriteFile(hFile2, &bBuff, dwRead, &dwWrite, NULL );

					dwReadSize+=4096;
				}

				CloseHandle(hFile);
				CloseHandle(hFile2);
			}
			pSys->m_cProg.SetPos(10+(70/(int)pSys->BgmList.size())*i);
		}
	}
	pSys->m_cProg.SetPos(80);

	WCHAR szFile[_MAX_PATH];	// ファイル
	WCHAR szDrive[_MAX_DRIVE];
	WCHAR szDir[_MAX_DIR];
	WCHAR szFname[_MAX_FNAME];
	WCHAR szExt[_MAX_EXT];
	_tsplitpath_s( pSys->m_strDataPath.GetBuffer(0), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFname, _MAX_FNAME, szExt, _MAX_EXT );
	_tmakepath_s(  szFile, _MAX_PATH, L"", L"", szFname, szExt );

	CString strSaveFile=L"";
	strSaveFile = pSys->m_strSavePath;
	strSaveFile+="\\";
	strSaveFile+=szFile;
	pSys->m_cRead.SaveDataFile(strSaveFile.GetBuffer(0),&pSys->BgmList);


	if(pSys->m_bBgmFile == TRUE){
		CString strSaveFile = pSys->m_strSavePath;
		strSaveFile+="\\titles_custom.txt";

		HANDLE hFile = CreateFile( strSaveFile.GetBuffer(0), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ){
			pSys->PostMessageW(WM_USER+101);
			CloseHandle(hFile);
			return FALSE;
		}

		DWORD dwWrite;
		CStringA strWrite="@,カスタムデータ\r\n";
		WriteFile(hFile,strWrite.GetBuffer(0), strWrite.GetLength(), &dwWrite, NULL );
		for(int i=0; i<(int)pSys->BgmList.size(); i++ ){
			if( pSys->m_bKouma == FALSE ){
				if( pSys->BgmList[i].strWaveFile.empty() == true ){
					strWrite.Format("%08X,%08X,%08X,%s\r\n",
						pSys->BgmList[i].uiDefOffset,
						pSys->BgmList[i].uiDefLoop,
						(pSys->BgmList[i].uiDefEnd-pSys->BgmList[i].uiDefLoop),
						pSys->BgmList[i].strBgmName.c_str()
						);
				}else{
					strWrite.Format("%08X,%08X,%08X,%s\r\n",
						pSys->BgmList[i].uiOffset,
						pSys->BgmList[i].uiLoop,
						(pSys->BgmList[i].uiEnd-pSys->BgmList[i].uiLoop),
						pSys->BgmList[i].strBgmName.c_str()
						);
				}
			}else{
				if( pSys->BgmList[i].strWaveFile.empty() == true ){
					strWrite.Format("%%%s,%08X,%08X,%08X,%s\r\n",
						pSys->BgmList[i].strFileName.c_str(),
						pSys->BgmList[i].uiDefOffset,
						pSys->BgmList[i].uiDefLoop,
						(pSys->BgmList[i].uiDefEnd-pSys->BgmList[i].uiDefLoop),
						pSys->BgmList[i].strBgmName.c_str()
						);
				}else{
					strWrite.Format("%%%s,%08X,%08X,%08X,%s\r\n",
						pSys->BgmList[i].strFileName.c_str(),
						pSys->BgmList[i].uiOffset,
						pSys->BgmList[i].uiLoop,
						(pSys->BgmList[i].uiEnd-pSys->BgmList[i].uiLoop),
						pSys->BgmList[i].strBgmName.c_str()
						);
				}
			}
			WriteFile(hFile,strWrite.GetBuffer(0), strWrite.GetLength(), &dwWrite, NULL );
		}
		CloseHandle(hFile);
	}

	pSys->m_cProg.SetPos(100);
	pSys->PostMessageW(WM_USER+100);

	return 0;
}

LRESULT CthbgmChg2Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	if( message == WM_USER+100 ){
		MessageBox(L"終了しました");
		EnableItemChk();
	}else if( message == WM_USER+101 ){
		MessageBox(L"エラーが発生したので中止します");
		EnableItemChk();
	}else if( message == WM_USER+102){
		MessageBox(L"キャンセルされました");
	}

	if(message>= WM_USER+100 && message<= WM_USER+102){
		if( m_pThread != NULL ){
			if ( ::WaitForSingleObject(m_pThread->m_hThread, 2000) == WAIT_TIMEOUT ){
				::TerminateThread(m_pThread->m_hThread, 0xffffffff);
			}
			delete m_pThread;
			m_pThread = NULL;
		}
		::CloseHandle(hEvent);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CthbgmChg2Dlg::OnBnClickedCheck16()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	UpdateData(TRUE);
	if( m_bChk16 == TRUE ){
		GetDlgItem(IDC_EDIT_LOOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_END)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOOP2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_END2)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_EDIT_LOOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_END)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LOOP2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_END2)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}
