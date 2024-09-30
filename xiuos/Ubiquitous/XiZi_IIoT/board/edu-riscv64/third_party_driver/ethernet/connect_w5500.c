
#include "connect_w5500.h"

#include <bus.h>
#include <dev_pin.h>
#include <drv_io_config.h>
#include <fpioa.h>
#include <string.h>
#include <stdlib.h>
#include <xs_base.h>

#include "gpio_common.h"
#include "gpiohs.h"
#include "socket.h"
#include "w5500.h"

#define SPI_LORA_FREQUENCY 10000000

// spi operations
extern void spi_enter_cris(void);
extern void spi_exit_cris(void);
extern void spi_select_cs(void);
extern void spi_deselete_cs(void);

// global configurations for w5500 tcp connection
uint32_t get_gbuf_size() {
  static const uint32_t g_wiznet_buf_size = 2048;
  return g_wiznet_buf_size;
}

wiz_NetInfo *get_gnetinfo() {
  static wiz_NetInfo g_wiz_netinfo = {.mac = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11},
                                      .ip = {192, 168, 130, 77},
                                      .sn = {255, 255, 254, 0},
                                      .gw = {192, 168, 130, 1},
                                      .dns = {0, 0, 0, 0},
                                      .dhcp = NETINFO_STATIC};
  return &g_wiz_netinfo;
}

int network_init() {
  wiz_NetInfo check_wiz_netinfo;
  check_wiz_netinfo.dhcp = NETINFO_STATIC;
  ctlnetwork(CN_SET_NETINFO, (void *)get_gnetinfo());
  ctlnetwork(CN_GET_NETINFO, (void *)&check_wiz_netinfo);

  if (memcmp(get_gnetinfo(), &check_wiz_netinfo, sizeof(wiz_NetInfo)) != 0) {
    KPrintf(
        "mac: %d; ip: %d; gw: %d; sn: %d; dns: %d; dhcp: %d;\n",
        memcmp(&get_gnetinfo()->mac, &check_wiz_netinfo.mac, sizeof(uint8_t) * 6),
        memcmp(&get_gnetinfo()->ip, &check_wiz_netinfo.ip, sizeof(uint8_t) * 4),
        memcmp(&get_gnetinfo()->sn, &check_wiz_netinfo.sn, sizeof(uint8_t) * 4),
        memcmp(&get_gnetinfo()->gw, &check_wiz_netinfo.gw, sizeof(uint8_t) * 4),
        memcmp(&get_gnetinfo()->dns, &check_wiz_netinfo.dns, sizeof(uint8_t) * 4),
        memcmp(&get_gnetinfo()->dhcp, &check_wiz_netinfo.dhcp, sizeof(uint8_t)));
    KPrintf("WIZCHIP set network information fail.\n");
    return ERROR;
  }
  uint8_t tmpstr[6];
  ctlwizchip(CW_GET_ID, (void *)tmpstr);
  KPrintf("=== %s NET CONF ===\r\n", (char *)tmpstr);
  KPrintf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", get_gnetinfo()->mac[0],
          get_gnetinfo()->mac[1], get_gnetinfo()->mac[2], get_gnetinfo()->mac[3],
          get_gnetinfo()->mac[4], get_gnetinfo()->mac[5]);
  KPrintf("SIP: %d.%d.%d.%d\r\n", get_gnetinfo()->ip[0], get_gnetinfo()->ip[1],
          get_gnetinfo()->ip[2], get_gnetinfo()->ip[3]);
  KPrintf("GAR: %d.%d.%d.%d\r\n", get_gnetinfo()->gw[0], get_gnetinfo()->gw[1],
          get_gnetinfo()->gw[2], get_gnetinfo()->gw[3]);
  KPrintf("SUB: %d.%d.%d.%d\r\n", get_gnetinfo()->sn[0], get_gnetinfo()->sn[1],
          get_gnetinfo()->sn[2], get_gnetinfo()->sn[3]);
  KPrintf("DNS: %d.%d.%d.%d\r\n", get_gnetinfo()->dns[0], get_gnetinfo()->dns[1],
          get_gnetinfo()->dns[2], get_gnetinfo()->dns[3]);
  KPrintf("======================\r\n");

  return EOK;
}

/****************** spi init ******************/
static struct Bus *w5500_spi_bus;
int w5500_spi_init() {
  x_err_t ret = EOK;

  w5500_spi_bus = BusFind(SPI_BUS_NAME_1);
  w5500_spi_bus->owner_haldev =
      BusFindDevice(w5500_spi_bus, SPI_1_DEVICE_NAME_0);
  w5500_spi_bus->owner_driver = BusFindDriver(w5500_spi_bus, SPI_1_DRV_NAME);

  w5500_spi_bus->match(w5500_spi_bus->owner_driver,
                       w5500_spi_bus->owner_haldev);

  struct BusConfigureInfo configure_info;
  struct SpiMasterParam spi_master_param;
  spi_master_param.spi_data_bit_width = 8;
  spi_master_param.spi_work_mode = SPI_MODE_0 | SPI_MSB;
  spi_master_param.spi_maxfrequency = SPI_LORA_FREQUENCY;
  spi_master_param.spi_data_endian = 0;

  configure_info.configure_cmd = OPE_CFG;
  configure_info.private_data = (void *)&spi_master_param;
  ret = BusDrvConfigure(w5500_spi_bus->owner_driver, &configure_info);
  if (ret) {
    KPrintf("spi drv OPE_CFG error drv %8p cfg %8p\n",
            w5500_spi_bus->owner_driver, &spi_master_param);
    return ERROR;
  }

  configure_info.configure_cmd = OPE_INT;
  ret = BusDrvConfigure(w5500_spi_bus->owner_driver, &configure_info);
  if (ret) {
    KPrintf("spi drv OPE_INT error drv %8p\n", w5500_spi_bus->owner_driver);
    return ERROR;
  }

  return EOK;
}

