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
   evhttp_set_gencb(http_server, loggingRequestHandler, NULL);

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
