#pragma once
#include "agent.hpp"
#include <string>
#include <vector>

class RuleBasedPlanner : public Strategy {
public:
    std::string name() const override { return "RuleBasedPlanner"; }
    RunResult run(const std::string& task) override;
};

class GithubRetriever : public Strategy {
public:
    std::string name() const override { return "GithubRetriever"; }
    bool supports(const std::string& task) const override;
    RunResult run(const std::string& task) override;
};

class OpenAIReasoner : public Strategy {
public:
    std::string name() const override { return "OpenAIReasoner"; }
    bool supports(const std::string& task) const override;
    RunResult run(const std::string& task) override;
};