cmake_minimum_required(VERSION 3.25)

project(fftease)

# ╭──────────────────────────────────────╮
# │               pd.cmake               │
# ╰──────────────────────────────────────╯
cmake_policy(SET CMP0135 NEW)
set(PDCMAKE_FILE ${CMAKE_BINARY_DIR}/pd.cmake)
if(NOT EXISTS ${PDCMAKE_FILE})
    message(STATUS "Downloading pd.cmake")
    file(
        DOWNLOAD https://raw.githubusercontent.com/pure-data/pd.cmake/refs/tags/v0.1.0/pd.cmake ${PDCMAKE_FILE}
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS)
endif()
include(${PDCMAKE_FILE})

# ╭──────────────────────────────────────╮
# │             Share Files              │
# ╰──────────────────────────────────────╯
set(SHARE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/bloscbank.c"
    "${CMAKE_CURRENT_LIST_DIR}/convert.c"
    "${CMAKE_CURRENT_LIST_DIR}/fft.c"
    "${CMAKE_CURRENT_LIST_DIR}/fft4.c"
    "${CMAKE_CURRENT_LIST_DIR}/fftease_setup.c"
    "${CMAKE_CURRENT_LIST_DIR}/fftease_utilities.c"
    "${CMAKE_CURRENT_LIST_DIR}/fold.c"
    "${CMAKE_CURRENT_LIST_DIR}/leanconvert.c"
    "${CMAKE_CURRENT_LIST_DIR}/leanunconvert.c"
    "${CMAKE_CURRENT_LIST_DIR}/limit_fftsize.c"
    "${CMAKE_CURRENT_LIST_DIR}/limited_oscbank.c"
    "${CMAKE_CURRENT_LIST_DIR}/makewindows.c"
    "${CMAKE_CURRENT_LIST_DIR}/oscbank.c"
    "${CMAKE_CURRENT_LIST_DIR}/overlapadd.c"
    "${CMAKE_CURRENT_LIST_DIR}/PenroseOscil.c"
    "${CMAKE_CURRENT_LIST_DIR}/PenroseRand.c"
    "${CMAKE_CURRENT_LIST_DIR}/power_of_two.c"
    "${CMAKE_CURRENT_LIST_DIR}/unconvert.c")

add_library(fftease_shared STATIC ${SHARE_FILES})

#╭──────────────────────────────────────╮
#│               Objects                │
#╰──────────────────────────────────────╯
pd_add_external(bthresher~ "${CMAKE_CURRENT_LIST_DIR}/bthresher~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(burrow~ "${CMAKE_CURRENT_LIST_DIR}/burrow~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(cavoc~ "${CMAKE_CURRENT_LIST_DIR}/cavoc~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(cavoc27~ "${CMAKE_CURRENT_LIST_DIR}/cavoc27~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(centerring~ "${CMAKE_CURRENT_LIST_DIR}/centerring~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(codepend~ "${CMAKE_CURRENT_LIST_DIR}/codepend~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(cross~ "${CMAKE_CURRENT_LIST_DIR}/cross~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(dentist~ "${CMAKE_CURRENT_LIST_DIR}/dentist~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(disarrain~ "${CMAKE_CURRENT_LIST_DIR}/disarrain~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(disarray~ "${CMAKE_CURRENT_LIST_DIR}/disarray~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(drown~ "${CMAKE_CURRENT_LIST_DIR}/drown~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(enrich~ "${CMAKE_CURRENT_LIST_DIR}/enrich~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(ether~ "${CMAKE_CURRENT_LIST_DIR}/ether~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(leaker~ "${CMAKE_CURRENT_LIST_DIR}/leaker~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(mindwarp~ "${CMAKE_CURRENT_LIST_DIR}/mindwarp~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(morphine~ "${CMAKE_CURRENT_LIST_DIR}/morphine~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(multyq~ "${CMAKE_CURRENT_LIST_DIR}/multyq~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pileup~ "${CMAKE_CURRENT_LIST_DIR}/pileup~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvcompand~ "${CMAKE_CURRENT_LIST_DIR}/pvcompand~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvgrain~ "${CMAKE_CURRENT_LIST_DIR}/pvgrain~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvharm~ "${CMAKE_CURRENT_LIST_DIR}/pvharm~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvoc~ "${CMAKE_CURRENT_LIST_DIR}/pvoc~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvtuner~ "${CMAKE_CURRENT_LIST_DIR}/pvtuner~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvwarp~ "${CMAKE_CURRENT_LIST_DIR}/pvwarp~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(pvwarpb~ "${CMAKE_CURRENT_LIST_DIR}/pvwarpb~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(reanimator~ "${CMAKE_CURRENT_LIST_DIR}/reanimator~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(resent~ "${CMAKE_CURRENT_LIST_DIR}/resent~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(residency~ "${CMAKE_CURRENT_LIST_DIR}/residency~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(residency_buffer~ "${CMAKE_CURRENT_LIST_DIR}/residency_buffer~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(schmear~ "${CMAKE_CURRENT_LIST_DIR}/schmear~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(scrape~ "${CMAKE_CURRENT_LIST_DIR}/scrape~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(shapee~ "${CMAKE_CURRENT_LIST_DIR}/shapee~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(swinger~ "${CMAKE_CURRENT_LIST_DIR}/swinger~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(taint~ "${CMAKE_CURRENT_LIST_DIR}/taint~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(thresher~ "${CMAKE_CURRENT_LIST_DIR}/thresher~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(vacancy~ "${CMAKE_CURRENT_LIST_DIR}/vacancy~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(xsyn~ "${CMAKE_CURRENT_LIST_DIR}/xsyn~.c" LINK_LIBRARIES fftease_shared)
pd_add_external(loopsea~ "${CMAKE_CURRENT_LIST_DIR}/loopsea~.c" LINK_LIBRARIES fftease_shared)

#╭──────────────────────────────────────╮
#│              Data Files              │
#╰──────────────────────────────────────╯
file(GLOB HELP_FILES "${CMAKE_CURRENT_LIST_DIR}/fftease-helpfiles/*")
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/sound/*")

pd_add_datafile(loopsea~ "${HELP_FILES}") 
pd_add_datafile(loopsea~ "${DATA_FILES}" DESTINATION "sound")
pd_add_datafile(loopsea~ "${CMAKE_CURRENT_LIST_DIR}/fftease-meta.pd")
pd_add_datafile(loopsea~ "${CMAKE_CURRENT_LIST_DIR}/smap.pd")
pd_add_datafile(loopsea~ "${CMAKE_CURRENT_LIST_DIR}/collect.pl")
pd_add_datafile(loopsea~ "${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt")
pd_add_datafile(loopsea~ "${CMAKE_CURRENT_LIST_DIR}/CHANGELOG.txt")





