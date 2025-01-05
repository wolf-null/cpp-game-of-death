#ifndef GAMEOFDEATH_CONWAY_HPP
#define GAMEOFDEATH_CONWAY_HPP

#include <iostream>
#include <numeric>
#include <algorithm>
#include <optional>
#include <memory>

#include "node.hpp"
#include "logger/standard_logger.hpp"


namespace errors {
    struct EXECUTOR_COLLECT_ENVIRONMENT : public std::runtime_error {
        EXECUTOR_COLLECT_ENVIRONMENT() : std::runtime_error("NodeExecutor failed during neighborhood") {};
    };

    struct EXECUTOR_GET_NODE_STATE : public std::runtime_error {
        EXECUTOR_GET_NODE_STATE() : std::runtime_error("NodeExecutor failed to retrieve own node state") {};
    };
}

namespace conway {
    using namespace topology;

    enum class CellState {
        DEAD = 0,
        ALIVE = 1
    };


    enum class EnvironmentState {
        DEAD = 0,
        SUPPORT = 1,
        BIRTH = 2
    };


    using ConwayNode = Node<CellState>;
    using ConwayNodeExecutorBase = NodeExecutor<CellState>;


    class ConwayNodeExecutor : public ConwayNodeExecutorBase {

        auto environment_state() try {
            std::function<CellState(ConwayNode *)> value_extractor = [](ConwayNode *node) -> CellState {
                return node->value()->get();
            };
            auto cell_states = this->node()->neighborhood()->map<CellState>(value_extractor);
            auto alive_count = std::count(cell_states.begin(), cell_states.end(), CellState::ALIVE);
            switch (alive_count) {
                case 2:
                    return EnvironmentState::SUPPORT;
                case 3:
                    return EnvironmentState::BIRTH;
                default:
                    return EnvironmentState::DEAD;
            }
        } catch (std::exception &exc) {
            throw errors::EXECUTOR_COLLECT_ENVIRONMENT();
        }

        CellState current_state() try {
            return node()->value()->get();
        } catch (std::exception &exc) {
            throw errors::EXECUTOR_GET_NODE_STATE();
        }

        CellState eval_new_state(CellState current, EnvironmentState environment) {
            if (current == CellState::ALIVE) {
                bool is_keep_alive = environment == EnvironmentState::BIRTH or environment == EnvironmentState::SUPPORT;
                return is_keep_alive ? CellState::ALIVE : CellState::DEAD;
            } else {
                bool is_birth = environment == EnvironmentState::BIRTH;
                return is_birth ? CellState::ALIVE : CellState::DEAD;
            }
        }

    public:
        ~ConwayNodeExecutor() {
            debug << "~ConwayNodeExecutor()\n";
        }

        virtual void exec() override {
            auto env = environment_state();
            auto state = current_state();
            auto new_state = eval_new_state(state, env);
            node()->value()->stage(new_state);
        }
    };


    /// Conway node factory method
    ConwayNode *make_conway_node(CellState state = CellState::DEAD) {
        return new ConwayNode(state, new ConwayNodeExecutor);
    }

}


#endif //GAMEOFDEATH_CONWAY_HPP
