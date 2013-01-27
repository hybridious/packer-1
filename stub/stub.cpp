/*
	ѹ�������ļ��Ŀ�ʼ��
	{
		0H	WORD Signature 'MZ'
		2H	WORD '00'
		4H	DWORD OEP
		8H	DWORD �����RVA
		0CH	DWORD �ض�λ��RVA
		
	}

	ѹ���������ο�ʼ��
	{
		DWORD ѹ�����ı��'PACK'
		DWORD ԭ���Ĵ�С(���ڰ�ȫ��ѹ)
	}

*/

#include <Windows.h>

/*
#include "../zlib-1.2.7/zlib.h"

#pragma comment(lib, "zlib")
*/
#include "../aplib.h"
#pragma comment(lib, "../aplib")

// ��������msvcr�������Ӿ�̬���п�
// #pragma comment(lib, "LIBCMT")
// �ҹ��ư�msvcr110�༭��msvcrt������ <- ��ȷ

// �ں�data����text�Σ������������data�γ�ͻ
#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")



BOOL g_isFirstCall = TRUE;
// ��ЩҪ�õ���ֵ�����������������ļ�ͷ������ʹ�õĵط�
DWORD g_dwOEP			= NULL;
DWORD g_dwImportTable	= NULL;

VOID DePack(PIMAGE_NT_HEADERS pNtHeader)
{
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION( pNtHeader );
	PVOID pBase				= GetModuleHandle( NULL );
	PVOID pDePack			= NULL;
	DWORD dwOldProtect		= 0;
	DWORD dwDePackedSize	= 0;
	DWORD nSection			= pNtHeader->FileHeader.NumberOfSections;

	// ѭ�����β���ѹ(Ҫ��ѹ�Ķ������'PACK')
	DWORD *pSig = NULL;
	for (int i = 0; i < nSection; ++i) {
		pSig = (DWORD *)(pSectionHeader[i].VirtualAddress + (DWORD)pBase);
		if (*pSig == 'PACK') {
			
			DWORD dwOriSize = aPsafe_get_orig_size(pSig + 2);
			
			pDePack = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwOriSize);

			dwDePackedSize = aPsafe_depack(pSig + 2, *(pSig + 1), pDePack, dwOriSize);
			
			if ( dwDePackedSize ) {
				VirtualProtect(pSig, dwDePackedSize, PAGE_READWRITE, &dwOldProtect);
				// ZeroMemory(pSig, dwDePackedSize);
				memcpy(pSig, pDePack, dwDePackedSize);
				VirtualProtect(pSig, dwDePackedSize, dwOldProtect, &dwOldProtect);
			}

			HeapFree(GetProcessHeap(), NULL, pDePack);
		}	
	}
}


VOID FillIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor)
{
	HMODULE hModule					= NULL;
	DWORD pBase						= (DWORD)GetModuleHandle( NULL );
	PCHAR pFuncationName			= NULL;
	PIMAGE_THUNK_DATA pOriginThunk	= NULL;
	PIMAGE_THUNK_DATA pFirstThunk	= NULL;

	// д�ڴ�Ȩ��
	//PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)
	//	(((PIMAGE_DOS_HEADER)pBase)->e_lfanew + pBase);

	//DWORD dwIATAddr = 
	//	pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress + pBase;
	//DWORD dwSize = 
	//	pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size;
	
	DWORD OldProtect = 0;
	// VirtualProtect((PVOID)dwIATAddr, dwSize, PAGE_READWRITE, &OldProtect);
	
	for (int i = 0; pImportDescriptor[i].Name; ++i) {

		hModule			= LoadLibraryA( (char *)(pBase + pImportDescriptor[i].Name) );
		pOriginThunk	= (PIMAGE_THUNK_DATA)(pImportDescriptor[i].OriginalFirstThunk + pBase);
		pFirstThunk		= (PIMAGE_THUNK_DATA)(pImportDescriptor[i].FirstThunk + pBase);

		// ��Щ������first��ȴ��Щ������original
		if ( !((DWORD)pOriginThunk ^ pBase) ) 
			pOriginThunk = pFirstThunk;

		for (int j = 0; pOriginThunk[j].u1.AddressOfData; ++j) {
			
			if (pOriginThunk[j].u1.AddressOfData & 0x80000000) {
				// ��ŵ���
				pFuncationName = (PCHAR)(pOriginThunk[j].u1.AddressOfData & 0x7FFFFFFF);
			} else {
				// ���ֵ���
				pFuncationName = (PCHAR)
					((PIMAGE_IMPORT_BY_NAME)(pOriginThunk[j].u1.AddressOfData + pBase))->Name;
			}

			VirtualProtect(&pFirstThunk[j], 4, PAGE_READWRITE, &OldProtect);
			pFirstThunk[j].u1.Function = (DWORD)GetProcAddress(hModule, pFuncationName);
			VirtualProtect(&pFirstThunk[j], 4, OldProtect, &OldProtect);
		}
	}

	// VirtualProtect((PVOID)dwIATAddr, dwSize, OldProtect, &OldProtect);
}



VOID PerformRelocation()
{
	
}


int APIENTRY CustomEntry( void )
{
	/*
		������������������3��push
	*/
	_asm add esp, 12
	_asm {
		pushad
		pushfd
	}

	
	if (g_isFirstCall) {
		g_isFirstCall = FALSE;
		
		// dll ��GetModuleHandle����ʹ������
		HANDLE hBase = GetModuleHandle(NULL);

		g_dwOEP			= *(((DWORD *)hBase) + 1);
		g_dwImportTable	= *(((DWORD *)hBase) + 2);

		PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)
			(((PIMAGE_DOS_HEADER)hBase)->e_lfanew + (DWORD)hBase);
		// 
		DePack( pNtHeader );

		FillIAT( (PIMAGE_IMPORT_DESCRIPTOR)(g_dwImportTable + (DWORD)hBase) );
	}
	_asm {
		popfd
		popad
		push g_dwOEP
		ret
	}
}

