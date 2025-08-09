#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <random>
#include <cmath>

struct HeatCell {
    double heat = 0.0;
    double click_rate = 0.0;
    double revenue = 0.0;
    int x, y;
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
    bool running = true;
    
public:
    HeatGrid(int w, int h) : width(w), height(h) {
        grid.resize(h);
        for (int y = 0; y < h; ++y) {
            grid[y].resize(w);
            for (int x = 0; x < w; ++x) {
                grid[y][x].x = x;
                grid[y][x].y = y;
            }
        }
    }
    
    void set_heat(int x, int y, double heat) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x].heat = heat;
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

class RealtimeAdSystem {
private:
    HeatGrid grid;
    std::mt19937 rng;
    std::vector<AdZone> ad_zones;
    
public:
    RealtimeAdSystem() : grid(50, 50), rng(std::random_device{}()) {}
    
    void inject_user_activity() {
        int x = rng() % 50;
        int y = rng() % 50;
        double heat = 10 + (rng() % 90);
        grid.set_heat(x, y, heat);
    }
    
    void optimize_placements() {
        auto hot_zones = grid.get_hot_zones(40.0);
        
        ad_zones.clear();
        for (auto& zone : hot_zones) {
            AdZone ad;
            ad.x = zone.first;
            ad.y = zone.second;
            ad.w = 3;
            ad.h = 3;
            ad.roi = grid.get_zone_revenue(ad.x, ad.y, ad.w, ad.h);
            ad.bid = ad.roi * 0.8;
            ad.active = ad.roi > 5.0;
            
            if (ad.active) {
                ad_zones.push_back(ad);
            }
        }
        
        std::sort(ad_zones.begin(), ad_zones.end(), [](const AdZone& a, const AdZone& b) {
            return a.roi > b.roi;
        });
    }
    
    std::vector<AdZone> get_top_zones(int limit = 5) {
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
    
    void run_cycle() {
        for (int i = 0; i < 20; ++i) {
            inject_user_activity();
        }
        grid.propagate_step();
        optimize_placements();
    }
    
    void run_simulation(int cycles) {
        std::cout << "Heat Propagation Ad System" << std::endl;
        
        for (int cycle = 0; cycle < cycles; ++cycle) {
            run_cycle();
            
            auto top_zones = get_top_zones(3);
            
            std::cout << "Cycle " << cycle + 1 << ":" << std::endl;
            for (int i = 0; i < top_zones.size(); ++i) {
                std::cout << "Zone " << i + 1 << ": (" 
                         << top_zones[i].x << "," << top_zones[i].y << ") "
                         << "ROI: $" << top_zones[i].roi 
                         << " Bid: $" << top_zones[i].bid << std::endl;
            }
            
            std::cout << "Total Revenue: $" << total_revenue() << std::endl;
            std::cout << "---" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        grid.stop();
    }
};

int main() {
    RealtimeAdSystem system;
    system.run_simulation(10);
    return 0;
}