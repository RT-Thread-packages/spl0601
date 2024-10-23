#ifndef SENSOR_GOER_SPL0601_H__
#define SENSOR_GOER_SPL0601_H__

#include <rtdevice.h>
#include <rtthread.h>

#if defined(RT_VERSION_CHECK)
    #if (RTTHREAD_VERSION >= RT_VERSION_CHECK(5, 0, 2))
        #define RT_SIZE_TYPE   rt_ssize_t
    #else
        #define RT_SIZE_TYPE   rt_size_t
    #endif
#endif

int rt_hw_spl0601_init(const char *name, struct rt_sensor_config *cfg);

#endif
