#include "BasePerfCase.hpp"
#include<iostream>


namespace bsp_perf {
namespace common {

constexpr char BasePerfCase::LOG_TAG[];

BasePerfCase::BasePerfCase(bsp_perf::shared::ArgParser&& args): m_args{std::move(args)}
{
    std::cout << LOG_TAG << "::BasePerfCase()" << std::endl;
}

void BasePerfCase::run(int32_t cycles)
{
    std::cout << LOG_TAG << "BasePerfCase::run()" << std::endl;
    onInit();
    while(cycles-- > 0)
    {
        onProcess();
        onRender();
    }

    onRelease();
}

} // namespace common
} // namespace bsp_perf
