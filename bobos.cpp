// bobos.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "stopwatch.h"
#include <conio.h>
#include "mmio.h"
#include "FileIoHelperClass.h"

/*
uint32_t mymain(int argc, wchar_t ** argtv)
{
	printf("mymain\n");
	return 0;
}
*/

// http://copynull.tistory.com/95
// http://egloos.zum.com/sweeper/v/2990023

int _tmain(int argc, _TCHAR* argv[])
{
	uint32_t FileSize = 4000;
	LARGE_INTEGER Size;
	Size.QuadPart = (LONGLONG)(1024 * 1024) * (LONGLONG)FileSize;

	_ASSERTE(create_very_big_file(L"big.txt", FileSize));

	StopWatch sw;
	sw.Start();
	_ASSERTE(file_copy_using_read_write(L"big.txt", L"big2.txt"));
	sw.Stop();
	print("info] time elapsed = %f", sw.GetDurationSecond());

	// 메모리에 할당할 크기 단위(BufSize) 설정
	DWORD BufSize = 4000;
	PUCHAR Buf = (PUCHAR)malloc(BufSize);

	// 메모리 오프셋 설정
	LARGE_INTEGER offset;
	offset.QuadPart = (LONGLONG)0;

	// FileIoHelper 모듈 이용
	FileIoHelper helper;
	sw.Start();

	helper.FIOpenForRead(L"big.txt");
	helper.FIOCreateFile(L"big3.txt", Size);

	while (offset.QuadPart < Size.QuadPart)
	{
		// BufSize 만큼 나누어 떨어지지 않을 경우 처리
		if ((Size.QuadPart - offset.QuadPart) >(LONGLONG)BufSize)
			BufSize = 4000;
		else
			BufSize = (DWORD)(Size.QuadPart - offset.QuadPart);

		helper.FIOReadFromFile(offset, BufSize, Buf);
		helper.FIOWriteToFile(offset, BufSize, Buf);

		offset.QuadPart += (LONGLONG)BufSize;
	}

	sw.Stop();
	print("info] time elapsed = %f", sw.GetDurationSecond());

	free(Buf);
	helper.FIOClose();
	_getch();
	return 0;
}