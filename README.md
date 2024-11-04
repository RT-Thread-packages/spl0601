# SPL0601

## 简介

本软件包是为 Goer SPL0601 气压传感器提供的通用传感器驱动包。通过使用此软件包，开发者可以快速的利用 RT-Thread 将此传感器驱动起来。

本篇文档主要内容如下：

- 传感器介绍
- 支持情况
- 使用说明

## 传感器介绍

SPL0601 是 Goer（歌尔）公司专为可穿戴设备和 IOT 市场开发的一款超低功耗加气压传感器，尺寸小巧且内置温度传感器。

## 支持情况

| 包含设备         | 气压计 | 温度计 |
| ---------------- | -------- | ------ |
| **通讯接口**     |          |        |
| IIC              | √        | √      |
| SPI              |          |        |
| **工作模式**     |          |        |
| 轮询             | √        | √      |
| 中断             |          |        |
| FIFO             |          |        |
| **电源模式**     |          |        |
| 掉电             |          |        |
| 低功耗           |          |        |
| 普通             | √        | √      |
| 高功耗           |          |        |
| **数据输出速率** | √        | √      |
| **测量范围**     |          |        |
| **自检**         |          |        |
| **多实例**       | √        | √      |

## 使用说明

### 依赖

- RT-Thread 4.0.0+
- Sensor 组件
- IIC 驱动：SPL0601 设备使用 IIC 进行数据通讯，需要系统 IIC 驱动框架支持；

### 获取软件包

使用 BMA400 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```
SPL0601: SPL0601 Digital pressure sensor
    [*]   Enable SPL0601 pressure
    [*]   Enable SPL0601 temperature
        Version (latest)  --->
```

**Enable SPL0601 pressure**： 配置开启大气压强测量功能

**Enable SPL0601 temperature**：配置开启温度测量功能

**Version**：软件包版本选择

### 使用软件包

BMA400 软件包初始化函数如下所示：

```
int rt_hw_spl0601_init(const char *name, struct rt_sensor_config *cfg);
```

该函数需要由用户调用，函数主要完成的功能有，

- 设备配置和初始化（根据传入的配置信息，配置接口设备和中断引脚）；
- 注册相应的传感器设备，完成 SPL0601 设备的注册；

#### 初始化示例

```
#include "spl0601_sensor_v1.h"

int spl0601_port(void)
{
    struct rt_sensor_config cfg;
    cfg.intf.dev_name = "i2c1";
    rt_hw_spl0601_init("spl0601", &cfg);
    return 0;
}
INIT_APP_EXPORT(spl0601_port);
```

## 注意事项

暂无

## 联系人信息

维护人:

- [gztss](https://github.com/gztss)
