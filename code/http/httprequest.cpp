#include "httprequest.h"
using namespace std;

const unordered_set<string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const unordered_map<string, int> HttpRequest::DEFAULT_HTML_TAG {
            {"/register.html", 0}, {"/login.html", 1},  };

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::IsKeepAlive() const {
    if(header_.count("Connection") == 1) {
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

bool HttpRequest::parse(Buffer& buff) {
    const char CRLF[] = "\r\n";
    if(buff.ReadableBytes() <= 0) {
        return false;
    }
    while(buff.ReadableBytes() && state_ != FINISH) {   
        const char* lineEnd = search(buff.Peek(), buff.BeginWriteConst(), CRLF, CRLF + 2);
        std::string line(buff.Peek(), lineEnd);
        switch(state_)  //此处各种错误/异常状态没有判断，鲁棒性不强，后期可以继续完善
        {
        case REQUEST_LINE:
            if(!ParseRequestLine_(line)) {
                return false;
            }
            ParsePath_();
            break;    
        case HEADERS:
            ParseHeader_(line);
            if(buff.ReadableBytes() <= 2) {
                state_ = FINISH;
            }
            break;
        case BODY:
            ParseBody_(line);
            break;
        default:
            break;
        }
        if(lineEnd == buff.BeginWrite()) { break; }
        buff.RetrieveUntil(lineEnd + 2);
    }
    return true;
}

void HttpRequest::ParsePath_() {
    if(path_ == "/") {
        path_ = "/index.html"; 
    }
    else {
        for(auto &item: DEFAULT_HTML) { //通过根据当前post请求中用户需要的html页面，返将对应的html地址加入path路径中作为response发送给用户。
            if(item == path_) {
                path_ += ".html";
                break;
            }
        }
    }
}

bool HttpRequest::ParseRequestLine_(const string& line) {
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {   
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    return false;
}

void HttpRequest::ParseHeader_(const string& line) {
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    }
    else {
        state_ = BODY;
    }
}

void HttpRequest::ParseBody_(const string& line) {
    body_ = line;
    ParsePost_();
    state_ = FINISH;
}

int HttpRequest::ConverHex(char ch) {
    if(ch >= 'A' && ch <= 'F') return ch -'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch -'a' + 10;
    return ch;
}

void HttpRequest::ParsePost_() {
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded") {
        ParseFromUrlencoded_();
        if(DEFAULT_HTML_TAG.count(path_)) {
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            if(tag == 0 || tag == 1) { //如果是注册或者登录页面
                if(svParser(post_["username"]))
                    path_ = "/welcome.html";
                else
                    path_ = "/error.html";
            }
        }
    }   
}

bool HttpRequest::svParser(const std::string& codeText){
    SourceManager SM(codeText);
    string *psm = &SM.fd.filememo;
    cout<<*psm<<endl;
    cout<<"------------"<<endl;
    cout << "SM.fd.filesize:" << SM.fd.filememo.size() << endl;
    // Lexer lex(psm, SM.fd.filesize);
    // cout<<endl;
    // Parser par(lex.getTokenVector());
    return true;
}

void HttpRequest::ParseFromUrlencoded_() {
    if(body_.size() == 0) { return; }
    cout<<body_<<endl;
    std::string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;
    std::string codeText;
    for(; i < n; i++) {
        char ch = body_[i];
        //cout<<body_[i];
        // if(body_[i] == '%'){
        //     if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '6'){
        //         codeText.append("&");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '0'){
        //         codeText.append(" ");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '1'){
        //         codeText.append("!");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '2'){
        //         codeText.append("\"");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '3'){
        //         codeText.append("#");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '4'){
        //         codeText.append("$");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '5'){
        //         codeText.append("%");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '7'){
        //         codeText.append("'");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '8'){
        //         codeText.append("(");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == '9'){
        //         codeText.append(")");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == 'A'){
        //         codeText.append("*");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == 'B'){
        //         codeText.append("+");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == 'C'){
        //         codeText.append(",");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '2' && body_[i+2] == 'F'){
        //         codeText.append("/");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '3' && body_[i+2] == 'A'){
        //         codeText.append(":");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '3' && body_[i+2] == 'B'){
        //         codeText.append(";");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '3' && body_[i+2] == 'D'){
        //         codeText.append("=");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '3' && body_[i+2] == 'E'){
        //         codeText.append(">");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '3' && body_[i+2] == 'F'){
        //         codeText.append("?");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '4' && body_[i+2] == '0'){
        //         codeText.append("@");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '5' && body_[i+2] == 'B'){
        //         codeText.append("[");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '5' && body_[i+2] == 'C'){
        //         codeText.append("\\");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '5' && body_[i+2] == 'D'){
        //         codeText.append("]");
        //         i += 2;
        //     }
        //     else if(i < n - 2 && body_[i+1] == '7' && body_[i+2] == 'C'){
        //         codeText.append("|");
        //         i += 2;
        //     }
        //     else
        //         break;
        // }
        // codeText.push_back(body_[i]);
        switch (ch) {
        case '=':
            key = body_.substr(j, i - j);
            j = i + 1;
            break;
        case '+':
            body_[i] = ' ';
            break;
        case '%': //此处应该有bug
            num = ConverHex(body_[i + 1]) * 16 + ConverHex(body_[i + 2]);
            body_[i + 2] = num % 10 + '0';
            body_[i + 1] = num / 10 + '0';
            i += 2;
            break;
        case '&':
            value = body_.substr(j, i - j);
            j = i + 1;
            post_[key] = value;
            break;
        default:
            break;
        }
    }
    cout<<codeText<<endl;
    assert(j <= i);
    if(post_.count(key) == 0 && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

std::string HttpRequest::path() const{
    return path_;
}

std::string& HttpRequest::path(){
    return path_;
}
std::string HttpRequest::method() const {
    return method_;
}

std::string HttpRequest::version() const {
    return version_;
}

std::string HttpRequest::GetPost(const std::string& key) const {
    assert(key != "");
    if(post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}

std::string HttpRequest::GetPost(const char* key) const {
    assert(key != nullptr);
    if(post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}