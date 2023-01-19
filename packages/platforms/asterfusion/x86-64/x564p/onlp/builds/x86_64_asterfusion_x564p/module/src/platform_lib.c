/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2013 Accton Technology Corporation.
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
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <onlp/onlp.h>
#include <onlplib/file.h>
#include <onlplib/i2c.h>
#include <AIM/aim.h>

#include "platform_lib.h"

int
onlp_i2c_block_read_native(int bus, uint8_t addr, uint8_t offset, int size,
                    uint8_t* rdata, uint32_t flags)
{
    int fd, rv;

    fd = onlp_i2c_open(bus, addr, flags);

    if(fd < 0) {
        return fd;
    }

    size = size;
    
    rv = i2c_smbus_read_block_data(fd, offset, rdata);
    if (rv < 0) {
        AIM_LOG_ERROR("i2c-%d: reading address 0x%x, offset %d, failed: %{errno}",
                      bus, addr, offset, errno);    
        close(fd);
        return ONLP_STATUS_E_I2C;
    }
    
    close(fd);
    return ONLP_STATUS_OK;
}

int
onlp_i2c_block_write(int bus, uint8_t addr, uint8_t offset, int size,
                  uint8_t* wdata, uint32_t flags)
{
    int fd;

    fd = onlp_i2c_open(bus, addr, flags);

    if(fd < 0) {
      return fd;
    }

    int count = size;
    uint8_t* p = wdata;
    while(count > 0) {
      int wsize = (count >= ONLPLIB_CONFIG_I2C_BLOCK_SIZE) ? ONLPLIB_CONFIG_I2C_BLOCK_SIZE : count;
      int retries = (flags & ONLP_I2C_F_DISABLE_WRITE_RETRIES) ? 1 : ONLPLIB_CONFIG_I2C_WRITE_RETRY_COUNT;

      int err = -1;
      while(retries-- && err < 0) {
          if(flags & ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE) {
              err = i2c_smbus_write_block_data(fd, offset, wsize, p);
              if (!err) goto finish;
          } else {
              err = i2c_smbus_write_i2c_block_data(fd,
                                                 offset,
                                                 wsize,
                                                 p);
          }
      }

      if(err) {
          AIM_LOG_ERROR("i2c-%d: writing address 0x%x, offset %d, size=%d failed: %{errno}\n",
                        bus, addr, offset, wsize, errno);
          goto error;
      }

      p += wsize;
      count -= wsize;
    }

finish:
  close(fd);
  return ONLP_STATUS_OK;

error:
  close(fd);
  return ONLP_STATUS_E_I2C;
}


int pltfm_qsfp_present_get(int port, int *mask)
{
    port = port;
    mask = mask;
    return ONLP_STATUS_E_UNSUPPORTED;
}

int 
pltfm_thermal_get(onlp_thermal_info_t* info, int thermal_id)
{
    int error = 0;
    uint8_t bank = 0xFF;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x03 /* Thermals */, 0x04 /* Thermal value */};

    error = 0;
    memset(rddata, 0, 256);
    
    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[1], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    error |= onlp_i2c_block_read_native(I2C_BUS_2, AF_BMC_ADDR, bank, 0xFF, rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
    if (error) {
        return ONLP_STATUS_E_INTERNAL;
    }
    dump_data("Thermal", rddata);
    /* Thermals */
    info->mcelsius = rddata[thermal_id];

    return ONLP_STATUS_OK;
}


int 
pltfm_psu_get(onlp_psu_info_t* info, int id)
{
    int error = 0;
    uint8_t bank = 0xFF;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x0b, 0xFF};
    const unsigned char subcmd[] = {0x00, 0x01};
    int i = 1;

    {
        error = 0;
        memset(rddata, 0, 256);
        
        wrdata[0] = subcmd[i];    
        
        error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
        error |= onlp_i2c_block_read_native(I2C_BUS_2, AF_BMC_ADDR, bank, 0xFF, rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
        if (error) {
            return ONLP_STATUS_E_INTERNAL;
        }

        dump_data("PSU Power", rddata);
        if (id == PSU1_ID) {
            info->caps = 0;
            if (rddata[9] == 0) {
                return ONLP_STATUS_OK;
            }
            if (rddata[9] == 1)
                info->caps |= ONLP_PSU_CAPS_AC;
            else if (rddata[9] == 2)
                info->caps |= ONLP_PSU_CAPS_DC48;

            uint32_t caps = ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_IOUT | ONLP_PSU_CAPS_POUT | ONLP_PSU_CAPS_PIN;
            info->caps |= caps;
            info->mvout = rddata[1];
            info->miout = rddata[3];        
            info->mpout = rddata[5];
            info->mpin  = rddata[7];
        }

        if (id == PSU2_ID) {
            info->caps = 0;
            if (rddata[18] == 0) {
                return ONLP_STATUS_OK;
            }
            if (rddata[18] == 1)
                info->caps |= ONLP_PSU_CAPS_AC;
            else if (rddata[18] == 2)
                info->caps |= ONLP_PSU_CAPS_DC48;

            uint32_t caps = ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_IOUT | ONLP_PSU_CAPS_POUT | ONLP_PSU_CAPS_PIN;
            info->caps |= caps;
    
            info->mvout = rddata[10];
            info->miout = rddata[12];        
            info->mpout = rddata[14];
            info->mpin  = rddata[16]; 
        }
    }

    return ONLP_STATUS_OK;
}


