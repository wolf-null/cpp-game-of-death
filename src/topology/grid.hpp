#ifndef CPP_GAME_OF_DEATH_GRID_HPP
#define CPP_GAME_OF_DEATH_GRID_HPP

#include <vector>
#include <memory>
#include <functional>
#include <cassert>

#include "node.hpp"

using Index = std::size_t;


template <typename TNode>
struct Grid : public std::vector <TNode*> {
    ~Grid() {
        for (auto item : *this) {
            delete item;
        }
    }
};

enum class GridTopology {
    RAW,
    TORUS
};

namespace grid_assets {

    Index ij_2_idx(Index i, Index j, Index width) {
        return j + i * width;
    }

    template <typename TNode>
    void try_subscribe(TNode * node, TNode * neighbor) {
        if(neighbor == nullptr)
            return;
        node->neighborhood()->subscribe_to(neighbor);
    }

    template <typename TNode>
    using node_value_type = typename TNode::TValue::Type;

    template <typename TNode>
    using executor_base_type = NodeExecutor <node_value_type<TNode>>;

    template <typename TNode>
    TNode* make_node(
        executor_base_type<TNode> * executor = nullptr,
        node_value_type<TNode> initial_value = node_value_type<TNode>{}
    ) {
        return new TNode(initial_value, executor ? executor : new executor_base_type<TNode>);
    }

    template <typename TNode>
    TNode * get_node_if_exists(
            Grid<TNode> &grid,
            Index i,
            Index j,
            Index width,
            Index height,
            GridTopology topology = GridTopology::RAW
    ) {
        switch (topology) {
            case GridTopology::RAW:
                if (i < 0 || i >= height || j < 0 || j >= width)
                    return nullptr;
                return grid[ij_2_idx(i, j, width)];
            case GridTopology::TORUS:
                j = j % width + (j < 0 && j % width != 0 ? width : 0);
                i = i % height + (i < 0 && i % height != 0 ? height : 0);
                return grid[ij_2_idx(i, j, width)];
            default:
                throw;
        }
    }
}

template <typename TNode, typename TExecutor = grid_assets::executor_base_type <TNode>>
Grid<TNode> make_grid(
    Index width,
    Index height,
    TExecutor * executor = nullptr, // TODO: executor factory
    GridTopology topology = GridTopology::RAW
) {
    using namespace grid_assets;

    Index size = width * height;
    Grid<TNode> grid;
    grid.resize(size);
    for (Index idx = 0; idx != size; ++ idx) {
        {
            grid[idx] = make_node<TNode>(new TExecutor{});
        }
        TNode * node = grid[idx];
        assert(node == node->executor()->node());
        assert(node->value() == node->executor()->node()->value());
    }

    for (Index i = 0; i != height; ++ i) {
        for (Index j = 0; j != width; ++j) {
            TNode *node = grid[ij_2_idx(i, j, width)];
            assert(node == node->executor()->node());
            assert(node->value() == node->executor()->node()->value());
            try_subscribe(node, get_node_if_exists(grid, i - 1, j, width, height, topology));
            try_subscribe(node, get_node_if_exists(grid, i + 1, j, width, height, topology));
            try_subscribe(node, get_node_if_exists(grid, i, j - 1, width, height, topology));
            try_subscribe(node, get_node_if_exists(grid, i, j + 1, width, height, topology));
        }
    }

    return grid;
}

template <typename ValueType>
Grid<Node<ValueType>> make_grid_v(
        Index width,
        Index height,
        grid_assets::executor_base_type <Node<ValueType>> * executor = nullptr,
        GridTopology topology = GridTopology::RAW
) {
    return make_grid<Node<ValueType>>(width, height, executor, topology);
}

#endif //CPP_GAME_OF_DEATH_GRID_HPP
