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
        explicit NodeArray(Index size) {
            items_.resize(size);
        }

        TNode* replace_node(TNode * node, Index idx) {
            std::swap (items_[idx], node);
            return node;
        }

        TNode* operator[](Index idx) {
            return items_.at(idx);
        }

        Index size() const {
            return items_.size();
        }

        ~NodeArray() {
            for (auto *node: items_) delete node;
        }

        void apply(std::function<void(TNode*)> proc) {
            for (auto *node: items_) proc(node);
        }

        void apply(std::function<void(int, TNode*)> proc) {
            for (auto idx = 0, it = items_.begin(); it != items_.end(); ++it, ++ idx) {
                proc(idx, *it);
            }
        }
    };
}

#endif //CPP_GAME_OF_DEATH_NODE_ARRAY_HPP
