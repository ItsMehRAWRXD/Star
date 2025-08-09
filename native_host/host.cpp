#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "../self_learning_agent/src/agent.hpp"
#include "../self_learning_agent/src/strategies.hpp"
#include "../self_learning_agent/src/http.hpp"

static std::string read_message() {
    uint32_t len = 0;
    if (fread(&len, 4, 1, stdin) != 1) return {};
    std::string buf; buf.resize(len);
    if (len > 0) fread(&buf[0], 1, len, stdin);
    return buf;
}

static void write_message(const std::string& s) {
    uint32_t len = static_cast<uint32_t>(s.size());
    fwrite(&len, 4, 1, stdout);
    if (len > 0) fwrite(s.data(), 1, len, stdout);
    fflush(stdout);
}

static std::string extract(const std::string& json, const std::string& key, const std::string& defv) {
    // naive extractor: "key":"value"
    std::string pat = "\"" + key + "\"";
    size_t p = json.find(pat);
    if (p == std::string::npos) return defv;
    p = json.find(':', p);
    if (p == std::string::npos) return defv;
    // skip spaces
    while (p < json.size() && (json[p] == ':' || json[p] == ' ')) ++p;
    if (p >= json.size()) return defv;
    if (json[p] == '"') {
        size_t q1 = p + 1; size_t q2 = json.find('"', q1);
        if (q2 == std::string::npos) return defv;
        return json.substr(q1, q2 - q1);
    } else {
        size_t q = p;
        while (q < json.size() && (isdigit((unsigned char)json[q]) || json[q]=='-')) ++q;
        return json.substr(p, q - p);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    AgentConfig cfg;
    SelfLearningAgent agent(cfg);

    while (true) {
        std::string msg = read_message();
        if (msg.empty()) break;
        std::string task = extract(msg, "task", "");
        std::string policy = extract(msg, "policy", "bandit");
        std::string maxStepsStr = extract(msg, "maxSteps", "8");
        int maxSteps = 8; try { maxSteps = std::stoi(maxStepsStr); } catch (...) {}
        if (task.empty()) {
            write_message("{\"error\":\"missing task\"}");
            continue;
        }
        cfg.policy_mode = policy.empty() ? cfg.policy_mode : policy;
        SelfLearningAgent local(cfg);
        RunResult r = local.run(task, maxSteps);
        std::ostringstream out;
        out << "{\"output\":\"";
        for (char c : r.output) {
            if (c == '"') out << "\\\""; else if (c=='\\') out << "\\\\"; else if (c=='\n') out << "\\n"; else out << c;
        }
        out << "\",\"strategy\":\"" << r.strategy_name << "\",\"score\":" << r.success_score << ",\"latency\":" << r.latency_s << "}";
        write_message(out.str());
    }
    return 0;
}