#include <unistd.h>
#include "server/webserver.h"
#include "Lexer.h"
#include "LogError.h"
#include "Parser.h"
int main() {
    // fs::path fp;
    // //cin>>fp;
    // char path[100];
    // getcwd(path, 100);
    // cout<<path<<endl;
    // fp = "./sv-JTracing/test/test_module.sv";
    // SourceManager SM(fp);
    // string *psm = &SM.fd.filememo;
    // Lexer lex(psm, SM.fd.filesize);
    // cout<<endl;
    // Parser par(lex.getTokenVector());
    WebServer server(4466);          
    server.Start();
} 