#pragma once
#include <string>
#include <vector>
#include <array>
#include <random>
#include <functional>

namespace BranchTetris {

constexpr int GRID_WIDTH = 10;
constexpr int GRID_HEIGHT = 20;

enum class CellType : char {
    EMPTY = ' ',
    STABLE = '#',
    FALLING = 'x',
    WRONG = '?',
    PATTERN = 'P'
};

struct Cell {
    CellType type = CellType::EMPTY;
    std::string data;
    int stability = 0;
    bool marked_for_clear = false;
};

struct Block {
    std::vector<std::vector<bool>> shape;
    std::string code_fragment;
    int x, y;
    bool is_wrong_info;
    int fall_speed;
};

class TetrisGrid {
private:
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> grid;
    std::vector<Block> falling_blocks;
    std::mt19937 rng;
    int score = 0;
    
public:
    TetrisGrid() : rng(std::random_device{}()) { clear_grid(); }
    
    void clear_grid();
    void spawn_wrong_info(const std::string& bad_code);
    void spawn_pattern_block(const std::string& pattern);
    void update();
    void gravity_step();
    void clear_lines();
    void clear_wrong_info();
    
    bool place_block(const Block& block);
    bool can_place(const Block& block, int x, int y);
    void mark_wrong_for_clearing();
    
    std::string render_grid();
    std::string extract_stable_code();
    std::vector<std::string> get_complete_patterns();
    
    int get_score() const { return score; }
    bool is_game_over() const;
};

class BranchProcessor {
private:
    TetrisGrid grid;
    std::vector<std::function<std::string(const std::string&)>> validators;
    std::vector<std::function<Block(const std::string&)>> block_generators;
    
public:
    void register_validator(std::function<std::string(const std::string&)> validator);
    void register_block_gen(std::function<Block(const std::string&)> gen);
    
    void feed_code(const std::string& code);
    void process_step();
    std::string get_cleaned_code();
    
    void inject_wrong_info(const std::string& noise);
    void stabilize_pattern(const std::string& pattern);
    
    std::string render() { return grid.render_grid(); }
    int get_score() { return grid.get_score(); }
};

// Block generators for different code patterns
namespace BlockGen {
    Block if_block(const std::string& condition);
    Block loop_block(const std::string& loop_code);
    Block function_block(const std::string& func_code);
    Block wrong_info_block(const std::string& noise);
    Block pattern_block(const std::string& pattern);
}

// Validators that detect wrong information
namespace Validators {
    std::string check_syntax(const std::string& code);
    std::string check_logic(const std::string& code);
    std::string check_security(const std::string& code);
    std::string check_patterns(const std::string& code);
}

// Tetris-like shapes for code blocks
namespace Shapes {
    extern const std::vector<std::vector<bool>> I_SHAPE;  // |
    extern const std::vector<std::vector<bool>> O_SHAPE;  // []
    extern const std::vector<std::vector<bool>> T_SHAPE;  // T
    extern const std::vector<std::vector<bool>> L_SHAPE;  // L
    extern const std::vector<std::vector<bool>> Z_SHAPE;  // Z
}

class CodeCleaner {
private:
    BranchProcessor processor;
    std::string accumulated_code;
    
public:
    void input_stream(const std::string& raw_code);
    void run_cleaning_cycle();
    std::string output_clean();
    
    void visualize_process();
    void auto_clean(const std::string& messy_code);
    
    struct Stats {
        int blocks_cleared = 0;
        int patterns_formed = 0;
        int wrong_info_removed = 0;
        int clean_lines = 0;
    } stats;
};

}