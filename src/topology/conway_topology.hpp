#ifndef CPP_GAME_OF_DEATH_CONWAY_TOPOLOGY_HPP
#define CPP_GAME_OF_DEATH_CONWAY_TOPOLOGY_HPP

#include <vector>
#include <memory>
#include "conway_node.hpp"

using Index = std::size_t;
using ConwayGrid = std::vector <std::unique_ptr<ConwayNode>>;

// TODO: Associate with more general type of nodes

enum class GridTopology {
    RAW,
    TORUS
};

namespace conway_assets {

    Index ij_2_idx(Index i, Index j, Index width) {
        return j + i * width;
    }

    void try_subscribe(ConwayNode * node, ConwayNode * neighbor) {
        if(neighbor == nullptr)
            return;
        node->neighborhood()->subscribe_to(neighbor);
    }

    ConwayNode * get_node_if_exists(
        ConwayGrid &grid,
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
                return grid[ij_2_idx(i, j, width)].get();
            case GridTopology::TORUS:
                j = j % width + (j < 0 && j % width != 0 ? width : 0);
                i = i % height + (i < 0 && i % height != 0 ? height : 0);
                return grid[ij_2_idx(i, j, width)].get();
            default:
                throw;
        }
    }
}


ConwayGrid make_grid(Index width, Index height, GridTopology topology = GridTopology::RAW) {
    using namespace conway_assets;

    Index size = width * height;
    ConwayGrid grid;
    grid.resize(size);
    for (Index idx = 0; idx != size; ++ idx)
        grid[idx] = make_conway_node();

    for (Index i = 0; i != height; ++ i) {
        for (Index j = 0; j != width; ++j) {
            ConwayNode *node = grid[ij_2_idx(i, j, width)].get();

            try_subscribe(node, get_node_if_exists(grid, i - 1, j, width, height, topology));
            try_subscribe(node, get_node_if_exists(grid, i + 1, j, width, height, topology));
            try_subscribe(node, get_node_if_exists(grid, i, j - 1, width, height, topology));
            try_subscribe(node, get_node_if_exists(grid, i, j + 1, width, height, topology));
        }
    }

    return grid;
}


#endif //CPP_GAME_OF_DEATH_CONWAY_TOPOLOGY_HPP
