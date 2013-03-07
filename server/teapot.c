/**
 * I'm a teapot.
 *
 * David Collien 2013
 * For use on a Raspberry Pi, concealed inside a teapot.
 * This is a HTTP Teapot Server. In order for this device to not to be confused with a HTCPCP server,
 * the HTTP Teapot Server will respond with HTTP Status 418: I'm a teapot. See RFC 2324.
 **/

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <event.h>
#include <evhttp.h>

#include <signal.h>

#define BUFFER_SIZE 1024

void brokenPipe(int signum);
void teapotRequestHandler(struct evhttp_request *request, void *arg);

void teaEncrypt(uint32_t* v, uint32_t* k);
void teaDecrypt(uint32_t* v, uint32_t* k);

int main(int argc, char **argv) {
   short          http_port = 8000;
   char          *http_addr = "127.0.0.1";
   struct evhttp *http_server = NULL;

   if (argc > 1) {
      http_addr = argv[1];
      if (argc > 2) {
         http_port = atoi(argv[2]);
      }
   }

   // don't exit on broken pipe (just fail with message)
   signal(SIGPIPE, brokenPipe);

   // start the eventlib http server
   event_init();
   http_server = evhttp_start(http_addr, http_port);
   evhttp_set_gencb(http_server, teapotRequestHandler, NULL);

   fprintf(stderr, "Teapot Server started on %s port %d\n", http_addr, http_port);
   event_dispatch();

   fprintf(stderr, "Teapot Server Died\n");

   return EXIT_SUCCESS;
}

void brokenPipe(int signum) {
   fprintf(stderr, "Broken Spout\n");
}

void teapotRequestHandler(struct evhttp_request *request, void *arg) {
   //fprintf(stderr, "%s\n", request->uri);

   char contentLength[BUFFER_SIZE] = "";
   char responseText[BUFFER_SIZE] = "I'm a teapot\n";
   size_t length = strlen(responseText);

   struct evbuffer *responseBuffer = evbuffer_new();

   evhttp_add_header(request->output_headers, "Content-Type", "text/plain");

   sprintf(contentLength, "%d", (unsigned int)length);
   evbuffer_add(responseBuffer, responseText, length);
   evhttp_send_reply(request, 418, "I'm a teapot", responseBuffer);
   evbuffer_free(responseBuffer);

   return;
}


// TEA implementation from David Wheeler and Roger Needham, TEA, a Tiny Encryption Algorithm. (1995)
void teaEncrypt(uint32_t* v, uint32_t* k) {
   uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
   uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
   uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
   for (i=0; i < 32; i++) {                       /* basic cycle start */
      sum += delta;
      v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
      v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);  
   }                                              /* end cycle */
   v[0]=v0; v[1]=v1;
}
 
void teaDecrypt(uint32_t* v, uint32_t* k) {
   uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
   uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
   uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
   for (i=0; i<32; i++) {                         /* basic cycle start */
      v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
      v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
      sum -= delta;                                   
   }                                              /* end cycle */
   v[0]=v0; v[1]=v1;
}
