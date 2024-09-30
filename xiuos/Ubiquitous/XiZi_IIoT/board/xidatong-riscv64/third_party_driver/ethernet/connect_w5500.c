
#include "connect_w5500.h"

#include <bus.h>
#include <dev_pin.h>
#include <drv_io_config.h>
#include <fpioa.h>
#include <string.h>
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
const uint32_t g_wiznet_buf_size = 2048;

static wiz_NetInfo g_wiz_netinfo = {.mac = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11},
                                    .ip = {192, 168, 31, 13},
                                    .sn = {255, 255, 255, 0},
                                    .gw = {192, 168, 31, 1},
                                    .dns = {0, 0, 0, 0},
                                    .dhcp = NETINFO_STATIC};

int network_init() {
  wiz_NetInfo check_wiz_netinfo;
  check_wiz_netinfo.dhcp = NETINFO_STATIC;
  ctlnetwork(CN_SET_NETINFO, (void *)&g_wiz_netinfo);
  ctlnetwork(CN_GET_NETINFO, (void *)&check_wiz_netinfo);

  if (memcmp(&g_wiz_netinfo, &check_wiz_netinfo, sizeof(wiz_NetInfo)) != 0) {
    KPrintf(
        "mac: %d; ip: %d; gw: %d; sn: %d; dns: %d; dhcp: %d;\n",
        memcmp(&g_wiz_netinfo.mac, &check_wiz_netinfo.mac, sizeof(uint8_t) * 6),
        memcmp(&g_wiz_netinfo.ip, &check_wiz_netinfo.ip, sizeof(uint8_t) * 4),
        memcmp(&g_wiz_netinfo.sn, &check_wiz_netinfo.sn, sizeof(uint8_t) * 4),
        memcmp(&g_wiz_netinfo.gw, &check_wiz_netinfo.gw, sizeof(uint8_t) * 4),
        memcmp(&g_wiz_netinfo.dns, &check_wiz_netinfo.dns, sizeof(uint8_t) * 4),
        memcmp(&g_wiz_netinfo.dhcp, &check_wiz_netinfo.dhcp, sizeof(uint8_t)));
    KPrintf("WIZCHIP set network information fail.\n");
    return ERROR;
  }
  uint8_t tmpstr[6];
  ctlwizchip(CW_GET_ID, (void *)tmpstr);
  KPrintf("=== %s NET CONF ===\r\n", (char *)tmpstr);
  KPrintf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", g_wiz_netinfo.mac[0],
          g_wiz_netinfo.mac[1], g_wiz_netinfo.mac[2], g_wiz_netinfo.mac[3],
          g_wiz_netinfo.mac[4], g_wiz_netinfo.mac[5]);
  KPrintf("SIP: %d.%d.%d.%d\r\n", g_wiz_netinfo.ip[0], g_wiz_netinfo.ip[1],
          g_wiz_netinfo.ip[2], g_wiz_netinfo.ip[3]);
  KPrintf("GAR: %d.%d.%d.%d\r\n", g_wiz_netinfo.gw[0], g_wiz_netinfo.gw[1],
          g_wiz_netinfo.gw[2], g_wiz_netinfo.gw[3]);
  KPrintf("SUB: %d.%d.%d.%d\r\n", g_wiz_netinfo.sn[0], g_wiz_netinfo.sn[1],
          g_wiz_netinfo.sn[2], g_wiz_netinfo.sn[3]);
  KPrintf("DNS: %d.%d.%d.%d\r\n", g_wiz_netinfo.dns[0], g_wiz_netinfo.dns[1],
          g_wiz_netinfo.dns[2], g_wiz_netinfo.dns[3]);
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

/****************** basic functions ********************/

enum TCP_OPTION {
  SEND_DATA = 0,
  RECV_DATA,
};

uint32_t wiz_client_op(uint8_t sn, uint8_t *buf, uint32_t buf_size,
                       uint8_t dst_ip[4], uint16_t dst_port,
                       enum TCP_OPTION opt) {
  // assert(buf_size <= g_wiznet_buf_size);
  int32_t ret;
  switch (getSn_SR(sn)) {
    case SOCK_CLOSE_WAIT:
      wiz_sock_disconnect(sn);
      break;
    case SOCK_CLOSED:
      wiz_socket(sn, Sn_MR_TCP, 5000, 0x00);
      break;
    case SOCK_INIT:
      KPrintf("[SOCKET CLIENT] sock init.\n");
      wiz_sock_connect(sn, dst_ip, dst_port);
      break;
    case SOCK_ESTABLISHED:
      if (getSn_IR(sn) & Sn_IR_CON) {
        printf("[SOCKET CLIENT] %d:Connected\r\n", sn);
        setSn_IR(sn, Sn_IR_CON);
      }
      if (opt == SEND_DATA) {
        uint32_t sent_size = 0;
        ret = wiz_sock_send(sn, buf, buf_size);
        if (ret < 0) {
          wiz_sock_close(sn);
          return ret;
        }
      } else if (opt == RECV_DATA) {
        uint32_t size = 0;
        if ((size = getSn_RX_RSR(sn)) > 0) {
          if (size > buf_size) size = buf_size;
          ret = wiz_sock_recv(sn, buf, size);
          if (ret <= 0) return ret;
        }
      }
      break;
    default:
      break;
  }
}

