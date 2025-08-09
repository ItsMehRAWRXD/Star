#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <sstream>

struct Metrics {
    double revenue = 0.0;
    double cost = 0.0;
    double scale = 0.0;
    double roi = 0.0;
};

struct Code {
    std::string data;
    Metrics metrics;
    double priority = 0.0;
};

class Copilot {
private:
    std::mt19937 rng;
    
public:
    Copilot() : rng(std::random_device{}()) {}
    
    Code payment() {
        Code c;
        c.data = R"(
class Payment {
    std::map<std::string, double> tiers = {{"basic", 9.99}, {"pro", 29.99}, {"enterprise", 99.99}};
    double process(double amount) { return amount * 0.971; }
    double revenue(std::string tier, int users) { return tiers[tier] * users * 12; }
};)";
        c.metrics = {100000.0, -2000.0, 9.0, 50.0};
        c.priority = calc_priority(c.metrics);
        return c;
    }
    
    Code analytics() {
        Code c;
        c.data = R"(
class Analytics {
    std::map<int, double> ltv;
    void track(int user, double revenue) { ltv[user] += revenue; if(ltv[user] > 1000) upsell(user); }
    std::vector<int> churn_risk() { std::vector<int> r; for(auto& [u,v] : ltv) if(v > 100) r.push_back(u); return r; }
    void upsell(int user) {}
};)";
        c.metrics = {50000.0, -500.0, 8.5, 25.0};
        c.priority = calc_priority(c.metrics);
        return c;
    }
    
    Code api() {
        Code c;
        c.data = R"(
class API {
    std::map<std::string, int> usage;
    std::map<std::string, double> revenue;
    bool request(std::string key) { 
        usage[key]++; 
        if(usage[key] > 1000) { revenue[key] += 0.01; return true; }
        return false;
    }
    double total() { double t = 0; for(auto& [k,v] : revenue) t += v; return t; }
};)";
        c.metrics = {75000.0, -1000.0, 9.5, 75.0};
        c.priority = calc_priority(c.metrics);
        return c;
    }
    
    Code optimizer() {
        Code c;
        c.data = R"(
class Optimizer {
    void cache() {}
    void scale() {}
    void compress() {}
    double savings() { return 18000.0; }
};)";
        c.metrics = {0.0, 18000.0, 8.0, 10.0};
        c.priority = calc_priority(c.metrics);
        return c;
    }
    
    Code marketplace() {
        Code c;
        c.data = R"(
class Marketplace {
    double commission = 0.15;
    std::vector<double> transactions;
    void process(double amount) { 
        double fee = amount * commission;
        transactions.push_back(fee);
    }
    double total() { double t = 0; for(double f : transactions) t += f; return t; }
};)";
        c.metrics = {500000.0, -10000.0, 10.0, 50.0};
        c.priority = calc_priority(c.metrics);
        return c;
    }
    
    std::vector<Code> suggest(const std::string& input) {
        std::vector<Code> codes;
        if (input.find("pay") != std::string::npos) codes.push_back(payment());
        if (input.find("track") != std::string::npos) codes.push_back(analytics());
        if (input.find("api") != std::string::npos) codes.push_back(api());
        if (input.find("cost") != std::string::npos) codes.push_back(optimizer());
        if (input.find("market") != std::string::npos) codes.push_back(marketplace());
        
        std::sort(codes.begin(), codes.end(), [](const Code& a, const Code& b) {
            return a.priority > b.priority;
        });
        
        return codes;
    }
    
    std::string report(const std::vector<Code>& codes) {
        std::stringstream ss;
        double total_revenue = 0, total_cost = 0;
        for (const auto& c : codes) {
            total_revenue += c.metrics.revenue;
            total_cost += c.metrics.cost;
        }
        ss << "Revenue: $" << total_revenue << " Cost: $" << total_cost << " Net: $" << (total_revenue + total_cost);
        return ss.str();
    }
    
private:
    double calc_priority(const Metrics& m) {
        return (m.revenue * 0.4) + (m.roi * 1000 * 0.3) + (m.scale * 1000 * 0.2) + (m.cost * 0.1);
    }
};

int main() {
    Copilot c;
    
    std::vector<std::string> inputs = {"payment", "analytics", "api", "cost", "marketplace"};
    
    for (const auto& input : inputs) {
        auto codes = c.suggest(input);
        if (!codes.empty()) {
            std::cout << codes[0].data << std::endl;
            std::cout << c.report(codes) << std::endl << std::endl;
        }
    }
    
    return 0;
}