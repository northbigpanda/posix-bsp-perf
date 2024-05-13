/*
MIT License

Copyright (c) 2024 Clarence Zhou<287334895@qq.com> and contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef DDR_PERF_HPP
#define DDR_PERF_HPP

#include <framework/BasePerfCase.hpp>
#include <shared/ArgParser.hpp>
#include <profiler/PerfProfiler.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <ctime>

namespace bsp_perf {
namespace perf_cases {

constexpr int32_t num_1M = 1000000;

class ddrPerf : public bsp_perf::common::BasePerfCase
{

public:
    static constexpr char LOG_TAG[] {"[ddrPerf]: "};

    ddrPerf(bsp_perf::shared::ArgParser&& args):
        BasePerfCase(std::move(args))
    {
        auto& params = getArgs();
        std::string ret;
        params.getOptionVal("--case_name", ret);
        m_profiler = std::make_unique<bsp_perf::common::PerfProfiler>(ret);

    }
    ddrPerf(const ddrPerf&) = delete;
    ddrPerf& operator=(const ddrPerf&) = delete;
    ddrPerf(ddrPerf&&) = delete;
    ddrPerf& operator=(ddrPerf&&) = delete;
    ~ddrPerf()
    {
        // Release any resources held by m_profiler
        m_profiler.reset();
    }
private:

    void onInit() override
    {
        auto& params = getArgs();
        size_t mem_size;
        params.getOptionVal("--size_mb", mem_size);
        m_bytes_cnt = bsp_perf::common::memValueMB(mem_size);
        m_test_arr.reset(new std::byte[m_bytes_cnt]);
        srand(time(0));
        for(size_t i = 0; i < m_bytes_cnt; i++)
        {
            m_test_arr[i] = static_cast<std::byte>(rand() % 256);
        }

    }

    void onProcess() override
    {
        auto begin = m_profiler->getCurrentTimePoint();
        doWritingPerf();
        auto end = m_profiler->getCurrentTimePoint();
        auto write_latency = m_profiler->getLatencyUs(begin, end);
        m_write_bw_mb = getDDRBandwidthMB(write_latency);
        m_profiler->asyncRecordPerfData("Writing BandWidth", m_write_bw_mb, "MB/s");

        begin = m_profiler->getCurrentTimePoint();
        doReadingPerf();
        end = m_profiler->getCurrentTimePoint();
        auto read_latency = m_profiler->getLatencyUs(begin, end);
        m_read_bw_mb = getDDRBandwidthMB(read_latency);
        m_profiler->asyncRecordPerfData("Reading BandWidth", m_write_bw_mb, "MB/s");
    }

    void onRender() override
    {
        m_profiler->printPerfData("Writing BandWidth", m_write_bw_mb, "MB/s");
        m_profiler->printPerfData("Reading BandWidth", m_read_bw_mb, "MB/s");
    }

    void onRelease() override
    {
        m_test_arr.reset();
    }

    inline void doWritingPerf()
    {
        for(size_t i = 0; i < m_bytes_cnt; i++)
        {
            m_test_arr[i] = std::byte(0x55);
        }
    }

    inline void doReadingPerf()
    {
        size_t sum = 0;
        for(size_t i = 0; i < m_bytes_cnt; i++)
        {
            sum += static_cast<uint8_t>(m_test_arr[i]);
        }
    }

    inline float getDDRBandwidthMB(const size_t& test_time_us)
    {
        float bytes_per_us = 0.0;
	    float bytes_per_sec = 0.0;
	    float mb_per_sec = 0.0;

        if(test_time_us > 0)
        {
            bytes_per_us = static_cast<float>(m_bytes_cnt) / static_cast<float>(test_time_us);
            bytes_per_sec = bytes_per_us * num_1M;
            mb_per_sec = bytes_per_sec / (1024 * 1024);
        }

        return mb_per_sec;
    }

private:
    std::string m_name {"ddrPerf"};
    std::unique_ptr<bsp_perf::common::PerfProfiler> m_profiler{nullptr};
    std::unique_ptr<std::byte[]> m_test_arr{nullptr};
    size_t m_bytes_cnt{0};

    float m_write_bw_mb{0.0};
    float m_read_bw_mb{0.0};
};

} // namespace perf_cases
} // namespace bsp_perf

#endif // DDR_PERF_HPP
