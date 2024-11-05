#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/queue.h>
#include <time.h>


#define FILE_NAME_LOGS "/var/tmp/aesdsocketdata"
#define TIMER_LOG_SECONDS 10
#define CLOCKID CLOCK_REALTIME  // Reloj en tiempo real

#define ERROR_LOG(_msg_,...)          syslog(LOG_ERR,"ERROR: " _msg_ "\n" , ##__VA_ARGS__)
#define INFO_LOG(_msg_,...)          	syslog(LOG_INFO,"INFO: " _msg_ "\n" , ##__VA_ARGS__)


struct thread_data_socket_connection{
		int socket_fd;
		pthread_mutex_t *mutex;
		bool is_finished;

};

struct entry_list{
	 pthread_t thread;
     struct thread_data_socket_connection* data;
     LIST_ENTRY(entry_list) entries;            
 };

 struct timer_struct_mutex{
	pthread_mutex_t *mutex;
 };


int client_fd;
bool exec = true;
int socket_fd = 0;
pthread_mutex_t mutex_file;
LIST_HEAD(listhead, entry_list) client_list;



int init_timer_log_time();
void process_log_timer(int sig, siginfo_t *si, void *uc);
int create_socket_server(int *socket_fd);
int run_daemon();
static void* log_client_message(void *thread_param);
void check_active_elements_list();
void remove_elements_list();

static void signal_handler( int signal_name);



int main(int argc, char *argv[]) {

	int c;
	int enable_daemon = 0;	
	int rv_function;
	int thread_rv;
 	struct  entry_list *new_entry;

	//get parameters if user wants to run as daemon
	while ((c = getopt(argc, argv, "d")) != -1) {
	    switch (c) {
	      case 'd':
	        enable_daemon = 1;
	        printf("Daemon mode\n");
	        break;
	    }
	 }

	//check if we need to enable daemon
	if (enable_daemon) {
		if((rv_function = run_daemon()) != EXIT_SUCCESS){
			ERROR_LOG("Daemon error status: %d", rv_function);
			return -1;
		}
  
  }

	//set the signal action of SIGINT and SIGTERM
	signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);


	init_timer_log_time(&mutex_file);

	//create a server socket file descriptor
 if ((rv_function = create_socket_server(&socket_fd)) !=  EXIT_SUCCESS){
			ERROR_LOG("Creating socker server status: %d", rv_function);
			return -1;
	}

	if (pthread_mutex_init(&mutex_file, NULL) != 0) {
        ERROR_LOG("File mutex initialization error");
        return -1;
    } 
  
  LIST_INIT(&client_list);

  INFO_LOG("Starting the socket \n");
	while (exec){

		new_entry = malloc(sizeof(struct entry_list)); 
		new_entry->data = malloc(sizeof(struct thread_data_socket_connection));
		new_entry->data->is_finished = false;
		new_entry->data->mutex = &mutex_file;


		struct sockaddr_in info_response;

    socklen_t addr_len = sizeof(info_response);

    new_entry->data->socket_fd = accept(socket_fd, (struct sockaddr *)&info_response, &addr_len);

		if (new_entry->data->socket_fd == -1) {

	    INFO_LOG("server: accepting new conection");

	    continue; 

    }

   // Log accepted connection
    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &info_response.sin_addr, client_ip, INET_ADDRSTRLEN);

    INFO_LOG( "Accepted connection from %s", client_ip);

    
    LIST_INSERT_HEAD(&client_list, new_entry, entries);

  	thread_rv  = pthread_create(&new_entry->thread, NULL, log_client_message,new_entry->data);

  	if (thread_rv == 0) {

	    INFO_LOG("server: accepting new conection");

	    continue; 

    }
	 	INFO_LOG("Closing connection from %s", client_ip);

	 	check_active_elements_list();

	}	


	INFO_LOG("Closing application");


	close(socket_fd);

	remove_elements_list();	

	INFO_LOG("Application closed");

	return 0;

}


int create_socket_server(int *socket_fd){

	struct addrinfo info;
	int optval = 1;

	memset(&info, 0, sizeof(info));
	
	info.ai_flags = AI_PASSIVE;

	info.ai_family = AF_INET;
	
	info.ai_socktype = SOCK_STREAM;

	struct addrinfo *info_res;

	*socket_fd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

	if (*socket_fd == -1)
	{
		ERROR_LOG("server: get socket");
		return -1;
	}

	if (setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
      ERROR_LOG("setsockopt error:");
      close(*socket_fd);
      return EXIT_FAILURE;
  }
	
	if(getaddrinfo(NULL,"9000", &info, &info_res) != 0)	
	{
		ERROR_LOG("server: get address info");
		return -1;
	} 

	if(bind(*socket_fd, info_res->ai_addr  ,sizeof(struct sockaddr)) != 0)
	{
		ERROR_LOG("server: get address info");		
		return -1;
	}

	if(listen(*socket_fd, 10) != 0)
	{
		ERROR_LOG("server: error lisening");		
		return -1;
	}

	freeaddrinfo(info_res);


	return EXIT_SUCCESS;
}



