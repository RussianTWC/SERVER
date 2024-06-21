#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

void send_file(SOCKET* sock, const char* file_name);

int main() {
	WORD dllVer = MAKEWORD(2, 1);
	WSADATA wsad;

	WSAStartup(dllVer, &wsad);

	SOCKADDR_IN addr_info;
	memset(&addr_info, 0, sizeof(SOCKADDR_IN));

	int size_addr = sizeof(addr_info);
	addr_info.sin_port = htons(4321);
	addr_info.sin_family = AF_INET;

	SOCKET s_listen = socket(AF_INET, SOCK_STREAM, 0);
	bind(s_listen, (SOCKADDR_IN*)&addr_info, sizeof(addr_info));
	listen(s_listen, SOMAXCONN);

	SOCKET s_for_connect = accept(s_listen, (SOCKADDR_IN*)&addr_info, &size_addr);

	if (s_for_connect != 0) {
		printf("Connect\n");

		char* path = malloc(33*sizeof(char));
		if (path == NULL)
			return -1;
		recv(s_for_connect, path, 33, 0);
		send_file(&s_for_connect, path);
	}

	system("pause");
	return 0;
}

void send_file(SOCKET* sock, const char* file_name) {
	FILE* file;
	errno_t err;
	err = fopen_s(&file, file_name, "rb");

	if (err != 0) {
		printf("Error. open file\n");
		return;
	}
	else {
		fseek(file, 0, SEEK_END);
		int file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		char* bytes = (char*)malloc((file_size + 1)*sizeof(char));
		if (bytes == NULL)
			return;
		fread(bytes, sizeof(*bytes), file_size, file);

		printf("size: %d\nname: %s\n", file_size, file_name);
		char buf[1024];
		snprintf(buf, 1024, "%d", file_size);
		if (buf == NULL)
			printf("erroe buf");

		send(*sock, buf, 1024, 0);
		send(*sock, file_name, 32, 0);
		send(*sock, bytes, file_size, 0);

		fclose(file);
	}
	return;
}