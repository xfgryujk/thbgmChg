// thbgmChg2.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CthbgmChg2App:
// このクラスの実装については、thbgmChg2.cpp を参照してください。
//

class CthbgmChg2App : public CWinApp
{
public:
	CthbgmChg2App();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CthbgmChg2App theApp;