void spi_write_byte(uint8_t tx_data) {
  struct BusBlockWriteParam write_param;
  write_param.buffer = &tx_data;
  write_param.size = 1;
  BusDevWriteData(w5500_spi_bus->owner_haldev, &write_param);
}
uint8_t spi_read_byte(void) {
  uint8_t result = 0;
  struct BusBlockReadParam read_param;
  read_param.buffer = &result;
  read_param.size = 1;
  BusDevReadData(w5500_spi_bus->owner_haldev, &read_param);
  return result;
}
void spi_write_burst(uint8_t *tx_buf, uint16_t len) {
  struct BusBlockWriteParam write_param;
  write_param.buffer = tx_buf;
  write_param.size = len;
  BusDevWriteData(w5500_spi_bus->owner_haldev, &write_param);
}
void spi_read_burst(uint8_t *rx_buf, uint16_t len) {
  struct BusBlockReadParam read_param;
  read_param.buffer = rx_buf;
  read_param.size = len;
  BusDevReadData(w5500_spi_bus->owner_haldev, &read_param);
}

/****************** chip init ******************/

void wiz_reset() {
  gpiohs_set_drive_mode(WIZ_RST_PIN, GPIO_DM_OUTPUT);
  gpiohs_set_pin(WIZ_RST_PIN, GPIO_PV_LOW);
  MdelayKTask(20);

  gpiohs_set_pin(WIZ_RST_PIN, GPIO_PV_HIGH);
  MdelayKTask(20);
}

void wiz_spi_handler_reg() {
  // spi ops registration
#if (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_) || \
    (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_)
  /* register SPI device CS select callback function */
  gpiohs_set_drive_mode(SPI1_CS0_PIN, GPIO_DM_OUTPUT);
  reg_wizchip_cs_cbfunc(spi_select_cs, spi_deselete_cs);
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
  reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif

  reg_wizchip_spi_cbfunc(spi_read_byte, spi_write_byte);
  reg_wizchip_cris_cbfunc(spi_enter_cris, spi_exit_cris);
  reg_wizchip_spiburst_cbfunc(spi_read_burst, spi_write_burst);
}

int wiz_chip_cfg_init() {
  uint8_t mem_size[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};

  /* reset WIZnet chip internal PHY, configures PHY mode. */
  if (ctlwizchip(CW_INIT_WIZCHIP, (void *)mem_size) == -1) {
    KPrintf("WIZCHIP initialize failed.");
    return ERROR;
  }

  struct wiz_NetTimeout_t net_timeout;
  net_timeout.retry_cnt = 5;
  net_timeout.time_100us = 20000;
  ctlnetwork(CN_SET_TIMEOUT, (void *)&net_timeout);

  return EOK;
}

/****************** interrupt handle ******************/
void wiz_irq_handler() {}

int wiz_interrupt_init() {
  int32_t ret = -ERROR;

  struct Bus *pin_bus = PinBusInitGet();

  struct PinParam pin_param;
  struct BusConfigureInfo pin_configure_info;

  pin_configure_info.configure_cmd = OPE_CFG;
  pin_configure_info.private_data = (void *)&pin_param;

  pin_param.cmd = GPIO_CONFIG_MODE;
  pin_param.pin = BSP_WIZ_INT_PIN;
  pin_param.mode = GPIO_CFG_INPUT_PULLUP;
  ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
  if (ret != EOK) {
    KPrintf("config pin_param  %d input failed!\n", pin_param.pin);
    return -ERROR;
  }

  pin_param.cmd = GPIO_IRQ_REGISTER;
  pin_param.pin = BSP_WIZ_INT_PIN;
  pin_param.irq_set.irq_mode = GPIO_IRQ_EDGE_FALLING;
  pin_param.irq_set.hdr = wiz_irq_handler;
  pin_param.irq_set.args = NONE;
  ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
  if (ret != EOK) {
    KPrintf("register pin_param  %d  irq failed!\n", pin_param.pin);
    return -ERROR;
  }

  pin_param.cmd = GPIO_IRQ_DISABLE;
  pin_param.pin = BSP_WIZ_INT_PIN;
  ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
  if (ret != EOK) {
    KPrintf("disable pin_param  %d  irq failed!\n", pin_param.pin);
    return -ERROR;
  }

  // 4. enable interuption
  pin_param.cmd = GPIO_IRQ_ENABLE;
  pin_param.pin = BSP_WIZ_INT_PIN;
  ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
  if (ret != EOK) {
    KPrintf("enable pin_param  %d  irq failed!\n", pin_param.pin);
    return -ERROR;
  }

  return EOK;

  return EOK;
}

int HwWiznetInit(void) {
  wiz_reset();

  if (EOK != w5500_spi_init()) {
    return ERROR;
  }

  wiz_spi_handler_reg();

  if (EOK != wiz_chip_cfg_init()) {
    return ERROR;
  }

  network_init();

  return EOK;
}