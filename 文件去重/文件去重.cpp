// �ļ�ȥ��.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "�ļ�ȥ��.h"
#include"List.h"


struct FileInfo
{
	DWORD dwSize = 0;//�ļ���С
	LPTSTR pName = NULL;//�ļ�·��
	UINT AddCount = 0;//����Ӵ���
};
// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szPath[MAX_PATH];
CList<FileInfo> fsl;
HWND hListWnd1;
HWND hListWnd2;




int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  �ڴ˷��ô��롣
	hInst = hInstance;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
	return GetLastError();
}

VOID WalkCur(HWND hDlg, LPTSTR szCurPath)
{
	SetCurrentDirectory(szCurPath);
	//GetCurrentDirectory(MAX_PATH, szCurPath);
	TCHAR szTemp[MAX_PATH];
	wsprintf(szTemp, _T("%s*.*"), szCurPath);
	WIN32_FIND_DATA wfd;
	ZeroMemory(&wfd, sizeof(wfd));
	HANDLE hfind = FindFirstFile(szTemp, &wfd);
	while (FindNextFile(hfind, &wfd)) {
		BOOL bAdd = TRUE;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {//Ϊ�ļ���
			if (wfd.cFileName[0] == '.') {
				continue;
			}
			TCHAR szNew[MAX_PATH];
			wsprintf(szNew, _T("%s%s\\"), szPath, wfd.cFileName);
			bAdd = FALSE;
			WalkCur(hDlg, szNew);
		}
		FileInfo*p = new FileInfo;
		p->dwSize = wfd.nFileSizeLow;

		TCHAR sz[MAX_PATH];
		wsprintf(sz, _T("%s%s"), szCurPath, wfd.cFileName);

		for (FileInfo* pt = fsl.GetFirst(); pt; pt = fsl.GetNext()) {
			if (pt->dwSize == p->dwSize) {//�����ظ�
				if (pt->AddCount < 1) {
					pt->AddCount += 1; 
					//����ָ���
					ListBox_AddString(hListWnd1, _T(""));
					ListBox_AddString(hListWnd2, _T(""));

					ListBox_AddString(hListWnd1, pt->pName);
				}
				ListBox_AddString(hListWnd1, sz);
				ListBox_AddString(hListWnd2, sz);
				bAdd = FALSE;
			}
		}
		if (bAdd){
			LPTSTR pName = new TCHAR[MAX_PATH];
			lstrcpy(pName, sz);
			p->pName = pName;
			fsl.AddToTail(p);
		}

	}
	FindClose(hfind);
}

VOID OnInit(HWND hDlg)
{
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MY));
	SendMessage(hDlg, WM_SETICON, NULL, (LPARAM)hIcon);

	hListWnd1 = GetDlgItem(hDlg, IDC_LIST1);
	hListWnd2 = GetDlgItem(hDlg, IDC_LIST2);

	//����ˮƽ������
	SendMessage(hListWnd1, LB_SETHORIZONTALEXTENT, 2500, 0);
	SendMessage(hListWnd2, LB_SETHORIZONTALEXTENT, 2500, 0);

	GetCurrentDirectory(MAX_PATH, szPath);
	lstrcat(szPath, _T("\\"));
	WalkCur(hDlg, szPath);

	ListBox_AddString(hListWnd2, _T(""));//����ָ���
	fsl.EmptyList();
}

VOID AddItem()
{
	TCHAR sz[MAX_PATH];
	ListBox_GetText(hListWnd1, ListBox_GetCurSel(hListWnd1), sz);
	ListBox_AddString(hListWnd2, sz);
}

VOID DeleteItem()
{
	ListBox_DeleteString(hListWnd2, ListBox_GetCurSel(hListWnd2));
}

VOID DoDelte(BOOL bDelOnlyRead)
{
	int count = ListBox_GetCount(hListWnd2);
	TCHAR sz[MAX_PATH];
	TCHAR szNew[MAX_PATH];
	szNew[0] = '\0';
	sz[0] = '\0';
	for (int i = 0; i < count; i++) {
		ListBox_GetText(hListWnd2, i, sz);
		if (sz[0] != '\0') {
			if (bDelOnlyRead) {
				SetFileAttributes(sz, FILE_ATTRIBUTE_NORMAL);//�����ļ���������
			}
			DeleteFile(sz);
		}
		ListBox_DeleteString(hListWnd2, i);
	}
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		OnInit(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK://���
			AddItem();
			break;
		case IDC_BUTTON1://ɾ��
			DeleteItem();
			break;
		case IDC_BUTTON2://ִ��ȥ��
			DoDelte(Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK1)));
			break;
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return GetLastError();
		}
	}
	return (INT_PTR)FALSE;
}
