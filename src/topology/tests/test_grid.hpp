#ifndef CPP_GAME_OF_DEATH_TEST_GRID_HPP
#define CPP_GAME_OF_DEATH_TEST_GRID_HPP
#include <iostream>
#include <memory>
#include <cassert>

#include "../grid.hpp"

struct test_grid {
    using ValueType = int;
    using TNode = Node<ValueType>;
    class NodeExecutorMock : public grid_assets::executor_base_type<TNode> {
        void exec() override {
            auto neighbor_count = node()->neighborhood()->size();
            std::cout << " this node has " << neighbor_count << " neighbors\n";
            node()->value()->stage(int(neighbor_count));
            node()->value()->commit();
        }
    };

    Index width_;
    Index height_;

    test_grid(Index width, Index height) : width_{width}, height_{height} {};

    void perform_tests() {
        using namespace grid_assets;
        Grid<TNode> grid = make_grid<TNode, NodeExecutorMock> (width_, height_);
        for (Index i = 0; i != height_; ++ i)
            for (Index j = 0; j != width_; ++ j) {
                // Calculate expected number of neighbors
                bool is_h_border = i == 0 || i + 1 == height_;
                bool is_v_border = j == 0 || j + 1 == width_;
                bool is_border = is_h_border || is_v_border;
                bool is_corner = is_h_border && is_v_border;

                ValueType expected_neighbor_count;
                if (is_corner)
                    expected_neighbor_count = 2;
                else if (is_border)
                    expected_neighbor_count = 3;
                else
                    expected_neighbor_count = 4;

                // Exec node and find real number of neighbors.
                std::cout << "[" << i << ", " << j << "]";
                std::cout << " (expect " << expected_neighbor_count << " neighbors)";
                TNode * node = grid[ij_2_idx(i, j, width_)];
                node->executor()->exec();

                std::cout << node->value() << " vs " << node->executor()->node()->value() << "\n";
                ValueType neighbor_count = node->value()->get();
                assert(neighbor_count == expected_neighbor_count);
            }
    }
};


#endif //CPP_GAME_OF_DEATH_TEST_GRID_HPP
