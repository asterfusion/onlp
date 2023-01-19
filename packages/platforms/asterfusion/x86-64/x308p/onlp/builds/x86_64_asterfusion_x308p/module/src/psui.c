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
#include <onlp/platformi/psui.h>
#include <onlplib/file.h>

#include "platform_lib.h"

#define VALIDATE(_id)                           \
    do {                                        \
        if(!ONLP_OID_IS_PSU(_id)) {             \
            return ONLP_STATUS_E_INVALID;       \
        }                                       \
    } while(0)

static onlp_psu_info_t pinfo[] =
{
    { }, /* Not used */
    {
        { ONLP_PSU_ID_CREATE(PSU1_ID), "PSU-1", 0, { 0, }},
        "N/A",
        "N/A",
        0,
        ONLP_PSU_CAPS_AC | ONLP_PSU_CAPS_VIN | ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_IIN | ONLP_PSU_CAPS_IOUT |
        ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT,
        0,
        0,
        0,
        0,
        0,
        0,
    },
    {
        { ONLP_PSU_ID_CREATE(PSU2_ID), "PSU-2", 0, { 0, }},
        "N/A",
        "N/A",
        0,
        ONLP_PSU_CAPS_AC | ONLP_PSU_CAPS_VIN | ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_IIN | ONLP_PSU_CAPS_IOUT |
        ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT,
        0,
        0,
        0,
        0,
        0,
        0,
    },
};

int
onlp_psui_init(void)
{    
    return ONLP_STATUS_OK;
}

int
onlp_psui_info_get(onlp_oid_t id, onlp_psu_info_t* info)
{
    int pid;
    int pw_exist = 0, pw_good = 0;

    VALIDATE(id);

    pid = ONLP_OID_ID_GET(id);
    /* Set the onlp_oid_hdr_t */
    *info = pinfo[pid];

     /* Get power present status */
    if (pltfm_psu_present_get(&pw_exist, pid) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (pw_exist != PSU_STATUS_PRESENT) {
        info->status &= ~ONLP_PSU_STATUS_PRESENT;
        info->status |=  ONLP_PSU_STATUS_FAILED;    
    } else {
        info->status |= ONLP_PSU_STATUS_PRESENT;

        /* Get power good status */
        if (pltfm_psu_pwgood_get(&pw_good, pid) != ONLP_STATUS_OK) {
            return ONLP_STATUS_E_INTERNAL;
        }

        if (pw_good != PSU_STATUS_POWER_GOOD) {
            info->status |= ONLP_PSU_STATUS_UNPLUGGED;
        } else {
            info->status &= ~ONLP_PSU_STATUS_UNPLUGGED;
        }

        if (pltfm_psu_get(info, pid) != ONLP_STATUS_OK) {
            return ONLP_STATUS_E_INTERNAL;
        }
    }
    return ONLP_STATUS_OK;
}

int onlp_psui_status_get(onlp_oid_t id, uint32_t* rv) {
    id = id;
    rv = rv;
    return ONLP_STATUS_OK;
}
int onlp_psui_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* rv) { 
    id = id;
    rv = rv;
    return ONLP_STATUS_E_UNSUPPORTED;
}
int onlp_psui_ioctl(onlp_oid_t pid, va_list vargs) {
    pid = pid;
    vargs = vargs;
    return ONLP_STATUS_E_UNSUPPORTED;
}

