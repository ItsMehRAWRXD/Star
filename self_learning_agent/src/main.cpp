#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include "agent.hpp"

static void print_help() {
    std::cout << "Usage: agent <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  --help                        Show help\n";
    std::cout << "  run --task <text> [--policy rules|score|bandit] [--max-steps N]\n";
}

int main(int argc, char** argv) {
    if (argc <= 1 || std::string(argv[1]) == "--help") {
        print_help();
        return 0;
    }

    std::string cmd = argv[1];
    if (cmd == "run") {
        std::string task;
        std::string policy = "bandit";
        int max_steps = 12;
        for (int i = 2; i < argc; ++i) {
            std::string a = argv[i];
            if (a == "--task" && i + 1 < argc) { task = argv[++i]; }
            else if (a == "--policy" && i + 1 < argc) { policy = argv[++i]; }
            else if (a == "--max-steps" && i + 1 < argc) { max_steps = std::stoi(argv[++i]); }
            else if (a == "--help") { print_help(); return 0; }
        }
        if (task.empty()) { std::cerr << "--task is required\n"; return 1; }

        AgentConfig cfg;
        cfg.policy_mode = policy;
        SelfLearningAgent agent(cfg);
        RunResult res = agent.run(task, max_steps);
        std::cout << res.output << "\n";
        return 0;
    }

    print_help();
    return 0;
}