int run_daemon(){

  	pid_t daemon_pid;

    daemon_pid = fork();

    if (daemon_pid == -1) {
      ERROR_LOG("Failed to fork, exit\n");
      exit(1);
    } 

    else if (daemon_pid == 0){
      INFO_LOG("This is child process, continue..\n");
			return 0;
    }

    else {
      exit(EXIT_FAILURE);

    }
}


static void* log_client_message(void *thread_param){


	struct thread_data_socket_connection* data = (struct thread_data_socket_connection* )thread_param;

    
  char buffer_packet[1024];

	
	FILE *fp = fopen(FILE_NAME_LOGS, "a+");

	if (fp == NULL)

	{	    
	    syslog(LOG_ERR,"The input file don't exists ");

			return NULL;
	 }

	syslog(LOG_DEBUG,"Writting to the input file ... ");

	int message_lenght = 0;

	buffer_packet[message_lenght] = '\0';


  if (pthread_mutex_lock (data->mutex) != 0) {
      data->is_finished = true;
      return NULL;
  }

	
	while ((message_lenght = recv(data->socket_fd, buffer_packet, sizeof(buffer_packet) - 1 , 0)) > 0) {

		syslog(LOG_INFO,"Receiving this to file  %s", buffer_packet);

		fwrite(buffer_packet, message_lenght, 1, fp);

		if (strchr(buffer_packet, '\n')){

        fflush(fp);

    		char buffer_response[2048] = {0};  // Initialize to zero

    		fseek(fp, 0, SEEK_SET);

				while ((message_lenght = fread(buffer_response, 1, sizeof(buffer_response), fp)) > 0) {

					syslog(LOG_INFO,"Sending this   %s", buffer_response);

	    		send(data->socket_fd, buffer_response, message_lenght, 0); // Send only the received message length

			}

			break; // Exit loop after finding a newline

		}

	}
	pthread_mutex_unlock (data->mutex);
	close(data->socket_fd);
	fclose(fp); // close the file
	data->is_finished = true;

	return NULL;

}


static void signal_handler( int signal_name){

	if (signal_name == SIGINT || signal_name == SIGTERM){

		syslog(LOG_INFO, "Caught signal, exiting");

		if (socket_fd != -1){

			syslog(LOG_INFO, "Close socket file descriptor");

			close(socket_fd);
	    
	    }
		exec = false;
		remove(FILE_NAME_LOGS);
	}

	syslog(LOG_INFO, "Projects close");


}

void check_active_elements_list(){

		struct  entry_list *iter_entry;

	  iter_entry = LIST_FIRST(&client_list);

	  while(iter_entry != NULL)
	  {
	  		if(iter_entry->data != NULL ){

	  			if(iter_entry->data->is_finished){

	  				printf("Delete element list");

	  				free(iter_entry->data);

			  		free(iter_entry);

	  				pthread_join(iter_entry->thread, NULL); 

	  		}

	  		iter_entry = LIST_NEXT(iter_entry, entries);

	  	}

	  }

}

void remove_elements_list(){

		struct entry_list *iter_entry;

	  iter_entry = LIST_FIRST(&client_list);

	  while(iter_entry != NULL)
	  {
	  		pthread_join(iter_entry->thread, NULL); 

	  		if(iter_entry->data != NULL ){
						close(iter_entry->data->socket_fd);
	  				free(iter_entry->data);

	  		}

	  		free(iter_entry);


	  		iter_entry = LIST_NEXT(iter_entry, entries);

	  }

}


int init_timer_log_time(pthread_mutex_t *a_mutex){


	timer_t timerid;
    struct sigevent sev;
    struct sigaction sa;
    struct itimerspec its;
    struct timer_struct_mutex timer_data;

    timer_data.mutex = a_mutex;

    // Configuración del manejador para la señal del temporizador
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = process_log_timer;  // Asigna la función de manejo de la señal
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        exit(EXIT_FAILURE);
    }

    // Configuración para el temporizador
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timer_data;

    // Crear el temporizador
    if (timer_create(CLOCKID, &sev, &timerid) == -1) {
        exit(EXIT_FAILURE);
    }

    // Configuración del intervalo del temporizador (10 segundos)
    its.it_value.tv_sec = TIMER_LOG_SECONDS;  // Tiempo inicial para el primer vencimiento
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = TIMER_LOG_SECONDS;  // Intervalo de repetición (cada 10 segundos)
    its.it_interval.tv_nsec = 0;

    // Iniciar el temporizador
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        exit(EXIT_FAILURE);
    }

		return 0;
}


void process_log_timer(int sig, siginfo_t *si, void *uc) {

	char outstr[200];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
	   perror("localtime");
	   exit(EXIT_FAILURE);
	}

	if (strftime(outstr, sizeof(outstr), "timestamp:%a, %d %b %Y %T %z\n", tmp) == 0) {
	   ERROR_LOG("strftime returned 0");
	   exit(EXIT_FAILURE);
	}

	FILE *fp = fopen(FILE_NAME_LOGS, "a+");

	if (fp == NULL)

	{	    
	    syslog(LOG_ERR,"The input file don't exists ");

	   exit(EXIT_FAILURE);
	}	

	fwrite(outstr, strlen(outstr), 1, fp);
  fflush(fp);


}
