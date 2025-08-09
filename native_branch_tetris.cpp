#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <sstream>

constexpr int GRID_WIDTH = 10;
constexpr int GRID_HEIGHT = 20;

enum class CellType : char {
    EMPTY = ' ', STABLE = '#', FALLING = 'x', WRONG = '?', PATTERN = 'P'
};

struct Cell {
    CellType type = CellType::EMPTY;
    char data = ' ';
    int stability = 0;
    bool marked = false;
};

struct Block {
    std::vector<std::vector<bool>> shape;
    int x = 0, y = 0;
    bool is_wrong = false;
    char symbol = 'x';
};

class NativeTetris {
private:
    std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> grid;
    std::vector<Block> blocks;
    std::mt19937 rng;
    int score = 0;
    int wrong_count = 0;
    
    static const std::vector<std::vector<bool>> shapes[7];
    
public:
    NativeTetris() : rng(std::random_device{}()) {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell.type = CellType::EMPTY;
                cell.data = ' ';
            }
        }
    }
    
    void spawn_block(bool wrong_info = false) {
        Block b;
        b.shape = shapes[rng() % 7];
        b.x = GRID_WIDTH / 2 - 1;
        b.y = 0;
        b.is_wrong = wrong_info;
        b.symbol = wrong_info ? '?' : '#';
        blocks.push_back(b);
        if (wrong_info) wrong_count++;
    }
    
    bool can_place(const Block& b, int x, int y) {
        for (int dy = 0; dy < b.shape.size(); dy++) {
            for (int dx = 0; dx < b.shape[dy].size(); dx++) {
                if (b.shape[dy][dx]) {
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= GRID_WIDTH || ny >= GRID_HEIGHT) return false;
                    if (grid[ny][nx].type != CellType::EMPTY) return false;
                }
            }
        }
        return true;
    }
    
    void place_block(const Block& b) {
        for (int dy = 0; dy < b.shape.size(); dy++) {
            for (int dx = 0; dx < b.shape[dy].size(); dx++) {
                if (b.shape[dy][dx]) {
                    int nx = b.x + dx, ny = b.y + dy;
                    if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
                        grid[ny][nx].type = b.is_wrong ? CellType::WRONG : CellType::STABLE;
                        grid[ny][nx].data = b.symbol;
                        grid[ny][nx].stability = b.is_wrong ? 0 : 10;
                    }
                }
            }
        }
    }
    
    void gravity() {
        for (auto& block : blocks) {
            if (can_place(block, block.x, block.y + 1)) {
                block.y++;
            } else {
                place_block(block);
            }
        }
        blocks.erase(std::remove_if(blocks.begin(), blocks.end(), 
            [this](const Block& b) { return !can_place(b, b.x, b.y + 1); }), blocks.end());
    }
    
    void clear_wrong() {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x].type == CellType::WRONG) {
                    grid[y][x].stability--;
                    if (grid[y][x].stability <= 0) {
                        grid[y][x].type = CellType::EMPTY;
                        grid[y][x].data = ' ';
                        score += 10;
                        wrong_count--;
                    }
                }
            }
        }
    }
    
    void clear_lines() {
        for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
            bool full = true;
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x].type == CellType::EMPTY) {
                    full = false;
                    break;
                }
            }
            if (full) {
                for (int move_y = y; move_y > 0; move_y--) {
                    for (int x = 0; x < GRID_WIDTH; x++) {
                        grid[move_y][x] = grid[move_y - 1][x];
                    }
                }
                for (int x = 0; x < GRID_WIDTH; x++) {
                    grid[0][x].type = CellType::EMPTY;
                    grid[0][x].data = ' ';
                }
                score += 100;
                y++;
            }
        }
    }
    
    void process_patterns() {
        for (int y = 0; y < GRID_HEIGHT - 2; y++) {
            for (int x = 0; x < GRID_WIDTH - 2; x++) {
                bool pattern = true;
                for (int dy = 0; dy < 3; dy++) {
                    for (int dx = 0; dx < 3; dx++) {
                        if (grid[y + dy][x + dx].type == CellType::EMPTY) {
                            pattern = false;
                            break;
                        }
                    }
                    if (!pattern) break;
                }
                if (pattern) {
                    for (int dy = 0; dy < 3; dy++) {
                        for (int dx = 0; dx < 3; dx++) {
                            if (grid[y + dy][x + dx].type == CellType::WRONG) {
                                grid[y + dy][x + dx].type = CellType::STABLE;
                                grid[y + dy][x + dx].data = '#';
                                grid[y + dy][x + dx].stability = 20;
                                wrong_count--;
                            }
                        }
                    }
                    score += 200;
                }
            }
        }
    }
    
    void update() {
        gravity();
        clear_wrong();
        clear_lines();
        process_patterns();
        
        if (rng() % 10 == 0) spawn_block(false);
        if (rng() % 15 == 0) spawn_block(true);
    }
    
    std::string render() {
        std::stringstream ss;
        ss << "Score: " << score << " Wrong: " << wrong_count << "\n";
        ss << "+----------+\n";
        for (int y = 0; y < GRID_HEIGHT; y++) {
            ss << "|";
            for (int x = 0; x < GRID_WIDTH; x++) {
                ss << grid[y][x].data;
            }
            ss << "|\n";
        }
        ss << "+----------+\n";
        return ss.str();
    }
    
    bool is_stable() {
        return wrong_count == 0 && blocks.empty();
    }
    
    std::string extract_code() {
        std::stringstream code;
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x].type == CellType::STABLE) {
                    if (x == 0) code << "if (condition) {\n";
                    else if (x == GRID_WIDTH - 1) code << "}\n";
                    else code << "    operation();\n";
                }
            }
        }
        return code.str();
    }
    
    int get_score() const { return score; }
    int get_wrong_count() const { return wrong_count; }
};

