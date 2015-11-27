#include"main.h"

stats_struct initialize_stats(){
  stats_struct stats;
  stats.requests_denied = 0;
  stats.local_domains_resolved = 0;
  stats.extern_domains_resolved = 0;
  return stats;
}

void print_stats(){
    pthread_mutex_lock(&stats_mutex);
    char * time = NULL;
    asctime_r(&stats.last_time,time);
    int sum = stats.requests_denied + stats.extern_domains_resolved + stats.local_domains_resolved;
    printf("Server start time: %sLocal domains resolved: %d\nExtern domains resolved:%d\nRequests refused: %d\nTotal requests received: %d\nLast information received: %s\n",asctime(stats.start_time),stats.local_domains_resolved,stats.extern_domains_resolved,stats.requests_denied,sum,time);
    pthread_mutex_unlock(&stats_mutex);
}

void statistics() {
    pthread_t reader;
    pthread_mutex_init(&stats_mutex,NULL);	
    stats = initialize_stats();
    char *pipe_name = (char *)malloc(MAX_PIPE_NAME);
    sem_wait(config_mutex);
    strcpy(pipe_name,config->pipe_name);
    sem_post(config_mutex);  
    int fd = open(pipe_name, O_RDONLY);
    struct tm start_time;
    read(fd,&start_time,sizeof(struct tm));
    stats.start_time = &start_time;
    stats.last_time = start_time;
    close(fd);
    printf("\nServer Start time: %s\n",asctime(stats.start_time));
    printf("Started statistics process\n");
    pthread_create(&reader, NULL,reader_code,NULL);
    while(TRUE){
	sleep(15);
	print_stats();
    }
    close(fd);
}

void *reader_code(){
    char *pipe_name = (char *)malloc(MAX_PIPE_NAME);
    sem_wait(config_mutex);
    strcpy(pipe_name,config->pipe_name);
    sem_post(config_mutex);  
    int fd = open(pipe_name, O_RDONLY);
    char aux;
    while(1){
	read(fd,&aux,sizeof(char));
	pthread_mutex_lock(&stats_mutex);
	if(aux == 'l'){
	    stats.local_domains_resolved++;
	}
	else if(aux == 'e'){
	    stats.extern_domains_resolved++;
	}
	else if(aux == 'd'){
	    stats.requests_denied++;
	}
	time_t rawtime;
	time (&rawtime);
	localtime_r(&rawtime,&stats.last_time);
	pthread_mutex_unlock(&stats_mutex);
    }
}
