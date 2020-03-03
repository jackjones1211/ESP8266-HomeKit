#ifndef __SMART_H
#define __SMART_H


void ICACHE_FLASH_ATTR
smartconfig_task(void *pvParameters);

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata);

#endif