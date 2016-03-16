#include<stdio.h>
#include<winsock2.h>
#include<winbase.h>
#include<time.h>

#pragma comment(lib, "ws2_32")

#define RefreshTimes 1587
int cnt = 0;
char host[128];
char path[128];
char NUM[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

DWORD WINAPI SendThread(LPVOID lpparam);
void Setparam(){
	char Host_Temp[] = "lvyou.baidu.com";
	char Path_Temp[] = "/main/event/nanji/ajax/good?wcuid=oVFEBt7-gTCS2S7Rf-V6IcIF5Wug";
	fflush(stdout);
	strcpy_s(path, &Path_Temp[0]);
	strcpy_s(host, &Host_Temp[0]);
}

void main(int argc, char* argv[]){
	HANDLE hThread[RefreshTimes];
	DWORD dwThread[RefreshTimes];

	int i, num1, num2;
	srand((unsigned)time(NULL));
	Setparam();

	for (i = 0; i < RefreshTimes; ++i){
		hThread[i] = CreateThread(NULL, 0, SendThread, (LPVOID)i, 0, &dwThread[i]);
		if (hThread == NULL){
			printf("\tCreateThread Failed.\n");
			exit(0);
		}
		num2 = (rand() % 100)+ 1000;
		Sleep(num2);
		CloseHandle(hThread);
	}
	system("pause");
}

DWORD WINAPI SendThread(LPVOID lpParam){
	WSADATA wsd;
	SOCKET local;
	SOCKADDR_IN addr;
	HOSTENT *Host = NULL;

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0){
		printf("\tFailed To Load Winsock Library!\n");
		exit(0);
	}
	local = socket(AF_INET, SOCK_STREAM, 0);
	if (local == INVALID_SOCKET){
		printf("\tFailed To Create Socket\n");
		exit(0);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.S_un.S_addr = inet_addr(host);
	if (addr.sin_addr.S_un.S_addr == INADDR_NONE){
		Host = gethostbyname(host);
		if (Host == NULL){
			printf("\tUnable To Resolve Server:%s\n", host);
			return 0;
		}
		CopyMemory(&addr.sin_addr, Host->h_addr_list[0], Host->h_length);
		printf("\tSend To:%s", inet_ntoa(addr.sin_addr));
	}

	if (SOCKET_ERROR == connect(local, (sockaddr*)&addr, sizeof(SOCKADDR))){
		printf("\tFailed To Connect To The Server!\n");
		exit(0);
	}

	//构造发送数据
	srand((unsigned)time(NULL));
	char baiduid[33];
	memset(baiduid, '\0', sizeof(baiduid));
	for (int i = 0; i < 32; ++i){
		baiduid[i] = NUM[rand() % 16];
	}
	char SendBuf[640];
	memset(SendBuf, '\0', sizeof(SendBuf));
	char PartOne[] = " HTTP/1.1\r\nHost: lvyou.baidu.com\r\nConnection: keep-alive\r\nAccept: application/json, text/javascript, */*; q=0.01\r\nX-Rquested-With: XMLHttpRequest\r\nUser-Agent: Mozilla/5.0 (Linux; U; Android 4.3; en-us; SM-N900T Build/JSS15J) Applewebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile safari/534.30\r\nReferer: http://lvyou.baidu.com/main/event/nanji/user?wcuid=oVFEBt7-gTCS2S7Rf-V6IcIF5Wug&from=timeline&isappinstalled=0\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Language: zh-CN,zh;q=0.8\r\nCookie: BAIDUID=";
	char PartTwo[] = ":FG=1;\r\n\r\n";
	strcpy_s(SendBuf, "GET ");
	strcat_s(SendBuf, path);
	strcat_s(SendBuf, PartOne);	
	strcat_s(SendBuf, baiduid);
	strcat_s(SendBuf, PartTwo);
	
	if (send(local, SendBuf, strlen(SendBuf) + 1, 0) == SOCKET_ERROR){
		printf("\tSend Data Error.\n");
	}
	closesocket(local);
	WSACleanup();
	printf("       %d\n", ++cnt);
	return 0;
}