int
pltfm_psu_present_get(int *pw_exist, int id)
{
    int error = 0;
    uint8_t bank = 0xFF;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x0b, 0xFF};
    const unsigned char subcmd[] = {0x00, 0x01};

    error = 0;
    memset(rddata, 0, 256);
    
    wrdata[0] = subcmd[0];    

    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    error |= onlp_i2c_block_read_native(I2C_BUS_2, AF_BMC_ADDR, bank, 0xFF, rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
    if (error) {
        return ONLP_STATUS_E_INTERNAL;
    }
    
    dump_data("PSU Present Status", rddata);
    /* PSU Status */
    if (id == PSU1_ID) {
        if (rddata[3] == 0)
            *pw_exist = 1;  /* PSU1 present */
    }
    if (id == PSU2_ID) {
        if (rddata[6] == 0)
            *pw_exist = 1;  /* PSU2 present */
    }
    
    return ONLP_STATUS_OK;
}


int
pltfm_psu_pwgood_get(int *pw_good, int id)
{
    int error = 0;
    uint8_t bank = 0xFF;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x0b, 0xFF};
    const unsigned char subcmd[] = {0x00, 0x01};

    memset(rddata, 0, 256);
    wrdata[0] = subcmd[0];    
    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    error |= onlp_i2c_block_read_native(I2C_BUS_2, AF_BMC_ADDR, bank, 0xFF, rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
    if (error) {
        return ONLP_STATUS_E_INTERNAL;
    }
    
    dump_data("PSU Power Status", rddata);
    /* PSU Power */
    if (id == PSU1_ID) {
        if (rddata[1] == 1)
            *pw_good = 1;  /* PSU1 Power */
    }
    if (id == PSU2_ID) {
        if (rddata[4] == 1)
            *pw_good = 1;   /* PSU2 Power */
    }

    return ONLP_STATUS_OK;
}


int
pltfm_onie_info_get(onlp_onie_info_t* onie)
{
    int error = 0;
    uint8_t bank = 0xFF;
    unsigned char rddata[256];
    unsigned char wrdata[2] = {0xAA, 0xAA};
    const unsigned char cmd[] = {0x01, 0x02};
    af_onie_code code;
    
    for (code = AF_PRODUCT_NAME; code <= AF_SWITCH_VENDOR; error = 0, code ++) {
        memset(rddata, 0, 256);

        wrdata[0] = code;

        error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, (unsigned char *)&wrdata[0], ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
        error |= onlp_i2c_block_read_native(I2C_BUS_2, AF_BMC_ADDR, bank, sizeof(rddata), (unsigned char *)&rddata[0], ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
        if (error) {
            return ONLP_STATUS_E_INTERNAL;
        } else {
            switch(code) {
                case AF_PRODUCT_NAME:
                    onie->product_name = aim_memdup((void *)&rddata[1], rddata[0]);
                    break;
                case AF_PART_NUMBER:
                    onie->part_number = aim_memdup((void *)&rddata[1], rddata[0]);
                    break;
                case AF_SERIAL_NUMBER:
                    onie->serial_number = aim_memdup((void *)&rddata[1], rddata[0]);
                    break;
                case AF_BASE_MAC_ADDRESS:
                    AIM_MEMCPY((void *)&onie->mac[0], (void *)&rddata[1], 6);
                    break;
                case AF_MANUFACTURE_DATE:
                    onie->manufacture_date = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_DEV_VERSION:
                    onie->device_version = rddata[1];
                    break;
                case AF_LABEL_VERSION:
                    onie->label_revision = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_PLATFORM_NAME:
                    onie->platform_name = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_ONIE_VERSION:
                    onie->onie_version = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_MAC_RANGE:
                    onie->mac_range = rddata[0] << 8 | rddata[1];
                    break;
                case AF_OEM:
                    onie->manufacturer = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_COUNTRY_CODE:
                    onie->country_code = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_VENDOR_NAME:
                    onie->vendor = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_DIAG_VERSION:
                    onie->diag_version = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                case AF_SERVICE_TAG:
                    onie->service_tag = aim_memdup((void *)&rddata[1], (size_t)rddata[0]);
                    break;
                default:
                    break;
            }
        }
    }

    error = 0;
    memset(rddata, 0, 256);
    
    wrdata[0] = AF_CRC32;

    error |= onlp_i2c_block_write(I2C_BUS_2, AF_BMC_ADDR, cmd[0], 2, wrdata, ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE | ONLP_I2C_F_DISABLE_WRITE_RETRIES);
    error |= onlp_i2c_block_read_native(I2C_BUS_2, AF_BMC_ADDR, bank, sizeof(rddata), rddata, ONLP_I2C_F_USE_SMBUS_BLOCK_READ | ONLP_I2C_F_DISABLE_READ_RETRIES);
    if (!error) {
        onie->crc = (rddata[3] << 24) | (rddata[2] << 16) | (rddata[1] << 8) | rddata[0];
    }

    return ONLP_STATUS_OK;
}


