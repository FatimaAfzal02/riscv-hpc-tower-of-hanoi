#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <cstdlib>

#ifdef __riscv
    #define RISCV_TARGET 1
#endif

class TowerOfHanoi {
private:
    int num_disks;
    std::vector<std::stack<int>> rods;
    int moves_count;
    bool verbose;
    const char* rod_names[3];

public:
    TowerOfHanoi(int disks, bool verbose_mode = false)
        : num_disks(disks), moves_count(0), verbose(verbose_mode) {
        rods.resize(3);
        rod_names[0] = "A (Source)";
        rod_names[1] = "B (Aux)";
        rod_names[2] = "C (Dest)";
        for (int i = disks; i >= 1; --i) {
            rods[0].push(i);
        }
    }

    void solve_recursive(int n, int from, int to, int aux) {
        if (n == 1) {
            move_disk(from, to);
            return;
        }
        solve_recursive(n - 1, from, aux, to);
        move_disk(from, to);
        solve_recursive(n - 1, aux, to, from);
    }

    void solve_iterative() {
        long long total_moves = (1LL << num_disks) - 1;
        int pegs[3] = {0, 1, 2};
        if (num_disks % 2 != 0) {
            std::swap(pegs[1], pegs[2]);
        }

        for (long long move = 1; move <= total_moves; ++move) {
            if (move % 2 == 1) {
                move_disk(pegs[0], pegs[1]);
                std::rotate(pegs, pegs + 1, pegs + 3);
            } else {
                int p1 = pegs[1], p2 = pegs[2];
                if (rods[p1].empty() ||
                    (!rods[p2].empty() && rods[p1].top() > rods[p2].top())) {
                    move_disk(p2, p1);
                } else {
                    move_disk(p1, p2);
                }
            }
        }
    }

    void move_disk(int from, int to) {
        moves_count++;
        int disk = rods[from].top();
        rods[from].pop();
        rods[to].push(disk);

        if (verbose) {
            std::cout << "Move " << std::setw(3) << moves_count
                      << ": Disk " << std::setw(2) << disk
                      << " from " << rod_names[from]
                      << " to "   << rod_names[to] << std::endl;
        }
    }

    void reset() {
        moves_count = 0;
        rods.clear();
        rods.resize(3);
        for (int i = num_disks; i >= 1; --i) {
            rods[0].push(i);
        }
    }

