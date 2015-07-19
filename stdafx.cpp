// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// bobos.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

bool create_bob_txt();
bool is_file_existsW(_In_ const wchar_t* file_path);
void print(_In_ const char* fmt, _In_ ...);
bool WriteBOM(HANDLE hFile);

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

void print(_In_ const char* fmt, _In_ ...)
{
	char log_buffer[2048];
	va_list args;

	va_start(args, fmt);
	HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
	if (S_OK != hRes)
	{
		fprintf(
			stderr,
			"%s, StringCbVPrintfA() failed. res = 0x%08x",
			__FUNCTION__,
			hRes
			);
		return;
	}

	OutputDebugStringA(log_buffer);
	fprintf(stdout, "%s \n", log_buffer);
}

bool create_bob_txt()
{
	// current directory 를 구한다.
	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR)* buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	// current dir \\ bob.txt 파일명 생성
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\bob.txt",
		buf)))
	{
		print("err, can not create file name");
		free(buf);
		return false;
	}
	free(buf); buf = NULL;

	if (true == is_file_existsW(file_name))
	{
		::DeleteFileW(file_name);
	}

	// 파일 생성
	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}

	// 파일에 데이터 쓰기
	DWORD bytes_written = 0;
	wchar_t string_buf[1024];
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	char string_buf_mb[1024];
	WriteBOM(file_handle);
	WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, string_buf_mb, sizeof(string_buf_mb), NULL, FALSE);

	//	if (!WriteFile(file_handle, string_buf, wcslen(string_buf), &bytes_written, NULL))
	if (!WriteFile(file_handle, string_buf_mb, strlen(string_buf_mb), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// 영어로 쓰기
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}


	WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, string_buf_mb, sizeof(string_buf_mb), NULL, FALSE);

	//	if (!WriteFile(file_handle, string_buf, wcslen(string_buf), &bytes_written, NULL))
	if (!WriteFile(file_handle, string_buf_mb, strlen(string_buf_mb), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}




	char string_bufa[1024];
	if (!SUCCEEDED(StringCbPrintfA(
		string_bufa,
		sizeof(string_bufa),
		"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}



	wchar_t string_buf_16[1024];
	MultiByteToWideChar(CP_ACP, 0, string_bufa, -1, string_buf_16, sizeof(string_buf_16));
	WideCharToMultiByte(CP_UTF8, 0, string_buf_16, -1, string_bufa, sizeof(string_buf_mb), NULL, FALSE);

	if (!WriteFile(file_handle, string_bufa, strlen(string_bufa), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// 영어로 쓰기
	if (!SUCCEEDED(StringCbPrintfA(
		string_bufa,
		sizeof(string_bufa),
		"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	MultiByteToWideChar(CP_ACP, 0, string_bufa, -1, string_buf_16, sizeof(string_buf_16));
	WideCharToMultiByte(CP_UTF8, 0, string_buf_16, -1, string_bufa, sizeof(string_buf_mb), NULL, FALSE);

	if (!WriteFile(file_handle, string_bufa, strlen(string_bufa), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// 파일 닫기
	CloseHandle(file_handle);
	DeleteFileW(file_name);
	return true;

}

bool is_file_existsW(_In_ const wchar_t* file_path)
{
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH))) return false;

	WIN32_FILE_ATTRIBUTE_DATA info = { 0 };

	if (GetFileAttributesExW(file_path, GetFileExInfoStandard, &info) == 0)
		return false;
	else
		return true;
}

bool WriteBOM(HANDLE hFile)
{
	DWORD cbWritten = 0;
	BYTE ByteOrderMark[] = { 0xEF, 0xBB, 0xBF };
	return (TRUE != WriteFile(
		hFile,
		ByteOrderMark,
		sizeof(ByteOrderMark),
		&cbWritten,
		NULL));
}