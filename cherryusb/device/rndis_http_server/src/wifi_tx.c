#include "lwip.h"
#include "pico/cyw43_arch.h"

#include "usbd_core.h"
#include "usbd_rndis.h"

struct pbuf *out_pkt;
volatile bool link_up = false;

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
