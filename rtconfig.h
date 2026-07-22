#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512

/* kservice optimization */

#define RT_DEBUG

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart4"
#define RT_VER_NUM 0x40101
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M7

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 4
#define DFS_FILESYSTEM_TYPES_MAX 4
#define DFS_FD_MAX 16
#define RT_USING_DFS_DEVFS
#define RT_USING_FAL
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define FAL_USING_SFUD_PORT
#define FAL_USING_NOR_FLASH_DEV_NAME "GD25Q32C"

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 128
#define RT_USING_CAN
#define RT_CAN_USING_HDR
#define RT_CAN_USING_CANFD
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_DAC
#define RT_USING_PWM
#define RT_USING_MTD_NOR
#define RT_USING_SPI
#define RT_USING_SPI_BITOPS
#define RT_SPI_BITOPS_DEBUG
#define RT_USING_QSPI
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
#define RT_SFUD_SPI_MAX_HZ 50000000

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_FS
#define RT_USING_POSIX_DEVIO
#define RT_USING_POSIX_POLL
#define RT_USING_POSIX_SELECT
#define RT_USING_POSIX_SOCKET

/* Interprocess Communication (IPC) */

#define RT_USING_POSIX_PIPE
#define RT_USING_POSIX_PIPE_SIZE 512

/* Socket is in the 'Network' category */


/* Network */

#define RT_USING_SAL
#define SAL_INTERNET_CHECK

/* Docking with protocol stacks */

#define SAL_USING_LWIP
#define SAL_USING_POSIX
#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0
#define RT_USING_LWIP
#define RT_USING_LWIP212
#define RT_USING_LWIP_VER_NUM 0x20102
#define RT_LWIP_MEM_ALIGNMENT 4
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_DNS

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "192.168.176.30"
#define RT_LWIP_GWADDR "192.168.176.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 10
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_PRIORITY 12
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_SO_LINGER 0
#define LWIP_NETIF_LOOPBACK 0
#define RT_LWIP_USING_PING

/* Utilities */

#define RT_USING_RYM
#define YMODEM_USING_CRC_TABLE
#define YMODEM_USING_FILE_TRANSFER
#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_W
#define ULOG_OUTPUT_LVL 4
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE

/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_WEBCLIENT
#define WEBCLIENT_USING_SAMPLES
#define WEBCLIENT_NOT_USE_TLS
#define PKG_USING_WEBCLIENT_V220
#define PKG_WEBCLIENT_VER_NUM 0x20200

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* CYW43012 WiFi */


/* BL808 WiFi */


/* CYW43439 WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */

#define PKG_USING_CJSON
#define PKG_USING_CJSON_V1717

/* XML: Extensible Markup Language */


/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */


/* u8g2: a monochrome graphic library */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */

#define PKG_USING_UCOSIII_WRAPPER
#define PKG_USING_UCOSIII_WRAPPER_AUTOINIT
#define PKG_USING_UCOSIII_WRAPPER_TINY
#define PKG_USING_UCOSIII_WRAPPER_LATEST_VERSION
#define PKG_USING_FLASHDB
#define FDB_USING_KVDB
#define FDB_USING_TSDB
#define FDB_USING_FAL_MODE
#define FDB_WRITE_GRAN_1BIT
#define FDB_WRITE_GRAN 1
#define FDB_NOT_USING_FILE_MODE
#define FDB_DEBUG_ENABLE
#define PKG_USING_FLASHDB_V20100
#define PKG_FLASHDB_VER_NUM 0x20100
#define PKG_USING_QPC
#define PKG_USING_QPC_LATEST_VERSION

/* peripheral libraries and drivers */

/* HAL & SDK Drivers */

/* STM32 HAL & SDK Drivers */


/* Infineon HAL Packages */


/* Kendryte SDK */


/* WCH HAL & SDK Drivers */


/* AT32 HAL & SDK Drivers */


/* sensors drivers */


/* touch drivers */

#define PKG_USING_CAN_YMODEM
#define RT_CAN_YMODEM_PACK_SIZE_TX 1100
#define PKG_USING_CAN_YMODEM_V100

/* AI packages */


/* Signal Processing and Control Algorithm Packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */

#define PKG_USING_NETWORK_SAMPLES
#define PKG_USING_NETWORK_SAMPLES_LATEST_VERSION
#define NETWORK_SAMPLES_USING_TCP_CLIENT
#define NETWORK_SAMPLES_USING_TCP_SERVER
#define NETWORK_SAMPLES_USING_TCP_CLIENT_SELECT

/* entertainment: terminal games and other interesting software packages */


/* Arduino libraries */


/* Projects and Demos */


/* Sensors */


/* Display */


/* Timing */


/* Data Processing */


/* Data Storage */

/* Communication */


/* Device Control */


/* Other */


/* Signal IO */


/* Uncategorized */

#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32H7

/* Hardware Drivers Config */

#define SOC_STM32H743II

/* Onboard Peripheral Drivers */

#define BSP_USING_SPI_FLASH
#define BSP_USING_ETH
#define PHY_USING_LAN8720A
#define ETH_RESET_PIN "PC.2"

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define BSP_USING_UART4
#define BSP_USING_UART7
#define BSP_USING_UART8
#define BSP_UART7_RX_USING_DMA


#define BSP_USING_FDCAN
#define BSP_USING_FDCAN1
#define BSP_USING_FDCAN2
#define BSP_USING_SPI
#define BSP_USING_SPI1
#define BSP_USING_SPI2
#define BSP_USING_SPI3
#define BSP_USING_SPI4
#define BSP_USING_SOFT_SPI2
#define BSP_S_SPI2_SCK_PIN 50
#define BSP_S_SPI2_MOSI_PIN 49
#define BSP_S_SPI2_MISO_PIN 49

#define BSP_USING_SOFT_SPI3
 #define BSP_S_SPI3_SCK_PIN 50
 #define BSP_S_SPI3_MOSI_PIN 49
 #define BSP_S_SPI3_MISO_PIN 48

//#define BSP_S_SPI3_SCK_PIN 20
//#define BSP_S_SPI3_MOSI_PIN 24
//#define BSP_S_SPI3_MISO_PIN 23



#define BSP_USING_I2C
#define BSP_USING_I2C1

/* Notice: PF0 --> 80; PF1 --> 81 For DAC */

#define BSP_I2C1_SCL_PIN 81
#define BSP_I2C1_SDA_PIN 80
#define BSP_USING_I2C2

/* Notice: PI4 --> 132; PI5 --> 133 For Temp */

#define BSP_I2C2_SCL_PIN 132
#define BSP_I2C2_SDA_PIN 133
#define BSP_USING_ON_CHIP_FLASH
#define BSP_USING_PWM
#define BSP_USING_PWM5
#define BSP_USING_PWM5_CH1
#define BSP_USING_ADC
#define BSP_USING_ADC1
#define BSP_USING_ADC2
#define BSP_USING_ADC3
#define RT_USING_WDT

/* Board extended module Drivers */

#define BSP_USING_TMC5160
#define BSP_USING_MCP4725
#define RT_MCP4725_IIC_NAME "i2c1"
#define BSP_USING_ADC128S102
#define RT_ADC128S102_SPI_NAME "spi3"
#define BSP_USING_AD7730
#define RT_AD7730_SPI_NAME "spi1"
#define APP_USING_LIQUID_MODULE
#define APP_USING_OPT_MODULE
#define APP_USING_AIR_MODULE

#endif
