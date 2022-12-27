#include <unistd.h>
#include "server/webserver.h"

int main() {
    WebServer server(1316);          
    server.Start();
} 
  