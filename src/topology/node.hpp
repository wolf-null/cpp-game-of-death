#ifndef GAMEOFDEATH_NODE_HPP
#define GAMEOFDEATH_NODE_HPP

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>

#include "logger/standard_logger.hpp"


namespace errors {
    struct EXECUTOR_UNLINKED : public std::runtime_error {
        EXECUTOR_UNLINKED() : std::runtime_error("NodeExecutor is not lnked to Node") {};
    };

    struct EXECUTOR_NULLPTR_LINKAGE : public std::runtime_error {
        EXECUTOR_NULLPTR_LINKAGE() : std::runtime_error("Trying to link NodeExecutor to nullptr") {};
    };

    struct NEIGHBORHOOD_OVERFLOW : public std::overflow_error {
        NEIGHBORHOOD_OVERFLOW() : std::overflow_error("Can't add neighbor due to capacity limit") {};
    };

    struct NEIGHBORHOOD_DEREFERENCE : public std::runtime_error {
        NEIGHBORHOOD_DEREFERENCE() : std::runtime_error("Can't dereference. Out of range") {};
    };

}


template <typename ValueType> class Node;

/**
 * Node's value delegate (bridged property).
 * Implements differentiation between present node's value and its next value.
 * @tparam ValueType
 */
template <typename ValueType>
class Value {
    ValueType value_;
    std::optional<ValueType> staged_value_;

public:
    using Type = ValueType;

    Value() : value_{} {};
    Value(ValueType initial_value) : value_{initial_value} {};
    ~Value() {
        debug << "~Value()\n";
    }

    /// Get current node's value.
    ValueType get() {
        return value_;
    }

    /// Propose next node's value.
    void stage(ValueType staged_value) {
        staged_value_ = std::optional<ValueType>{staged_value};
    }

    /// Set proposed node's value to current node's value.
    void commit() {
        if(staged_value_.has_value()) [[likely]] {
            value_ = staged_value_.value();
            staged_value_.reset();
            if constexpr (std::is_same<decltype(value_), int>::value)
                debug << std::to_string(value_);
        }
    }

    /// Get if there is any next value proposed.
    bool is_staged() {
        return staged_value_.has_value();
    }
};

/**
 * Node's execution logic delegate (bridged property).
 * Bridges node's ability to execute it's internal logic (mainly its proposing it's next value depend on current value).
 * Certain behavior is to be implemented in inheritors, by overriding the `exec()` virtual method.
 * @tparam ValueType
 */
template <typename ValueType>
class NodeExecutor {
protected:
    using Node_ = Node<ValueType>;
    Node_ * node_ = nullptr;
public:
    NodeExecutor() = default;
    virtual ~NodeExecutor() {debug << "~NodeExecutor()\n";};

    /// Overridable node's execution behavior.
    virtual void exec() {};

    /// Linked node getter. Allows to access node's properties in `exec()` implementation.
    Node_* node() const {
        if (node_ == nullptr)
            throw errors::EXECUTOR_UNLINKED();
        return node_;
    }

    /// Linked node setter.
    void link_to_node(Node_ * node) {
        if (node == nullptr)
            throw errors::EXECUTOR_NULLPTR_LINKAGE();
        node_ = node;
        node_->set_executor(this);
    }
};

/**
 * Node's list neighbors delegate (bridged property).
 * Implements node's ability to have neighbors.
 * Under the hood is it a container of references to other nodes of type TNode.
 * @tparam TNode a class specified from Node<TValue> template
 */
template <class TNode>
class Neighborhood {
public:
    using NeighborArray = std::vector<TNode *>;

private:
    NeighborArray neighbors_;

public:
    Neighborhood() = default;
    ~Neighborhood() { debug <<"~Neighborhood()\n"; }

    /// Add a @param node to the neighborhood.
    void subscribe_to(TNode * node) {
        neighbors_.push_back(node);
    }

    /// Remove @param node for the neighborhood.
    void unsubscribe_from(TNode * node) {
        auto found = std::find(neighbors_.begin(), neighbors_.end(), node);
        if(found == neighbors_.end())
            return;
        neighbors_.erase(found);
    }

    /// Number of neighbors.
    const std::size_t size() const {
        return neighbors_.size();
    }

    /// Forward iterator for the neighbors.
    auto begin() {
        return neighbors_.begin();
    }

    /// Tail iterator for the neighbors.
    auto end() {
        return neighbors_.end();
    }

    /**
     * Apply predicate to each neighbor consequently.
     * @param predicate a method applied to a node with return type @tparam MappedT
     * @return a vector of returns from applying @param predicate to each neighbor.
     */
    template <typename MappedT>
    auto map(std::function<MappedT(TNode*)> &predicate) {
        std::vector<MappedT> result;
        std::transform(begin(), end(), std::back_inserter(result), predicate);
        return result;
    }
};

/**
 * A composite object, describing a Node.
 * @see Value delegating storage of node's value.
 * @see Neighborhood delegating node's neighbors.
 * @see Executor delegating node's behavior.
 * @tparam ValueType
 */
template <typename ValueType>
struct Node {
    using TNode = Node<ValueType>;
    using TValue = Value<ValueType>;
    using TNeighborhood = Neighborhood<TNode>;
    using TExecutor = NodeExecutor<ValueType>;

    friend TValue;
    friend TExecutor;
    friend TNeighborhood;

private:
    TValue * value_;
    TNeighborhood * neighborhood_;
    TExecutor * executor_;

    /// Value delegate setter. To be used during composition (construction) only.
    void set_value(TValue * value) {
        value_ = value;
    }

    /// Neighborhood setter. To be used during composition (construction) only.
    void set_neighborhood(TNeighborhood * neighborhood) {neighborhood_ = neighborhood;}

    /// Executor setter. To be used during composition (construction) only.
    void set_executor(TExecutor * executor) {executor_ = executor;}

public:
    /// Value delegate getter.
    TValue * value() {return value_;}

    /// Neighborhood delegate getter.
    TNeighborhood * neighborhood() {return neighborhood_;}

    /// Executor delegate getter.
    TExecutor * executor() {
        return executor_;
    }

    Node() {
        (new TExecutor)->link_to_node(this);
        set_neighborhood(new TNeighborhood);
        set_value(new TValue);
    };

    Node(
        ValueType initial_value,
        TExecutor * node_executor = nullptr
    ) {
        (node_executor ? node_executor : new TExecutor)->link_to_node(this);
        set_neighborhood(new TNeighborhood);
        set_value(new TValue(initial_value));
    }

    /// Node destruction also destroys its delegates.
    virtual ~Node()
    {
        debug << "~Node()\n";
        delete value_;
        delete neighborhood_;
        delete executor_;
    }
};

#endif
