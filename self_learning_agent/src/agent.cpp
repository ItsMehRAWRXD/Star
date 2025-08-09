#include "agent.hpp"
#include "strategies.hpp"
#include <algorithm>
#include <chrono>

static bool is_safe_task(const std::string& t) {
    std::string lower;
    lower.reserve(t.size());
    for (char c : t) lower.push_back(std::tolower(static_cast<unsigned char>(c)));
    const char* blocked[] = {
        "malware","ransomware","dropper","keylogger","botnet","exploit","payload","shellcode","backdoor","evasion",
        "harm","kill","weapon"
    };
    for (auto* k : blocked) {
        if (lower.find(k) != std::string::npos) return false;
    }
    return true;
}

SelfLearningAgent::SelfLearningAgent(const AgentConfig& cfg) : cfg_(cfg) {
    strategies_.emplace_back(std::make_unique<RuleBasedPlanner>());
    strategies_.emplace_back(std::make_unique<GithubRetriever>());
    strategies_.emplace_back(std::make_unique<OpenAIReasoner>());
    for (auto& s : strategies_) metrics_[s->name()] = StrategyMetrics{};
}

RunResult SelfLearningAgent::run(const std::string& task, int max_steps) {
    if (!is_safe_task(task)) {
        return {"Task blocked by safety filter.", 0.0, 0.0, "Safety"};
    }
    Strategy* active = choose(task);
    active->warmup(task);
    RunResult last{"",0.0,0.0,active->name()};

    for (int step = 0; step < max_steps; ++step) {
        auto t0 = std::chrono::high_resolution_clock::now();
        RunResult r = active->run(task);
        auto t1 = std::chrono::high_resolution_clock::now();
        double lat = std::chrono::duration<double>(t1 - t0).count();
        r.latency_s = lat;
        update_metrics(r);
        history_.push_back({r.success_score, r.latency_s, r.strategy_name});
        last = r;

        if (should_rebranch()) {
            if (cooldown_ > 0) { --cooldown_; }
            else {
                Strategy* next = choose(task);
                if (next->name() != active->name()) {
                    auto state = active->handoff_state();
                    next->accept_handoff(state);
                    active = next;
                    cooldown_ = cfg_.cooldown_steps;
                }
            }
        }
        if (r.success_score >= 0.8) break;
    }

    return last;
}

std::vector<Strategy*> SelfLearningAgent::candidates(const std::string& task) {
    std::vector<Strategy*> out;
    for (auto& s : strategies_) if (s->supports(task)) out.push_back(s.get());
    if (out.empty()) out.push_back(strategies_[0].get());
    return out;
}

Strategy* SelfLearningAgent::choose(const std::string& task) {
    auto cands = candidates(task);
    if (cfg_.policy_mode == "rules") return policy_rules(cands);
    if (cfg_.policy_mode == "score") return policy_score(cands);
    return policy_bandit(cands);
}

Strategy* SelfLearningAgent::policy_rules(const std::vector<Strategy*>& cands) {
    for (auto* s : cands) if (s->name() == "OpenAIReasoner") return s;
    for (auto* s : cands) if (s->name() == "GithubRetriever") return s;
    return cands.front();
}

Strategy* SelfLearningAgent::policy_score(const std::vector<Strategy*>& cands) {
    double best = -1e9; Strategy* bests = cands.front();
    for (auto* s : cands) {
        auto& m = metrics_[s->name()];
        double val = cfg_.success_weight * m.success_rate() - cfg_.latency_weight * m.avg_latency();
        if (val > best) { best = val; bests = s; }
    }
    return bests;
}

Strategy* SelfLearningAgent::policy_bandit(const std::vector<Strategy*>& cands) {
    double r = (double)rand() / RAND_MAX;
    if (r < cfg_.epsilon) return cands[rand() % cands.size()];
    return policy_score(cands);
}

bool SelfLearningAgent::should_rebranch() const {
    int n = (int)history_.size();
    if (n == 0) return false;
    int w = std::min(cfg_.eval_interval_steps, n);
    double sum_s = 0.0, sum_l = 0.0;
    for (int i = n - w; i < n; ++i) { sum_s += history_[i].score; sum_l += history_[i].latency; }
    double avg_s = sum_s / w, avg_l = sum_l / w;
    return (avg_s < cfg_.min_success_rate) || (avg_l > cfg_.max_latency_s);
}

void SelfLearningAgent::update_metrics(const RunResult& r) {
    auto& m = metrics_[r.strategy_name];
    m.attempts += 1;
    m.total_latency_s += r.latency_s;
    if (r.success_score >= 0.6) m.successes += 1;
}