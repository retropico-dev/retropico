/*
 * author : Shuichi TAKANO
 * since  : Sat Sep 04 2021 18:13:23
 */

#include <util/work_meter.h>

namespace util {
    void _workMeterReset() {}

    void _workMeterMark(uint32_t tag) {}

    void _workMeterEnum(int scale, int div, const WorkMeterEnumFunc &func) {}

    uint32_t _workMeterGetCounter() { return 0; }
}
