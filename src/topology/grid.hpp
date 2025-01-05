#ifndef CPP_GAME_OF_DEATH_GRID_HPP
#define CPP_GAME_OF_DEATH_GRID_HPP

#include <vector>
#include <memory>
#include <functional>
#include <cassert>

#include "node.hpp"
#include "node_array.hpp"

namespace topology::grid::errors {
    struct TOPOLOGY_NOT_IMPLEMENTED : public std::runtime_error {
        TOPOLOGY_NOT_IMPLEMENTED() : std::runtime_error("Selected topology is not implemented for get_node_if_exists()") {};
    };
}

namespace topology::grid {

    /// Alias for node indices and coordinates type
    using Index = topology::Index;

    /// Grid object is the same as @see NodeArray
    template<typename TNode>
    using Grid  = topology::NodeArray<TNode>;

    /// Topology of the whole grid.
    enum class GridTopology {
        RAW, ///< Flat topology of the grid. Borders are not connected.
        TORUS ///< Grid topology is folded so the opposite borders are glued, so the space enclosed.
    };

    /// Extract value type from specialized Node class
    template<typename TNode>
    using node_value_type = typename TNode::TValue::Type;

    /**
     * Extracts NodeExecutor type for given @tparam TNode.
     * Use this as the base class for custom node executors.
     * @see NodeExecutor
     */
    template<typename TNode>
    using executor_base_type = NodeExecutor<node_value_type<TNode>>;

    /**
     * Definition of the factory method of node's executors.
     * Used in make_grid(). Must return proper NodeExecutor*
     * The factory receives four args: (i, j, height, width) where
     * height, width - actual size of the grid (whether the topology is)
     * i, j - actual node coordinates for which Executor is requested
     * @see NodeExecutor
     * @see make_grid
     */
    template<typename TNode>
    using t_executor_factory = executor_base_type<TNode>*(Index, Index, Index, Index);


    /**
     * Assets namespace is not meant to be used from outside
     */
    namespace assets {
        /// Taxicab metric (i, j) -> linear index (node number)
        Index ij_2_idx(Index i, Index j, Index width) {
            return j + i * width;
        }

        /// If @param neighbor is not NULL adds it to the neighborhood of @param node
        template<typename TNode>
        void try_subscribe(TNode *node, TNode *neighbor) {
            if (neighbor == nullptr)
                return;
            node->neighborhood()->subscribe_to(neighbor);
        }

        /**
         * Basic Node factory method.
         * @tparam TNode type of node
         * @param executor instance. If null will instantiate default executor (it's exec() does nothing)
         * @param initial_value
         * @return assembled node instance
         *
         */
        template<typename TNode>
        [[nodiscard]] TNode *make_node(
                executor_base_type<TNode> *executor = nullptr,
                node_value_type<TNode> initial_value = node_value_type<TNode>{}
        ) {
            return new TNode(initial_value, executor ? executor : new executor_base_type<TNode>);
        }

        /**
         * Attempts to locate node in the Grid at (@tparam i, @tparam j) in selected topology.
         * Implements (i, j) coordinates mapping into the real index (node number).
         * And by this, partially implements the topological structure.
         * @return pointer to Node. If there is no node at (i, j) returns nullptr
         * @see Node
         */
        template<typename TNode>
        [[nodiscard]] TNode *get_node_if_exists(
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
                    throw errors::TOPOLOGY_NOT_IMPLEMENTED();
            }
        }
    }

    /**
     * Builds Taxicab Metric grid of nodes.
     * @tparam TNode
     * @tparam TExecutor custom executor type for overriding the behavior. Must be inherited from executor_base_type<TNode>
     * @param width grid actual width
     * @param height grid actual height
     * @param executor_factory if NULL uses @tparam TExecutor() for each node, otherwise use @see t_executor_factory
     * @param topology the way border nodes are connected
     * @return built Grid<TNode> object
     */
    template<typename TNode, typename TExecutor = executor_base_type<TNode>>
    Grid<TNode> make_grid(
            Index width,
            Index height,
            t_executor_factory<TNode> * executor_factory = nullptr,  // TODO: make tests
            GridTopology topology = GridTopology::RAW
    ) {
        using namespace topology::grid::assets;

        Index size = width * height;
        Grid<TNode> grid;
        grid.resize(size);
        for (Index i = 0; i != height; ++ i) {
            for (Index j = 0; j != width; ++ j) {
                Index idx = ij_2_idx(i, j, width);

                {
                    grid[idx] = make_node<TNode>(
                        executor_factory ? executor_factory(i, j, height, width) : new TExecutor{}
                    );
                }

                // Check is there issues with TNode scope and cross-pointers.
                TNode *node = grid[idx];
                assert(node == node->executor()->node());
                assert(node->value() == node->executor()->node()->value());
            }
        }

        for (Index i = 0; i != height; ++i) {
            for (Index j = 0; j != width; ++j) {
                // Secondary cross-pointer checks
                TNode *node = grid[ij_2_idx(i, j, width)];
                assert(node == node->executor()->node());
                assert(node->value() == node->executor()->node()->value());

                // Connect neighbor nodes to this (i, j) node
                try_subscribe(node, get_node_if_exists(grid, i - 1, j, width, height, topology));
                try_subscribe(node, get_node_if_exists(grid, i + 1, j, width, height, topology));
                try_subscribe(node, get_node_if_exists(grid, i, j - 1, width, height, topology));
                try_subscribe(node, get_node_if_exists(grid, i, j + 1, width, height, topology));
            }
        }

        return grid;
    }

    /// Alias for building Grid for given @tparam ValueType
    template<typename ValueType>
    Grid<Node<ValueType>> make_grid_v(
            Index width,
            Index height,
            t_executor_factory<Node<ValueType>> * executor_factory = nullptr,
            GridTopology topology = GridTopology::RAW
    ) {
        return make_grid<Node<ValueType>>(width, height, executor_factory, topology);
    }

}
#endif //CPP_GAME_OF_DEATH_GRID_HPP
