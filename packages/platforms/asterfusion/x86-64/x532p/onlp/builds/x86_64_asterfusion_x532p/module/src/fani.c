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
 * Fan Platform Implementation Defaults.
 *
 ***********************************************************/
#include <onlp/platformi/fani.h>
#include "x86_64_asterfusion_x532p_int.h"
#include <onlplib/file.h>
#include <onlplib/i2c.h>
#include "platform_lib.h"

onlp_fan_info_t fan_info[] = {
    { }, /* Not used */
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN1), "FANTRAY 1", 0 },
        ONLP_FAN_STATUS_PRESENT,
        //ONLP_FAN_CAPS_SET_RPM | ONLP_FAN_CAPS_GET_RPM,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN2), "FANTRAY 2", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN3), "FANTRAY 3", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN4), "FANTRAY 4", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN5), "FANTRAY 5", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN6), "FANTRAY 6", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN7), "FANTRAY 7", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN8), "FANTRAY 8", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN9), "FANTRAY 9", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN10), "FANTRAY 10", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    }
};
    
/*
 * This function will be called prior to all of onlp_fani_* functions.
 */
int
onlp_fani_init(void)
{
    return ONLP_STATUS_OK;
}


int
onlp_fani_rpm_set(onlp_oid_t id, int rpm)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/*
 * This function sets the fan speed of the given OID as a percentage.
 *
 * This will only be called if the OID has the PERCENTAGE_SET
 * capability.
 *
 * It is optional if you have no fans at all with this feature.
 */
int
onlp_fani_percentage_set(onlp_oid_t id, int percentage)
{
    int  fid, perc_val = 0, rc;
    fid = ONLP_OID_ID_GET(id);

    /* Set fan speed 
       Driver accept value in range between 128 and 255.
       Value 10 is 10%.
       Value 20 is 20%.
       Value 30 is 30%.
       ...
    */

    if (percentage < 0 || percentage >= 100) {
        return ONLP_STATUS_E_INVALID;
    } else {
        if (percentage < 10)
            perc_val = 1;
        else
            perc_val = percentage/10;
    }
    
    switch (fid)
	{
        case FAN_ID_FAN1:    
        case FAN_ID_FAN2:
        case FAN_ID_FAN3:
        case FAN_ID_FAN4:
        case FAN_ID_FAN5:    
        case FAN_ID_FAN6:
        case FAN_ID_FAN7:
        case FAN_ID_FAN8:
        case FAN_ID_FAN9:
        case FAN_ID_FAN10:
			rc = onlp_fani_rpm_set(id, perc_val);
            break;
        default:
            return ONLP_STATUS_E_INVALID;
    }
	return rc;   
}

int
onlp_fani_info_get(onlp_oid_t id, onlp_fan_info_t* info)
{
    int fan_id ,rc;
    
    fan_id = ONLP_OID_ID_GET(id);
    *info = fan_info[fan_id];
    info->caps |= ONLP_FAN_CAPS_GET_RPM;
       
    switch (fan_id) {
        case FAN_ID_FAN1:    
        case FAN_ID_FAN2:
        case FAN_ID_FAN3:
        case FAN_ID_FAN4:
        case FAN_ID_FAN5:    
        case FAN_ID_FAN6:
        case FAN_ID_FAN7:
        case FAN_ID_FAN8:
        case FAN_ID_FAN9:
        case FAN_ID_FAN10:
            rc = pltfm_fan_info_get(info, fan_id);
            break;
        default:            
            return ONLP_STATUS_E_INTERNAL;
            break;
    }

    return rc;
}

int onlp_fani_status_get(onlp_oid_t id, uint32_t* rv)
{
    return ONLP_STATUS_OK;
}

int onlp_fani_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* hdr)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_fani_mode_set(onlp_oid_t id, onlp_fan_mode_t mode)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_fani_dir_set(onlp_oid_t id, onlp_fan_dir_t dir)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_fani_ioctl(onlp_oid_t fid, va_list vargs)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

