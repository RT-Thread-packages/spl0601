#include <rtthread.h>
#include <rtdevice.h>

#include "spl06_01.h"

#define HW_ADR 0x77

static void spl0601_get_calib_param(spl0601_t *hdev);

/*****************************************************************************
 函 数 名  : spl0601_write
 功能描述  : I2C 寄存器写入子函数
 输入参数  : uint8_t hwadr   硬件地址
             uint8_t regadr  寄存器地址
             uint8_t val     值
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
static void spl0601_write(spl0601_t *hdev, uint8_t hwadr, uint8_t regadr, uint8_t val)
{
    uint8_t buf[2] = { regadr, val };
    struct rt_i2c_msg msg;
    msg.addr = hwadr;
    msg.buf = buf;
    msg.len = 2;
    msg.flags = RT_I2C_WR;
    rt_i2c_transfer(hdev->bus, &msg, 1);
}


/*****************************************************************************
 函 数 名  : spl0601_read
 功能描述  : I2C 寄存器读取子函数
 输入参数  : uint8_t hwadr   硬件地址
             uint8_t regadr  寄存器地址
 输出参数  : 
 返 回 值  : uint8_t 读出值
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
static uint8_t spl0601_read(spl0601_t *hdev, uint8_t hwadr, uint8_t regadr)
{
    struct rt_i2c_msg msg;
    msg.addr = hwadr;
    msg.buf = &regadr;
    msg.len = 1;
    msg.flags = RT_I2C_WR;
    rt_i2c_transfer(hdev->bus, &msg, 1);
    msg.flags = RT_I2C_RD;
    rt_i2c_transfer(hdev->bus, &msg, 1);
    return regadr;
}

/*****************************************************************************
 函 数 名  : spl0601_init
 功能描述  : SPL06-01 初始化函数
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
int spl0601_init(spl0601_t *hdev, const char *busname)
{
    hdev->bus = rt_i2c_bus_device_find(busname);
    if (hdev->bus == RT_NULL)
    {
        return -1;
    }
    hdev->i32rawPressure = 0;
    hdev->i32rawTemperature = 0;
    hdev->chip_id = 0x34; /* read Chip Id */
    spl0601_get_calib_param(hdev);
    // sampling rate = 1Hz; Pressure oversample = 2;
    spl0601_rateset(hdev, PRESSURE_SENSOR, 32, 8);   
    // sampling rate = 1Hz; Temperature oversample = 1; 
    spl0601_rateset(hdev, TEMPERATURE_SENSOR, 32, 8);
    //Start background measurement
    return 0;
}

