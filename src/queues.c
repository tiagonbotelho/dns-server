#include"main.h"

void schedule_request(int queue,short dns_id, int sockfd, unsigned char *dns_name, struct sockaddr_in dest){
  dnsrequest request;
  request.mtype = queue;
  memcpy(&request.dest,&dest,sizeof(struct sockaddr_in));
  memcpy(&request.dns_name,dns_name,sizeof(char)*IP_SIZE);
  request.dns_id = dns_id;
  request.sockfd = sockfd;
  msgsnd(requests_queue,&request,sizeof(dnsrequest)-sizeof(long),0);
}

dnsrequest get_request(int queue){
  dnsrequest request;

  if (msgrcv(requests_queue,&request,sizeof(dnsrequest)-sizeof(long),queue,IPC_NOWAIT) == -1) {
    request.dns_id = -1;
  }
  return request;
}

