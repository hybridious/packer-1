// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "PEUtils.h"


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
} 

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DragQueryFile((HDROP)wParam, 0, m_szImagePath, MAX_PATH);
	SetDlgItemText(IDC_EDIT_FILEPATH, m_szImagePath);
	_wsplitpath(m_szImagePath, NULL, NULL, m_szImageName, NULL);
	
	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}


LRESULT CMainDlg::OnBnClickedButtonGo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PEUtils PE( m_szImagePath );
	// �������������
	/*
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = PE.GetImportDescriptor();
	PIMAGE_THUNK_DATA pThunkData = NULL;
	for (int i = 0; pImportDescriptor[i].FirstThunk; ++i) {

		pThunkData = PE.GetThunkData( &pImportDescriptor[i] );
		OutputDebugStringA("\n");
		OutputDebugStringA(PE.GetDllName( &pImportDescriptor[i] ));
		for (int j = 0; pThunkData[j].u1.Function; ++j) {
			// ���λ��1����ŷ�ʽ����
			// ���λ��0�����ַ�ʽ����
			if (pThunkData[j].u1.AddressOfData & 0x80000000) {
				
			} else {
				char *p = PE.GetFunctionName( &pThunkData[j] );
				OutputDebugStringA("\n\t");
				OutputDebugStringA(p);
				
			}
		}
	}

	*/
	

	// ����һ����������
	// ��ӡ����
	/*
	PIMAGE_SECTION_HEADER pSectionHeader = utils.GetSectionHeader();
	for (int i = 0; i < utils.GetNumberOfSection(); ++i) {
		OutputDebugStringA("\n");
		OutputDebugStringA((PCHAR)pSectionHeader[i].Name);
	}
	utils.AddSection("new_sec", 0x450, IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE);
	for (int i = 0; i < utils.GetNumberOfSection(); ++i) {
		OutputDebugStringA("\n");
		OutputDebugStringA((PCHAR)pSectionHeader[i].Name);
	}
	*/

	/*
		���̣�
			��������PE��Ȼ�����
			�޸�ԭPE���ֱ�־��ϣ������������ε�stub����
			��ȡstub���ض�λ���ݣ�ģ������ض�λ
			���ļ���text��ԭ������ִ������ݵ�ѹ����
			���ļ�����Դ��ʹ��ԭ���ģ�����е�����Ӧ��Ҳ�ǲ��ܶ��ɡ������ڻ�û��dll�����ܵ���
			���ļ����ض�λʹ��stub�ģ�stub����ģ�����ļ����ض�λ
			���ļ��ĵ����ʹ��stub�ģ�stub����ԭ��ѹ��ԭ����Ҫ�õ�IAT
			����Ҫ����tls������û��
	*/

	PEUtils stub(L"stub.dll");


		/*
		����һЩ�ط�
	*/
	if (PE.IsExecutable()) {

		if (PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)) {
			ZeroMemory(
				(PVOID)(PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT) + PE.GetImageBase()), 
				PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT));
			ZeroMemory(
				&(PE.GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]),
				sizeof(IMAGE_DATA_DIRECTORY));
		}
	} else {}

	/*
		����stub��ȥ������
	*/
	// ��ֻҪ��һ��.text�ξ�����
	// tls directory Ҳ����ȥ�ɡ���������
	DWORD dwVSizeOfStub = stub.GetSectionHeader()->Misc.VirtualSize;

	if (PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_TLS))
		dwVSizeOfStub += sizeof (IMAGE_TLS_DIRECTORY);

	PIMAGE_SECTION_HEADER pStubSection =
		PE.AddSection(".stub", dwVSizeOfStub, IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_SHARED);

	PENTRY_DATA pTls = PE.OutSourceEntryData(IMAGE_DIRECTORY_ENTRY_TLS);

	/*
		��������ʵ�������ļ���CodeBase(Ҳ��stub��CodeBase��)
		EntryPoint�ĳ�stub��EntryPoint
		Ȼ��ʼģ��stub������λ
	*/
	DWORD dwOEP = 
		PE.GetNtHeader()->OptionalHeader.ImageBase + PE.GetEntryPoint();

	PE.GetNtHeader()->OptionalHeader.AddressOfEntryPoint = 
		pStubSection->VirtualAddress + 
		(stub.GetEntryPoint() - 
		stub.GetNtHeader()->OptionalHeader.BaseOfCode);

	// ����stub��ģ���ض�λ����

	// ��ַ��ֵ
	DWORD dwImageBaseDiff = 
		stub.GetNtHeader()->OptionalHeader.ImageBase - 
		PE.GetNtHeader()->OptionalHeader.ImageBase;
	// ��Ϊ�Լ��ڵ�һ������, ʵ�������Ǽ��������һ�����Σ���ֵ�Ͳ��ü���ô��
	dwImageBaseDiff -= pStubSection->VirtualAddress;
	// stub��Ϊ�����ڵ�һ�����Σ�������ֱ�Ӿ�ִ���ˣ�ʵ������ƫ����0�����ǵ�һ��������
	dwImageBaseDiff += stub.GetSectionHeader()->VirtualAddress;

	stub.PerformRelocation( dwImageBaseDiff );



	// �Ѹĺõ����ݸ��ƽ�ȥ������
	memcpy(
		(PVOID)(pStubSection->VirtualAddress + PE.GetImageBase()),
		(PVOID)(stub.GetSectionHeader()->VirtualAddress + stub.GetImageBase()),
		dwVSizeOfStub);


	// ��¼һ��ԭ���ĵ�����ַ
	DWORD dwImportTable = 
		PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_IMPORT);

	// DOS ͷ���ò��ϵĵط��������ðɡ���
	*(((DWORD *)PE.GetImageBase()) + 1) = dwOEP;
	*(((DWORD *)PE.GetImageBase()) + 2) = dwImportTable;



	// ԭ�����ָ��stub�������IAT���øģ���ʱPE loader����PEʱ��ѵ�ַ����ԭIAT��
	// stub��ԭԭIATʱֱ�Ӹ���stub��IAT����������ֻ��һ��
	// ����shellcode�㺯����ַ�͵������㡣������
	DWORD dwAddrDiff = pStubSection->VirtualAddress - stub.GetNtHeader()->OptionalHeader.BaseOfCode;

	PE.GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 
		stub.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_IMPORT) + dwAddrDiff;

	PE.GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = 
		stub.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_IMPORT);

	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = PE.GetImportDescriptor();
	PIMAGE_THUNK_DATA pFirstThunkData = NULL;
	PIMAGE_THUNK_DATA pOriginThunkData = NULL;
	for (int i = 0; pImportDescriptor[i].FirstThunk; ++i) {

		pImportDescriptor[i].FirstThunk += dwAddrDiff;
		pImportDescriptor[i].OriginalFirstThunk += dwAddrDiff;
		pImportDescriptor[i].Name += dwAddrDiff;

		pFirstThunkData = PE.GetFirstThunkData( &pImportDescriptor[i] );
		pOriginThunkData = PE.GetOriginThunkData( &pImportDescriptor[i] );
		for (int j = 0; pFirstThunkData[j].u1.AddressOfData; j++) {
			
			if (pFirstThunkData[j].u1.AddressOfData & 0x80000000) {
				; // stub��û��ŵ��뺯��
			} else {
				pFirstThunkData[j].u1.AddressOfData = pFirstThunkData[j].u1.AddressOfData + dwAddrDiff;
				pOriginThunkData[j].u1.AddressOfData = pOriginThunkData[j].u1.AddressOfData + dwAddrDiff;
			}
		}
	}

	// ���������ָ��

	// �����tls����
	if (PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_TLS)) {
		
		// ��tls����ȥ�����ε�stub���������֮��
		memcpy(
			(PVOID)(pStubSection->VirtualAddress + PE.GetImageBase() + stub.GetSectionHeader()->Misc.VirtualSize),
			pTls->pAddress,
			pTls->dwSize);

		// ��¼Tls���ַ
		DWORD dwTls = 
			PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_TLS);
		*(((DWORD *)PE.GetImageBase()) + 3) = dwTls;

	
		// Tls ָ����λ���λ��
		PE.GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = 
			pStubSection->VirtualAddress + stub.GetSectionHeader()->Misc.VirtualSize;

		PIMAGE_TLS_DIRECTORY pTlsDirectory = (PIMAGE_TLS_DIRECTORY)
			(PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_TLS) + PE.GetImageBase());

		// stub�����ʹ���֮���е�00....������ȥ�ɡ���������
		DWORD dwImportVA = 
			PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_IMPORT) + PE.GetNtHeader()->OptionalHeader.ImageBase;
		DWORD dwRawSize = pTlsDirectory->EndAddressOfRawData - pTlsDirectory->StartAddressOfRawData;
		pTlsDirectory->StartAddressOfRawData	= dwImportVA - 0x250;
		pTlsDirectory->EndAddressOfRawData		= dwImportVA - 0x250 + dwRawSize;
		pTlsDirectory->AddressOfIndex			= dwImportVA - 0x250 + dwRawSize;
		pTlsDirectory->AddressOfCallBacks		= dwImportVA - 0x250 + dwRawSize + 0x50;
	}


	/*
		����һЩ�ط�
	*/
	if (PE.IsExecutable()) {

		if (PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BASERELOC)) {
			ZeroMemory(
				(PVOID)(PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_BASERELOC) + PE.GetImageBase()), 
				PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BASERELOC));
		}

		if (PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG)) {
			ZeroMemory(
				(PVOID)(PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG) + PE.GetImageBase()), 
				PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG));
			ZeroMemory(
				&(PE.GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG]),
				sizeof(IMAGE_DATA_DIRECTORY));
		}

		if (PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_DEBUG)) {
			ZeroMemory(
				(PVOID)(PE.GetDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_DEBUG) + PE.GetImageBase()), 
				PE.GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_DEBUG));
			ZeroMemory(
				&(PE.GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG]),
				sizeof(IMAGE_DATA_DIRECTORY));
		}
	} else {
	
		/*
			��ʼ��PE���ض�λ��ָ��stub��
			��ѹ����ض�λ��������stub
		*/

	

		// �����ض�λת��
	}

	PE.Pack();

	PE.WriteImageToFile();

	return 0;
}


LRESULT CMainDlg::OnBnClickedButtonBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(TRUE, NULL, NULL, NULL, L"EXE�ļ�(*.exe)\0*.exe\0�����ļ�(*.*)\0*.*\0", *this);

	if (IDOK == FileDlg.DoModal()) {
		/*
			�����ļ�·�����ļ���
		*/
		wcscpy_s(m_szImagePath, _MAX_FNAME, FileDlg.m_szFileName);
		wcscpy_s(m_szImageName, _MAX_FNAME, FileDlg.m_szFileTitle);

		SetDlgItemText(IDC_EDIT_FILEPATH, m_szImagePath);
	}
	return 0;
}
