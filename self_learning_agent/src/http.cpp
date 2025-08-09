#include "http.hpp"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

static std::string shell_escape(const std::string& s) {
    std::string out; out.reserve(s.size() + 2);
    out.push_back('\'');
    for (char c : s) {
        if (c == '\'') out += "'\\''"; else out.push_back(c);
    }
    out.push_back('\'');
    return out;
}

static HttpResponse run_curl(const std::string& method, const HttpRequest& req, const std::string& data) {
    std::ostringstream cmd;
    cmd << "curl -s -S -i -X " << method << " ";
    for (auto& h : req.headers) {
        cmd << "-H " << shell_escape(h.name + ": " + h.value) << " ";
    }
    if (method == "POST") {
        cmd << "--data " << shell_escape(data) << " ";
    }
    cmd << shell_escape(req.url);

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return {};
    std::string all;
    char buf[4096];
    while (true) {
        size_t n = fread(buf, 1, sizeof(buf), pipe);
        if (n == 0) break;
        all.append(buf, n);
    }
    pclose(pipe);

    // Separate headers and body
    std::string::size_type sep = all.find("\r\n\r\n");
    std::string headers = sep != std::string::npos ? all.substr(0, sep) : std::string();
    std::string body = sep != std::string::npos ? all.substr(sep + 4) : all;

    long status = 0;
    auto p = headers.find("HTTP/");
    if (p != std::string::npos) {
        auto sp = headers.find(' ', p);
        if (sp != std::string::npos) {
            status = std::strtol(headers.c_str() + sp + 1, nullptr, 10);
        }
    }
    return {status >= 200 && status < 300, status, body};
}

HttpResponse HttpClient::get(const HttpRequest& req) {
    return run_curl("GET", req, "");
}

HttpResponse HttpClient::post(const HttpRequest& req, const std::string& body) {
    return run_curl("POST", req, body);
}