#include "strategies.hpp"
#include "http.hpp"
#include <chrono>
#include <sstream>
#include <cctype>
#include <cstdlib>

static std::string to_query(const std::string& s) {
    std::string q;
    q.reserve(s.size());
    for (char c : s) q.push_back(std::isspace((unsigned char)c) ? '+' : c);
    return q;
}

RunResult RuleBasedPlanner::run(const std::string& task) {
    (void)task;
    std::ostringstream oss;
    oss << "Steps:\n";
    oss << "1) Restate the task\n";
    oss << "2) Identify IO\n";
    oss << "3) Sketch algorithm\n";
    oss << "4) Implement minimal solution\n";
    oss << "5) Add tests\n";
    return {oss.str(), 0.55, 0.0, name()};
}

bool GithubRetriever::supports(const std::string& task) const { (void)task; return true; }

RunResult GithubRetriever::run(const std::string& task) {
    std::string lang = "C++";
    std::string q = to_query(task);
    std::string url = "https://api.github.com/search/code?q=" + q + "+language:" + lang + "&per_page=3";
    HttpClient client;
    HttpRequest req{url};
    req.headers.push_back({"Accept","application/vnd.github+json"});
    const char* tok = std::getenv("GITHUB_TOKEN");
    if (tok) req.headers.push_back({"Authorization", std::string("Bearer ") + tok});
    auto resp = client.get(req);
    std::string output = "GitHub examples (paths only):\n";
    if (resp.ok && !resp.body.empty()) {
        // naive extraction of "path" fields to avoid full JSON dep
        size_t pos = 0; int count = 0;
        while ((pos = resp.body.find("\"path\"", pos)) != std::string::npos && count < 5) {
            size_t colon = resp.body.find(':', pos);
            size_t quote1 = resp.body.find('"', colon+1);
            size_t quote2 = resp.body.find('"', quote1+1);
            if (quote1 != std::string::npos && quote2 != std::string::npos) {
                output += "- "+ resp.body.substr(quote1+1, quote2-quote1-1) + "\n";
                ++count;
            }
            pos = quote2 == std::string::npos ? pos+1 : quote2+1;
        }
        if (count == 0) output += "None found or blocked\n";
    } else {
        output += "None found or blocked\n";
    }
    double score = output.find('-') != std::string::npos ? 0.6 : 0.4;
    return {output, score, 0.0, name()};
}

bool OpenAIReasoner::supports(const std::string& task) const {
    (void)task;
    return std::getenv("OPENAI_API_KEY") != nullptr;
}

RunResult OpenAIReasoner::run(const std::string& task) {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) return {"OpenAI not configured", 0.4, 0.0, name()};
    std::string model = std::getenv("OPENAI_MODEL") ? std::getenv("OPENAI_MODEL") : "gpt-4o-mini";

    // Minimal JSON body
    std::ostringstream body;
    body << "{\"model\":\"" << model << "\",\"messages\":[";
    body << "{\"role\":\"system\",\"content\":\"Provide concise, safe, benign assistance.\"},";
    body << "{\"role\":\"user\",\"content\":\"Task: " << HttpClient::escape_json(task) << "\\nRespond with short steps and a minimal C++ example if relevant.\"}]}";

    HttpClient client;
    HttpRequest req{"https://api.openai.com/v1/chat/completions"};
    req.headers.push_back({"Content-Type","application/json"});
    req.headers.push_back({"Authorization", std::string("Bearer ") + api_key});
    auto resp = client.post(req, body.str());

    std::string out = "OpenAI error or unavailable";
    if (resp.ok) {
        // naive extraction of first message content
        auto key = std::string("\"content\":");
        size_t pos = resp.body.find(key);
        if (pos != std::string::npos) {
            size_t q1 = resp.body.find('"', pos + key.size());
            size_t q2 = resp.body.find('"', q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos && q2 > q1) {
                out = resp.body.substr(q1 + 1, q2 - q1 - 1);
            }
        }
    }
    return {out, 0.7, 0.0, name()};
}