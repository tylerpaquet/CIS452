#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int main()
{	
	/*
	*Memory States
	*0x1000 - Memory Committed
	*0x10000 - Memory Freed
	*0x2000 - Memory Reserved
	*/
	
	SYSTEM_INFO lpSystemInfo;
	GetSystemInfo(&lpSystemInfo);
	MEMORY_BASIC_INFORMATION lpBuffer;
	
	printf("The page size is %u bytes\n", lpSystemInfo.dwPageSize);

	int *memory;
	memory = (int *) malloc(1000000);
	VirtualQuery(memory, &lpBuffer, sizeof(lpBuffer));
	if(lpBuffer.State == 0x1000)
	{
		printf("State after memory allocation: Memory Committed\n");
	}
	else if(lpBuffer.State == 0x10000)
	{
		printf("State after memory allocation: Memory Freed\n");
	}
	else if(lpBuffer.State == 0x2000)
	{
		printf("State after memory allocation: Memory Reserved\n");
	}

	free(memory);

	VirtualQuery(memory, &lpBuffer, sizeof(lpBuffer));

	if(lpBuffer.State == 0x1000)
	{
		printf("State after memory is freed: Memory Committed\n");
	}
	else if(lpBuffer.State == 0x10000)
	{
		printf("State after memory is freed: Memory Freed\n");
	}
	else if(lpBuffer.State == 0x2000)
	{
		printf("State after memory is freed: Memory Reserved\n");
	}
}