void wiz_client_op_test(char *addr, uint16_t port, char *msg) {
  /* argv[1]: ip
   * argv[2]: port
   * argv[3]: msg
   */
  uint8_t client_sock = 2;
  uint8_t ip[4] = {192, 168, 31, 127};
  uint32_t tmp_ip[4];
  KPrintf("wiz client to %s", addr);
  sscanf(addr, "%d.%d.%d.%d", &tmp_ip[0], &tmp_ip[1], &tmp_ip[2], &tmp_ip[3]);
  for (int i = 0; i < 4; ++i) {
    ip[i] = (uint8_t)tmp_ip[i];
  }
  uint8_t buf[g_wiznet_buf_size];
  KPrintf("wiz_server, send to %d.%d.%d.%d %d\n",  // tip info
          ip[0], ip[1], ip[2], ip[3], port);
  sscanf(msg, "%s", buf);
  wiz_client_op(client_sock, buf, g_wiznet_buf_size, ip, port, SEND_DATA);
  MdelayKTask(10);
  memset(buf, 0, g_wiznet_buf_size);
  // waiting for a responding.
  wiz_client_op(client_sock, buf, g_wiznet_buf_size, ip, port, RECV_DATA);
  KPrintf("received msg: %s\n", buf);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) |
                     SHELL_CMD_PARAM_NUM(3),
                 wiz_client_op, wiz_client_op_test,
                 wiz_sock_recv or wiz_sock_send data as tcp client);

int32_t wiz_server_op(uint8_t sn, uint8_t *buf, uint32_t buf_size,
                      uint16_t port, enum TCP_OPTION opt) {
  int32_t ret = 0;
  uint16_t size = 0, sentsize = 0;
  switch (getSn_SR(sn)) {
    case SOCK_ESTABLISHED:
      if (getSn_IR(sn) & Sn_IR_CON) {
        printf("%d:Connected\r\n", sn);
        setSn_IR(sn, Sn_IR_CON);
      }
      if (opt == SEND_DATA) {
        uint32_t sent_size = 0;
        ret = wiz_sock_send(sn, buf, buf_size);
        if (ret < 0) {
          wiz_sock_close(sn);
          return ret;
        }
      } else if (opt == RECV_DATA) {
        uint32_t size = 0;
        if ((size = getSn_RX_RSR(sn)) > 0) {
          if (size > buf_size) size = buf_size;
          ret = wiz_sock_recv(sn, buf, size);
          return ret;
        }
      }
      break;
    case SOCK_CLOSE_WAIT:
      printf("%d:CloseWait\r\n", sn);
      if ((ret = wiz_sock_disconnect(sn)) != SOCK_OK) return ret;
      printf("%d:Closed\r\n", sn);
      break;
    case SOCK_INIT:
      printf("%d:Listen, port [%d]\r\n", sn, port);
      if ((ret = wiz_sock_listen(sn)) != SOCK_OK) return ret;
      break;
    case SOCK_CLOSED:
      printf("%d:LBTStart\r\n", sn);
      if ((ret = wiz_socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
      printf("%d:Opened\r\n", sn);
      break;
    default:
      break;
  }
  return 0;
}

void wiz_server(void *param) {
  uint16_t port = *(uint16_t *)param;
  KPrintf("wiz server, listen port: %d\n", port);
  uint8_t buf[g_wiznet_buf_size];
  memset(buf, 0, g_wiznet_buf_size);
  int ret = 0;
  while (1) {
    ret = wiz_server_op(0, buf, g_wiznet_buf_size, port, RECV_DATA);
    if (ret > 0) {
      wiz_server_op(0, buf, g_wiznet_buf_size, port, SEND_DATA);
    };
  }
}

void wiz_server_test(uint16_t port) {
  /* argv[1]: port
   */
  int32 wiz_server_id =
      KTaskCreate("wiz_server", wiz_server, (void *)&port, 4096, 25);
  x_err_t flag = StartupKTask(wiz_server_id);
  if (flag != EOK) {
    KPrintf("StartupKTask wiz_server_id failed .\n");
  }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) |
                     SHELL_CMD_PARAM_NUM(1),
                 wiz_server_op, wiz_server_test,
                 wiz_sock_recv or wiz_sock_send data as tcp server);

int32_t loopback_udps(uint8_t sn, uint8_t *buf, uint16_t port) {
  int32_t ret;
  uint16_t size, sentsize;
  uint8_t destip[4];
  uint16_t destport;
  // uint8_t  packinfo = 0;
  switch (getSn_SR(sn)) {
    case SOCK_UDP:
      if ((size = getSn_RX_RSR(sn)) > 0) {
        if (size > g_wiznet_buf_size) size = g_wiznet_buf_size;
        ret = wiz_sock_recvfrom(sn, buf, size, destip, (uint16_t *)&destport);
        if (ret <= 0) {
          printf("%d: wiz_sock_recvfrom error. %ld\r\n", sn, ret);
          return ret;
        }
        size = (uint16_t)ret;
        sentsize = 0;
        while (sentsize != size) {
          ret = wiz_sock_sendto(sn, buf + sentsize, size - sentsize, destip,
                                destport);
          if (ret < 0) {
            printf("%d: wiz_sock_sendto error. %ld\r\n", sn, ret);
            return ret;
          }
          sentsize += ret;  // Don't care SOCKERR_BUSY, because it is zero.
        }
      }
      break;
    case SOCK_CLOSED:
      printf("%d:LBUStart\r\n", sn);
      if ((ret = wiz_socket(sn, Sn_MR_UDP, port, 0x00)) != sn) return ret;
      printf("%d:Opened, port [%d]\r\n", sn, port);
      break;
    default:
      break;
  }
  return 1;
}

void ifconfig() {
  wiz_NetInfo wiz_netinfo;
  ctlnetwork(CN_GET_NETINFO, (void *)&wiz_netinfo);
  uint8_t tmpstr[6];
  ctlwizchip(CW_GET_ID, (void *)tmpstr);
  KPrintf("=== %s NET CONF ===\r\n", (char *)tmpstr);
  KPrintf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", wiz_netinfo.mac[0],
          wiz_netinfo.mac[1], wiz_netinfo.mac[2], wiz_netinfo.mac[3],
          wiz_netinfo.mac[4], wiz_netinfo.mac[5]);
  KPrintf("SIP: %d.%d.%d.%d\r\n", wiz_netinfo.ip[0], wiz_netinfo.ip[1],
          wiz_netinfo.ip[2], wiz_netinfo.ip[3]);
  KPrintf("GAR: %d.%d.%d.%d\r\n", wiz_netinfo.gw[0], wiz_netinfo.gw[1],
          wiz_netinfo.gw[2], wiz_netinfo.gw[3]);
  KPrintf("SUB: %d.%d.%d.%d\r\n", wiz_netinfo.sn[0], wiz_netinfo.sn[1],
          wiz_netinfo.sn[2], wiz_netinfo.sn[3]);
  KPrintf("DNS: %d.%d.%d.%d\r\n", wiz_netinfo.dns[0], wiz_netinfo.dns[1],
          wiz_netinfo.dns[2], wiz_netinfo.dns[3]);
  KPrintf("======================\r\n");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 ifconfig, ifconfig, printf w5500 configurations);

