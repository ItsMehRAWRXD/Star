#pragma once
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <unordered_map>
#include <functional>

namespace BranchCore {

struct Point2D { int x, y; };
struct Rect2D { int x, y, w, h; };

struct CodeRegion {
    Rect2D bounds;
    std::string content;
    int complexity;
    std::vector<Point2D> entry_points;
    std::vector<Point2D> exit_points;
};

struct BranchPattern {
    std::string condition;
    std::string true_path;
    std::string false_path;
    Point2D position;
    int weight;
};

class Vision2D {
private:
    std::vector<std::vector<char>> grid;
    int width, height;
    
public:
    Vision2D(int w, int h) : width(w), height(h), grid(h, std::vector<char>(w, ' ')) {}
    
    void place(const std::string& code, Point2D pos);
    void connect(Point2D from, Point2D to, char style = '-');
    std::vector<CodeRegion> analyze_regions();
    std::string render();
    bool can_place(const std::string& code, Point2D pos);
    std::vector<Point2D> find_optimal_positions(int size);
};

class BranchEngine {
private:
    std::mt19937 rng;
    std::vector<std::function<std::string(const BranchPattern&)>> generators;
    Vision2D* vision;
    
public:
    BranchEngine() : rng(std::random_device{}()) {}
    
    void set_vision(Vision2D* v) { vision = v; }
    void register_generator(std::function<std::string(const BranchPattern&)> gen);
    
    std::string generate(const BranchPattern& pattern);
    std::string generate_complex(const std::vector<BranchPattern>& patterns);
    std::vector<BranchPattern> optimize_layout(const std::vector<BranchPattern>& patterns);
    
    // Advanced generation modes
    std::string generate_vectorized(const std::vector<BranchPattern>& patterns);
    std::string generate_obfuscated(const BranchPattern& pattern, int level);
    std::string generate_adaptive(const BranchPattern& pattern, const CodeRegion& context);
};

// Minimal generators
namespace Generators {
    std::string if_else(const BranchPattern& p);
    std::string ternary(const BranchPattern& p);
    std::string switch_table(const BranchPattern& p);
    std::string loop_break(const BranchPattern& p);
    std::string lambda_dispatch(const BranchPattern& p);
    std::string goto_jump(const BranchPattern& p);
    std::string exception_flow(const BranchPattern& p);
    std::string computed_jump(const BranchPattern& p);
}

// Utilities
namespace Utils {
    std::string var_name(int seed = 0);
    std::string obfuscate(const std::string& code, int level);
    std::vector<BranchPattern> parse_conditions(const std::string& input);
    std::string optimize_code(const std::string& code);
}

}