#include <Windows.h>
#include <stdio.h>

void StartMonitor(wchar_t* dir)
{
	HANDLE hDir = INVALID_HANDLE_VALUE;
	BYTE *buffer = nullptr;
	DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_CREATION | \
		FILE_NOTIFY_CHANGE_DIR_NAME | \
		FILE_NOTIFY_CHANGE_FILE_NAME | \
		FILE_NOTIFY_CHANGE_LAST_WRITE | \
		FILE_NOTIFY_CHANGE_SIZE;
	static const DWORD RECORDS_BUFFER_SIZE_BYTES = 8 * 1024;
	DWORD dwBytesReturned;
	BOOL br;
	FILE_NOTIFY_INFORMATION* info = NULL;
	WCHAR filename[MAX_PATH];

	buffer = (BYTE*)malloc(RECORDS_BUFFER_SIZE_BYTES);
	if (buffer == NULL)
	{
		wprintf(L"malloc failed\n");
		goto Cleanup;
	}

	hDir = CreateFileW(dir, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hDir == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateFileW failed 0x%08x\n", GetLastError());
		goto Cleanup;
	}

	wprintf(L"started\n");

	while (1)
	{
		br = ReadDirectoryChangesW(hDir, buffer, RECORDS_BUFFER_SIZE_BYTES, TRUE, dwNotifyFilter, &dwBytesReturned, NULL, NULL);
		if (!br)
		{
			wprintf(L"ReadDirectoryChagnesW failed 0x%08x\n", GetLastError());
			goto Cleanup;
		}

		info = (FILE_NOTIFY_INFORMATION*)buffer;
		do {
			ZeroMemory(filename, sizeof(filename));
			CopyMemory(filename, info->FileName, info->FileNameLength);
			wprintf(L"NextEntryOffset=%d, Action=%d, FileName=%s\n", info->NextEntryOffset, info->Action, filename);
			if (info->NextEntryOffset == 0)
			{
				break;
			}
			info = (FILE_NOTIFY_INFORMATION*)((BYTE*)info + info->NextEntryOffset);
		} while (1);
	}


Cleanup:

	if (info != nullptr)
	{
		free(info);
	}
	if (hDir != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDir);
	}
}

int wmain(int argc, wchar_t *argv[])
{
	if (argc != 2) {
		return 1;
	}
	StartMonitor(argv[1]);
	return 0;
}
