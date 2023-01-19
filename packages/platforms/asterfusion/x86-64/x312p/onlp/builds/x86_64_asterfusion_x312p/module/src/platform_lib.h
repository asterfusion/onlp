/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2021 Asterfusion Data Technologies Co., Ltd.
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
#ifndef __PLATFORM_LIB_H__
#define __PLATFORM_LIB_H__

#include <onlp/fan.h>
#include <onlp/psu.h>
#include <onlp/platformi/thermali.h>
#include <onlp/platformi/ledi.h>
#include <onlp/platformi/sysi.h>
#include "x86_64_asterfusion_x312p_int.h"
#include "x86_64_asterfusion_x312p_log.h"

#include <x86_64_asterfusion_x312p/x86_64_asterfusion_x312p_config.h>
#define PSU_STATUS_PRESENT    1
#define PSU_STATUS_POWER_GOOD 1
#define FAN_PRESENT           0
#define FAN_CTRL_SET1         1
#define FAN_CTRL_SET2         2

#define QSFP_NUM              12
#define SFP_NUM               50
#define THERMAL_NUM           9
#define LED_NUM               4
#define FAN_NUM               10


#define THERMAL_SHUTDOWN_DEFAULT        105000

#define THERMAL_ERROR_DEFAULT           95000
#define THERMAL_ERROR_FAN_PERC          100

#define THERMAL_WARNING_DEFAULT         77000
#define THERMAL_WARNING_FAN_PERC        80

#define THERMAL_NORMAL_DEFAULT          72000
#define THERMAL_NORMAL_FAN_PERC         50
    
    
#define AF_BMC_ADDR              0x3e

/* I2C bus */
#define I2C_BUS_0               0
#define I2C_BUS_1               1
#define I2C_BUS_2               2
#define I2C_BUS_3               3
#define I2C_BUS_4               4
#define I2C_BUS_5               5
#define I2C_BUS_6               6
#define I2C_BUS_7               7
#define I2C_BUS_8               8  
#define I2C_BUS_9               9  
    
/* PSU */
#define PSU1_MUX_MASK         0x01
#define PSU2_MUX_MASK         0x02
#define PSU_THERMAL1_OFFSET   0x8D
#define PSU_THERMAL2_OFFSET   0x8E
#define PSU_THERMAL_REG       0x58
#define PSU_FAN_RPM_REG       0x58
#define PSU_FAN_RPM_OFFSET    0x90
#define PSU_REG               0x58
#define PSU_VOUT_OFFSET1      0x20
#define PSU_VOUT_OFFSET2      0x8B
#define PSU_IOUT_OFFSET       0x8C
#define PSU_POUT_OFFSET       0x96
#define PSU_PIN_OFFSET        0x97
#define PSU_STATE_REG         0x33
#define PSU_PRESENT_OFFSET    0x03
#define PSU_PWGOOD_OFFSET     0x02

/* LED */
#define LED_REG               0x22
#define LED_OFFSET            0x02
#define LED_SYS_AND_MASK      0x3F
#define LED_SYS_GMASK         0x40
#define LED_SYS_YMASK         0x80
#define LED_FAN_AND_MASK      0xF3
#define LED_FAN_GMASK         0x04
#define LED_FAN_YMASK         0x08
#define LED_PSU1_AND_MASK     0xFC
#define LED_PSU1_GMASK        0x01
#define LED_PSU1_YMASK        0x02
#define LED_PSU1_OFFMASK      0x03
#define LED_PSU2_AND_MASK     0xCF
#define LED_PSU2_GMASK        0x10
#define LED_PSU2_YMASK        0x20
#define LED_PSU2_OFFMASK      0x30

/* SYS */
#define CPLD_REG              0x33
#define CPLD_VER_OFFSET       0x01

/* QSFP */
#define QSFP_PRES_REG1        0x20
#define QSFP_PRES_REG2        0x21
#define QSFP_PRES_OFFSET1     0x00
#define QSFP_PRES_OFFSET2     0x01

/* FAN */
#define FAN_REG               0x20
#define FAN_1_2_PRESENT_MASK  0x04
#define FAN_3_4_PRESENT_MASK  0x40
#define FAN_5_6_PRESENT_MASK  0x04
#define FAN_7_8_PRESENT_MASK  0x40

/** led_id */
typedef enum led_id_e {
    LED_SYSTEM_LED  = 1,
    LED_FAN_LED     = 2,
    LED_PSU1_LED    = 3,
    LED_PSU2_LED    = 4,
    LED_FAN_TRAY1   = 5,
    LED_FAN_TRAY2   = 6,
    LED_FAN_TRAY3   = 7,
    LED_FAN_TRAY4   = 8,
} led_id_t;

/** led_oid */
typedef enum led_oid_e {
    LED_OID_SYSTEM  = ONLP_LED_ID_CREATE(LED_SYSTEM_LED),
    LED_OID_FAN     = ONLP_LED_ID_CREATE(LED_FAN_LED),
    LED_OID_PSU1    = ONLP_LED_ID_CREATE(LED_PSU1_LED),
    LED_OID_PSU2    = ONLP_LED_ID_CREATE(LED_PSU2_LED),
    LED_OID_FAN_TRAY1 = ONLP_LED_ID_CREATE(LED_FAN_TRAY1),
    LED_OID_FAN_TRAY2 = ONLP_LED_ID_CREATE(LED_FAN_TRAY2),
    LED_OID_FAN_TRAY3 = ONLP_LED_ID_CREATE(LED_FAN_TRAY3),
    LED_OID_FAN_TRAY4 = ONLP_LED_ID_CREATE(LED_FAN_TRAY4), 
} led_oid_t;

