#ifndef DRV_DAC_MCP4725_H 
#define DRV_DAC_MCP4725_H 

#define  MCP4725_VREF       3300
#define  MCP4725_ADDRESS    0X60 


#define  MCP4725_CMD_FASTMODE (0X00)
#define MCP4725_CMD_WRITEDAC (0x40)
#define MCP4725_CMD_WRITEDACEEPROM (0x60)

#define  MCP4725_IIC_NAME       RT_MCP4725_IIC_NAME

#define  MCP4725_DAC_NAME       ("EDAC1")

struct drv_dac_mcp4725
{
    struct rt_i2c_bus_device *bus;
    struct rt_dac_device stm32_dac_device;
};

#endif // DRV_DAC_MCP4725_H