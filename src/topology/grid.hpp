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


    /// NodeArray object is the same as @see NodeArray
    template<typename TNode>
    using NodeArray  = topology::NodeArray<TNode>;


    /// Topology of the whole grid.
    enum class GridTopology {
        RAW, ///< Flat topology of the grid. Borders are not connected.
        TORUS ///< Grid topology is folded so the opposite borders are glued, so the space enclosed.
    };


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
    class ExecutorFactory {
        Index width_;
        Index height_;
    public:
        explicit ExecutorFactory() = delete;
        explicit ExecutorFactory(Index width, Index height) : width_{width}, height_{height} {};

        Index width () const {return width_;}
        Index height () const {return height_;}

        using ExecutorBaseType = executor_base_type<TNode>;
        virtual ExecutorBaseType* make_executor_at (Index i, Index j) = 0;
    };

    /**
     * Assets namespace is not meant to be used from outside
     */
    namespace assets {
        /// Arguments must have form (i, j, width) -> idx, all of type `topology::Index`
        class Indexer {
            Index width_;
            Index height_;
        public:
            Index width() const {return width_;}
            Index height() const {return height_;}
            explicit Indexer() = delete;
            explicit Indexer(Index width, Index height) : width_{width}, height_{height} {};

            virtual Index operator() (Index i, Index j) const = 0;

            Index of(Index i, Index j) const {return operator()(i, j);}
        };

        /// Taxicab metric (i, j) -> linear index (node number)
        class IJIndexer : public Indexer {
        public:
            explicit IJIndexer(Index width, Index height) : Indexer{width, height} {};

            Index operator() (Index i, Index j) const override {
                return j + i * width();
            }
        };

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
         * Attempts to locate node in the NodeArray at (i, j) in selected topology.
         * Implements (i, j) coordinates mapping into the real index (node number).
         * And by this, partially implements the topological structure.
         * @return pointer to Node. If there is no node at (i, j) returns nullptr
         * @see Node
         */
        template<typename TNode>
        [[nodiscard]] TNode *get_node_if_exists(
                NodeArray<TNode> &grid,
                Index i,
                Index j,
                Indexer & indexer,
                GridTopology topology = GridTopology::RAW
        ) {
            switch (topology) {
                case GridTopology::RAW:
                    if (i < 0 || i >= indexer.height() || j < 0 || j >= indexer.width())
                        return nullptr;
                    return grid[indexer(i, j)];
                case GridTopology::TORUS:
                    j = j % indexer.width() + (j < 0 && j % indexer.width() != 0 ? indexer.width() : 0);
                    i = i % indexer.height() + (i < 0 && i % indexer.height() != 0 ? indexer.height() : 0);
                    return grid[indexer(i, j)];
                default:
                    throw errors::TOPOLOGY_NOT_IMPLEMENTED();
            }
        }
    }

    template<typename TNode, typename TExecutor = executor_base_type<TNode>>
    class NewExecutor : public ExecutorFactory<TNode> {
    public:
        explicit NewExecutor(Index width, Index height) : ExecutorFactory<TNode>(width, height) {};
        TExecutor* make_executor_at (Index i, Index j) override {
            return new TExecutor();
        }
    };

    /**
     * Builds Taxicab Metric grid of nodes.
     * @tparam TNode
     * @tparam TExecutor custom executor type for overriding the behavior. Must be inherited from executor_base_type<TNode>
     * @param width grid actual width
     * @param height grid actual height
     * @param executor_factory if nullptr uses new TExecutor() for each node, otherwise uses executor_factory to build node
     * @param topology the way border nodes are connected
     * @return built NodeArray<TNode> object
     */
    template<typename TNode, typename TExecutor = executor_base_type<TNode>>
    NodeArray<TNode> make_grid(
            Index width,
            Index height,
            ExecutorFactory<TNode> * executor_factory = nullptr,  // TODO: cover with tests
            GridTopology topology = GridTopology::RAW
    ) {
        using namespace topology::grid::assets;
        auto ij_2_idx = IJIndexer(width, height);

        if (!executor_factory)
            // TODO: Fix memory leak
            executor_factory = new NewExecutor<TNode, TExecutor>(width, height);

        Index size = width * height;
        NodeArray<TNode> node_array(size);
        for (Index i = 0; i != height; ++ i) {
            for (Index j = 0; j != width; ++ j) {
                Index idx = ij_2_idx(i, j);

                {
                    // Localize the scope to make sure destructor of new-made Node won't be invoked.
                    node_array.replace_node(
                        make_node<TNode>(executor_factory->make_executor_at(i, j)),
                        idx
                    );
                }

                // Check is there issues with TNode scope and cross-pointers.
                TNode *node = node_array[idx];
                assert(node == node->executor()->node());
                assert(node->value() == node->executor()->node()->value());
            }
        }

        for (Index i = 0; i != height; ++i) {
            for (Index j = 0; j != width; ++j) {
                // Secondary cross-pointer checks
                TNode *node = node_array[ij_2_idx(i, j)];
                assert(node == node->executor()->node());
                assert(node->value() == node->executor()->node()->value());

                // Connect neighbor nodes to this (i, j) node
                try_subscribe(node, get_node_if_exists(node_array, i - 1, j, ij_2_idx, topology));
                try_subscribe(node, get_node_if_exists(node_array, i + 1, j, ij_2_idx, topology));
                try_subscribe(node, get_node_if_exists(node_array, i, j - 1, ij_2_idx, topology));
                try_subscribe(node, get_node_if_exists(node_array, i, j + 1, ij_2_idx, topology));
            }
        }

        return node_array;
    }

    /// Alias for building NodeArray for given @tparam ValueType
    template<typename ValueType>
    NodeArray<Node<ValueType>> make_grid_v(
            Index width,
            Index height,
            ExecutorFactory<Node<ValueType>> * executor_factory = nullptr,
            GridTopology topology = GridTopology::RAW
    ) {
        return make_grid<Node<ValueType>>(width, height, executor_factory, topology);
    }

}
#endif //CPP_GAME_OF_DEATH_GRID_HPP
