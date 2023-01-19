/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *        Copyright 2014, 2015 Big Switch Networks, Inc.
 *        Copyright 2021 Asterfusion Data Technologies Co., Ltd.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#include <onlp/platformi/sysi.h>
#include <onlp/platformi/ledi.h>
#include <onlp/platformi/psui.h>
#include <onlp/platformi/thermali.h>
#include <onlp/platformi/fani.h>
#include <onlplib/file.h>
#include <onlplib/crc32.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "platform_lib.h"

const char*
onlp_sysi_platform_get(void)
{   
    return "x86-64-asterfusion-x308p-r0";
}

int
onlp_sysi_init(void)
{    
    return ONLP_STATUS_OK;
}

int
onlp_sysi_onie_data_get(uint8_t** data, int* size)
{
    data = data;
    size = size;
    return ONLP_STATUS_E_UNSUPPORTED;
}

void onlp_sysi_onie_data_free(uint8_t* data) {
    data = data;
    return;
}

int onlp_sysi_onie_data_phys_addr_get(void** physaddr)
{
    physaddr = physaddr;
    /* We cannot supply this API to map a physical eeprom */
    return ONLP_STATUS_E_INTERNAL;
}

int
onlp_sysi_oids_get(onlp_oid_t* table, int max)
{
    onlp_oid_t* e = table;
    memset(table, 0, max*sizeof(onlp_oid_t));
    int i;

     /* 2 PSUs */
    *e++ = ONLP_PSU_ID_CREATE(1);
    *e++ = ONLP_PSU_ID_CREATE(2);

    /* 4 LEDs Item */
    for (i = 1; i <= LED_NUM; i++) {
        *e++ = ONLP_LED_ID_CREATE(i);
    }

    /* 10 THERMALs Item */
    for (i = 1; i <= THERMAL_NUM; i++) {
        *e++ = ONLP_THERMAL_ID_CREATE(i);
    }

    /* 10 Fans Item */
    for (i = 1; i <= FAN_NUM; i++) {
        *e++ = ONLP_FAN_ID_CREATE(i);
    }
    
    return ONLP_STATUS_OK;
}

int onlp_sysi_onie_info_get(onlp_onie_info_t* onie)
{
    int rc;
    if ((rc = pltfm_onie_info_get(onie)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }
    
    return ONLP_STATUS_OK;
}

int
onlp_sysi_platform_manage_init(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_sysi_platform_manage_fans(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_sysi_platform_manage_leds(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_sysi_platform_info_get(onlp_platform_info_t* pi)
{
    char *cpld_version = "Unknown";
    char *other_version = "Unknown";
    
    pi->cpld_versions = aim_memdup((void*)cpld_version, strlen(cpld_version));
    pi->other_versions = aim_memdup((void*)other_version, strlen(cpld_version));
    
    return ONLP_STATUS_OK;
}


int onlp_sysi_platform_set(const char* platform)
{
    platform = platform;
    return ONLP_STATUS_OK;
}

void onlp_sysi_platform_info_free(onlp_platform_info_t* info)
{
    aim_free(info->cpld_versions);
    aim_free(info->other_versions);
}

int onlp_sysi_ioctl(int code, va_list vargs)
{
    code = code;
    vargs = vargs;
    return ONLP_STATUS_E_UNSUPPORTED;
}
int onlp_sysi_debug(aim_pvs_t* pvs, int argc, char** argv)
{
    pvs = pvs;
    argc = argc;
    argv = argv;
    return ONLP_STATUS_E_UNSUPPORTED;
}

