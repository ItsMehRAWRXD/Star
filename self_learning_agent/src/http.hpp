#pragma once
#include <string>
#include <vector>

struct Header { std::string name; std::string value; };
struct HttpRequest { std::string url; std::vector<Header> headers; };
struct HttpResponse { bool ok{false}; long status{0}; std::string body; };

class HttpClient {
public:
    HttpResponse get(const HttpRequest& req);
    HttpResponse post(const HttpRequest& req, const std::string& body);
    static std::string escape_json(const std::string& in) {
        std::string out; out.reserve(in.size());
        for (char c : in) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else out.push_back(c);
        }
        return out;
    }
};