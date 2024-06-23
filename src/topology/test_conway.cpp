#include "test_conway.hpp"


void test_conway() {
    TestConwayLogic<0>().test();
    TestConwayLogic<1>().test();
    TestConwayLogic<8>().test();
}
