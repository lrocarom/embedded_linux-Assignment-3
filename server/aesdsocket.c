#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>



int socket_fd;
int client_fd;
bool exec = true;
char *routeName = "/var/tmp/aesdsocketdata";

int log_client_message(int fd){

    
    char buffer_packet[1024];

	
	FILE *fp = fopen(routeName, "a+");

	if (fp == NULL)

	{	    
	    syslog(LOG_ERR,"The input file don't exists ");

	    return -1;

	 }

	syslog(LOG_DEBUG,"Writting to the input file ... ");

	int message_lenght = 0;

	buffer_packet[message_lenght] = '\0';

	
	while ((message_lenght = recv(fd, buffer_packet, sizeof(buffer_packet) - 1 , 0)) > 0) {


		syslog(LOG_INFO,"Receiving this to file  %s", buffer_packet);


	     fwrite(buffer_packet, message_lenght, 1, fp);

		if (strchr(buffer_packet, '\n')){

        	fflush(fp);


    		char buffer_response[2048] = {0};  // Initialize to zero

    		fseek(fp, 0, SEEK_SET);

			while ((message_lenght = fread(buffer_response, 1, sizeof(buffer_response), fp)) > 0) {


				syslog(LOG_INFO,"Sending this   %s", buffer_response);


	    		send(fd, buffer_response, message_lenght, 0); // Send only the received message length

			}

 
			break; // Exit loop after finding a newline
		}

	}


	fclose(fp); // close the file

	return 0;

}


static void signal_handler( int signal_name){

	if (signal_name == SIGINT || signal_name == SIGTERM){

		syslog(LOG_INFO, "Caught signal, exiting");

		if (socket_fd != -1){
			shutdown(socket_fd, 2);
			close(socket_fd	);
	    
	    }

	    if (client_fd != -1){
		 shutdown(client_fd, 2);
	     close(client_fd);
		
		}
		exec = false;
		remove(routeName);
	}

}


int main(int argc, char *argv[]) {


	int c;

	int enable_daemon = 0;

	while ((c = getopt(argc, argv, "d")) != -1) {
	  
	    switch (c) {
	  
	      case 'd':
	  
	        enable_daemon = 1;
	  
	        break;
	
	  
	    }
	 }

	if (enable_daemon) {
		int pid = fork();

		if (pid == -1) {
			syslog(LOG_ERR, "Failed to create the daemon");
			return -1;
		} else if (pid == 0) {
			syslog(LOG_INFO, "Daemon created succesfull");
		}
		else{
			return 0;

		}
	}
  


	signal(SIGINT, signal_handler);

    signal(SIGTERM, signal_handler);

	struct addrinfo info;

	memset(&info, 0, sizeof(info));
	
	info.ai_flags = AI_PASSIVE;

	info.ai_family = AF_INET;
	
	info.ai_socktype = SOCK_STREAM;

	struct addrinfo *info_res;

	socket_fd = socket(AF_INET,SOCK_STREAM,0);

	if (socket_fd == -1)

	{
		perror("server: get socket");
		return -1;
	}
	int opt = 1;  // option for setsockopt


	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
		return -1;
    }
	
	if(getaddrinfo(NULL,"9000", &info, &info_res) != 0)
	
	{
		perror("server: get address info");
		return -1;
	} 

	if(bind(socket_fd, info_res->ai_addr  ,sizeof(struct sockaddr)) != 0)

	{
		perror("server: bind socket");		
		return -1;
	}

	if(listen(socket_fd, 10) != 0)

	{
		perror("server: error lisening");		
		return -1;
	}

	freeaddrinfo(info_res);


	while (exec){

		struct sockaddr_in info_response;


        socklen_t addr_len = sizeof(info_response);



        client_fd = accept(socket_fd, (struct sockaddr *)&info_response, &addr_len);

		if (client_fd == -1) {
        
            perror("server: accept");
        
            continue; 
        
        }

		   // Log accepted connection
        char client_ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &info_response.sin_addr, client_ip, INET_ADDRSTRLEN);

        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

		 int result = log_client_message(client_fd);

		 close(client_fd); // Close the client connection


		 syslog(LOG_INFO,"Closing connection from %s", client_ip);

	}	


	return 0;

}