/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "dhserver.h"
#include "dnserver.h"
#include "netif/etharp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/icmp.h"
#include "lwip/udp.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "httpd.h"
#include "usbd_core.h"
#include "usbd_rndis.h"
#include "cdc_rndis_device.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

struct pbuf *out_pkt;
volatile bool link_up = false;
static volatile absolute_time_t next_wifi_try;

void cyw43_cb_tcpip_set_link_up(cyw43_t *self, int itf) {
    if(!link_up){
		link_up = true;
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, link_up);
	}
}

void cyw43_cb_tcpip_set_link_down(cyw43_t *self, int itf) {
    if(link_up){
		link_up = false;
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, link_up);
	}
}

void cyw43_cb_process_ethernet(void *cb_data, int itf, size_t len, const uint8_t *buf) {
	out_pkt = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	//memcpy(out_pkt->payload, buf, len);
	pbuf_take(out_pkt, buf, len);
	int ret = usbd_rndis_eth_tx(out_pkt);
	if (0 != ret) {
        ret = ERR_BUF;
    }
	pbuf_free(out_pkt);
	out_pkt = NULL;
}


/* Macro Definition */
#define LWIP_SYS_TIME_MS 1
#define NUM_DHCP_ENTRY   3
#define PADDR(ptr)       ((ip_addr_t *)ptr)

/* Static Variable Definition*/
static uint8_t hwaddr[6]  = { 0x20, 0x89, 0x84, 0x6A, 0x96, 00 };
static uint8_t ipaddr[4]  = { 192, 168, 7, 1 };
static uint8_t netmask[4] = { 255, 255, 255, 0 };
static uint8_t gateway[4] = { 0, 0, 0, 0 };

static uint32_t     sys_tick;
static struct netif netif_data;

/* Static Function Declaration */
static void  user_init_lwip(void);
static err_t netif_init_cb(struct netif *netif);
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p);
static void  lwip_service_traffic(void);
static bool  dns_query_proc(const char *name, ip_addr_t *addr);

/* Function Definition */
void sys_timer_callback(void)
{
    sys_tick++;
}

uint32_t sys_now(void)
{
    return sys_tick;
}

static void user_init_lwip(void)
{
    struct netif *netif = &netif_data;

    lwip_init();
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, hwaddr, 6);

    netif = netif_add(netif, PADDR(ipaddr), PADDR(netmask), PADDR(gateway), NULL, netif_init_cb, netif_input);
    netif_set_default(netif);
}

static err_t netif_init_cb(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu        = 1500;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state      = NULL;
    netif->name[0]    = 'E';
    netif->name[1]    = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output     = etharp_output;
    return ERR_OK;
}

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    int ret;

    ret = usbd_rndis_eth_tx(p);

    if (0 != ret) {
        ret = ERR_BUF;
    }

    return ret;
}

static void lwip_service_traffic(void)
{
    //err_t        err;
    struct pbuf *p;

    p = usbd_rndis_eth_rx();

    if (p != NULL) {
        /* entry point to the LwIP stack */
        int eth_frame_send_success=cyw43_send_ethernet(&cyw43_state, CYW43_ITF_STA, p->tot_len, (void*)p, true);
		p = (struct pbuf *) eth_frame_send_success;
	    //err = netif_data.input(p, &netif_data);
		pbuf_free(p);
    }
}

void core1(){
    user_init_lwip();
    //while (!netif_is_up(&netif_data)) {
    //    ;
    //}
    //httpd_init();
	//USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

    while (1) {
		cdc_acm_data_send_with_dtr(2,read_buffer[0],10);
		cdc_acm_data_send_with_dtr(3,read_buffer[1],10);
    }
}

int main(void)
{
    set_sys_clock_khz(200000, true);
	uint8_t rndis_mac[6] = { 0x20, 0x89, 0x84, 0x6A, 0x96, 0xAA };
	cyw43_arch_init_with_country(CYW43_COUNTRY_INDIA);
    cyw43_arch_enable_sta_mode();
	cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 20, 1, 1, 1));
	cyw43_hal_get_mac(0, rndis_mac);

    cdc_rndis_init(rndis_mac);
	
	multicore_launch_core1(core1);
	
	while (1) {
		if (!link_up) {
            if (absolute_time_diff_us(get_absolute_time(), next_wifi_try) < 0) {
                cyw43_arch_wifi_connect_async("ssid", "password", CYW43_AUTH_WPA2_AES_PSK);
                next_wifi_try = make_timeout_time_ms(10000);
            }
        } else {
			struct pbuf *p;
			p = usbd_rndis_eth_rx();
			if (p != NULL) {
				/* entry point to the LwIP stack */
				int eth_frame_send_success=cyw43_send_ethernet(&cyw43_state, CYW43_ITF_STA, p->tot_len, (void*)p, true);
				//err = netif_data.input(p, &netif_data);
				pbuf_free(p);
				p = (struct pbuf *) eth_frame_send_success;
			}
		}
    }

    return 0;
}

