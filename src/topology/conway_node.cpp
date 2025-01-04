#include "conway_node.hpp"


std::unique_ptr<ConwayNode> make_conway_node(CellState state) {
    return std::make_unique<ConwayNode>(state, new ConwayNodeExecutor);
}
