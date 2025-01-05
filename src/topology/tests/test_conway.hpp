#ifndef GAMEOFDEATH_TEST_CONWAY_HPP
#define GAMEOFDEATH_TEST_CONWAY_HPP

#include <array>
#include <cassert>
#include <tuple>

#include "topology/node.hpp"
#include "topology/conway_node.hpp"

template <int NUM_NEIGHBORS>
struct TestConwayLogic {
    using Node_ = conway::ConwayNode;

    Node_ * core_node;
    std::array<Node_*, NUM_NEIGHBORS> env_nodes;

    TestConwayLogic() {
        using namespace conway;

        core_node = new conway::ConwayNode {CellState::DEAD, new ConwayNodeExecutor};

        for(int idx{0}; idx != NUM_NEIGHBORS; ++ idx)
            env_nodes[idx] = new ConwayNode {CellState::DEAD, new ConwayNodeExecutor};

        for (auto& env_node : env_nodes) {
            core_node->neighborhood()->subscribe_to(env_node);
        }
    }

    auto set_state_by_seed(int seed, Node_& node, bool commit_now = true) {
        using namespace conway;

        bool state_code = seed & 0x1;
        auto state = state_code ? CellState::ALIVE : CellState::DEAD;
        node.value()->stage(state);
        if (commit_now)
            node.value()->commit();
        return state_code;
    }

    std::tuple <conway::CellState, conway::EnvironmentState> seed_states(int seed) {
        using namespace conway;

        auto cell_value = set_state_by_seed(seed, *core_node);
        auto env_value_sum {0};
        for (auto& env_node : env_nodes) {
            seed >>= 1;
            env_value_sum += set_state_by_seed(seed, *env_node);
        }

        auto cell_state = cell_value ? CellState::ALIVE : CellState::DEAD;
        EnvironmentState env_state{};
        switch (env_value_sum) {
            case 2:
                env_state = EnvironmentState::SUPPORT;
                break;
            case 3:
                env_state = EnvironmentState::BIRTH;
                break;
            default:
                env_state = EnvironmentState::DEAD;
        }

        return std::make_tuple(cell_state, env_state);
    }

    conway::CellState predict_next_state_value(std::tuple <conway::CellState, conway::EnvironmentState> input) {
        using namespace conway;

        switch (std::get<0>(input)) {
            case CellState::DEAD:
                switch (std::get<1>(input)) {
                    case EnvironmentState::DEAD: return CellState::DEAD;
                    case EnvironmentState::SUPPORT: return CellState::DEAD;
                    case EnvironmentState::BIRTH: return CellState::ALIVE;
                    default: throw;
                }

            case CellState::ALIVE:
                switch (std::get<1>(input)) {
                    case EnvironmentState::DEAD: return CellState::DEAD;
                    case EnvironmentState::SUPPORT: return CellState::ALIVE;
                    case EnvironmentState::BIRTH: return CellState::ALIVE;
                    default: throw;
                }

            default: throw;
        }
    }

    std::tuple <conway::CellState, conway::EnvironmentState, conway::CellState> bake_test_case(int seed) {
        using namespace conway;

        auto input = seed_states(seed);
        auto output = predict_next_state_value(input);
        return std::make_tuple(std::get<0>(input), std::get<1>(input), output);
    }

    void test(int seed) {
        auto test_case = bake_test_case(seed);
        core_node->executor()->exec();
        core_node->value()->commit();
        auto new_state = core_node->value()->get();
        assert (new_state == std::get<2>(test_case));
    }

    void test() {
        const int min_seed = 0;
        int number_of_nodes = 1 + NUM_NEIGHBORS;
        const int max_seed = (-1 << number_of_nodes) ^ (-1);
        for (int seed{min_seed}; seed != max_seed + 1; ++ seed)
            test(seed);
    }
};


void test_conway() {
    TestConwayLogic<0>().test();
    TestConwayLogic<1>().test();
    TestConwayLogic<8>().test();
}


#endif //GAMEOFDEATH_TEST_CONWAY_HPP
