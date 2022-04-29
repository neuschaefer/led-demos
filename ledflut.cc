#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "led-matrix.h"

#define BUFSIZE 2048

#define WIDTH  3
#define HEIGHT 2

using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

uint32_t* pixels;
volatile int running = 1;
volatile int server_sock;

float intensity = 0.6f;

Canvas* canvas;

void cleanup_matrix(){
  canvas->Clear();
  delete canvas;
}

void set_pixel(unsigned x, unsigned y, uint8_t r, uint8_t g, uint8_t b){
  //r = (uint8_t)(((float)r)*0.2);
  //g = (uint8_t)(((float)g)*0.2);
  //b = (uint8_t)(((float)b)*0.2);
  canvas->SetPixel((WIDTH*32-1) - x, (HEIGHT*32-1) - y, r*intensity, g*intensity, b*intensity);
}

void * handle_client(void *s){
   int sock = *(int*)s;
   char buf[BUFSIZE];
   int read_size, read_pos = 0;
   uint32_t x,y;
   uint8_t r, g, b;
   while(running && (read_size = recv(sock , buf + read_pos, sizeof(buf) - read_pos , 0)) > 0){
      read_pos += read_size;
      int found = 1;
      while (found){
         found = 0;
         for (int i = 0; i < read_pos; i++){
            if (buf[i] == '\n'){
               buf[i] = 0;
               if(sscanf(buf,"PX %u %u %2hhx%2hhx%2hhx", &x, &y, &r, &g, &b) == 5){
                  set_pixel(x, y, r, g, b);
               }
               else if(!strncmp(buf, "SIZE", 4)){
                  static const char out[] = "SIZE 96 64";
                  printf("SIZE requested\n");
                  send(sock, out, sizeof(out)-1, MSG_DONTWAIT | MSG_NOSIGNAL);
               } else {
                  printf("Client send corrupted shit, throwing them out.\n");
                  printf("\tthey said: \"%s\"\n", buf);
                  goto out;
               }
               int offset = i + 1;
               int count = read_pos - offset;
               if (count > 0)
                  memmove(buf, buf + offset, count); // TODO: ring buffer?
               read_pos -= offset;
               found = 1;
               break;
            }
         }
         if (sizeof(buf) - read_pos == 0){ // received only garbage for a whole buffer. start over!
            buf[sizeof(buf) - 1] = 0;
            printf("GARBAGE BUFFER.\n");
            read_pos = 0;
         }
      }
   }

out:
   close(sock);
   printf("Client disconnected\n");
   return 0;
}

void * handle_clients(void * foobar){
   pthread_t thread_id;
   int client_sock;
   socklen_t addr_len;
   struct sockaddr_in addr;
   addr_len = sizeof(addr);
   struct timeval tv;
   
   printf("Starting Server...\n");
   
   server_sock = socket(PF_INET, SOCK_STREAM, 0);

   tv.tv_sec = 10;
   tv.tv_usec = 0;

   addr.sin_addr.s_addr = INADDR_ANY;
   addr.sin_port = htons(9737);
   addr.sin_family = AF_INET;

   if (server_sock == -1){
      perror("socket() failed");
      return 0;
   }
   int sockval = 1;
   if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &sockval, sizeof(int)) < 0)
      printf("setsockopt(SO_REUSEADDR) failed\n");
   if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEPORT, &sockval, sizeof(int)) < 0)
      printf("setsockopt(SO_REUSEPORT) failed\n");

   if(bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1){
      perror("bind() failed");
      return 0;
   }

   if (listen(server_sock, 3) == -1){
      perror("listen() failed");
      return 0;
   }
   printf("Listening...\n");

   setsockopt(server_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
   setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
   running = 1;
   while(running){
      client_sock = accept(server_sock, (struct sockaddr*)&addr, &addr_len);
      if(client_sock > 0){
         printf("Client %s connected\n", inet_ntoa(addr.sin_addr));
         if( pthread_create( &thread_id , NULL ,  handle_client , (void*) &client_sock) < 0)
         {
            close(client_sock);
            perror("could not create thread");
         }
      }
   }
   close(server_sock);
   return 0;
}

int main(int argc, char *argv[]) {

  if(argc >= 2){
	int ity = atoi(argv[1]);
	printf("Got intensity %d\n", ity);
	if(ity != 0){
	   intensity = ((float)ity/255.0f);
	   printf("Setting to %f percent\n", intensity);
	}
  }

  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  RGBMatrix::Options options;
  options.rows = 32;    // A 32x32 display. Use 16 when this is a 16x32 display.
  options.chain_length = WIDTH;  // Number of boards chained together.
  options.parallel = HEIGHT;      // Number of chains in parallel (1..3).
  Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &options);

  canvas->Clear();

  pthread_t thread_id;
  if(pthread_create(&thread_id , NULL, handle_clients , NULL) < 0){
    perror("could not create thread");
    cleanup_matrix();
    return 1;
  }

  pthread_join(thread_id, NULL);
  close(server_sock);
  cleanup_matrix();
  return 0;
}

