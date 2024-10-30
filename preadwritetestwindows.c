#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include "preadwrite.h"
#include <sys/stat.h>

void ErrorExit(LPCTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    wsprintf((LPTSTR)lpDisplayBuf,
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    _ftprintf(stderr, _T("%s\n"), (LPCTSTR)lpDisplayBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

const char * readFileToString(const char * filePath, int *filePathDataSize)
{
	int bufferSize = 1024;
	char readBuffer[bufferSize];
	char * data = NULL;
	int dataSize = 0;
	int readCount = -1;
	int dataIdx = 0;
	int i;

	int fd = _open(filePath, _O_RDONLY);
	if(fd == -1)
	{
		ErrorExit(_T("_open"));
		return NULL;
	}
	readCount = _read(fd, readBuffer, bufferSize * sizeof(char));
	while(readCount > 0)
	{
		if(data == NULL)
		{
			dataSize = readCount;
			data = (char *)malloc(sizeof (char) * dataSize);
		} else {
			dataSize += readCount;
			data = (char *)realloc(data, sizeof (char) * dataSize);
		}
		for(i = 0; i < readCount; i++, dataIdx++)
		{
			data[dataIdx] = readBuffer[i];
		}
		readCount = _read(fd, readBuffer, bufferSize);
	}
	if(readCount == -1)
		ErrorExit(_T("_read"));

	_close(fd);
	*filePathDataSize = dataSize;
	
	return data;
}

int main(void)
{
	DWORD status;
	LPTSTR tempDir;
	HANDLE hTempDir;
	LPTSTR tempFilePath;
	HANDLE hTempFilePath;
	int fd;
	int writeRes;
	int closeRes;
	int i;
	int pfuncResult;
	const char *fileData = NULL;
	int fileDataSize = 0;
	char readBuffer[10] = {0};

	hTempDir = LocalAlloc(LHND, sizeof (TCHAR) * (MAX_PATH + 1));

	if(hTempDir == 0)
	{
		ErrorExit(_T("LocalAlloc"));
	}
	
	tempDir = LocalLock(hTempDir);

	if(tempDir == NULL)
	{
		ErrorExit(_T("LocalLock"));
	}

	status = GetTempPath(MAX_PATH + 1, tempDir);

	if(status == 0)
	{
		ErrorExit(_T("GetTempPath"));
	}

	hTempFilePath = LocalAlloc(LHND, sizeof (TCHAR) * (MAX_PATH + 1));

	if(hTempFilePath == 0)
	{
		ErrorExit(_T("LocalAlloc"));
	}
	
	tempFilePath = LocalLock(hTempFilePath);

	if(tempFilePath == NULL)
	{
		ErrorExit(_T("LocalLock"));
	}

	lstrcpy(tempFilePath, tempDir);
	if(tempDir[lstrlen(tempDir) - 1] != '\\')
	{
		lstrcat(tempFilePath, _T("\\"));
	}

	lstrcat(tempFilePath, _T("pwrite.txt"));

	// write past end
	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY | _O_TRUNC | _O_CREAT, _S_IREAD | _S_IWRITE);
	if(fd == -1)
		ErrorExit(_T("_open"));
	writeRes = _write(fd, "abc123\n", strlen("abc123\n"));
	if(writeRes == -1)
		ErrorExit(_T("_write"));
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));


	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("_open");
	}
	pfuncResult = pwrite(fd, "def456\n", strlen("def456\n"), 9);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));

	fileData = readFileToString(tempFilePath, &fileDataSize);
	for(i = 0; i < fileDataSize; i++)
	{
		if(isprint(fileData[i]) || isspace(fileData[i]))
		{
			printf("%c", fileData[i]);
		} else {
			printf("[0x%02hx]", ((unsigned short)fileData[i] & 0xFF));
		}
	}
	printf("\n\n");

	// write in middle
	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY | _O_TRUNC | _O_CREAT, _S_IREAD | _S_IWRITE);
	if(fd == -1)
		ErrorExit(_T("_open"));
	writeRes = _write(fd, "abc123\ndef123\nghi123\n", strlen("abc123\ndef123\nghi123\n"));
	if(writeRes == -1)
		ErrorExit(_T("_write"));
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));


	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("_open");
	}
	pfuncResult = pwrite(fd, "jkl567\n", 7, 7);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));

	fileData = readFileToString(tempFilePath, &fileDataSize);
	for(i = 0; i < fileDataSize; i++)
	{
		if(isprint(fileData[i]) || isspace(fileData[i]))
		{
			printf("%c", fileData[i]);
		} else {
			printf("[0x%02hx]", ((unsigned short)fileData[i] & 0xFF));
		}
	}
	printf("\n\n");

	// write in appended file
	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY | _O_TRUNC | _O_CREAT, _S_IREAD | _S_IWRITE);
	if(fd == -1)
		ErrorExit(_T("_open"));
	writeRes = _write(fd, "abc123\ndef123\nghi123\n", strlen("abc123\ndef123\nghi123\n"));
	if(writeRes == -1)
		ErrorExit(_T("_write"));
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));


	fd = _open(tempFilePath, _O_WRONLY  | _O_BINARY | _O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("_open");
	}
	pfuncResult = pwrite(fd, "jkl567\n", 7, 7);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));

	fileData = readFileToString(tempFilePath, &fileDataSize);
	for(i = 0; i < fileDataSize; i++)
	{
		if(isprint(fileData[i]) || isspace(fileData[i]))
		{
			printf("%c", fileData[i]);
		} else {
			printf("[0x%02hx]", ((unsigned short)fileData[i] & 0xFF));
		}
	}
	printf("\n\n");

	// read in middle
	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY | _O_TRUNC | _O_CREAT, _S_IREAD | _S_IWRITE);
	if(fd == -1)
		ErrorExit(_T("_open"));
	writeRes = write(fd, "abc123\ndef123\nghi123\n", strlen("abc123\ndef123\nghi123\n"));
	if(writeRes == -1)
		ErrorExit(_T("write"));
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));


	fd = _open(tempFilePath, _O_WRONLY | _O_BINARY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		ErrorExit(_T("_open"));
	}
	pfuncResult = pwrite(fd, "jkl567\n", 7, 7);
	if(pfuncResult == -1)
	{
		ErrorExit(_T("pwrite"));
	}
	closeRes = _close(fd);
	if(closeRes == -1)
		ErrorExit(_T("_close"));

	fd = _open(tempFilePath, _O_RDONLY | _O_BINARY , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		ErrorExit(_T("open"));
	}
	pfuncResult = pread(fd, readBuffer, 7, 7);
	if(pfuncResult == -1)
	{
		ErrorExit(_T("pread"));
	}
	closeRes = close(fd);
	if(closeRes == -1)
		ErrorExit(_T("close"));

	printf("\"%s\"\n", readBuffer);
	printf("\n\n");

	return 0;
}

