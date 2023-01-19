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
 * Thermal Sensor Platform Implementation.
 *
 ***********************************************************/
#include <onlp/platformi/thermali.h>
#include <onlplib/file.h>
#include "x86_64_asterfusion_x308p_log.h"
#include "platform_lib.h"

#define VALIDATE(_id)                           \
    do {                                        \
        if(!ONLP_OID_IS_THERMAL(_id)) {             \
            return ONLP_STATUS_E_INVALID;       \
        }                                       \
    } while(0)

static onlp_thermal_info_t thermal_info[] = {
    { }, /* Not used */

    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_MAIN_BOARD_LEFT), "Far left of mainboard", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_MAIN_BOARD_RIGHT), "Far right of mainboard", 0, { 0, }},
                ONLP_THERMAL_STATUS_PRESENT,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN1), "Fan 1", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC1_JUNCTION), "GHC-1 Junction", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC1_AMBIENT), "GHC-1 Ambient", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC2_JUNCTION), "GHC-2 Junction", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_GHC2_AMBIENT), "GHC-2 Ambient", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_BF_JUNCTION), "BF Junction <- from BMC", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_BF_AMBIENT), "BF Ambient <- from BMC", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN2), "Fan 2", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_TOFINO_MAIN), "Tofino Main Temp Sensor <- from tofino", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_TOFINO_REMOTE), "Tofino Remote Temp Sensor <- from tofino", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_PSU1), "PSU 1", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_PSU2), "PSU 2", 0, { 0, }},
                0x0,
                ONLP_THERMAL_CAPS_ALL, 0, THERMAL_THRESHOLD_INIT_DEFAULTS
    }
    #if 0
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN1), "FAN-1 Thermal", 0},
                ONLP_THERMAL_STATUS_PRESENT,
                ONLP_THERMAL_CAPS_GET_TEMPERATURE, 0
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN2), "FAN-2 Thermal", 0},
                ONLP_THERMAL_STATUS_PRESENT,
                ONLP_THERMAL_CAPS_GET_TEMPERATURE, 0
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN3), "FAN-3 Thermal", 0},
                ONLP_THERMAL_STATUS_PRESENT,
                ONLP_THERMAL_CAPS_GET_TEMPERATURE, 0
    },
    { { ONLP_THERMAL_ID_CREATE(THERMAL_ID_FAN4), "FAN-4 Thermal", 0},
                ONLP_THERMAL_STATUS_PRESENT,
                ONLP_THERMAL_CAPS_GET_TEMPERATURE, 0
    }
    #endif
};



/*
 * This will be called to intiialize the thermali subsystem.
 */
int
onlp_thermali_init(void)
{    
    return ONLP_STATUS_OK;
}

/*
 * Retrieve the information structure for the given thermal OID.
 *
 * If the OID is invalid, return ONLP_E_STATUS_INVALID.
 * If an unexpected error occurs, return ONLP_E_STATUS_INTERNAL.
 * Otherwise, return ONLP_STATUS_OK with the OID's information.
 *
 * Note -- it is expected that you fill out the information
 * structure even if the sensor described by the OID is not present.
 */
int
onlp_thermali_info_get(onlp_oid_t id, onlp_thermal_info_t* info)
{   
    int sensor_id = 0;

    VALIDATE(id);
    sensor_id = ONLP_OID_ID_GET(id);
    /* Set the onlp_oid_hdr_t and capabilities */
    *info = thermal_info[sensor_id];

    return pltfm_thermal_get(info, sensor_id);
}
int onlp_thermali_status_get(onlp_oid_t id, uint32_t* rv)
{
    id = id;
    rv = rv;
    return ONLP_STATUS_E_UNSUPPORTED;
}
int onlp_thermali_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* rv)
{
    id = id;
    rv = rv;
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_thermali_ioctl(int id, va_list vargs)
{
    id = id;
    vargs = vargs;
    return ONLP_STATUS_E_UNSUPPORTED;
}

