#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const char * readFileToString(const char * filePath, int *filePathDataSize)
{
	int bufferSize = 1024;
	char readBuffer[bufferSize];
	char * data = NULL;
	int dataSize = 0;
	int readCount = -1;
	int dataIdx = 0;
	int i;

	int fd = open(filePath, O_RDONLY);
	if(fd == -1)
	{
		perror("open");
		return NULL;
	}
	readCount = read(fd, readBuffer, bufferSize * sizeof(char));
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
		readCount = read(fd, readBuffer, bufferSize);
	}
	close(fd);
	*filePathDataSize = dataSize;
	
	return data;
}

int main(void)
{
	const char * tempDir;
	char tempFilePath[PATH_MAX + 1];
	int fd;
	ssize_t pfuncResult;
	const char *fileData;
	int fileDataSize;
	int i;
	int writeRes;
	int closeRes;
	char readBuffer[10] = {0};

	tempDir = getenv("TMPDIR");

	if(tempDir == NULL) {
		tempDir = getenv("TEMP");

		if(tempDir == NULL)
		{
			tempDir = getenv("TMP");

			if(tempDir == NULL)
			{
				tempDir = "/tmp";
			}
		}
	}

	sprintf(tempFilePath, "%s/writepastend.txt", tempDir);
	//sprintf(tempFilePath, "%s/writepastend.txt", ".");
	fprintf(stderr, "%s\n", tempFilePath);

	// write past end
	fd = open(tempFilePath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	write(fd, "abc123\n", strlen("abc123\n"));
	close(fd);


	fd = open(tempFilePath, O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("open");
	}
	pfuncResult = pwrite(fd, "def456\n", strlen("def456\n"), 9);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	close(fd);


	fileData = readFileToString(tempFilePath, &fileDataSize);
	for(i = 0; i < fileDataSize; i++)
	{
		if(isprint(fileData[i]) || isspace(fileData[i]))
		{
			printf("%c", fileData[i]);
		} else {
			printf("[0x%02hx]", ((unsigned short)fileData[i] & 0xFF));
		}
		if(i > 0 && i % 16 == 0)
		{
			printf("\n");
		}

	}
	printf("\n\n");

	// write in middle
	fd = open(tempFilePath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
		perror("open");
	writeRes = write(fd, "abc123\ndef123\nghi123\n", strlen("abc123\ndef123\nghi123\n"));
	if(writeRes == -1)
		perror("write");
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");


	fd = open(tempFilePath, O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("open");
	}
	pfuncResult = pwrite(fd, "jkl567\n", 7, 7);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");

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
	fd = open(tempFilePath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
		perror("open");
	writeRes = write(fd, "abc123\ndef123\nghi123\n", strlen("abc123\ndef123\nghi123\n"));
	if(writeRes == -1)
		perror("write");
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");


	fd = open(tempFilePath, O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("open");
	}
	pfuncResult = pwrite(fd, "jkl567\n", 7, 7);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");

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
	fd = open(tempFilePath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
		perror("open");
	writeRes = write(fd, "abc123\ndef123\nghi123\n", strlen("abc123\ndef123\nghi123\n"));
	if(writeRes == -1)
		perror("write");
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");

	fd = open(tempFilePath, O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("open");
	}
	pfuncResult = pwrite(fd, "jkl567\n", 7, 7);
	if(pfuncResult == -1)
	{
		perror("pwrite");
	}
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");

	fd = open(tempFilePath, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1)
	{
		perror("open");
	}
	pfuncResult = pread(fd, readBuffer, 7, 7);
	if(pfuncResult == -1)
	{
		perror("pread");
	}
	closeRes = close(fd);
	if(closeRes == -1)
		perror("close");

	printf("\"%s\"\n", readBuffer);
	printf("\n\n");

	return 0;
}

