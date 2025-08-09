#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>
#include <cmath>

struct HeatCell {
    double heat = 0.0;
    double click_rate = 0.0;
    double revenue = 0.0;
    int x, y;
    std::atomic<bool> dirty{false};
};

struct AdZone {
    int x, y, w, h;
    double bid = 0.0;
    double roi = 0.0;
    bool active = false;
};

class HeatGrid {
private:
    std::vector<std::vector<HeatCell>> grid;
    int width, height;
    std::atomic<bool> running{true};
    
public:
    HeatGrid(int w, int h) : width(w), height(h) {
        grid.resize(h, std::vector<HeatCell>(w));
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                grid[y][x].x = x;
                grid[y][x].y = y;
            }
        }
    }
    
    void set_heat(int x, int y, double heat) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x].heat = heat;
            grid[y][x].dirty = true;
        }
    }
    
    double get_heat(int x, int y) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return grid[y][x].heat;
        }
        return 0.0;
    }
    
    void propagate_step() {
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 1; x < width - 1; ++x) {
                if (grid[y][x].dirty) {
                    double avg_heat = (
                        grid[y-1][x].heat + grid[y+1][x].heat +
                        grid[y][x-1].heat + grid[y][x+1].heat
                    ) / 4.0;
                    
                    grid[y][x].heat = grid[y][x].heat * 0.7 + avg_heat * 0.3;
                    grid[y][x].click_rate = std::min(1.0, grid[y][x].heat / 100.0);
                    grid[y][x].revenue = grid[y][x].click_rate * grid[y][x].heat * 0.05;
                }
            }
        }
    }
    
    std::vector<std::pair<int, int>> get_hot_zones(double threshold = 50.0) {
        std::vector<std::pair<int, int>> zones;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x].heat > threshold) {
                    zones.push_back({x, y});
                }
            }
        }
        return zones;
    }
    
    double get_zone_revenue(int x, int y, int w, int h) {
        double total = 0.0;
        for (int dy = 0; dy < h; ++dy) {
            for (int dx = 0; dx < w; ++dx) {
                if (x + dx < width && y + dy < height) {
                    total += grid[y + dy][x + dx].revenue;
                }
            }
        }
        return total;
    }
    
    void stop() { running = false; }
    bool is_running() { return running; }
};

class PropagationAgent {
private:
    HeatGrid* grid;
    std::thread worker;
    std::atomic<bool> active{true};
    
public:
    PropagationAgent(HeatGrid* g) : grid(g) {
        worker = std::thread([this]() {
            while (active && grid->is_running()) {
                grid->propagate_step();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    ~PropagationAgent() {
        active = false;
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    void stop() { active = false; }
};

class HeatInjector {
private:
    HeatGrid* grid;
    std::thread worker;
    std::atomic<bool> active{true};
    std::mt19937 rng;
    
public:
    HeatInjector(HeatGrid* g) : grid(g), rng(std::random_device{}()) {
        worker = std::thread([this]() {
            while (active && grid->is_running()) {
                inject_user_activity();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    ~HeatInjector() {
        active = false;
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    void inject_user_activity() {
        int x = rng() % 100;
        int y = rng() % 100;
        double heat = 10 + (rng() % 90);
        grid->set_heat(x, y, heat);
    }
    
    void stop() { active = false; }
};

class AdOptimizer {
private:
    HeatGrid* grid;
    std::vector<AdZone> ad_zones;
    std::thread worker;
    std::atomic<bool> active{true};
    
public:
    AdOptimizer(HeatGrid* g) : grid(g) {
        worker = std::thread([this]() {
            while (active && grid->is_running()) {
                optimize_placements();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    ~AdOptimizer() {
        active = false;
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    void optimize_placements() {
        auto hot_zones = grid->get_hot_zones(60.0);
        
        ad_zones.clear();
        for (auto& zone : hot_zones) {
            AdZone ad;
            ad.x = zone.first;
            ad.y = zone.second;
            ad.w = 5;
            ad.h = 5;
            ad.roi = grid->get_zone_revenue(ad.x, ad.y, ad.w, ad.h);
            ad.bid = ad.roi * 0.8;
            ad.active = ad.roi > 10.0;
            
            if (ad.active) {
                ad_zones.push_back(ad);
            }
        }
        
        std::sort(ad_zones.begin(), ad_zones.end(), [](const AdZone& a, const AdZone& b) {
            return a.roi > b.roi;
        });
    }
    
    std::vector<AdZone> get_top_zones(int limit = 10) {
        std::vector<AdZone> top(ad_zones.begin(), 
                               ad_zones.begin() + std::min(limit, (int)ad_zones.size()));
        return top;
    }
    
    double total_revenue() {
        double total = 0.0;
        for (const auto& zone : ad_zones) {
            if (zone.active) {
                total += zone.roi;
            }
        }
        return total;
    }
    
    void stop() { active = false; }
};

class RealtimeAdSystem {
private:
    HeatGrid grid;
    PropagationAgent propagator;
    HeatInjector injector;
    AdOptimizer optimizer;
    
public:
    RealtimeAdSystem() : grid(100, 100), propagator(&grid), injector(&grid), optimizer(&grid) {}
    
    void run_simulation(int seconds) {
        auto start = std::chrono::steady_clock::now();
        
        while (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start).count() < seconds) {
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            auto top_zones = optimizer.get_top_zones(5);
            
            std::cout << "Top Ad Zones:" << std::endl;
            for (int i = 0; i < top_zones.size(); ++i) {
                std::cout << "Zone " << i + 1 << ": (" 
                         << top_zones[i].x << "," << top_zones[i].y << ") "
                         << "ROI: $" << top_zones[i].roi 
                         << " Bid: $" << top_zones[i].bid << std::endl;
            }
            
            std::cout << "Total Revenue: $" << optimizer.total_revenue() << std::endl;
            std::cout << "---" << std::endl;
        }
        
        shutdown();
    }
    
    void shutdown() {
        grid.stop();
        propagator.stop();
        injector.stop();
        optimizer.stop();
    }
};

int main() {
    RealtimeAdSystem system;
    
    std::cout << "Real-time Heat Propagation Ad System" << std::endl;
    std::cout << "Running 10-second simulation..." << std::endl;
    
    system.run_simulation(10);
    
    return 0;
}