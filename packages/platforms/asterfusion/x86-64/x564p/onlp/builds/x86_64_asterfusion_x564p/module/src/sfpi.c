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
 *
 ***********************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <onlplib/sfp.h>
#include <onlp/platformi/sfpi.h>
#include "x86_64_asterfusion_x564p_log.h"
#include "platform_lib.h"

int
onlp_sfpi_init(void)
{  
    return ONLP_STATUS_OK;
}

int
onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    int p;
    for(p = 1; p <= QSFP_NUM; p++) {
        AIM_BITMAP_SET(bmap, p);
    }
    return ONLP_STATUS_OK;
}

int
onlp_sfpi_is_present(int port)
{
    int status, presentchan, rc, pres_val;    
           
    if ((rc = pltfm_qsfp_present_get(port, &pres_val)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }
    
    presentchan = (( (port - 1) % 8 ));    
    presentchan = (( ((presentchan % 2)) == 1 ? presentchan - 1 : presentchan + 1 ));

    /* status: 0 -> Down, 1 -> Up */
    status=(( ((pres_val & ( 1 << presentchan)))  != 0 ? 0 : 1 ));
   
    return status;
}


int
onlp_sfpi_presence_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    int p = 1;
    int rc = 0;
    
    for (p = 1; p <= QSFP_NUM; p++) {        
        rc = onlp_sfpi_is_present(p);
        AIM_BITMAP_MOD(dst, p, (1 == rc) ? 1 : 0);
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_port_map(int port, int* rport)
{
    *rport = port;
    return 0;
}

int
onlp_sfpi_rx_los_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    AIM_BITMAP_CLR_ALL(dst);
    AIM_BITMAP_SET(dst, 17);
    AIM_BITMAP_SET(dst, 19);
    return ONLP_STATUS_OK;
}

/*
 * Read the SFP eeprom into data[]
 *
 * Return MISSING if SFP is missing.
 * Return OK if eeprom is read
 */
int
onlp_sfpi_eeprom_read(int port, uint8_t data[256])
{
#if 0
    int size = 0;
    memset(data, 0, 256);

	if(onlp_file_read(data, 256, &size, PORT_EEPROM_FORMAT, (port+1)) != ONLP_STATUS_OK) {
        AIM_LOG_ERROR("Unable to read eeprom from port(%d)\r\n", port);
        return ONLP_STATUS_E_INTERNAL;
    }

    if (size != 256) {
        AIM_LOG_ERROR("Unable to read eeprom from port(%d), size is different!\r\n", port);
        return ONLP_STATUS_E_INTERNAL;
    }
#endif
    return ONLP_STATUS_OK;
}

int
onlp_sfpi_dom_read(int port, uint8_t data[256])
{
#if 0
    FILE* fp;
    char file[64] = {0};

    sprintf(file, PORT_EEPROM_FORMAT, sfp_bus_index[port]);
    fp = fopen(file, "r");
    if(fp == NULL) {
        AIM_LOG_ERROR("Unable to open the eeprom device file of port(%d)", port);
        return ONLP_STATUS_E_INTERNAL;
    }

    if (fseek(fp, 256, SEEK_CUR) != 0) {
        fclose(fp);
        AIM_LOG_ERROR("Unable to set the file position indicator of port(%d)", port);
        return ONLP_STATUS_E_INTERNAL;
    }

    int ret = fread(data, 1, 256, fp);
    fclose(fp);
    if (ret != 256) {
        AIM_LOG_ERROR("Unable to read the module_eeprom device file of port(%d, %d)", port, ret);
        return ONLP_STATUS_E_INTERNAL;
    }
#endif
    return ONLP_STATUS_OK;
}

int
onlp_sfpi_control_set(int port, onlp_sfp_control_t control, int value)
{
    AIM_LOG_MSG("No control set API supported.");
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_sfpi_control_get(int port, onlp_sfp_control_t control, int* value)
{
    AIM_LOG_MSG("No control get API supported.");
    return ONLP_STATUS_E_UNSUPPORTED;
}

/*
 * This is a generic ioctl interface.
 */
int
onlp_sfpi_ioctl(int port, va_list vargs)
{
    AIM_LOG_MSG("No ioctls API supported.");
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Returns whether or not the given control is suppport on the given port.
 * @param port The port number.
 * @param control The control.
 * @param rv [out] Receives 1 if supported, 0 if not supported.
 * @note This provided for convenience and is optional.
 * If you implement this function your control_set and control_get APIs
 * will not be called on unsupported ports.
 */
int
onlp_sfpi_control_supported(int port, onlp_sfp_control_t control, int* rv)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/*
 * If the platform requires any setup or equalizer modifications
 * based on the actual SFP that was inserted then that custom
 * setup should be performed here.
 *
 * After a new SFP is detected by the ONLP framework this
 * function will be called to perform the (optional) setup.
 */
int
onlp_sfpi_post_insert(int port, sff_info_t* sff_info)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief Read a byte from an address on the given SFP port's bus.
 * @param port The port number.
 * @param devaddr The device address.
 * @param addr The address.
 */
int
onlp_sfpi_dev_readb(int port, uint8_t devaddr, uint8_t addr)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Write a byte to an address on the given SFP port's bus.
 */
int onlp_sfpi_dev_writeb(int port, uint8_t devaddr, uint8_t addr, uint8_t value)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Read a word from an address on the given SFP port's bus.
 * @param port The port number.
 * @param devaddr The device address.
 * @param addr The address.
 * @returns The word if successful, error otherwise.
 */
int onlp_sfpi_dev_readw(int port, uint8_t devaddr, uint8_t addr)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief Write a word to an address on the given SFP port's bus.
 */
int onlp_sfpi_dev_writew(int port, uint8_t devaddr, uint8_t addr, uint16_t value)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief Read from an address on the given SFP port's bus.
 * @param port The port number.
 * @param devaddr The device address.
 * @param addr The address.
 * @returns The data if successful, error otherwise.
 */
int onlp_sfpi_dev_read(int port, uint8_t devaddr, uint8_t addr, uint8_t* rdata, int size)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Write to an address on the given SFP port's bus.
 */
int onlp_sfpi_dev_write(int port, uint8_t devaddr, uint8_t addr, uint8_t* data, int size)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/*
 * De-initialize the SFPI subsystem.
 */
int
onlp_sfpi_denit(void)
{
    return ONLP_STATUS_OK;
}