const std::vector<std::vector<bool>> NativeTetris::shapes[7] = {
    {{1,1,1,1}},
    {{1,1},{1,1}},
    {{0,1,0},{1,1,1}},
    {{1,0,0},{1,1,1}},
    {{0,0,1},{1,1,1}},
    {{1,1,0},{0,1,1}},
    {{0,1,1},{1,1,0}}
};

class BranchGenerator {
private:
    NativeTetris tetris;
    std::string input_code;
    
public:
    void feed_input(const std::string& code) {
        input_code = code;
        for (char c : code) {
            if (c == '{' || c == '}' || c == ';') {
                tetris.spawn_block(false);
            } else if (c == '?' || c == '!') {
                tetris.spawn_block(true);
            }
        }
    }
    
    std::string process() {
        int cycles = 0;
        while (!tetris.is_stable() && cycles < 100) {
            tetris.update();
            cycles++;
        }
        return tetris.extract_code();
    }
    
    void visualize() {
        std::cout << tetris.render();
    }
    
    std::string generate_if_else(const std::string& condition, const std::string& true_block, const std::string& false_block) {
        return "if (" + condition + ") {\n    " + true_block + "\n} else {\n    " + false_block + "\n}";
    }
    
    std::string generate_loop(const std::string& condition, const std::string& body) {
        return "while (" + condition + ") {\n    " + body + "\n}";
    }
    
    std::string generate_switch(const std::string& var, const std::vector<std::string>& cases) {
        std::string result = "switch (" + var + ") {\n";
        for (int i = 0; i < cases.size(); i++) {
            result += "case " + std::to_string(i) + ": " + cases[i] + " break;\n";
        }
        result += "}";
        return result;
    }
    
    std::string obfuscate(const std::string& code, int level) {
        std::string result = code;
        for (int i = 0; i < level; i++) {
            if (result.find("if") != std::string::npos) {
                result = "((condition) ? (true_action) : (false_action))";
            }
        }
        return result;
    }
};

int main() {
    BranchGenerator gen;
    
    std::cout << "Native Branch Tetris Generator\n";
    std::cout << "==============================\n\n";
    
    gen.feed_input("if{condition}?wrong!info;");
    
    std::cout << "Processing...\n";
    gen.visualize();
    
    std::string clean_code = gen.process();
    std::cout << "\nCleaned code:\n" << clean_code << "\n\n";
    
    std::cout << "Generated patterns:\n";
    std::cout << gen.generate_if_else("x > 0", "positive()", "negative()") << "\n\n";
    std::cout << gen.generate_loop("i < 10", "process(i++)") << "\n\n";
    std::cout << gen.generate_switch("choice", {"option1()", "option2()", "option3()"}) << "\n\n";
    
    std::cout << "Obfuscated:\n";
    std::cout << gen.obfuscate("if (x > 0) action();", 2) << "\n";
    
    return 0;
}