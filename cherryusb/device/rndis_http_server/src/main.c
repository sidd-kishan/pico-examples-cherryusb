/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "usbd_core.h"
#include "usbd_rndis.h"
#include "cdc_rndis_device.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "lwip.h"


static volatile absolute_time_t next_wifi_try;
static volatile absolute_time_t comm_manager;
char connect_ssid[50], connect_password[80];
int connect_sec,connect_config;

void printline(int cdc,char string[],int len){
	char buf[2048];
	//memcpy(buf,"\r\n",2);
	//memcpy(buf+2,string,len);
	//memcpy(buf+2+len,"\r\n\r\n\0",5);
	sprintf(buf,"\r\n%s\r\n\0",string);
	cdc_acm_data_send_with_dtr(cdc,(uint8_t *)buf,strlen(buf));
}

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
	char wlan_scan_buffer[120];
    if (result) { 
        int len = sprintf(wlan_scan_buffer, "ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u test-ssid-sec: %d",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode, CYW43_AUTH_WPA2_AES_PSK);
		//memcpy(read_queue[0].buffer,wlan_scan_buffer,100);
		//read_queue[0].tail=len;
		printline(2,wlan_scan_buffer,len);
    }
    return 0;
}

void core1(){
    //user_init_lwip();
    lwip_init();
	
    while(true) {
        /**/
		//printline(2,(char *)read_queue[0].buffer,read_queue[0].tail);
		//read_queue[0].tail=0;
		//memcpy(connect_buf,(char *)read_queue[0].buffer,read_queue[0].tail);
		//read_queue[0].tail=0;
		//sscanf(connect_buf, "ssid: %*s password: %*s sec: %d", connect_ssid, connect_password, &connect_sec);
		connect_config=0;
		for(int i=0,j=0;i<read_queue[0].tail;i++){
			if(read_queue[0].buffer[i]==' '){
				connect_config+=1;
				j=0;
			}
			if(read_queue[0].buffer[i] != ' ' && connect_config==0){
				connect_ssid[j++]=read_queue[0].buffer[i];
			} else if(read_queue[0].buffer[i] != ' ' && connect_config==1){
				connect_password[j++]=read_queue[0].buffer[i];
			} else {
				connect_sec=read_queue[0].buffer[i];
			}
		}
		read_queue[0].tail=0;
		printline(3,(char *)read_queue[1].buffer,read_queue[1].tail);
		read_queue[1].tail=0;
		printline(4,(char *)read_queue[2].buffer,read_queue[2].tail);
		read_queue[2].tail=0;
		
		
		absolute_time_t start_time = get_absolute_time ();
		while (absolute_time_diff_us (start_time, get_absolute_time()) < 1000000);
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
	
	absolute_time_t scan_time = nil_time;
    bool scan_in_progress = false;
	next_wifi_try = nil_time;
	while (1) {
		if (!link_up) {
			if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
				if (!scan_in_progress) {
					cyw43_wifi_scan_options_t scan_options = {0};
					int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
					if (err == 0) {
						//printf("\nPerforming wifi scan\n");
						scan_in_progress = true;
					} else {
						//printf("Failed to start scan: %d\n", err);
						scan_time = make_timeout_time_ms(2000); // wait 2s and scan again
					}
				} else if (!cyw43_wifi_scan_active(&cyw43_state)) {
					scan_time = make_timeout_time_ms(2000); // wait 2s and scan again
					scan_in_progress = false; 
				}
			}
            if (absolute_time_diff_us(get_absolute_time(), next_wifi_try) < 0) {
                //cyw43_arch_wifi_connect_async("ssid", "password", CYW43_AUTH_WPA2_AES_PSK);
				//connect_buf_len = sprintf(connect_buf, "ssid: %*s password: %*s sec: %d", connect_ssid, connect_password, connect_sec);
				//connect_buf[connect_buf_len+1]=' ';
				printline(2,"----------",10);
				printline(2,connect_ssid,strlen(connect_ssid));
				printline(2,connect_password,strlen(connect_password));
				printline(2,"----------",10);
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

