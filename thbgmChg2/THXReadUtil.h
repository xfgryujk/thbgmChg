#pragma once


// MFCで使う時用
#undef new
#include <string>
#include <vector>
#include <map>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include <string>
//#include <vector>
//#include <map>

using namespace std;

#define VER_TH6 6
#define VER_TH7 7
#define VER_TH8 8
#define VER_TH9 9
#define VER_TH10 10

class CTHXReadUtil
{
public:
	CTHXReadUtil(void);
	~CTHXReadUtil(void);

public:
	typedef struct _BGM_LIST {
		string strBgmName;
		wstring strWaveFile;
		UINT uiLoop;
		UINT uiEnd;
		UINT uiOffset;
		string strFileName;
		UINT uiDefOffset;
		UINT uiDefLoop;
		UINT uiDefEnd;
	} BGM_LIST;

	BOOL LoadDataFile(LPCWSTR lpcwszFile);
	BOOL LoadBgmData(vector<CTHXReadUtil::BGM_LIST>* pBgmList);

	BOOL SaveDataFile(LPCWSTR lpcwszFile, vector<CTHXReadUtil::BGM_LIST>* pBgmList );
protected:
	typedef struct _th_file_entry {
		string name;
		int offset;
		int complen;
		int origlen;
	} th_file_entry;

	vector<th_file_entry*> FileList;
	map<string, th_file_entry*> FileMap;

	typedef struct _bitstrm {
		unsigned char b;
		unsigned char c;
		unsigned char *cur;
		unsigned char *base;
		int size;
	} bitstrm;

	DWORD m_dwFileVer;
	wstring m_strLoadDataFile;

protected:
	void th_crypter(unsigned char* in, unsigned char *out, int size, unsigned char key, unsigned char step, int block, int limit);
	void th_encrypt(unsigned char* in, unsigned char *out, int size, unsigned char key, unsigned char step, int block, int limit);
	int th_get_cryprm_index_10(char *name);
	int th_get_cryprm_index_9(BYTE bType);
	int th_extract_file(th_file_entry *entry, BYTE* pbOut, DWORD* dwOutSize);
	BOOL th_archive_file(LPCSTR lpcszFileName, BYTE* pbIn, DWORD dwInSize, BYTE* pbOut, DWORD* dwOutSize);

	void bitstrm_init(bitstrm *strm, unsigned char *buf, int size);
	int bitstrm_read(bitstrm *strm, unsigned int bits);
	int bitstrm_read_th06_int(bitstrm *strm);
	void bitstrm_read_th06_string(bitstrm *strm, string* str);
	void bitstrm_write(struct bitstrm *strm, int val,unsigned int bits);
	void bitstrm_write_th06_int(bitstrm *strm, int val);
	void bitstrm_write_th06_string(bitstrm *strm, string str);

	BOOL LoadFmt(BYTE* pbBuff, DWORD dwSize, vector<CTHXReadUtil::BGM_LIST>* pBgmList);
	BOOL LoadMusiccmt(BYTE* pbBuff, DWORD dwSize, vector<CTHXReadUtil::BGM_LIST>* pBgmList);
	BOOL LoadPos(BYTE* pbBuff, DWORD dwSize, int iIndex, vector<CTHXReadUtil::BGM_LIST>* pBgmList);

	BOOL ChangeFmt(vector<CTHXReadUtil::BGM_LIST>* pBgmList, BYTE* pbBuff, DWORD dwSize);
	BOOL CreateFileIndex(BYTE* pbOut, DWORD* dwOutSize, DWORD* dwOriSize);
};
