#include "lwip/init.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/pbuf.h"
#include "pico/stdlib.h"

extern struct pbuf *out_pkt;
extern volatile bool link_up;
static void  lwip_service_traffic(void);