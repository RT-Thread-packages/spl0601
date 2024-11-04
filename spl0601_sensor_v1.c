#include "spl0601_sensor_v1.h"
#include "spl06_01.h"

#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG
#define DBG_SECTION_NAME  "sensor.goer.spl0601"
#define DBG_COLOR
#include <rtdbg.h>

#if defined(SPL0601_USING_TEMP) || defined(SPL0601_USING_BARO)

static spl0601_t * _spl0601_create(struct rt_sensor_intf *intf)
{
    spl0601_t *hdev = rt_malloc(sizeof(spl0601_t));
    if (hdev == RT_NULL)
    {
        return RT_NULL;
    }
    spl0601_init(hdev, intf->dev_name);
    spl0601_rateset(hdev, PRESSURE_SENSOR, 32, 8);
    spl0601_rateset(hdev, TEMPERATURE_SENSOR, 32, 8);
    spl0601_start_continuous(hdev, CONTINUOUS_P_AND_T);
    return hdev;
}

static RT_SIZE_TYPE spl0601_fetch_data(struct rt_sensor_device *sensor, void *buf, rt_size_t len)
{
    spl0601_t *hdev = sensor->parent.user_data;
    struct rt_sensor_data *data = (struct rt_sensor_data *)buf;
    
    if (sensor->info.type == RT_SENSOR_CLASS_TEMP)
    {
        float temp_value;

        spl0601_get_raw_temp(hdev);
        temp_value = spl0601_get_temperature(hdev);

        data->type = RT_SENSOR_CLASS_TEMP;
        data->data.temp = temp_value * 10;
        data->timestamp = rt_sensor_get_ts();
    }
    else if (sensor->info.type == RT_SENSOR_CLASS_BARO)
    {
        float pres_value;

		spl0601_get_raw_pressure(hdev);
        pres_value = spl0601_get_pressure(hdev);

        data->type = RT_SENSOR_CLASS_BARO;
        data->data.baro = pres_value;
        data->timestamp = rt_sensor_get_ts();
    }

    return 1;
}

static rt_err_t spl0601_control(struct rt_sensor_device *sensor, int cmd, void *args)
{
    rt_err_t result = RT_EOK;
    spl0601_t *hdev = sensor->parent.user_data;

    switch (cmd)
    {
    case RT_SENSOR_CTRL_SET_ODR:
        if ((rt_uint32_t)args <= 128)
        {
            rt_uint32_t odr = (rt_uint32_t)args;
            if (sensor->info.type == RT_SENSOR_CLASS_TEMP)
                spl0601_rateset(hdev, TEMPERATURE_SENSOR, odr, 8);
            else if(sensor->info.type == RT_SENSOR_CLASS_BARO)
                spl0601_rateset(hdev, PRESSURE_SENSOR, odr, 8);
        }
        break;
    case RT_SENSOR_CTRL_SELF_TEST:
        result =  -RT_EINVAL;
        break;
    default:
        return -RT_ERROR;
    }
    return result;
}

static struct rt_sensor_ops sensor_ops =
{
    spl0601_fetch_data,
    spl0601_control
};

#endif

#ifdef SPL0601_USING_TEMP
static int rt_hw_spl0601_temp_init(const char *name, struct rt_sensor_config *cfg, spl0601_t *hdev)
{
    rt_int8_t result;
    rt_sensor_t sensor = RT_NULL;

    sensor = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor == RT_NULL)
        return -1;

    sensor->info.type       = RT_SENSOR_CLASS_TEMP;
    sensor->info.vendor     = RT_SENSOR_VENDOR_STM;
    sensor->info.model      = "spl0601_temp";
    sensor->info.unit       = RT_SENSOR_UNIT_DCELSIUS;
    sensor->info.intf_type  = RT_SENSOR_INTF_I2C;
    sensor->info.range_max  = 120;
    sensor->info.range_min  = -40;
    sensor->info.period_min = 80;
    sensor->info.fifo_max   = 0;

    rt_memcpy(&sensor->config, cfg, sizeof(struct rt_sensor_config));
    sensor->ops = &sensor_ops;
    
    result = rt_hw_sensor_register(sensor, name, RT_DEVICE_FLAG_RDWR, hdev);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        rt_free(sensor);
        return -RT_ERROR;
    }

    LOG_I("temp sensor init success");
    return RT_EOK;
}
#endif

#ifdef SPL0601_USING_BARO
static int rt_hw_spl0601_baro_init(const char *name, struct rt_sensor_config *cfg, spl0601_t *hdev)
{
    rt_int8_t result;
    rt_sensor_t sensor = RT_NULL;

    sensor = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor == RT_NULL)
        return -1;

    sensor->info.type       = RT_SENSOR_CLASS_BARO;
    sensor->info.vendor     = RT_SENSOR_VENDOR_STM;
    sensor->info.model      = "spl0601_baro";
    sensor->info.unit       = RT_SENSOR_UNIT_PERMILLAGE;
    sensor->info.intf_type  = RT_SENSOR_INTF_I2C;
    sensor->info.range_max  = 1000;
    sensor->info.range_min  = 0;
    sensor->info.period_min = 80;
    sensor->info.fifo_max   = 0;

    rt_memcpy(&sensor->config, cfg, sizeof(struct rt_sensor_config));
    sensor->ops = &sensor_ops;
    
    result = rt_hw_sensor_register(sensor, name, RT_DEVICE_FLAG_RDWR, hdev);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        rt_free(sensor);
        return -RT_ERROR;
    }

    LOG_I("baro sensor init success");
    return RT_EOK;
}
#endif

int rt_hw_spl0601_init(const char *name, struct rt_sensor_config *cfg)
{
#if defined(SPL0601_USING_TEMP) || defined(SPL0601_USING_BARO)
    spl0601_t *hdev = _spl0601_create(&cfg->intf);
    
    if (hdev)
    {
#ifdef SPL0601_USING_TEMP
        rt_hw_spl0601_temp_init(name, cfg, hdev);
#endif
#ifdef SPL0601_USING_BARO
        rt_hw_spl0601_baro_init(name, cfg, hdev);
#endif
        return 0;
    }
#endif
    return -1;
}