    void display_state() {
        std::cout << "\nCurrent State:\n";
        for (int i = 0; i < 3; ++i) {
            std::cout << rod_names[i] << ": ";
            std::stack<int> temp = rods[i];
            std::vector<int> disks_on_rod;
            while (!temp.empty()) {
                disks_on_rod.push_back(temp.top());
                temp.pop();
            }
            for (auto it = disks_on_rod.rbegin(); it != disks_on_rod.rend(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Total moves: " << moves_count << "\n" << std::endl;
    }

    void display_visual() {
        std::cout << "\n";

        int peg_spacing = num_disks * 2 + 3;
        int total_width = 3 * peg_spacing;

        std::vector<std::vector<char>> canvas(num_disks,
                                              std::vector<char>(total_width, ' '));

        for (int rod = 0; rod < 3; ++rod) {
            int peg_x = rod * peg_spacing + peg_spacing / 2;
            for (int row = 0; row < num_disks; ++row) {
                canvas[row][peg_x] = '|';
            }
        }

        for (int rod = 0; rod < 3; ++rod) {
            std::stack<int> temp = rods[rod];
            std::vector<int> disks;
            while (!temp.empty()) {
                disks.push_back(temp.top());
                temp.pop();
            }
            int peg_x = rod * peg_spacing + peg_spacing / 2;
            int row = 0;
            for (auto it = disks.rbegin();
                 it != disks.rend() && row < num_disks; ++it, ++row) {
                int disk_size = *it;
                for (int x = peg_x - disk_size; x <= peg_x + disk_size; ++x) {
                    if (x >= 0 && x < total_width) {
                        canvas[row][x] = '#';
                    }
                }
            }
        }

        for (int row = num_disks - 1; row >= 0; --row) {
            std::cout << "  ";
            for (int col = 0; col < total_width; ++col) {
                if      (canvas[row][col] == '|') std::cout << "|";
                else if (canvas[row][col] == '#') std::cout << "#";
                else                              std::cout << " ";
            }
            std::cout << "\n";
        }

        std::cout << "  ";
        for (int col = 0; col < total_width; ++col) std::cout << "=";
        std::cout << "\n";

        std::cout << "  ";
        {
            int last_pos = 0;
            for (int rod = 0; rod < 3; ++rod) {
                int peg_x  = rod * peg_spacing + peg_spacing / 2;
                int spaces = std::max(0, peg_x - last_pos);
                std::cout << std::string(spaces, ' ') << char('A' + rod);
                last_pos = peg_x + 1;
            }
            std::cout << "\n";
        }

        std::cout << "  ";
        {
            int last_pos = 0;
            const char* labels[3] = {"Src", "Aux", "Dst"};
            for (int rod = 0; rod < 3; ++rod) {
                int peg_x     = rod * peg_spacing + peg_spacing / 2;
                int label_len = (int)strlen(labels[rod]);
                int spaces    = std::max(0, peg_x - last_pos);
                std::cout << std::string(spaces, ' ') << labels[rod];
                last_pos = peg_x + label_len;
            }
            std::cout << "\n" << std::endl;
        }
    }

    bool verify_solution() {
        if (rods[2].size() != (size_t)num_disks) return false;
        std::stack<int> temp = rods[2];
        int expected = 1;
        while (!temp.empty()) {
            if (temp.top() != expected) return false;
            temp.pop();
            expected++;
        }
        return true;
    }
};

class PerformanceBenchmark {
public:
    struct Result {
        int disks;
        long long recursive_time_us;
        long long iterative_time_us;
    };

    static void run_benchmark_up_to(int max_disks) {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "PERFORMANCE BENCHMARK: Recursion vs Iteration" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        std::cout << std::setw(8) << "Disks" << std::setw(18) << "Recursion(us)"
                  << std::setw(18) << "Iteration(us)" << std::setw(18) << "Moves"
                  << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        std::vector<Result> results;
        long long closest_diff = LLONG_MAX;
        int closest_disks = 0;

        for (int d = 1; d <= max_disks; ++d) {
            TowerOfHanoi game(d, false);

            auto t0 = std::chrono::high_resolution_clock::now();
            game.solve_recursive(d, 0, 2, 1);
            auto t1 = std::chrono::high_resolution_clock::now();
            auto recursive_time =
                std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

            game.reset();
            t0 = std::chrono::high_resolution_clock::now();
            game.solve_iterative();
            t1 = std::chrono::high_resolution_clock::now();
            auto iterative_time =
                std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

            Result res = {d, recursive_time, iterative_time};
            results.push_back(res);

            long long diff = std::abs(recursive_time - iterative_time);
            if (diff < closest_diff) {
                closest_diff = diff;
                closest_disks = d;
            }

            std::cout << std::setw(8) << d << std::setw(18) << recursive_time
                      << std::setw(18) << iterative_time << std::setw(18)
                      << ((1LL << d) - 1) << std::endl;
        }

        std::cout << "\n" << std::string(70, '-') << std::endl;
        std::cout << "Closest performance at " << closest_disks << " disks"
                  << " (difference: " << closest_diff << " us)" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
    }

    static void run_benchmark_at(int disks) {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "PERFORMANCE BENCHMARK: Recursion vs Iteration" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        std::cout << "Benchmarking with " << disks << " disks ("
                  << ((1LL << disks) - 1) << " moves)" << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        TowerOfHanoi game(disks, false);

        auto t0 = std::chrono::high_resolution_clock::now();
        game.solve_recursive(disks, 0, 2, 1);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto recursive_time =
            std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        game.reset();
        t0 = std::chrono::high_resolution_clock::now();
        game.solve_iterative();
        t1 = std::chrono::high_resolution_clock::now();
        auto iterative_time =
            std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        std::cout << "Recursive time:  " << recursive_time << " us" << std::endl;
        std::cout << "Iterative time:  " << iterative_time << " us" << std::endl;
        std::cout << "Difference:      "
                  << std::abs((long long)(recursive_time - iterative_time))
                  << " us" << std::endl;

        if (recursive_time < iterative_time && recursive_time > 0) {
            std::cout << "Recursion was " << std::fixed << std::setprecision(1)
                      << (double)iterative_time / recursive_time
                      << "x faster" << std::endl;
        } else if (iterative_time < recursive_time && iterative_time > 0) {
            std::cout << "Iteration was " << std::fixed << std::setprecision(1)
                      << (double)recursive_time / iterative_time
                      << "x faster" << std::endl;
        } else {
            std::cout << "Both methods took the same time" << std::endl;
        }

        std::cout << std::string(70, '=') << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "Tower of Hanoi: Recursion vs Iteration" << std::endl;

    #ifdef RISCV_TARGET
    std::cout << "[Compiled for RISC-V]" << std::endl;
    #endif

    std::cout << std::string(70, '=') << std::endl;

    int  disks          = 4;
    bool verbose        = false;
    bool benchmark_upto = false;
    bool benchmark_at   = false;
    bool visual         = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-b") == 0) {
            benchmark_upto = true;
        } else if (strcmp(argv[i], "-B") == 0) {
            benchmark_at = true;
        } else if (strcmp(argv[i], "-g") == 0) {
            visual = true;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            disks = atoi(argv[++i]);
            if (disks <= 0) {
                std::cerr << "Error: disk count must be positive" << std::endl;
                return 1;
            }
            if (disks > 20) {
                std::cerr << "Warning: Capping at 20 disks (requested "
                          << disks << ")" << std::endl;
                disks = 20;
            }
        } else if (strcmp(argv[i], "-h") == 0) {
            std::cout << "Usage: " << argv[0]
                      << " [-v] [-b] [-B] [-g] [-n disks] [-h]\n"
                      << "  -v: Verbose output (show each move)\n"
                      << "  -b: Benchmark up to N disks (scaling table)\n"
                      << "  -B: Single benchmark at exactly N disks\n"
                      << "  -g: ASCII graphics visualization\n"
                      << "  -n: Number of disks (default: 4, max for -g: 6)\n"
                      << "  -h: This help\n";
            return 0;
        }
    }

    if (visual && disks > 6) {
        std::cout << "Warning: Visual mode limited to 6 disks" << std::endl;
        disks = 6;
    }

    TowerOfHanoi game(disks, verbose);

    if (visual) {
        std::cout << "\nINITIAL STATE:\n" << std::endl;
        game.display_visual();
    }

    std::cout << "DEMO 1: RECURSIVE SOLUTION" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    auto t0 = std::chrono::high_resolution_clock::now();
    game.solve_recursive(disks, 0, 2, 1);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    std::cout << "Completed in " << duration.count() << " us" << std::endl;
    std::cout << "Verified: " << (game.verify_solution() ? "PASS" : "FAIL") << std::endl;

    if (visual)       game.display_visual();
    else if (verbose) game.display_state();

    game.reset();
    std::cout << "\nDEMO 2: ITERATIVE SOLUTION" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    t0 = std::chrono::high_resolution_clock::now();
    game.solve_iterative();
    t1 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);

    std::cout << "Completed in " << duration.count() << " us" << std::endl;
    std::cout << "Verified: " << (game.verify_solution() ? "PASS" : "FAIL") << std::endl;

    if (visual)       game.display_visual();
    else if (verbose) game.display_state();

    if (benchmark_upto) {
        int bench_max = std::min(disks, 15);
        PerformanceBenchmark::run_benchmark_up_to(bench_max);
    }

    if (benchmark_at) {
        PerformanceBenchmark::run_benchmark_at(disks);
    }

    std::cout << "\n" << std::string(70, '=') << std::endl;

    return 0;
}
