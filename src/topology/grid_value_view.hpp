#ifndef CMAKESFMLPROJECT_GRID_VALUE_VIEW_HPP
#define CMAKESFMLPROJECT_GRID_VALUE_VIEW_HPP

#include <vector>
#include <optional>
#include <cassert>

#include "node.hpp"
#include "grid.hpp"


namespace topology::grid {
    using namespace topology::grid::assets;

    /**
     * View model class for
     * @tparam TNode
     */
    template <typename TNode>
    class GridValueViewModel {
    public:
        using ValueType = node_value_type<TNode>;
        using TValue = Value<ValueType> ;
        using TGrid = NodeArray<TNode>;

    private:
        std::vector <TValue *> values_;

        TGrid * grid_ = nullptr;
        Indexer * indexer_ = nullptr;

        void attach_to_grid_() {
            assert(("grid size must fit indexer's w*h", grid_->size() == indexer_->width() * indexer_->height()));
            grid_->apply(
                [this](int idx, TNode* node) {
                    values_[idx] = node;
                }
            );
        }

    public:
        /// Returns `Value<ValueType>` object of node at (i, j)
        const TValue * at(Index i, Index j) const {
            return values_.at((*indexer_)(i, j));
        }

        /// Returns a copy of current (actual) node's value at (i, j)
        ValueType get(Index i, Index j) const {
            return values_.at(indexer_->of(i, j))->get();
        }

        ValueType put(Index i, Index j, ValueType value, bool commit = false) {
            auto * value_object = values_.at(indexer_->of(i, j));
            value_object->stage(value);
            if (commit)
                value_object->commit();
        }

        void commit() {
            grid_->apply([](Node<ValueType>* node) {node->value()->commit();});
        }

        explicit GridValueViewModel(
            TGrid * grid,
            Indexer * indexer
        ) : grid_{grid}, indexer_ {indexer} {
            assert(indexer);
            assert(grid);
            attach_to_grid_();
        };

    };

}

#endif //CMAKESFMLPROJECT_GRID_VALUE_VIEW_HPP
