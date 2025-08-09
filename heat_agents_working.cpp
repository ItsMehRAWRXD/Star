#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>

struct HeatCell {
    double heat = 0.0;
    double revenue = 0.0;
};

struct AdZone {
    int x, y;
    double roi = 0.0;
    double bid = 0.0;
};

class HeatSystem {
private:
    std::vector<std::vector<HeatCell>> grid;
    std::vector<AdZone> zones;
    std::mt19937 rng;
    int size = 20;
    
public:
    HeatSystem() : rng(std::random_device{}()) {
        grid.resize(size, std::vector<HeatCell>(size));
    }
    
    void inject_heat() {
        for (int i = 0; i < 30; ++i) {
            int x = rng() % size;
            int y = rng() % size;
            double heat = 20 + (rng() % 80);
            grid[y][x].heat = heat;
            grid[y][x].revenue = heat * 0.1;
        }
    }
    
    void propagate() {
        auto old_grid = grid;
        for (int y = 1; y < size - 1; ++y) {
            for (int x = 1; x < size - 1; ++x) {
                double avg = (old_grid[y-1][x].heat + old_grid[y+1][x].heat + 
                             old_grid[y][x-1].heat + old_grid[y][x+1].heat) / 4.0;
                grid[y][x].heat = grid[y][x].heat * 0.6 + avg * 0.4;
                grid[y][x].revenue = grid[y][x].heat * 0.1;
            }
        }
    }
    
    void find_hot_zones() {
        zones.clear();
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (grid[y][x].heat > 30.0) {
                    AdZone zone;
                    zone.x = x;
                    zone.y = y;
                    zone.roi = grid[y][x].revenue * 10;
                    zone.bid = zone.roi * 0.7;
                    zones.push_back(zone);
                }
            }
        }
        
        std::sort(zones.begin(), zones.end(), [](const AdZone& a, const AdZone& b) {
            return a.roi > b.roi;
        });
    }
    
    void run_cycle(int cycle) {
        inject_heat();
        propagate();
        find_hot_zones();
        
        std::cout << "Cycle " << cycle << ":" << std::endl;
        
        int count = std::min(3, (int)zones.size());
        for (int i = 0; i < count; ++i) {
            std::cout << "Zone " << i + 1 << ": (" << zones[i].x << "," << zones[i].y << ") "
                     << "ROI: $" << zones[i].roi << " Bid: $" << zones[i].bid << std::endl;
        }
        
        double total = 0;
        for (const auto& zone : zones) {
            total += zone.roi;
        }
        std::cout << "Total Revenue: $" << total << std::endl;
        std::cout << "Active Zones: " << zones.size() << std::endl;
        std::cout << "---" << std::endl;
    }
};

int main() {
    HeatSystem system;
    
    std::cout << "Background Agent Heat Propagation for Ad Placement" << std::endl;
    std::cout << "Heat patterns propagate to identify optimal ad zones" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    for (int i = 1; i <= 8; ++i) {
        system.run_cycle(i);
    }
    
    return 0;
}