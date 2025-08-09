#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct RunResult {
    std::string output;
    double success_score{0.0};
    double latency_s{0.0};
    std::string strategy_name;
};

struct StrategyMetrics {
    int successes{0};
    int attempts{0};
    double total_latency_s{0.0};
    double success_rate() const { return attempts > 0 ? static_cast<double>(successes) / attempts : 0.0; }
    double avg_latency() const { return attempts > 0 ? total_latency_s / attempts : 0.0; }
};

struct AgentConfig {
    std::string policy_mode{"bandit"};
    double epsilon{0.15};
    double latency_weight{0.25};
    double success_weight{0.75};
    double max_latency_s{10.0};
    double min_success_rate{0.6};
    int eval_interval_steps{2};
    int cooldown_steps{1};
};

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual std::string name() const = 0;
    virtual bool supports(const std::string& task) const { (void)task; return true; }
    virtual void warmup(const std::string& task) { (void)task; }
    virtual RunResult run(const std::string& task) = 0;
    virtual std::unordered_map<std::string, std::string> handoff_state() const { return {}; }
    virtual void accept_handoff(const std::unordered_map<std::string, std::string>&) {}
};

class SelfLearningAgent {
public:
    explicit SelfLearningAgent(const AgentConfig& cfg);
    RunResult run(const std::string& task, int max_steps);

private:
    AgentConfig cfg_;
    std::vector<std::unique_ptr<Strategy>> strategies_;
    std::unordered_map<std::string, StrategyMetrics> metrics_;
    struct Hist { double score; double latency; std::string strategy; };
    std::vector<Hist> history_;
    int cooldown_{0};

    std::vector<Strategy*> candidates(const std::string& task);
    Strategy* choose(const std::string& task);
    Strategy* policy_rules(const std::vector<Strategy*>& cands);
    Strategy* policy_score(const std::vector<Strategy*>& cands);
    Strategy* policy_bandit(const std::vector<Strategy*>& cands);
    bool should_rebranch() const;
    void update_metrics(const RunResult& r);
};