/*****************************************************************************
 函 数 名  : spl0601_rateset
 功能描述  :  设置温度传感器的每秒采样次数以及过采样率
 输入参数  : uint8_t u8OverSmpl  过采样率         Maximal = 128
             uint8_t u8SmplRate  每秒采样次数(Hz) Maximal = 128
             uint8_t iSensor     0: Pressure; 1: Temperature
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月24日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_rateset(spl0601_t *hdev, uint8_t iSensor, uint8_t u8SmplRate, uint8_t u8OverSmpl)
{
    uint8_t reg = 0;
    int32_t i32kPkT = 0;
    switch(u8SmplRate)
    {
        case 2:
            reg |= (1<<4);
            break;
        case 4:
            reg |= (2<<4);
            break;
        case 8:
            reg |= (3<<4);
            break;
        case 16:
            reg |= (4<<4);
            break;
        case 32:
            reg |= (5<<4);
            break;
        case 64:
            reg |= (6<<4);
            break;
        case 128:
            reg |= (7<<4);
            break;
        case 1:
        default:
            break;
    }
    switch(u8OverSmpl)
    {
        case 2:
            reg |= 1;
            i32kPkT = 1572864;
            break;
        case 4:
            reg |= 2;
            i32kPkT = 3670016;
            break;
        case 8:
            reg |= 3;
            i32kPkT = 7864320;
            break;
        case 16:
            i32kPkT = 253952;
            reg |= 4;
            break;
        case 32:
            i32kPkT = 516096;
            reg |= 5;
            break;
        case 64:
            i32kPkT = 1040384;
            reg |= 6;
            break;
        case 128:
            i32kPkT = 2088960;
            reg |= 7;
            break;
        case 1:
        default:
            i32kPkT = 524288;
            break;
    }

    if(iSensor == PRESSURE_SENSOR)
    {
        hdev->i32kP = i32kPkT;
        spl0601_write(hdev, HW_ADR, 0x06, reg);
        if(u8OverSmpl > 8)
        {
            reg = spl0601_read(hdev, HW_ADR, 0x09);
            spl0601_write(hdev, HW_ADR, 0x09, reg | 0x04);
        }
        else
        {
            reg = spl0601_read(hdev, HW_ADR, 0x09);
            spl0601_write(hdev, HW_ADR, 0x09, reg & (~0x04));
        }
    }
    if(iSensor == TEMPERATURE_SENSOR)
    {
        hdev->i32kT = i32kPkT;
        spl0601_write(hdev, HW_ADR, 0x07, reg|0x80);  //Using mems temperature
        if(u8OverSmpl > 8)
        {
            reg = spl0601_read(hdev, HW_ADR, 0x09);
            spl0601_write(hdev, HW_ADR, 0x09, reg | 0x08);
        }
        else
        {
            reg = spl0601_read(hdev, HW_ADR, 0x09);
            spl0601_write(hdev, HW_ADR, 0x09, reg & (~0x08));
        }
    }

}

/*****************************************************************************
 函 数 名  : spl0601_get_calib_param
 功能描述  : 获取校准参数
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
static void spl0601_get_calib_param(spl0601_t *hdev)
{
    uint32_t h;
    uint32_t m;
    uint32_t l;
    h =  spl0601_read(hdev, HW_ADR, 0x10);
    l  =  spl0601_read(hdev, HW_ADR, 0x11);
    hdev->calib_param.c0 = (uint32_t)h<<4 | l>>4;
    hdev->calib_param.c0 = (hdev->calib_param.c0&0x0800)?(0xF000|hdev->calib_param.c0):hdev->calib_param.c0;
    h =  spl0601_read(hdev, HW_ADR, 0x11);
    l  =  spl0601_read(hdev, HW_ADR, 0x12);
    hdev->calib_param.c1 = (uint32_t)(h&0x0F)<<8 | l;
    hdev->calib_param.c1 = (hdev->calib_param.c1&0x0800)?(0xF000|hdev->calib_param.c1):hdev->calib_param.c1;
    h =  spl0601_read(hdev, HW_ADR, 0x13);
    m =  spl0601_read(hdev, HW_ADR, 0x14);
    l =  spl0601_read(hdev, HW_ADR, 0x15);
    hdev->calib_param.c00 = (int32_t)h<<12 | (int32_t)m<<4 | (int32_t)l>>4;
    hdev->calib_param.c00 = (hdev->calib_param.c00&0x080000)?(0xFFF00000|hdev->calib_param.c00):hdev->calib_param.c00;
    h =  spl0601_read(hdev, HW_ADR, 0x15);
    m =  spl0601_read(hdev, HW_ADR, 0x16);
    l =  spl0601_read(hdev, HW_ADR, 0x17);
    hdev->calib_param.c10 = (int32_t)(h & 0x0F) << 16 | (int32_t)m << 8 | l;
    hdev->calib_param.c10 = (hdev->calib_param.c10&0x080000)?(0xFFF00000|hdev->calib_param.c10):hdev->calib_param.c10;
    h =  spl0601_read(hdev, HW_ADR, 0x18);
    l  =  spl0601_read(hdev, HW_ADR, 0x19);
    hdev->calib_param.c01 = (uint32_t)h<<8 | l;
    h =  spl0601_read(hdev, HW_ADR, 0x1A);
    l  =  spl0601_read(hdev, HW_ADR, 0x1B);
    hdev->calib_param.c11 = (uint32_t)h<<8 | l;
    h =  spl0601_read(hdev, HW_ADR, 0x1C);
    l  =  spl0601_read(hdev, HW_ADR, 0x1D);
    hdev->calib_param.c20 = (uint32_t)h<<8 | l;
    h =  spl0601_read(hdev, HW_ADR, 0x1E);
    l  =  spl0601_read(hdev, HW_ADR, 0x1F);
    hdev->calib_param.c21 = (uint32_t)h<<8 | l;
    h =  spl0601_read(hdev, HW_ADR, 0x20);
    l  =  spl0601_read(hdev, HW_ADR, 0x21);
    hdev->calib_param.c30 = (uint32_t)h<<8 | l;
}


/*****************************************************************************
 函 数 名  : spl0601_start_temperature
 功能描述  : 发起一次温度测量
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_temperature(spl0601_t *hdev)
{
    spl0601_write(hdev, HW_ADR, 0x08, 0x02);
}

/*****************************************************************************
 函 数 名  : spl0601_start_pressure
 功能描述  : 发起一次压力值测量
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_pressure(spl0601_t *hdev)
{
    spl0601_write(hdev, HW_ADR, 0x08, 0x01);
}

/*****************************************************************************
 函 数 名  : spl0601_start_continuous
 功能描述  : Select node for the continuously measurement
 输入参数  : uint8_t mode  1: pressure; 2: temperature; 3: pressure and temperature
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月25日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_continuous(spl0601_t *hdev, uint8_t mode)
{
    spl0601_write(hdev, HW_ADR, 0x08, mode+4);
}

void spl0601_stop(spl0601_t *hdev)
{
    spl0601_write(hdev, HW_ADR, 0x08, 0);
}


/*****************************************************************************
 函 数 名  : spl0601_get_raw_temp
 功能描述  : 获取温度的原始值，并转换成32Bits整数
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_raw_temp(spl0601_t *hdev)
{
    uint8_t buf[3], reg = 0x03;
    struct rt_i2c_msg msg;
    msg.addr = HW_ADR;
    msg.buf = &reg;
    msg.len = 1;
    msg.flags = RT_I2C_WR;
    rt_i2c_transfer(hdev->bus, &msg, 1);
    msg.buf = buf;
    msg.len = 3;
    msg.flags = RT_I2C_RD;
    rt_i2c_transfer(hdev->bus, &msg, 1);
    
    hdev->i32rawTemperature = (int32_t)buf[0]<<16 | (int32_t)buf[1]<<8 | (int32_t)buf[2];
    hdev->i32rawTemperature= (hdev->i32rawTemperature&0x800000) ? (0xFF000000|hdev->i32rawTemperature) : hdev->i32rawTemperature;
}

/*****************************************************************************
 函 数 名  : spl0601_get_raw_pressure
 功能描述  : 获取压力原始值，并转换成32bits整数
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_raw_pressure(spl0601_t *hdev)
{
    uint8_t buf[3], reg = 0x00;
    struct rt_i2c_msg msg;
    msg.addr = HW_ADR;
    msg.buf = &reg;
    msg.len = 1;
    msg.flags = RT_I2C_WR;
    rt_i2c_transfer(hdev->bus, &msg, 1);
    msg.buf = buf;
    msg.len = 3;
    msg.flags = RT_I2C_RD;
    rt_i2c_transfer(hdev->bus, &msg, 1);
    
    hdev->i32rawPressure = (int32_t)buf[0]<<16 | (int32_t)buf[1]<<8 | (int32_t)buf[2];
    hdev->i32rawPressure= (hdev->i32rawPressure&0x800000) ? (0xFF000000|hdev->i32rawPressure) : hdev->i32rawPressure;
}


/*****************************************************************************
 函 数 名  : spl0601_get_temperature
 功能描述  : 在获取原始值的基础上，返回浮点校准后的温度值
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
float spl0601_get_temperature(spl0601_t *hdev)
{
    float fTCompensate;
    float fTsc;

    fTsc = hdev->i32rawTemperature / (float)hdev->i32kT;
    fTCompensate =  hdev->calib_param.c0 * 0.5 + hdev->calib_param.c1 * fTsc;
    return fTCompensate;
}

/*****************************************************************************
 函 数 名  : spl0601_get_pressure
 功能描述  : 在获取原始值的基础上，返回浮点校准后的压力值
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
float spl0601_get_pressure(spl0601_t *hdev)
{
    float fTsc, fPsc;
    float qua2, qua3;
    float fPCompensate;

    fTsc = hdev->i32rawTemperature / (float)hdev->i32kT;
    fPsc = hdev->i32rawPressure / (float)hdev->i32kP;
    qua2 = hdev->calib_param.c10 + fPsc * (hdev->calib_param.c20 + fPsc* hdev->calib_param.c30);
    qua3 = fTsc * fPsc * (hdev->calib_param.c11 + fPsc * hdev->calib_param.c21);

    fPCompensate = hdev->calib_param.c00 + fPsc * qua2 + fTsc * hdev->calib_param.c01 + qua3;
    return fPCompensate;
}
