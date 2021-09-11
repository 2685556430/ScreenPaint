#include <windows.h>
#include <vector>
#include <algorithm>
/*
���ܵ��Ż���
��������λͼ ���Ի��� ����Ӱ�� ����ڵ��ӻ��Ƶ���Ļ�� ����psͼ��ṹ

һЩ���㣺
�ڻ��߹������������ƶ���������Ե� �����ߡ� ԭ���������������ȡ����ʱ���£����������Ծ��
ͨ���۲� windows �Դ��Ļ�ͼ����ͬ��Ҳ�����������״�� ��ֻ������ƶ��ٶȼ����ʱ��Ż����

��Ȼ����ͬ��Ҳ����� ����Ծ�� �����

����֮������취��~~
*/
void CreatCanvas(HDC hdc, HDC &hdcBuffer, HBITMAP &BitMap) {//Ϊָ���豸���� ����
	hdcBuffer = CreateCompatibleDC(hdc);
	BitMap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN)*1.25, GetSystemMetrics(SM_CYSCREEN)*1.25);

	SelectObject(hdcBuffer, BitMap);

	BitBlt(hdcBuffer, 0, 0, GetSystemMetrics(SM_CXSCREEN)*1.25, GetSystemMetrics(SM_CYSCREEN)*1.25, hdc, 0, 0, SRCCOPY);//������������������
};
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstace, PSTR szCmdline, int iCmdShow) {
	/*��������*/
	WNDCLASS wc;
	static TCHAR *szAppName = TEXT("Screen_Paint");
	HWND hwnd = NULL;
	MSG msg;
	int nScreenWidth, nScreenHeight;
	/*��ȡ��Ļ�ߴ�*/
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN) * 1.25;
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN) * 1.25;
	/*������������� �����д���ע��*/
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);				//�������Ϊʮ��
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	/*ʧ���������ʾ*/
	if (!RegisterClass(&wc)) {
		MessageBox(NULL, TEXT("�ó���ֻ����windows NT�����У�"), szAppName, MB_ICONERROR);
		return 0;
	}
	/*��һ�����ô�����ʽ*/
	hwnd = CreateWindowEx(WS_EX_LAYERED, szAppName,    //WS_EX_LAYERED �ֲ㴰�ڣ�͸�����ڣ�
		TEXT("��Ļ����-Screen_Paint"),
		WS_POPUP, 0, 0, nScreenWidth, nScreenHeight,   //WS_POPUP �������ĶԻ��� ֻ�е���������ڵĲ�����ɺ���ܽ�����������
		NULL,
		NULL, hInstance,
		NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	/*��Ϣѭ��*/
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int nScreenWidth, nScreenHeight;
	long xPos, yPos;
	HDC hdc;
	static HDC hdcBuffer, auxiliaryBuffer;
	static HBITMAP BitMap, auxiliaryBitMap;
	static POINT pts[10000];
	static RECT erase = { 0,0,0,0 };
	static std::vector<HPEN> Pen;
	static HPEN hPenCur;
	static int i = 0;
	static int size = 20;
	static int leftDown = 0;
	static int rightDown = 0;

	/*����ΪʲôҪ��1.25  ����Ϊ��Ļ�з�������  �ҵ���Ļ�Ŵ�125%*/
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN) * 1.25;
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN) * 1.25;

	switch (message) {
	case WM_CREATE:
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		for (int i = 0; i <= 255; i += 255)
			for (int j = 0; j <= 255; j += 255)
				for (int k = 0; k <= 255; k += 255) {
					Pen.push_back(CreatePen(PS_SOLID, 3, RGB(i, j, k)));
				}

		hPenCur = Pen[4];//Ĭ�Ϻ�ɫ
		/*��������*/
		hdc = GetDC(NULL);
		CreatCanvas(hdc, hdcBuffer, BitMap);
		//CreatCanvas(hdc, auxiliaryBuffer, auxiliaryBitMap); ��δ�õ�
		ReleaseDC(NULL, hdc);
		return 0;
	case WM_KEYDOWN:
		/*��ɫ - �˳�*/
		switch (wParam) {
		case 'D'://Dark-��
			hPenCur = Pen[0];
			break;
		case 'B'://Blue-��
			hPenCur = Pen[1];
			break;
		case 'G'://Green-��
			hPenCur = Pen[2];
			break;
		case 'C'://Cyan-��
			hPenCur = Pen[3];
			break;
		case 'R'://Red-��
			hPenCur = Pen[4];
			break;
		case 'P'://Pink-��
			hPenCur = Pen[5];
			break;
		case 'Y'://Yellow-��
			hPenCur = Pen[6];
			break;
		case 'W'://White-��
			hPenCur = Pen[7];
			break;
		case VK_ESCAPE://ESC-�˳�
			PostQuitMessage(0);
		}
		return 0;
	case WM_MOUSEMOVE:
		/*�ƶ�����*/

		if (leftDown != 0) {//�������
			xPos = LOWORD(lParam) * 1.25;
			yPos = HIWORD(lParam) * 1.25;
			pts[i].x = xPos;
			pts[i++].y = yPos;
			/*���ͻ�����Ϣ ������������*/
			InvalidateRect(hwnd, NULL, FALSE);
		}
		else if (rightDown != 0) {//�Ҽ�����
			xPos = LOWORD(lParam) * 1.25;
			yPos = HIWORD(lParam) * 1.25;
			erase = { max(xPos - size, 0), max(yPos - size, 0), min(xPos + size, nScreenWidth), min(yPos + size, nScreenHeight) };
			InvalidateRect(hwnd, NULL, FALSE);
		}

		return 0;
	case WM_MOUSEWHEEL:
		if (GetAsyncKeyState(VK_CONTROL) & 0X8000) {//Ctrl + ���� �ı���Ƥ���ߴ�
			/*wParam ��λ�����Ź��ֹ�����Ϣ*/
			short val = HIWORD(wParam);
			if (val > 0)
				size = min(size + 2, 50);
			else if (val < 0)
				size = max(size - 2, 1);
		}
		return 0;
	case WM_LBUTTONDOWN://����滭
		/*��ʼ����*/
		leftDown = 1;
		i = 0;
		return 0;
	case WM_LBUTTONUP:
		/*��������*/
		leftDown = 0;
		return 0;
	case WM_RBUTTONDOWN://�Ҽ�����
		rightDown = 1;
		return 0;
	case WM_RBUTTONUP:
		rightDown = 0;
		erase = { 0,0,0,0 };
		return 0;
	case WM_PAINT:
		/*����*/
		if (leftDown != 0) {//����
			hdc = GetDC(NULL);

			SelectObject(hdc, hPenCur);
			Polyline(hdc, pts, i);
			ReleaseDC(NULL, hdc);

			ValidateRect(hwnd, NULL);
		}
		else if (rightDown != 0) {//����
			hdc = GetDC(NULL);
			//BitBlt(hdc, erase.left, erase.top, erase.right, erase.bottom, hdcBuffer, 0, 0, SRCCOPY); //û̫�㶮BitBlt ����ô�������򿽱���  

			StretchBlt(hdc, erase.left, erase.top, erase.right - erase.left, erase.bottom - erase.top, hdcBuffer, erase.left, erase.top, erase.right - erase.left, erase.bottom - erase.top, SRCCOPY);

			ValidateRect(hwnd, NULL);
		}
		return 0;
	case WM_DESTROY:
		/*ɾ����Ӧ��������Ͷ��� ���˳�*/
		ReleaseDC(NULL, hdcBuffer);
		DeleteObject(BitMap);
		//ReleaseDC(NULL, auxiliaryBuffer);
		//DeleteObject(auxiliaryBitMap);
		for (int i = 0; i < Pen.size(); i++)
			DeleteObject(Pen[i]);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}