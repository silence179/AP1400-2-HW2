#include <iostream>
#include <gtest/gtest.h>
#include "client.h"
#include "server.h"

extern std::vector<std::string> pending_trxs;

void show_pending_transactions() {
    std::cout << std::string(20, '*') << std::endl;
    for (auto i = pending_trxs.begin(); i != pending_trxs.end(); ++i) {
        std::cout << *i << std::endl;
    }
    // for (const auto& trx : pending_trxs) std::cout << trx << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}

int main(int argc, char **argv)
{
    if (false) // make false to run unit-tests
    {
        // debug section
        ::testing::InitGoogleTest(&argc, argv);
    }
    else
    {
        ::testing::InitGoogleTest(&argc, argv);
        std::cout << "running tests ..." << std::endl;
        int ret{RUN_ALL_TESTS()};
        if (!ret)
            std::cout << "<<<success>>>" << std::endl;
        else
            std::cout << "failed" << std::endl;
    }
    return 0;   
}
