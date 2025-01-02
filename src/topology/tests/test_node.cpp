#include "test_node.hpp"


void test_node() {
    using ValueType = int;
    using Node_ = Node <ValueType>;
    Node_ node;
    node.neighborhood()->subscribe_to(&node);

    assert(node.neighborhood()->size() == 1);

    Node_ * found_node;
    for (auto * iterated_node : *node.neighborhood()) {
        found_node = iterated_node;
    }
    assert(found_node == &node);

    node.executor()->exec();

    // Test value stage / commit
    ValueType new_value{6};
    node.value()->stage(new_value);
    node.value()->commit();
    assert(node.value()->get() == new_value);

    // Test exec

    class TestExecutor : public NodeExecutor<ValueType> {
        void exec() override {

        }
    };

    auto * conway = new TestExecutor;
    conway->link_to_node(&node);
    node.executor()->exec();

    // Test map
    std::function<int(Node_*)> test_predicate = [] (Node_ *n) -> ValueType {
        return n->value()->get() + 1;
    };

    auto map_result = node.neighborhood()->map <ValueType>(test_predicate);
    for (auto item : map_result) {
        assert(item == new_value + 1);
    }


    node.neighborhood()->unsubscribe_from(&node);
    assert(node.neighborhood()->size() == 0);
}
