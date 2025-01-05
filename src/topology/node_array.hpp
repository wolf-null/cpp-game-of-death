#ifndef CPP_GAME_OF_DEATH_NODE_ARRAY_HPP
#define CPP_GAME_OF_DEATH_NODE_ARRAY_HPP

#include <vector>

namespace topology {
    /// Alias for node indices and the coordinates inside the NodeArray
    using Index = std::size_t;

    /**
     * An  object responsible for storing nodes and defining their lifetime.
     * @tparam TNode supported node types
     * @see Node
     */
    template<typename TNode>
    class NodeArray {
        std::vector<TNode *> items_;
    public:
        TNode* & operator[](Index idx) {
            return items_[idx];
        }

        void resize(Index new_size) {
            items_.resize(new_size);
        }

        ~NodeArray() {
            for (auto *node: items_) delete node;
        }
    };
}

#endif //CPP_GAME_OF_DEATH_NODE_ARRAY_HPP
