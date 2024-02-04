#ifndef GAMEOFDEATH_NODE_HPP
#define GAMEOFDEATH_NODE_HPP

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>

#include "logger.h"


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


template <typename ValueType>
class Value {
    ValueType value_;
    std::optional<ValueType> staged_value_;

public:
    Value() : value_{} {};
    Value(ValueType initial_value) : value_{initial_value} {};
    ~Value() {
        logger << "~Value()\n";
    }

    ValueType get() {
        return value_;
    }

    void stage(ValueType staged_value) {
        staged_value_ = std::optional<ValueType>{staged_value};
    }

    void commit() {
        if(staged_value_.has_value()) [[likely]] {
            value_ = *staged_value_;
            staged_value_.reset();
        }
    }

    bool is_staged() {
        return staged_value_.has_value();
    }
};


template <typename ValueType>
class NodeExecutor {
protected:
    using Node_ = Node<ValueType>;
    Node_ * node_ = nullptr;
public:
    NodeExecutor() = default;
    virtual ~NodeExecutor() {logger << "~NodeExecutor()\n";};

    virtual void exec() {
        std::cout << "n";
    };

    Node_* node() const {
        if (node_ == nullptr)
            throw errors::EXECUTOR_UNLINKED();
        return node_;
    }

    void link_to_node(Node_ * node) {
        if (node == nullptr)
            throw errors::EXECUTOR_NULLPTR_LINKAGE();
        node_ = node;
        node_->executor_ = this;
    }
};


template <class TNode>
class Neighborhood {
public:
    using NeighborArray = std::vector<TNode *>;

private:
    NeighborArray neighbors_;

public:
    Neighborhood() = default;
    ~Neighborhood() { std::cout <<"~Neighborhood()\n"; }

    void subscribe_to(TNode * node) {
        neighbors_.push_back(node);
    }

    void unsubscribe_from(TNode * node) {
        auto found = std::find(neighbors_.begin(), neighbors_.end(), node);
        if(found == neighbors_.end())
            return;
        neighbors_.erase(found);
    }

    const std::size_t size() const {
        return neighbors_.size();
    }

    auto begin() {
        return neighbors_.begin();
    }

    auto end() {
        return neighbors_.end();
    }

    template <typename MappedT>
    auto map(std::function<MappedT(TNode*)> &predicate) {
        std::vector<MappedT> result;
        std::transform(begin(), end(), std::back_inserter(result), predicate);
        return result;
    }
};


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

public:
    TValue * value() {
        return value_;
    }
    TNeighborhood * neighborhood() {return neighborhood_;}
    TExecutor * executor() {
        return executor_;
    }

    Node() :
        value_{new TValue },
        neighborhood_{new TNeighborhood}
    {
        (new TExecutor)->link_to_node(this);
    };

    Node(
        ValueType initial_value,
        TExecutor * node_executor = nullptr
    ) : value_{new TValue {initial_value}}, neighborhood_{new TNeighborhood} {
        (node_executor ? node_executor : new TExecutor)->link_to_node(this);
    }

    virtual ~Node()
    {
        logger << "~Node()\n";
        delete value_;
        delete neighborhood_;
        delete executor_;
    }
};

#endif
