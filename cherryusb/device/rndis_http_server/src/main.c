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

/* Macro Definition */
#define LWIP_SYS_TIME_MS 1
#define NUM_DHCP_ENTRY   3
#define PADDR(ptr)       ((ip_addr_t *)ptr)

/* Static Variable Definition*/
static uint8_t hwaddr[6]  = { 0x20, 0x89, 0x84, 0x6A, 0x96, 00 };
static uint8_t ipaddr[4]  = { 192, 168, 7, 1 };
static uint8_t netmask[4] = { 255, 255, 255, 0 };
static uint8_t gateway[4] = { 0, 0, 0, 0 };

static dhcp_entry_t entries[NUM_DHCP_ENTRY] = {
    /* mac    ip address        subnet mask        lease time */
    { { 0 }, { 192, 168, 7, 2 }, { 255, 255, 255, 0 }, 24 * 60 * 60 },
    { { 0 }, { 192, 168, 7, 3 }, { 255, 255, 255, 0 }, 24 * 60 * 60 },
    { { 0 }, { 192, 168, 7, 4 }, { 255, 255, 255, 0 }, 24 * 60 * 60 }
};

static dhcp_config_t dhcp_config = {
    { 192, 168, 7, 1 }, /* server address */
    67,                 /* port */
    { 192, 168, 7, 1 }, /* dns server */
    "hpm",              /* dns suffix */
    NUM_DHCP_ENTRY,     /* num entry */
    entries             /* entries */
};

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
    err_t        err;
    struct pbuf *p;

    p = usbd_rndis_eth_rx();

    if (p != NULL) {
        /* entry point to the LwIP stack */
        err = netif_data.input(p, &netif_data);

        if (err != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
        }
    }
}

static bool dns_query_proc(const char *name, ip_addr_t *addr)
{
    if (strcmp(name, "rndis.hpm") == 0 || strcmp(name, "www.rndis.hpm") == 0) {
        addr->addr = *(uint32_t *)ipaddr;
        return true;
    }
    return false;
}

void core1(){
    user_init_lwip();
    while (!netif_is_up(&netif_data)) {
        ;
    }
    while (dhserv_init(&dhcp_config) != ERR_OK) {
        ;
    }
    while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK) {
        ;
    }
    httpd_init();

    while (1) {
        lwip_service_traffic();
    }
}

int main(void)
{
    
    printf("cherry usb rndis device sample.\n");
	
	uint8_t rndis_mac[6] = { 0x20, 0x89, 0x84, 0x6A, 0x96, 0xAA };

    cdc_rndis_init(rndis_mac);
	
	multicore_launch_core1(core1);
	
	while (1) {
    }

    return 0;
}

