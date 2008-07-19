//
// File.h
//

#ifndef _FILE_H_
#define _FILE_H_

class CBufferedFile {
public:
	CBufferedFile(DWORD bufferSize = 8192);
	virtual ~CBufferedFile();

	HANDLE GetFileHandle() { return File; }

	virtual BOOL Create(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDispostion, DWORD dwFlagsAndAttributes);
	virtual BOOL Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD *nBytesRead = NULL);
	virtual BOOL Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, DWORD *nNumberOfBytesWritten = NULL);
	virtual BOOL Close();
	virtual DWORD Seek(LONG lDistanceToMove, DWORD dwMoveMethod);

	void Lock();
	void Unlock();

protected:
	HANDLE File;

	BOOL EndOfFile;
	BYTE *Buffer;
	DWORD BufferSize;						// size of the whole buffer
	DWORD BufferPos;						// internal pointer to the buffer (read/write)
	DWORD BufferFilled;						// number of valid bytes in the buffer (read)
	DWORD BufferStart;						// position within the file where the buffer starts

	DWORD Access;							// desired access to the file

	BOOL EmptyCache();

	CRITICAL_SECTION CS;
};

#endif
