#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
#include "Lexer.h"
#include "LogError.h"
#include "Parser.h"
#include "../buffer/buffer.h"

class HttpRequest {
public:
    enum PARSE_STATE {
        REQUEST_LINE,   //正在解析首行
        HEADERS,    //请求头
        BODY,   //请求体
        FINISH, //解析完成  
    };

    HttpRequest() { Init(); }
    ~HttpRequest() = default;
    void Init();
    bool parse(Buffer& buff);
    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;
    bool IsKeepAlive() const;
    bool getIsFindCompileButton();

private:
    bool isFindCompileButton;
    bool ParseRequestLine_(const std::string& line);
    void ParseHeader_(const std::string& line);
    void ParseBody_(const std::string& line);
    void ParsePath_();
    void ParsePost_();
    std::string ParseFromUrlencoded_();
    bool svParser(const std::string& codeText); //解析SystemVerilog源代码
    PARSE_STATE state_; //解析状态
    std::string method_, path_, version_, body_;    //解析方法，请求路径，协议版本，请求体
    std::unordered_map<std::string, std::string> header_;   //请求头
    std::unordered_map<std::string, std::string> post_; //post请求表单数据

    static const std::unordered_set<std::string> DEFAULT_HTML;  //默认网页
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;
    static int ConverHex(char ch);  //转换16进制
};
void writeParserResult(string res);

#endif //HTTP_REQUEST_H