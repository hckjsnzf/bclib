
#ifndef ftp_cu_H
#define ftp_cu_H

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>
#include <ctype.h>
#include <utmpx.h>
#include <sys/select.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <errno.h>
#include <arpa/inet.h> 

#include <unistd.h>


#define DEFAULT_FTPPORT           21
#define Ftp_File_Mask             1


#define Ftp_Mode_Ascii            1
#define Ftp_Mode_Binary           2

#define FTP_TIMEOUT_SECEND  			60

extern int Connect(char* host,char* username,char* password,int port);

extern int Disconnect();

extern int Get_Server_Ver();

extern int ChangeDir(char* newdir);
extern char* GetCurrentDir();

extern int UploadFile(char* localfile,char* remotefile);

extern void PrintMsg();

extern int SetMode(int transmode);/*1-- ASCII£¬2 -- BINARY*/

extern int Mkdir(char * pathname);

extern int Rmdir(char * pathname);

extern int DownloadFile(char* remotefile,char* localfile);

extern int PasvMode();

extern int DownloadFile_pasv(char* remotefile,char* localfile);

extern int UploadFile_pasv(char* localfile,char* remotefile);

extern int CreatePasvConnect();

#endif


