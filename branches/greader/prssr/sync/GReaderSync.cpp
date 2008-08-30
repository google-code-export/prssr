// GReaderSync.cpp: implementation of the CGReaderSync class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "GReaderSync.h"
#include "../net/download.h"
#include "../Config.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGReaderSync::CGReaderSync(CDownloader *downloader) : CFeedSyncer(downloader)
{

}

CGReaderSync::~CGReaderSync() {

}

BOOL CGReaderSync::Authenticate(const CString &userName, const CString &password) {
	UserName = userName;
	Password = password;
	SID.Empty();

	return GetAuthToken();	
}

BOOL CGReaderSync::GetAuthToken() {
	SID.Empty();

	if (Downloader != NULL) {
		TCHAR tempFileName[MAX_PATH];
		GetTempFileName(Config.CacheLocation, L"rsr", 0, tempFileName);

		CString url;
		url.Format(_T("https://www.google.com/accounts/ClientLogin?service=reader&Email=%s&Passwd=%s"), UserName, Password);

		if (Downloader->SaveHttpObject(url, tempFileName)) {
			CString response;
	
			HANDLE hFile = CreateFile(tempFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				// suppose the response is short, so that we can read it at once
				DWORD read;
				DWORD size = GetFileSize(hFile, NULL);
				char *buffer = new char [size + 1];
				ReadFile(hFile, buffer, size, &read, NULL);
				buffer[size] = '\0';
				CloseHandle(hFile);

				// process the response
				char seps[] = "\r\n";
				char *token = strtok(buffer, seps);
				while (token != NULL) {
					char *eqSign = strchr(token, '=');
					if (eqSign != NULL) {
						*eqSign = '\0';					// separate the key and its value
						if (strcmp(token, "SID") == 0)
							SID.Format(_T("%S"), eqSign + 1);
					}

					token = strtok(NULL, seps);
				}

				delete [] buffer;
			}

			return !SID.IsEmpty();
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}