/** thermal_id */
typedef enum thermal_id_e {
    THERMAL_ID_LM75 = 1,
    THERMAL_ID_LM63 = 2,
	THERMAL_ID_LM86 = 3,
    THERMAL_ID_GHC1_JUNCTION = 4,
    THERMAL_ID_GHC1_AMBIENT = 5,
    THERMAL_ID_GHC2_JUNCTION = 6,
    THERMAL_ID_GHC2_AMBIENT = 7,
    THERMAL_ID_TOFINO_MAIN = 8,
    THERMAL_ID_TOFINO_REMOTE = 9,
    //THERMAL_ID_FAN1 = 7,
    //THERMAL_ID_FAN2 = 8,
    //THERMAL_ID_FAN3 = 9,
    //THERMAL_ID_FAN4 = 10
} thermal_id_t;

/** Thermal_oid */
typedef enum thermal_oid_e {
    THERMAL_OID_LM75                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_LM75),
    THERMAL_OID_LM63                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_LM63),
    THERMAL_OID_LM86                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_LM86),
    THERMAL_OID_GHC1_JUNCTION       = ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC1_JUNCTION),
    THERMAL_OID_GHC1_AMBIENT        = ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC1_AMBIENT),
    THERMAL_OID_GHC2_JUNCTION       = ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC2_JUNCTION),
    THERMAL_OID_GHC2_AMBIENT        = ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC2_AMBIENT),
    THERMAL_OID_TOFINO_MAIN         = ONLP_THERMAL_ID_CREATE(THERMAL_ID_TOFINO_MAIN),
    THERMAL_OID_TOFINO_REMOTE       = ONLP_THERMAL_ID_CREATE(THERMAL_ID_TOFINO_REMOTE),
#if 0
    THERMAL_OID_FAN1                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN1),
    THERMAL_OID_FAN2                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN2),
    THERMAL_OID_FAN3                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN3),
    THERMAL_OID_FAN4                = ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN4)
#endif
} thermal_oid_t;

/* Shortcut for CPU thermal threshold value. */
#define THERMAL_THRESHOLD_INIT_DEFAULTS  \
    { THERMAL_WARNING_DEFAULT, \
      THERMAL_ERROR_DEFAULT,   \
      THERMAL_SHUTDOWN_DEFAULT }

/** fan_id */
typedef enum fan_id_e {
    FAN_ID_FAN1 = 1,
    FAN_ID_FAN2 = 2,
    FAN_ID_FAN3 = 3,
    FAN_ID_FAN4 = 4,
    FAN_ID_FAN5 = 5,
    FAN_ID_FAN6 = 6,
    FAN_ID_FAN7 = 7,
    FAN_ID_FAN8 = 8,
    FAN_ID_FAN9 = 9,
    FAN_ID_FAN10 = 10,
} fan_id_t;

typedef enum psu_id_e {
    PSU1_ID = 1,
    PSU2_ID = 2,
} psu_id_t;
    
#define PSU_MAX 2

/* by tsihang. */
#define ONLP_I2C_F_USE_BLOCK_WRITE       0x200
#define ONLP_I2C_F_USE_SMBUS_BLOCK_WRITE 0x400
#define ONLP_I2C_F_DISABLE_WRITE_RETRIES 0x800
#ifndef ONLPLIB_CONFIG_I2C_WRITE_RETRY_COUNT
#define ONLPLIB_CONFIG_I2C_WRITE_RETRY_COUNT 16
#endif

/**
 * Number of elements in an array.
 */
#define ARRAYSIZE(_x) (sizeof(_x)/sizeof(_x[0]))

int pltfm_qsfp_present_get(int port, int *mask);
int pltfm_sfp_present_get(int port, long long int *mask);
int pltfm_thermal_get(onlp_thermal_info_t* info, int thermal_id);
int pltfm_psu_thermal_get(onlp_thermal_info_t* info, int id);
int pltfm_psu_get(onlp_psu_info_t* info, int id);
int pltfm_psu_present_get(int *pw_exist, int id);
int pltfm_psu_pwgood_get(int *pw_good, int id);
int pltfm_onie_info_get(onlp_onie_info_t* onie);
int pltfm_fan_info_get(onlp_fan_info_t* info, int fan_id);

int
onlp_i2c_block_read_native(int bus, uint8_t addr, uint8_t offset, int size,
                    uint8_t* rdata, uint32_t flags);

int
onlp_i2c_block_write(int bus, uint8_t addr, uint8_t offset, int size,
                  uint8_t* wdata, uint32_t flags);

static inline void dump_data(const char *comment,
    const unsigned char *rdata)
{
#if defined(ONLP_DEBUG)
    int i;

    fprintf(stdout, "%s : ", comment);
    for (i = 0; i < rdata[0]; i ++) {
        fprintf(stdout, "0x%02x ", rdata[i+1]);
    }
    fprintf(stdout, "\n");
#else
    comment = comment;
    rdata = rdata;
#endif
}


#endif  /* __PLATFORM_LIB_H__ */
