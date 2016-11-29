#include "StdAfx.h"
#include "THXReadUtil.h"

#include "lzss.h"

typedef struct _CRYPT_PARAM{
	unsigned char type;
	unsigned char key;
	unsigned char step;
	int block;
	int limit;
} CRYPT_PARAM;

#define CRYPRM_MAX 8
/* for Touhou Eiyashou */
CRYPT_PARAM g_cryprm_08[] = {
	{ 0x4d, 0x1b, 0x37, 0x40, 0x2000 },
	{ 0x54, 0x51, 0xe9, 0x40, 0x3000 },
	{ 0x41, 0xc1, 0x51, 0x1400, 0x2000 },
	{ 0x4a, 0x03, 0x19, 0x1400, 0x7800 },
	{ 0x45, 0xab, 0xcd, 0x200, 0x1000 },
	{ 0x57, 0x12, 0x34, 0x400, 0x2800 },
	{ 0x2d, 0x35, 0x97, 0x80, 0x2800 },
	{ 0x2a, 0x99, 0x37, 0x400, 0x1000 }
};
/* for Touhou Kaeizuka */
CRYPT_PARAM g_cryprm_09[] = {
	{ 0x4d, 0x1b, 0x37, 0x40,  0x2800 },
	{ 0x54, 0x51, 0xe9, 0x40,  0x3000 },
	{ 0x41, 0xc1, 0x51, 0x400, 0x400 },
	{ 0x4a, 0x03, 0x19, 0x400, 0x400 },
	{ 0x45, 0xab, 0xcd, 0x200, 0x1000 },
	{ 0x57, 0x12, 0x34, 0x400, 0x400 },
	{ 0x2d, 0x35, 0x97, 0x80,  0x2800 },
	{ 0x2a, 0x99, 0x37, 0x400, 0x1000 }
};

CRYPT_PARAM g_cryprm_10[] = {
	{ 0x00, 0x1b, 0x37, 0x40,  0x2800 },
	{ 0x00, 0x51, 0xe9, 0x40,  0x3000 },
	{ 0x00, 0xc1, 0x51, 0x80,  0x3200 },
	{ 0x00, 0x03, 0x19, 0x400, 0x7800 },
	{ 0x00, 0xab, 0xcd, 0x200, 0x2800 },
	{ 0x00, 0x12, 0x34, 0x80,  0x3200 },
	{ 0x00, 0x35, 0x97, 0x80,  0x2800 },
	{ 0x00, 0x99, 0x37, 0x400, 0x2000 }
};

//暗号用テーブル
//typeとの法則性がわからん
typedef struct _CRYPT_PARAM2{
	string strEx;
	unsigned char type;
} CRYPT_PARAM2;

#define CRYPRM2_MAX 14
CRYPT_PARAM2 g_cryprm2_9[] = {
	{ ".anm", 0x2d },
	{ ".ecl", 0x45 },
	{ ".end", 0x2d },
	{ ".fmt", 0x2d },
	{ ".dat", 0x2d },
	{ ".jpg", 0x4a },
	{ ".mid", 0x2d },
	{ ".png", 0x2d },
	{ ".rpy", 0x2d },
	{ ".sht", 0x2d },
	{ ".std", 0x2d },
	{ ".ver", 0x2d },
	{ ".wav", 0x57 },
	{ ".txt", 0x54 }
};

CTHXReadUtil::CTHXReadUtil(void)
{
	m_dwFileVer = VER_TH10;
}

CTHXReadUtil::~CTHXReadUtil(void)
{
	for( int i=0; i<(int)FileList.size(); i++ ){
		delete FileList[i];
	}
}

