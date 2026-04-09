/**
 * @file holoCoreSntp.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-28
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include "holoCoreSntp.h"
#include "sntp.h"
#include <utils_time.h>
#define sntp_server "ntp.aliyun.com"
#define UTC 8 // your Timezone, for example,beijing Timezone is GMT+8
void _startup_sntp(void *arg)
{
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, sntp_server);
	sntp_init();
}