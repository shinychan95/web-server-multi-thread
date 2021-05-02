#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

#define TRUE 1
#define FALSE 0

int* request_handler(void* arg);
void send_data(FILE* fp, char* ct, char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_size;

	int option;
	socklen_t optlen;
	char buf[BUF_SIZE];
	pthread_t t_id;
	int status;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	optlen = sizeof(option);
	option = TRUE;	
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, optlen);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = PF_INET;
	/* INADDR_ANY: 서버의 IP 주소를 자동으로 찾아서 대입해주는 함수
	 *   NIC(Network Interface Controller)가 2개 이상 장착된 컴퓨터의 경우,
	 *   특정 NIC의 IP로 지정하면, 나머지가 인식하지 못한다.
	 */
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
	if (listen(serv_sock, 20) == -1)
		error_handling("listen() error");

	printf("-----------------------------\n");
	printf("Web Server Now Starting...\n");
	printf("Address: %s & Port: %d\n", inet_ntoa(serv_adr.sin_addr), ntohs(serv_adr.sin_port));
	printf("-----------------------------\n\n");

	while (1)
	{
		clnt_adr_size = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
		printf("%d - [Connect] Request : %s:%d\n", clnt_sock, inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
		
		//TODO : Implementing multithreading and calling request handler
	}

	close(serv_sock);
	return 0;
}

int* request_handler(void *arg)
{
	int clnt_sock = *((int*)arg);	
	char req_line[SMALL_BUF];
	FILE* clnt_read;
	FILE* clnt_write;
	FILE* exist_check;
	
	char method[10];
	char ct[15];
	char file_name[30];
	char file_path[60];
	//TIP : You can modify file path setting depending on your directory.
	
	printf("%d - [Start] Request Handler Function\n", clnt_sock);

	// fdopen - 파일 기술자에서 파일 포인터를 생성하는 함수
	clnt_read = fdopen(clnt_sock, "r");
	if (!clnt_read) 
		perror("open");
	
	clnt_write = fdopen(dup(clnt_sock), "w");
	if (!clnt_write) 
		perror("write");
	
	// fgets - 파일 포인터가 가리키는 파일에서 정수 n으로 지정한 길이보다 하나 적게 문자열을 읽어 s에 저장합니다
	fgets(req_line, SMALL_BUF, clnt_read);
	req_line[strlen(req_line) - 1] = NULL;
	printf("%d - [Info] First Request Line : %s\n", clnt_sock, req_line);

	// strstr - 문자열에서 특정 문자열의 시작 위치를 알려주는 함수
	if (strstr(req_line, "HTTP/") == NULL) {
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return NULL;
	 }
	
	// strtok - 문자열에서 token을 뽑아내는 함수. 두번째 수행부터는 NULL을 넣고, 더이상 없으면 NULL 반환
	strcpy(method, strtok(req_line, " /"));
	if (strcmp(method, "GET") != 0) {
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return NULL;
	}

	strcpy(file_name, strtok(NULL, " "));

	//TODO : Parsing the HTTP request message and get method, file, HTTP version.
	//		 You need to do wrong or default page access handling.
	
	
	strcpy(ct, content_type(file_name));

	fclose(clnt_read);
	send_data(clnt_write, ct, file_path);

	printf("%d - [End] Request Handler Function\n", clnt_sock);
}

void send_data(FILE* fp, char* ct, char* file_name)
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char server[] = "Server:Linux Web Server \r\n";
	char cnt_len[SMALL_BUF];
	char cnt_type[SMALL_BUF];
	char buf[BUF_SIZE];
	FILE* send_file;
	int file_size;

	// 서식(format)을 지정하여 문자열을 만든다.
	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);

	send_file = fopen(file_name, "r");
	if (send_file == NULL)
	{
		send_error(fp);
		return;
	}

	// GET file size and construct header for Content-length
	fseek(send_file, 0, SEEK_END);
	file_size = ftell(send_file);
	sprintf(cnt_len, "Content-length:%d\r\n", file_size);
	fseek(send_file, 0, SEEK_SET);

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);

	//TODO : Implement sending binary data (incluidng text and image)
	//		 So that you could see image and text in your website.

	fflush(fp);
	fclose(fp);
}

char* content_type(char* file)
{
	char extension[SMALL_BUF];
	char file_name[SMALL_BUF];
	
	strcpy(file_name, file);
	strtok(file_name, ".");
	strcpy(extension, strtok(NULL, "."));

	//TODO : Checking file extension and return proper content type ("$format/$extension") 
	
}

void send_error(FILE* fp)
{	
	char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
	char server[] = "Server:Linux Web Server \r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[] = "Content-type:text/html\r\n\r\n";

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
	fflush(fp);
}

void error_handling(char* message)
{	
	printf("Error Handling Function Call\n");
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}