void char_arr_assign(uint8_t **dst, uint32_t *src, uint32_t len) {
  for (int i = 0; i < len; ++i) {
    (*dst)[i] = (uint8_t)(src[i]);
  }
}

void config_w5500_network(char *mac, char *ip, char *sn, char *gw, char *dns) {
  wiz_NetInfo wiz_netinfo;
  uint32_t tmp_arr[4];
  // config netinfo
  sscanf(mac, "%d.%d.%d.%d", &tmp_arr[0], &tmp_arr[1], &tmp_arr[2],
         &tmp_arr[3]);
  char_arr_assign((uint8_t **)&wiz_netinfo.mac, tmp_arr, 4);
  sscanf(ip, "%d.%d.%d.%d", &tmp_arr[0], &tmp_arr[1], &tmp_arr[2], &tmp_arr[3]);
  char_arr_assign((uint8_t **)&wiz_netinfo.ip, tmp_arr, 4);
  sscanf(sn, "%d.%d.%d.%d", &tmp_arr[0], &tmp_arr[1], &tmp_arr[2], &tmp_arr[3]);
  char_arr_assign((uint8_t **)&wiz_netinfo.sn, tmp_arr, 4);
  sscanf(gw, "%d.%d.%d.%d", &tmp_arr[0], &tmp_arr[1], &tmp_arr[2], &tmp_arr[3]);
  char_arr_assign((uint8_t **)&wiz_netinfo.gw, tmp_arr, 4);
  sscanf(dns, "%d.%d.%d.%d", &tmp_arr[0], &tmp_arr[1], &tmp_arr[2],
         &tmp_arr[3]);
  char_arr_assign((uint8_t **)&wiz_netinfo.dns, tmp_arr, 4);
  // set new netinfo
  ctlnetwork(CN_SET_NETINFO, (void *)&wiz_netinfo);
  ifconfig();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) |
                     SHELL_CMD_PARAM_NUM(5),
                 config_w5500_network, config_w5500_network,
                 set w5500 configurations);