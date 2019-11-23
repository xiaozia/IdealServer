
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Percentile.h
*   Last Modified : 2019-11-22 21:51
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_SUDOKU_PERCENTILE_H
#define  _EXAMPLE_SUDOKU_PERCENTILE_H

#include "ideal/base/Logger.h"
#include "ideal/base/FileUtil.h"

#include <algorithm>
#include <vector>

class Percentile {
public:
    Percentile(std::vector<int>& latencies, int infly) {
        stat << "recv " << ideal::Fmt("%6zd", latencies.size()) << " in-fly " << infly;

        if(!latencies.empty()) {
            std::sort(latencies.begin(), latencies.end());
            int min = latencies.front();
            int max = latencies.back();
            int sum = std::accumulate(latencies.begin(), latencies.end(), 0);
            int mean = sum / static_cast<int>(latencies.size());
            int median = getPercentile(latencies, 50);
            int p90 = getPercentile(latencies, 90);
            int p99 = getPercentile(latencies, 99);

            stat << " min " << min
                 << " max " << max
                 << " avg " << mean
                 << " median " << median
                 << " p90 " << p90
                 << " p99 " << p99;
        }
    }

    const ideal::LogStream::Buffer& report() const {
        return stat.buffer();
    }
    
    void save(const std::vector<int>& latencies, ideal::StringArg name) const {
        if(latencies.empty())
            return;

        ideal::FileUtil::AppendFile f(name);
        f.append("# ", 2);
        f.append(stat.buffer().data(), stat.buffer().length());
        f.append("\n", 1);

        const int kInterval = 5;
        int low = latencies.front() / kInterval * kInterval;
        int count = 0;
        int sum = 0;
        const double total = static_cast<double>(latencies.size());
        char buf[64] = { 0 };

        for(size_t i = 0; i < latencies.size(); ++i) {
            if(latencies[i] < low + kInterval)
                ++count;
            else {
                sum += count;
                int n = snprintf(buf, sizeof buf, "%4d %5d %5.2f\n", low, count, 100*sum/total);
                f.append(buf, n);
                low = latencies[i] / kInterval * kInterval;
                assert(latencies[i] < low + kInterval);
                count  = 1;
            }
        }
        sum += count;
        assert(sum == total);
        int n = snprintf(buf, sizeof buf, "%4d %5d %5.1f\n", low, count, 100*sum/total);
        f.append(buf, n);
    }

private:
    static int getPercentile(const std::vector<int>& latencies, int percent) {
        assert(!latencies.empty());
        size_t index = 0;
        if(percent > 0) {
            index = (latencies.size() * percent + 99) / 100 - 1;
            assert(index < latencies.size());
        }
        return latencies[index];
    }


private:
    ideal::LogStream stat;
};

#endif // _EXAMPLE_SUDOKU_PERCENTILE_H