BOOL CTHXReadUtil::LoadDataFile(LPCWSTR lpcwszFile)
{
	for( int i=0; i<(int)FileList.size(); i++ ){
		delete FileList[i];
	}
	FileList.clear();
	FileMap.clear();

	if( lpcwszFile == FALSE ){
		return FALSE;
	}

	HANDLE hFile = CreateFileW( lpcwszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	DWORD dwFileSize = GetFileSize( hFile, NULL );

	DWORD dwRead = 0;
	BYTE bHead[16];

	if( ReadFile(hFile, &bHead, 16, &dwRead, NULL ) == FALSE ){
		CloseHandle(hFile);
		return FALSE;
	}

	struct {
		unsigned long magic;
		unsigned long dir_size;
		unsigned long dir_compsize;
		unsigned long unk2;
	} hed;

	th_crypter(bHead, (unsigned char*)&hed, 0x10, 0x1B, 0x37, 0x10, 0x10);
	if(hed.magic != '1AHT') {
		if( ((int*)bHead)[0] == 'ZGBP' ){
			BYTE bBuff[12];
			th_crypter(bHead+4, bBuff, 0x0C, 0x1B, 0x37, 0x0C, 0x400);
			m_dwFileVer = VER_TH9;
			hed.magic = ((int*)bHead)[0];
			hed.unk2 = ((int*)bBuff)[0] - 123456;
			hed.dir_compsize = dwFileSize-(((int*)bBuff)[1] - 345678);
			hed.dir_size = ((int*)bBuff)[2] - 567891;
		}else if( ((int*)bHead)[0] == '4GBP' ){
			m_dwFileVer = VER_TH7;
			hed.magic = ((int*)bHead)[0];
			hed.unk2 = ((int*)bHead)[1];
			hed.dir_compsize = dwFileSize-((int*)bHead)[2];
			hed.dir_size = ((int*)bHead)[3];
		}
		else if( ((int*)bHead)[0] == '3GBP' ){
			m_dwFileVer = VER_TH6;
			hed.magic = ((int*)bHead)[0];
			bitstrm bs;
			bitstrm_init(&bs, bHead+4, 9);

			hed.unk2 = bitstrm_read_th06_int(&bs);
			int offset = bitstrm_read_th06_int(&bs);
			hed.dir_compsize = dwFileSize-offset;
			hed.dir_size = dwFileSize-offset;

			CloseHandle(hFile);
			return FALSE;
		}else{
			CloseHandle(hFile);
			return FALSE;
		}
	}else{
		m_dwFileVer = VER_TH10;
		hed.dir_size     -= 123456789;
		hed.dir_compsize -= 987654321;
		hed.unk2         += 0xF7E7F8AC;
	}

	SetFilePointer(hFile, dwFileSize - hed.dir_compsize, NULL, FILE_BEGIN );

	BYTE* pbBuff = NULL;
	pbBuff = new BYTE[hed.dir_compsize];

	if( ReadFile(hFile, pbBuff, hed.dir_compsize, &dwRead, NULL ) == FALSE ){
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);

	BYTE* pbCrypt = NULL;
	pbCrypt = new BYTE[hed.dir_compsize];
	if( m_dwFileVer >= VER_TH10 ){
		th_crypter(pbBuff, pbCrypt, hed.dir_compsize, 0x3E, 0x9B, 0x80, hed.dir_size);
	}else if( m_dwFileVer >= VER_TH8 ){
		th_crypter(pbBuff, pbCrypt, hed.dir_compsize, 0x3E, 0x9B, 0x80, 0x400);
	}else{
		memcpy( pbCrypt, pbBuff, hed.dir_compsize);
	}

	delete[] pbBuff;

	BYTE* pbUnComp = NULL;
	pbUnComp = new BYTE[hed.dir_size];
	if( m_dwFileVer >= VER_TH7 ){
		lzss_decompress(pbCrypt, hed.dir_compsize, pbUnComp, hed.dir_size);
	}else{
		memcpy( pbUnComp, pbCrypt, hed.dir_compsize);
	}

	delete[] pbCrypt;

	BYTE* pbPos = pbUnComp;
	bitstrm bs;
	bitstrm_init(&bs, pbUnComp, hed.dir_compsize);
	for( unsigned long i=0; i< hed.unk2; i++) {
		th_file_entry* pItem = NULL;
		pItem = new th_file_entry;
		if( m_dwFileVer == VER_TH6 ){
			bitstrm_read_th06_int(&bs);
			bitstrm_read_th06_int(&bs);
			bitstrm_read_th06_int(&bs);
			pItem->offset = bitstrm_read_th06_int(&bs);
			pItem->origlen = bitstrm_read_th06_int(&bs);
			bitstrm_read_th06_string(&bs, &pItem->name);
		}else if( m_dwFileVer >= VER_TH10 ){
			int ofs = 0;
			while(pbPos[3 + ofs]) ofs += 4;
			char* pName = _strdup(( const char *)pbPos);
			pItem->name    = pName;
			free(pName);
			pItem->offset  = *(unsigned long*)&pbPos[4 + ofs];
			pItem->origlen = *(unsigned long*)&pbPos[8 + ofs];
			pbPos += 16 + ofs;
		}else{
			int ofs = 0;
			char* pName = _strdup((const char *)pbPos);
			pItem->name    = pName;
			free(pName);
			ofs = (int)pItem->name.length() + 1;
			pItem->offset  = *(unsigned long*)&pbPos[ofs];
			pItem->origlen = *(unsigned long*)&pbPos[4 + ofs];
			pbPos += 12 + ofs;
			if( pItem->name.find("th08_") != string::npos ){
				m_dwFileVer = VER_TH8;
			}
		}
		FileList.push_back(pItem);

		FileMap.insert(pair<string,th_file_entry*>(pItem->name,pItem));
	}

	delete[] pbUnComp;

	for(unsigned long i=0; i < hed.unk2; ++i) {
		if( i<hed.unk2-1){
			FileList[i]->complen = FileList[i + 1]->offset - FileList[i]->offset;
			if(FileList[i]->complen <= 0) {
				return FALSE;
			}
		}else{
			FileList[i]->complen = (dwFileSize - hed.dir_compsize) - FileList[i]->offset;
			if(FileList[i]->complen <= 0) {
				return FALSE;
			}
		}
	}

	m_strLoadDataFile = lpcwszFile;

	return TRUE;
}

BOOL CTHXReadUtil::LoadBgmData(vector<CTHXReadUtil::BGM_LIST>* pBgmList)
{
	if( pBgmList == NULL ){
		return FALSE;
	}
	map<string, th_file_entry*>::iterator itr;

	if( m_dwFileVer >= VER_TH7 ){
		itr = FileMap.find("thbgm.fmt");
		if( itr == FileMap.end() ){
			return FALSE;
		}
		BYTE* pbOutBuff = new BYTE[itr->second->origlen];
		DWORD dwSize = itr->second->origlen;
		if( th_extract_file(itr->second, pbOutBuff, &dwSize ) == FALSE ){
			delete[] pbOutBuff;
			return FALSE;
		}
		LoadFmt(pbOutBuff, dwSize, pBgmList);
		delete[] pbOutBuff;

		itr = FileMap.find("musiccmt.txt");
		if( itr == FileMap.end() ){
			return FALSE;
		}
		pbOutBuff = new BYTE[itr->second->origlen+1];
		ZeroMemory(pbOutBuff, itr->second->origlen+1);
		dwSize = itr->second->origlen;
		if( th_extract_file(itr->second, pbOutBuff, &dwSize ) == FALSE ){
			delete[] pbOutBuff;
			return FALSE;
		}
		LoadMusiccmt(pbOutBuff, dwSize, pBgmList);
		delete[] pbOutBuff;
	}else{
		BYTE* pbOutBuff=NULL;
		for( int i=1; i<=17; i++ ){
			char szBuff[32]="";
			sprintf_s(szBuff,"th06_%02d.pos",i);

			itr = FileMap.find(szBuff);
			if( itr == FileMap.end() ){
				return FALSE;
			}
			pbOutBuff = new BYTE[itr->second->origlen];
			DWORD dwSize = itr->second->origlen;
			if( th_extract_file(itr->second, pbOutBuff, &dwSize ) == FALSE ){
				delete[] pbOutBuff;
				return FALSE;
			}
			DWORD dwWrite;
			CStringA str;
			str.Format("c:\\%s",szBuff);
			HANDLE hFile = CreateFileA( str.GetBuffer(0), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			WriteFile(hFile,pbOutBuff,dwSize,&dwWrite,NULL);
			CloseHandle(hFile);
			LoadPos(pbOutBuff, dwSize, i, pBgmList);
			delete[] pbOutBuff;
		}
		itr = FileMap.find("musiccmt.txt");
		if( itr == FileMap.end() ){
			return FALSE;
		}
		pbOutBuff = new BYTE[itr->second->origlen+1];
		ZeroMemory(pbOutBuff, itr->second->origlen+1);
		DWORD dwSize = itr->second->origlen;
		if( th_extract_file(itr->second, pbOutBuff, &dwSize ) == FALSE ){
			delete[] pbOutBuff;
			return FALSE;
		}
		LoadMusiccmt(pbOutBuff, dwSize, pBgmList);
		delete[] pbOutBuff;

	}

	return TRUE;
}

void CTHXReadUtil::th_crypter(unsigned char* in, unsigned char *out, int size, unsigned char key, unsigned char step, int block, int limit)
{
	unsigned char *i_itr = in;
	unsigned char *o_itr = out;
	unsigned char *o_base;
	int addup;
	int i;

	addup = size % block;
	if(addup >= block / 4) addup = 0;
	addup += size % 2;
	size -= addup;

	while(size > 0 && limit > 0) {
		if(size < block) block = size;
		/* first phase */
		o_base = o_itr;
		o_itr = o_itr + block - 1;
		for(i = (block + 1) / 2; i > 0; --i, ++i_itr) {
			/* decode char */
			*o_itr = *i_itr ^ key;
			o_itr -= 2;
			/* update key */
			key += step;
		}
		/* second phase */
		o_itr = o_base + block - 2;
		for(i = block / 2; i > 0; --i, ++i_itr) {
			/* decode char */
			*o_itr = *i_itr ^ key;
			o_itr -= 2;
			/* update key */
			key += step;
		}
		/* seek pointers */
		size -= block;
		o_itr = o_base + block;
		limit -= block;
	}
	/* transfer remaining data */
	size += addup;
	if(size > 0){
		memcpy(o_itr, i_itr, size);
	}
}

void CTHXReadUtil::th_encrypt(unsigned char* in, unsigned char *out, int size, unsigned char key, unsigned char step, int block, int limit)
{
	unsigned char *i_itr = in;
	unsigned char *o_itr = out;
	unsigned char *i_base;
	int addup;
	int i;

	int iInSize = size;
	addup = size % block;
	if(addup >= block / 4){
		addup = 0;
	}
	addup += size % 2;
	size -= addup;

	while(size > 0 && limit > 0) {
		if(size < block) block = size;
		/* first phase */
		i_base = i_itr;
		i_itr = i_itr + block - 1;
		for(i = (block + 1) / 2; i > 0; --i, ++o_itr) {
			/* decode char */
			*o_itr = *i_itr ^ key;
			i_itr -= 2;
			/* update key */
			key += step;
		}
		/* second phase */
		i_itr = i_base + block - 2;
		for(i = block / 2; i > 0; --i, ++o_itr) {
			/* decode char */
			*o_itr = *i_itr ^ key;
			i_itr -= 2;
			/* update key */
			key += step;
		}
		/* seek pointers */
		size -= block;
		i_itr = i_base + block;
		limit -= block;
	}
	/* transfer remaining data */
	size += addup;
	if(size > 0){
		memcpy(o_itr, in+(iInSize-size), size);
	}
}

int CTHXReadUtil::th_get_cryprm_index_10(char *name)
{
	char index = 0;
	while(*name) index += *name++;

	return index & 7;
}

int CTHXReadUtil::th_get_cryprm_index_9(BYTE bType)
{
	int iRet = -1;
	for(int i = 0; i < CRYPRM_MAX; ++i){
		if( m_dwFileVer == VER_TH9 ){
			if( g_cryprm_09[i].type == bType ){
				iRet = i;
			}
		}else if( m_dwFileVer == VER_TH8 ){
			if( g_cryprm_08[i].type == bType ){
				iRet = i;
			}
		}
	}
	return iRet;
}

void CTHXReadUtil::bitstrm_init(bitstrm *strm, unsigned char *buf, int size)
{
	strm->b = 0x80;
	strm->c = 0;
	strm->cur = strm->base = buf;
	strm->size = size;
}

int CTHXReadUtil::bitstrm_read(bitstrm *strm, unsigned int bits)
{
	int ret = 0;
	unsigned int i;

	for(i = 1 << (bits - 1); i > 0; i >>= 1) {
		if(strm->b == 0x80) {
			if(strm->cur - strm->base < strm->size)
				strm->c = *strm->cur++;
			else
				strm->c = 0;
		}
		if(strm->b & strm->c){
			ret |= i;
		}
		strm->b >>= 1;
		if (!strm->b){
			strm->b = 0x80;
		}
	}
	return ret;
}

int CTHXReadUtil::bitstrm_read_th06_int(bitstrm *strm)
{
	int ret = 0;
	unsigned int size = (unsigned int)bitstrm_read(strm, 2) +1;
	ret = bitstrm_read(strm, size*8);
	return ret;
}

void CTHXReadUtil::bitstrm_read_th06_string(bitstrm *strm, string* str)
{
	char c;
	do{
		c=(char)bitstrm_read(strm, 8);
		if(c!=0)
			*str+=c;
	}while(c != 0 );
}

void CTHXReadUtil::bitstrm_write(struct bitstrm *strm, int val,unsigned int bits)
{
	int ret = 0;
	unsigned int i;

	for(i = 1 << (bits - 1); i > 0; i >>= 1) {
		if( i & val ){
			*strm->cur |= strm->b;
		}else{
			BYTE a=(0xFF^strm->b);
			*strm->cur &= a;
		}
		strm->b >>= 1;
		if (!strm->b){
			strm->b = 0x80;
			strm->cur++;
		}
	}
}

void CTHXReadUtil::bitstrm_write_th06_int(bitstrm *strm, int val)
{
	int byte = 0;
	if(val/0xF>0){
		byte++;
	}
	if(val/0xFF>0){
		byte++;
	}
	if(val/0xFFF>0){
		byte++;
	}
	bitstrm_write(strm, byte, 2);
	bitstrm_write(strm, val, 8*(byte+1));
}

void CTHXReadUtil::bitstrm_write_th06_string(bitstrm *strm, string str)
{
	for( int i=0; i<(int)str.length(); i++ ){
		int a=(int)str.c_str()[i];
		bitstrm_write(strm, a, 8);
	}
	bitstrm_write(strm, 0, 8);
}

int CTHXReadUtil::th_extract_file(th_file_entry *entry, BYTE* pbOut, DWORD* dwOutSize)
{
	BYTE* pbRead = NULL;

	HANDLE hFile = CreateFileW( m_strLoadDataFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ){
		return FALSE;
	}
	SetFilePointer(hFile,entry->offset,NULL,FILE_BEGIN);
	DWORD dwRead = 0;

	pbRead = new BYTE[entry->complen];

	if( ReadFile(hFile, pbRead, entry->complen, &dwRead, NULL ) == FALSE ){
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);

	BYTE* pbUnComp = NULL;
	int iUnCompSize = 0;
	pbUnComp = new BYTE[entry->origlen];
	if( m_dwFileVer == VER_TH10 ){
		int i = th_get_cryprm_index_10((char *)entry->name.c_str());
		BYTE* pbComp = NULL;
		pbComp = new BYTE[entry->complen];
		th_crypter(pbRead, pbComp, entry->complen, g_cryprm_10[i].key, g_cryprm_10[i].step, g_cryprm_10[i].block, g_cryprm_10[i].limit);

		if( entry->complen != entry->origlen ){
			lzss_decompress(pbComp, entry->complen, pbUnComp, entry->origlen);
		}else{
			memcpy(pbUnComp,pbComp,entry->origlen);
		}
		delete[] pbComp;
		iUnCompSize = entry->origlen;
	}else{
		if( m_dwFileVer == VER_TH6 || m_dwFileVer == VER_TH7 ){
			lzss_decompress(pbRead, entry->complen, pbUnComp, entry->origlen);
			iUnCompSize = entry->origlen;
		}else{
			BYTE* pbComp = NULL;
			pbComp = new BYTE[entry->origlen];
			lzss_decompress(pbRead, entry->complen, pbComp, entry->origlen);
			int i = th_get_cryprm_index_9(pbComp[3]);
			if( m_dwFileVer == VER_TH8 ){
				th_crypter(pbComp+4, pbUnComp, entry->origlen - 4, g_cryprm_08[i].key, g_cryprm_08[i].step, g_cryprm_08[i].block, g_cryprm_08[i].limit);
			}else{
				th_crypter(pbComp+4, pbUnComp, entry->origlen - 4, g_cryprm_09[i].key, g_cryprm_09[i].step, g_cryprm_09[i].block, g_cryprm_09[i].limit);
			}
			delete[] pbComp;
			iUnCompSize = entry->origlen-4;
		}
	}

	if( pbOut != NULL ){
		if( *dwOutSize >=  (DWORD)iUnCompSize ){
			memcpy( pbOut, pbUnComp, iUnCompSize );
		}
	}
	*dwOutSize = iUnCompSize;

	delete[] pbUnComp;
	delete[] pbRead;

	return TRUE;
}

BOOL CTHXReadUtil::LoadFmt(BYTE* pbBuff, DWORD dwSize, vector<CTHXReadUtil::BGM_LIST>* pBgmList)
{
	DWORD dwRead= 0;
	while(dwRead<dwSize){
		BGM_LIST Item;
		char* pName = _strdup((const char *)pbBuff + dwRead);
		Item.strFileName = pName;
		free(pName);
		if( Item.strFileName.empty() == true ){
			break;
		}
		dwRead+=16;
		Item.uiDefOffset = *(unsigned int*)&pbBuff[dwRead];
		dwRead+=4;
		dwRead+=4;
		Item.uiDefLoop = *(unsigned int*)&pbBuff[dwRead];
		dwRead+=4;
		Item.uiDefEnd = *(unsigned int*)&pbBuff[dwRead];
		dwRead+=4;
		dwRead+=20;
/*
		Item.uiLoop = (Item.uiDefLoop/(44100*4))*100;
		Item.uiLoop += ((Item.uiDefLoop%(44100*4))/(441*4));
		Item.uiEnd = (Item.uiDefEnd/(44100*4))*100;
		Item.uiEnd += ((Item.uiDefEnd%(44100*4))/(441*4));
		Item.uiOffset = Item.uiDefOffset;
*/
		Item.uiLoop = Item.uiDefLoop;
		Item.uiEnd = Item.uiDefEnd;
		Item.uiOffset = Item.uiDefOffset;
		pBgmList->push_back(Item);
	}
	return TRUE;
}

BOOL CTHXReadUtil::LoadPos(BYTE* pbBuff, DWORD dwSize, int iIndex, vector<CTHXReadUtil::BGM_LIST>* pBgmList)
{
	if( dwSize<8 ){
		return FALSE;
	}

	char szBuff[32]="";
	sprintf_s(szBuff, "th06_%02d.wav", iIndex);

	BGM_LIST Item;
	Item.strFileName = szBuff;
	Item.uiDefOffset = 0x2C;
	Item.uiDefLoop = (*(unsigned int*)&pbBuff[0])*4 - 0x2c;
	Item.uiDefEnd = (*(unsigned int*)&pbBuff[4])*4 - 0x2c;
/*
	Item.uiLoop = (Item.uiDefLoop/(44100*4))*100;
	Item.uiLoop += ((Item.uiDefLoop%(44100*4))/(441*4));
	Item.uiEnd = (Item.uiDefEnd/(44100*4))*100;
	Item.uiEnd += ((Item.uiDefEnd%(44100*4))/(441*4));
	Item.uiOffset = Item.uiDefOffset;
*/
	Item.uiLoop = Item.uiDefLoop;
	Item.uiEnd = Item.uiDefEnd;
	Item.uiOffset = Item.uiDefOffset;
	pBgmList->push_back(Item);

	return TRUE;
}

BOOL CTHXReadUtil::LoadMusiccmt(BYTE* pbBuff, DWORD dwSize, vector<CTHXReadUtil::BGM_LIST>* pBgmList)
{
	string strBuff;
	strBuff = (char*)pbBuff;
	int pos = 0;

	do{
		pos = (int)strBuff.find("@bgm/",pos);
		if( pos != string::npos ){
			int end = (int)strBuff.find(".",pos);
			string strFile="";
			strFile.insert(0, strBuff.c_str()+pos+strlen("@bgm/"), end-pos-strlen("@bgm/"));
			strFile+=".wav";
			string strName="";
			int NextLineS = (int)strBuff.find("\r\n",end)+(int)strlen("\r\n");
			int NextLineE = (int)strBuff.find("\r\n",NextLineS);
			strName.insert(0, strBuff.c_str()+NextLineS, NextLineE-NextLineS);
			for( int i=0; i<(int)pBgmList->size(); i++ ){
				if( (*pBgmList)[i].strFileName.compare(strFile) == 0 ){
					(*pBgmList)[i].strBgmName = strName;
					break;
				}
			}
			pos = NextLineE;
		}
	}while(pos != string::npos);
	return TRUE;
}

BOOL CTHXReadUtil::SaveDataFile(LPCWSTR lpcwszFile, vector<CTHXReadUtil::BGM_LIST>* pBgmList )
{
	map<string, th_file_entry*>::iterator itr;
	if( m_dwFileVer >= VER_TH7 ){
		itr = FileMap.find("thbgm.fmt");
		if( itr == FileMap.end() ){
			return FALSE;
		}
		BYTE* pbOutBuff = new BYTE[itr->second->origlen];
		DWORD dwSize = itr->second->origlen;
		if( th_extract_file(itr->second, pbOutBuff, &dwSize ) == FALSE ){
			delete[] pbOutBuff;
			return FALSE;
		}

		ChangeFmt( pBgmList, pbOutBuff, dwSize );

		DWORD dwArchSize = dwSize+4;
		BYTE* pbArch = new BYTE[dwArchSize+4];
		th_archive_file("thbgm.fmt", pbOutBuff, dwSize, pbArch, &dwArchSize);
		if( dwArchSize >dwSize ){
			delete[] pbArch;
			pbArch = new BYTE[dwArchSize+4];
			th_archive_file("thbgm.fmt", pbOutBuff, dwSize, pbArch, &dwArchSize);
		}

		delete[] pbOutBuff;

		HANDLE hFile = CreateFileW( m_strLoadDataFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ){
			delete[] pbArch;
			return FALSE;
		}
		HANDLE hFile2 = CreateFileW( lpcwszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile2 == INVALID_HANDLE_VALUE ){
			delete[] pbArch;
			CloseHandle(hFile);
			return FALSE;
		}

		DWORD dwWrite =0;
		DWORD dwWritePos =0;

		struct {
			unsigned long magic;
			unsigned long ul1;
			unsigned long ul2;
			unsigned long ul3;
		} hed;
		WriteFile(hFile2,(unsigned char*)&hed, 16, &dwWrite, NULL );
		dwWritePos+=16;

		for( int i=0; i<(int)FileList.size(); i++ ){
			if( FileList[i]->name.compare("thbgm.fmt") == 0 ){
				FileList[i]->offset = dwWritePos;
				FileList[i]->complen = dwArchSize;
				FileList[i]->origlen = dwSize;
				if( m_dwFileVer == VER_TH8 || m_dwFileVer == VER_TH9 ){
					FileList[i]->origlen += 4;
				}

				WriteFile(hFile2,pbArch, dwArchSize, &dwWrite, NULL );
				dwWritePos+=dwArchSize;
			}else{
				SetFilePointer(hFile, FileList[i]->offset, NULL, FILE_BEGIN );
				DWORD dwRead;
				BYTE* pbReadBuff = new BYTE[FileList[i]->complen];
				ReadFile( hFile, pbReadBuff, FileList[i]->complen, &dwRead, NULL );

				FileList[i]->offset = dwWritePos;
				WriteFile(hFile2,pbReadBuff, FileList[i]->complen, &dwWrite, NULL );
				dwWritePos+=FileList[i]->complen;
				delete[] pbReadBuff;
			}
		}

		delete[] pbArch;

		DWORD dwFileCompSize = 0;
		DWORD dwFileOriSize = 0;
		CreateFileIndex(NULL,&dwFileCompSize,&dwFileOriSize);
		BYTE* pbFileIndex = new BYTE[dwFileCompSize];
		CreateFileIndex(pbFileIndex,&dwFileCompSize,&dwFileOriSize);
		WriteFile(hFile2,pbFileIndex,dwFileCompSize,&dwWrite,NULL);

		if( m_dwFileVer == VER_TH10 ){
			hed.magic = '1AHT';
			hed.ul1 = dwFileOriSize + 123456789;
			hed.ul2 = dwFileCompSize + 987654321;
			hed.ul3 = ((unsigned long)FileList.size()) - 0xF7E7F8AC;

			BYTE bHead[16];
			th_encrypt((unsigned char*)&hed, bHead, 0x10, 0x1B, 0x37, 0x10, 0x10);

			SetFilePointer(hFile2, 0, NULL, FILE_BEGIN);
			WriteFile(hFile2,bHead,16,&dwWrite,NULL);

		}else if( m_dwFileVer == VER_TH7 ){
			hed.magic = '4GBP';
			hed.ul1 = ((unsigned long)FileList.size());
			hed.ul2 = dwWritePos;
			hed.ul3 = dwFileOriSize;

			SetFilePointer(hFile2, 0, NULL, FILE_BEGIN);
			WriteFile(hFile2,(unsigned char*)&hed,16,&dwWrite,NULL);
		}else{
			hed.magic = 'ZGBP';
			hed.ul1 = ((unsigned long)FileList.size()) + 123456;
			hed.ul2 = dwWritePos + 345678;
			hed.ul3 = dwFileOriSize + 567891;

			BYTE bHead[12];
			th_encrypt((unsigned char*)&hed+4, bHead, 0x0C, 0x1B, 0x37, 0x0C, 0x400);

			SetFilePointer(hFile2, 0, NULL, FILE_BEGIN);
			WriteFile(hFile2,(unsigned char*)&hed,4,&dwWrite,NULL);
			WriteFile(hFile2,bHead,12,&dwWrite,NULL);
		}

		CloseHandle(hFile);
		CloseHandle(hFile2);

		delete[] pbFileIndex;
	}else{
		HANDLE hFile = CreateFileW( m_strLoadDataFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ){
			return FALSE;
		}
		HANDLE hFile2 = CreateFileW( lpcwszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile2 == INVALID_HANDLE_VALUE ){
			CloseHandle(hFile);
			return FALSE;
		}

		DWORD dwWrite =0;
		DWORD dwRead = 0;
		DWORD dwWritePos =0;

		BYTE bReadHead[13];
		DWORD dwReadFileSize = GetFileSize(hFile,NULL);
		ReadFile( hFile, &bReadHead, 13, &dwRead, NULL );

		BYTE bHead[13];
		ZeroMemory(bHead,13);
		bHead[0]='P';
		bHead[1]='B';
		bHead[2]='G';
		bHead[3]='3';
		WriteFile(hFile2,bHead, 13, &dwWrite, NULL );
		dwWritePos+=13;

		for( int i=0; i<(int)FileList.size(); i++ ){
			if( FileList[i]->name.find(".pos",0) == string::npos ){
				SetFilePointer(hFile, FileList[i]->offset, NULL, FILE_BEGIN );

				BYTE* pbReadBuff = new BYTE[FileList[i]->complen];
				ReadFile( hFile, pbReadBuff, FileList[i]->complen, &dwRead, NULL );

				FileList[i]->offset = dwWritePos;
				WriteFile(hFile2,pbReadBuff, FileList[i]->complen, &dwWrite, NULL );
				dwWritePos+=FileList[i]->complen;
				delete[] pbReadBuff;
			}else{
				string str = FileList[i]->name;
				int pos = (int)str.find(".pos",0);
				str.replace(pos,4,".wav");

				for( int j=0; j<(int)pBgmList->size(); j++ ){
					if( (*pBgmList)[j].strFileName.compare(str) == 0 ){
						struct {
							unsigned long loop;
							unsigned long end;
						} info;
						if( (*pBgmList)[j].strWaveFile.empty() == true ){
							int k = (*pBgmList)[j].uiDefLoop+0x2C;
							info.loop = k/4;
							k = (*pBgmList)[j].uiDefEnd+0x2C;
							info.end = k/4;
							DWORD dwArchSize = 16;
							BYTE* pbArch = new BYTE[16];
							ZeroMemory(pbArch, 16);
							th_archive_file(FileList[i]->name.c_str(), (BYTE*)&info, 8, pbArch, &dwArchSize);
							if( dwArchSize >16 ){
								delete[] pbArch;
								pbArch = new BYTE[dwArchSize+4];
								th_archive_file(FileList[i]->name.c_str(), (BYTE*)&info, 8, pbArch, &dwArchSize);
							}
							FileList[i]->offset = dwWritePos;
							FileList[i]->complen = dwArchSize+2;
							WriteFile(hFile2,pbArch, dwArchSize+2, &dwWrite, NULL );
							dwWritePos+=FileList[i]->complen;
							delete[] pbArch;
							break;
						}else{
							int k = (*pBgmList)[j].uiLoop+0x2C;
							info.loop = k/4;
							k = (*pBgmList)[j].uiEnd+0x2C;
							info.end = k/4;
							DWORD dwArchSize = 16;
							BYTE* pbArch = new BYTE[16];
							ZeroMemory(pbArch, 16);
							th_archive_file(FileList[i]->name.c_str(), (BYTE*)&info, 8, pbArch, &dwArchSize);
							if( dwArchSize >16 ){
								delete[] pbArch;
								pbArch = new BYTE[dwArchSize+4];
								th_archive_file(FileList[i]->name.c_str(), (BYTE*)&info, 8, pbArch, &dwArchSize);
							}
							FileList[i]->offset = dwWritePos;
							FileList[i]->complen = dwArchSize+2;
							WriteFile(hFile2,pbArch, dwArchSize+2, &dwWrite, NULL );
							dwWritePos+=FileList[i]->complen;
							delete[] pbArch;
							break;
						}
					}
				}
			}
		}
		bitstrm bs;
		bitstrm_init(&bs, bReadHead+4, 9);
		int num = bitstrm_read_th06_int(&bs);
		int offset = bitstrm_read_th06_int(&bs);

		SetFilePointer(hFile, offset, NULL, FILE_BEGIN );
		DWORD dwListSize = dwReadFileSize-offset;
		BYTE* pbList = new BYTE[dwListSize];
		ReadFile(hFile,pbList,dwListSize,&dwRead,NULL);

		BYTE* pbWriteList = new BYTE[dwListSize*2];

		bitstrm bsR;
		bitstrm bsW;
		bitstrm_init(&bsR, pbList, dwListSize);
		bitstrm_init(&bsW, pbWriteList, dwListSize*2);
		for(int i=0; i<num; i++ ){
			int a = bitstrm_read_th06_int(&bsR);
			int b = bitstrm_read_th06_int(&bsR);
			int c = bitstrm_read_th06_int(&bsR);
			int off = bitstrm_read_th06_int(&bsR);
			int ori = bitstrm_read_th06_int(&bsR);
			string name;
			bitstrm_read_th06_string(&bsR, &name);

			itr = FileMap.find(name);
			if( itr == FileMap.end() ){
				return FALSE;
			}
			bitstrm_write_th06_int(&bsW,a);
			bitstrm_write_th06_int(&bsW,b);
			bitstrm_write_th06_int(&bsW,c);
			bitstrm_write_th06_int(&bsW,itr->second->offset);
			bitstrm_write_th06_int(&bsW,itr->second->origlen);
			bitstrm_write_th06_string(&bsW, name);
		}

		dwListSize = (DWORD)(bsW.cur - bsW.base);

		WriteFile(hFile2,pbWriteList, dwListSize, &dwWrite, NULL );

		delete[] pbList;
		delete[] pbWriteList;

		bitstrm_init(&bs, bHead+4, 9);
		bitstrm_write_th06_int(&bs,num);
		bitstrm_write_th06_int(&bs,dwWritePos);

		SetFilePointer(hFile2, 0, NULL, FILE_BEGIN);
		WriteFile(hFile2,bHead, 13, &dwWrite, NULL );

		CloseHandle(hFile);
		CloseHandle(hFile2);
	}
	return TRUE;
}

BOOL CTHXReadUtil::ChangeFmt(vector<CTHXReadUtil::BGM_LIST>* pBgmList, BYTE* pbBuff, DWORD dwSize)
{
	DWORD dwRead= 0;
	int j=0;
	for( int i=0; i<(int)pBgmList->size(); i++ ){
		if( (*pBgmList)[i].strWaveFile.empty() == true ){
			dwRead+=16;
			j = (*pBgmList)[i].uiOffset;
			memcpy(pbBuff+dwRead, &j, sizeof(int) );
			dwRead+=4;
			dwRead+=4;
			j = (*pBgmList)[i].uiDefLoop;
			memcpy(pbBuff+dwRead, &j, sizeof(int) );
			dwRead+=4;
			j = (*pBgmList)[i].uiDefEnd;
			memcpy(pbBuff+dwRead, &j, sizeof(int) );
			dwRead+=4;
			dwRead+=20;
		}else{
			dwRead+=16;
			j = (*pBgmList)[i].uiOffset;
			memcpy(pbBuff+dwRead, &j, sizeof(int) );
			dwRead+=4;
			dwRead+=4;
			j = (*pBgmList)[i].uiLoop;
			memcpy(pbBuff+dwRead, &j, sizeof(int) );
			dwRead+=4;
			j = (*pBgmList)[i].uiEnd;
			memcpy(pbBuff+dwRead, &j, sizeof(int) );
			dwRead+=4;
			dwRead+=20;
		}
	}
	return TRUE;
}

BOOL CTHXReadUtil::th_archive_file(LPCSTR lpcszFileName, BYTE* pbIn, DWORD dwInSize, BYTE* pbOut, DWORD* dwOutSize)
{
	if( lpcszFileName == NULL || dwOutSize == NULL || pbIn == NULL ){
		return FALSE;
	}
	if( m_dwFileVer == VER_TH10 ){
		int iCompSize = 0;
		auto_ptr<vector<unsigned char> > res = lzss_compress(pbIn, dwInSize);

		BYTE* pbComp = NULL;
		if( (int)res->size() > dwInSize ){
			iCompSize = dwInSize;
			pbComp = new BYTE[iCompSize];
			memcpy( pbComp, pbIn, iCompSize );
		}else{
			iCompSize = (int)res->size();
			pbComp = new BYTE[iCompSize];
			memcpy( pbComp, &(*res)[0], iCompSize );
		}

		BYTE* pbEnc = new BYTE[iCompSize];
		int j = th_get_cryprm_index_10((char*)lpcszFileName);
		th_encrypt(pbComp, pbEnc, iCompSize, g_cryprm_10[j].key, g_cryprm_10[j].step, g_cryprm_10[j].block, g_cryprm_10[j].limit);

		if( pbOut != NULL && *dwOutSize>=(DWORD)iCompSize ){
			memcpy( pbOut, pbEnc, iCompSize );
		}
		*dwOutSize = iCompSize;

		delete[] pbEnc;
		delete[] pbComp;
	}else{
		if( m_dwFileVer == VER_TH6 || m_dwFileVer == VER_TH7 ){
			int iCompSize = 0;
			auto_ptr<vector<unsigned char> > res = lzss_compress(pbIn, dwInSize);

			BYTE* pbComp = NULL;
			iCompSize = (int)res->size();
			pbComp = new BYTE[iCompSize];
			memcpy( pbComp, &(*res)[0], iCompSize );

			if( pbOut != NULL && *dwOutSize>=(DWORD)iCompSize ){
				memcpy( pbOut, pbComp, iCompSize );
			}
			*dwOutSize = iCompSize;
			delete[] pbComp;
		}else{
			BYTE* pbComp = NULL;
			BYTE* pbEnc = NULL;
			char* p = NULL;
			BYTE bType = 0;
			for( int i=0; i<CRYPRM2_MAX; i++ ){
				p = (char*)strstr(lpcszFileName, g_cryprm2_9[i].strEx.c_str() );
				if( p != NULL ){
					bType = g_cryprm2_9[i].type;
					break;
				}
			}
			if( p==NULL ){
				return FALSE;
			}
			int j = th_get_cryprm_index_9(bType);

			pbEnc = new BYTE[dwInSize+4];
			pbEnc[0]='e';
			pbEnc[1]='d';
			pbEnc[2]='z';
			pbEnc[3]=bType;
			if( m_dwFileVer == VER_TH8 ){
				th_encrypt(pbIn, pbEnc+4, dwInSize, g_cryprm_08[j].key, g_cryprm_08[j].step, g_cryprm_08[j].block, g_cryprm_08[j].limit);
			}else{
				th_encrypt(pbIn, pbEnc+4, dwInSize, g_cryprm_09[j].key, g_cryprm_09[j].step, g_cryprm_09[j].block, g_cryprm_09[j].limit);
			}

			int iCompSize = 0;
			auto_ptr<vector<unsigned char> > res = lzss_compress(pbEnc, dwInSize+4);

			iCompSize = (int)res->size();
			pbComp = new BYTE[iCompSize];
			memcpy( pbComp, &(*res)[0], iCompSize );

			if( pbOut != NULL && *dwOutSize>=(DWORD)iCompSize ){
				memcpy( pbOut, pbComp, iCompSize );
			}
			*dwOutSize = iCompSize;
			delete[] pbComp;
			delete[] pbEnc;
		}
	}
	return TRUE;
}

BOOL CTHXReadUtil::CreateFileIndex(BYTE* pbOut, DWORD* dwOutSize, DWORD* dwOriSize)
{
	DWORD dwFileListSize = 0;
	for( int i=0; i<(int)FileList.size(); i++ ){
		if( m_dwFileVer == VER_TH10 ){
			dwFileListSize+=(((DWORD)FileList[i]->name.length()+1)/4)*4;
			if( ((DWORD)FileList[i]->name.length()+1)%4 != 0 ){
				dwFileListSize+=4;
			}
			dwFileListSize+=sizeof(int)*3;
		}else if( m_dwFileVer == VER_TH6 ){
		}else{
			dwFileListSize+=(DWORD)FileList[i]->name.length()+1;
			dwFileListSize+=sizeof(int)*3;
		}
	}

	*dwOriSize = dwFileListSize;
	BYTE* pbFileList = new BYTE[dwFileListSize];
	ZeroMemory(pbFileList,dwFileListSize);
	DWORD dwPos=0;
	for( int i=0; i<(int)FileList.size(); i++ ){
		if( m_dwFileVer == VER_TH10 ){
			memcpy( pbFileList+dwPos, FileList[i]->name.c_str(), FileList[i]->name.length() );
			dwPos+=(((DWORD)FileList[i]->name.length()+1)/4)*4;
			if( ((DWORD)FileList[i]->name.length()+1)%4 != 0 ){
				dwPos+=4;
			}
			memcpy( pbFileList+dwPos, &FileList[i]->offset, sizeof(int) );
			memcpy( pbFileList+dwPos+4, &FileList[i]->origlen, sizeof(int) );
			dwPos+=sizeof(int)*3;
		}else if( m_dwFileVer == VER_TH6 ){
		}else{
			memcpy( pbFileList+dwPos, FileList[i]->name.c_str(), FileList[i]->name.length() );
			dwPos+=(DWORD)FileList[i]->name.length()+1;
			memcpy( pbFileList+dwPos, &FileList[i]->offset, sizeof(int) );
			memcpy( pbFileList+dwPos+4, &FileList[i]->origlen, sizeof(int) );
			dwPos+=sizeof(int)*3;
		}
	}

	if( m_dwFileVer >= VER_TH7 ){
		int iCompSize = 0;
		auto_ptr<vector<unsigned char> > res = lzss_compress(pbFileList, dwFileListSize);

		BYTE* pbComp = NULL;
		if( (int)res->size() > dwFileListSize ){
			iCompSize = dwFileListSize;
			pbComp = new BYTE[iCompSize];
			memcpy( pbComp, pbFileList, iCompSize );
		}else{
			iCompSize = (int)res->size();
			pbComp = new BYTE[iCompSize];
			memcpy( pbComp, &(*res)[0], iCompSize );
		}
		
		if( pbOut == NULL || *dwOutSize < (DWORD)iCompSize){
			*dwOutSize = (DWORD)iCompSize;
			delete[] pbComp;
			delete[] pbFileList;
			return TRUE;
		}
		*dwOutSize = (DWORD)iCompSize;

		if( m_dwFileVer >= VER_TH10 ){
			th_encrypt(pbComp, pbOut, iCompSize, 0x3E, 0x9B, 0x80, dwFileListSize);
		}else if( m_dwFileVer >= VER_TH8 ){
			th_encrypt(pbComp, pbOut, iCompSize, 0x3E, 0x9B, 0x80, 0x400);
		}else{
			memcpy(pbOut,pbComp,iCompSize);
		}
		delete[] pbComp;
	}else{
	}

	delete[] pbFileList;

	return TRUE;
}
