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

static onlp_psu_info_t pinfo[] =
{
    { }, /* Not used */
    {
        { ONLP_PSU_ID_CREATE(PSU1_ID), "PSU-1", 0, { 0, } },
        "N/A",
        "N/A",
        0,
        ONLP_PSU_CAPS_AC | ONLP_PSU_CAPS_DC12 | ONLP_PSU_CAPS_DC48 | ONLP_PSU_CAPS_IIN | ONLP_PSU_CAPS_IOUT | ONLP_PSU_CAPS_VIN | ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT,
    },
    {
        { ONLP_PSU_ID_CREATE(PSU2_ID), "PSU-2", 0, { 0, } },
        "N/A",
        "N/A",
        0,
        ONLP_PSU_CAPS_AC | ONLP_PSU_CAPS_DC12 | ONLP_PSU_CAPS_DC48 | ONLP_PSU_CAPS_IIN | ONLP_PSU_CAPS_IOUT | ONLP_PSU_CAPS_VIN | ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT,
    }
};

int
onlp_psui_init(void)
{    
    return ONLP_STATUS_OK;
}

static int 
psu_status_info_get(int id, onlp_psu_info_t *info)
{   
    int pw_exist = 0, pw_good = 0, rc;  
    
    if ((id != PSU1_ID) && 
        (id != PSU2_ID))
        return ONLP_STATUS_E_INTERNAL;
    
     /* Get power present status */
    if ((rc = pltfm_psu_present_get(&pw_exist, id)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (pw_exist != PSU_STATUS_PRESENT) {
        info->status &= ~ONLP_PSU_STATUS_PRESENT;
        info->status |=  ONLP_PSU_STATUS_FAILED;    
        return ONLP_STATUS_OK;
    }    
    info->status |= ONLP_PSU_STATUS_PRESENT;
    
    /* Get power good status */
    if ((rc = pltfm_psu_pwgood_get(&pw_good, id)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }   
    
    if (pw_good != PSU_STATUS_POWER_GOOD) {        
        info->status |= ONLP_PSU_STATUS_UNPLUGGED;
        return ONLP_STATUS_OK;
    } else {
        info->status &= ~ONLP_PSU_STATUS_UNPLUGGED;
    }

    if ((rc = pltfm_psu_get(info, id)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }
    
    return ONLP_STATUS_OK;
}

int
onlp_psui_info_get(onlp_oid_t id, onlp_psu_info_t* info)
{        
    int pid;
    
    pid = ONLP_OID_ID_GET(id);
    memset(info, 0, sizeof(onlp_psu_info_t));
    
    /* Set the onlp_oid_hdr_t */
    *info = pinfo[pid]; 

    switch (pid) {
        case PSU1_ID:
        case PSU2_ID:
            return psu_status_info_get(pid, info);
            break;
        default:
            return ONLP_STATUS_E_INVALID;
            break;
    }

    return ONLP_STATUS_OK;


}
int onlp_psui_status_get(onlp_oid_t id, uint32_t* rv) {
    id = id;
    rv = 0;
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

