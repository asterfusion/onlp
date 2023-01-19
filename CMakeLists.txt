# CMake for onlp
# by tsihang (tsihang@asterfuion.com)
# 29 Dec 2020

cmake_minimum_required(VERSION 3.5)

#set(platform "x312p")
#set(platform "x312p_48y_t")
#set(platform "cx312p_48y_t")
#set(platform "x532p")
#set(platform "x532p_t")
#set(platform "cx532p_t")
#set(platform "x564p")
#set(platform "x564p_t")
#set(platform "cx564p_t")
set(platform "x308p")
#set(platform "x308p_48y_t")
#set(platform "cx308p_48y_t")

set(vbase "base")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_C_FLAGS "-g -O3 -Wall -Wextra -MMD -MF --std=gnu99")
#set(CMAKE_C_FLAGS "-g -O3 -Wextra -MMD -MF --std=gnu99")

project(onlp)

set(SOURCES
    main.c
)


# Basic packages
aux_source_directory(sm/infra/modules/AIM/module/src aim)
aux_source_directory(sm/infra/modules/AIM/module/src/posix aim_posix)
aux_source_directory(sm/bigcode/modules/IOF/module/src iof)
aux_source_directory(sm/bigcode/modules/cjson/module/src cjson)
aux_source_directory(sm/bigcode/modules/cjson_util/module/src cjson_util)
aux_source_directory(sm/bigcode/modules/sff/module/src sff)
aux_source_directory(sm/bigcode/modules/OS/module/src os)
aux_source_directory(sm/bigcode/modules/timer_wheel/module/src timer_wheel)

aux_source_directory(sm/bigcode/modules/BigData/BigHash/module/src bighash)
aux_source_directory(sm/bigcode/modules/BigData/BigList/module/src biglist)
aux_source_directory(sm/bigcode/modules/BigData/BigRing/module/src bigring)

# onlp packages
aux_source_directory(packages/${vbase}/any/onlp/src/onlp/module/src onlp_src)
aux_source_directory(packages/${vbase}/any/onlp/src/onlplib/module/src onlplib_src)
aux_source_directory(packages/platforms/asterfusion/x86-64/${platform}/onlp/builds/x86_64_asterfusion_${platform}/module/src onlp_pltfm_src)

add_executable(onlp
    ${SOURCES}
    ${aim}
    ${aim_posix}
    ${iof}
    ${cjson}
    ${cjson_util}
    ${sff}
    ${os}
    ${timer_wheel}
    ${bighash}
    ${biglist}
    ${bigring}
    ${onlp_src}
    ${onlplib_src}
    ${onlp_pltfm_src})

target_link_libraries(onlp -lpthread -lm)

target_include_directories(onlp PRIVATE 
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/packages/${vbase}/any/onlp/src/onlp/module/inc
	${PROJECT_SOURCE_DIR}/packages/${vbase}/any/onlp/src/onlplib/module/inc
	${PROJECT_SOURCE_DIR}/packages/platforms/asterfusion/x86-64/${platform}/onlp/builds/x86_64_asterfusion_${platform}/module/inc
	
	${PROJECT_SOURCE_DIR}/sm/infra/modules/AIM/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/IOF/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/cjson/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/cjson_util/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/sff/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/timer_wheel/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/OS/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/timer_wheel/module/inc

	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/BigData/BigHash/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/BigData/BigList/module/inc
	${PROJECT_SOURCE_DIR}/sm/bigcode/modules/BigData/BigRing/module/inc

)

