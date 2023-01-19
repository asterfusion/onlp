/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *        Copyright 2014, 2015 Big Switch Networks, Inc.
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
#include "x86_64_asterfusion_x564p_int.h"
#include <onlplib/file.h>
#include <onlplib/i2c.h>
#include "platform_lib.h"

onlp_fan_info_t fan_info[] = {
    { }, /* Not used */
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN1), "FANTRAY 1", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_SET_RPM | ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN2), "FANTRAY 2", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_SET_RPM | ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN3), "FANTRAY 3", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_SET_RPM | ONLP_FAN_CAPS_GET_RPM,
        0,
        0,
        ONLP_FAN_MODE_INVALID,
    },
    {
        { ONLP_FAN_ID_CREATE(FAN_ID_FAN4), "FANTRAY 4", 0 },
        ONLP_FAN_STATUS_PRESENT,
        ONLP_FAN_CAPS_SET_RPM | ONLP_FAN_CAPS_GET_RPM,
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
fan_info_get(onlp_fan_info_t* info, int fan_id)
{
    int error = 0;
    uint8_t bank = 0xFF;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x06, 0xFF};
    const unsigned char subcmd[] = {0x00 /* RPM */, 0x01 /* Status */};

    /* Status */
    error = 0;
    memset(rddata, 0, 256);
    
    wrdata[0] = (uint8_t)fan_id;
    wrdata[1] = subcmd[1];

    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    error |= onlp_i2c_block_read(I2C_BUS_2, AF_BMC_ADDR, bank, 0xFF, rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
    if (error) {
        fprintf(stdout, "%s id %d error %d\n", __func__, fan_id, error);
        return ONLP_STATUS_E_INTERNAL;
    }
    dump_data("FAN Status", rddata);
    
    info->status = 0;
    if(rddata[1] == 1){
        info->status = ONLP_FAN_STATUS_PRESENT;
        if (rddata[2] == 1)
            info->status |= ONLP_FAN_STATUS_F2B;
        if (rddata[2] == 2)
            info->status |= ONLP_FAN_STATUS_B2F;
    }

    /* RPM */
    error = 0;
    memset(rddata, 0, 256);
    
    wrdata[0] = (uint8_t)fan_id;
    wrdata[1] = subcmd[0];

    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    error |= onlp_i2c_block_read(I2C_BUS_2, AF_BMC_ADDR, bank, 0xFF, rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
    if (error) {
        fprintf(stdout, "%s id %d error %d\n", __func__, fan_id, error);
        return ONLP_STATUS_E_INTERNAL;
    }
    dump_data("FAN RPM", rddata);
    
    info->rpm = ((rddata[1] << 8) | rddata[0]);

    return ONLP_STATUS_OK;
}

int
onlp_fani_rpm_set(onlp_oid_t id, int rpm)
{
    int error = 0;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x07, 0xFF};
    const unsigned char subcmd[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    int fan_id = ONLP_OID_ID_GET(id);

    error = 0;
    memset(rddata, 0, 256);

    wrdata[0] = (uint8_t)fan_id;
    wrdata[1] = subcmd[rpm];
    
    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    if (error) {
        return ONLP_STATUS_E_INTERNAL;
    }
    return ONLP_STATUS_OK;
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
            rc = fan_info_get(info, fan_id);
            break;
        default:            
            return ONLP_STATUS_E_INTERNAL;
            break;
    }

    return rc;
}

int onlp_fani_status_get(onlp_oid_t id, uint32_t* rv)
{
    return ONLP_STATUS_E_UNSUPPORTED;
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

