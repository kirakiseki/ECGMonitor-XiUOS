
#include <string.h>
#include <shell.h>
#include <debug.h>

#include "lwip/apps/http_client.h"

void httpc_app_recv_end(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    httpc_state_t **req = (httpc_state_t**)arg;

    LWIP_DEBUGF(LWIP_DEBUG, ("[HTTPC] Transfer finished. rx_content_len is %lu\r\n", rx_content_len));
    printf("[HTTPC] Transfer finished. rx_content_len is %lu\r\n", rx_content_len);
    *req = NULL;
}


err_t httpc_app_headers_done(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {
    LWIP_DEBUGF(LWIP_DEBUG, ("[%s] headers done call back.\n", __func__));
    printf("[%s] headers done call back, content len: %d.\n", __func__, content_len);
    return ERR_OK;
}

err_t httpc_app_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    printf("[%s] Get %d Data\n", __func__, p->len);
   
    pbuf_free(p);
    return ERR_OK;
}

ip_addr_t *get_server_ip() {
    static ip_addr_t server_ip;
    return &server_ip;
}

httpc_state_t **get_conn_state() {
    static httpc_state_t *conn_state;
    return &conn_state;
}

httpc_connection_t *get_conn_setting() {
    static httpc_connection_t conn_setting;
    return &conn_setting;
}

void httpc_get_file_app(int argc, char *argv[]) {
    // deal input ip

    // get file from server
    const uint8_t server_ip_by_arr[4] = {39, 156, 66, 10};
    // const uint8_t server_ip_by_arr[4] = {114, 215, 151, 106};
    IP4_ADDR(get_server_ip(), 
        server_ip_by_arr[0], server_ip_by_arr[1], server_ip_by_arr[2], server_ip_by_arr[3]);

    get_conn_setting()->use_proxy = 0;
    get_conn_setting()->result_fn = httpc_app_recv_end;
    get_conn_setting()->headers_done_fn = httpc_app_headers_done;

    LWIP_DEBUGF(HTTPC_DEBUG, ("[%s] Calling httpc_get_file\n", __func__));
    printf("[%s] Calling httpc_get_file\n", __func__);
    err_t errnum = httpc_get_file(get_server_ip(), 80, "/index.html", get_conn_setting(), httpc_app_recv, NULL, get_conn_state());
    // err_t errnum = httpc_get_file_dns("https://www.baidu.com", 80, "/index.html", get_conn_setting(), httpc_app_recv, NULL, get_conn_state());

    if (errnum != ERR_OK) {
        printf("httpc_get_file failed (%d)\n", errnum);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_PARAM_NUM(0),
     lwip_http_test, httpc_get_file_app, get file from net server);