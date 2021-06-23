//! @file
//!
//! Vertices Network
//! See License.txt for details
//!
//! Created by Cyril on 16/04/2021.


#ifndef VERTICES_SDK_INC_VERTICES_CONFIG_H
#define VERTICES_SDK_INC_VERTICES_CONFIG_H

#include <assert.h>

/// Logs verbosity can be configured using VTC_LOG_LEVEL
/// Set the lower level of logs. Logs with higher severity will be printed.
/// 4 <-> Debug level
/// 3 <-> Info level
/// 2 <-> Warning level
/// 1 <-> Error level
#ifndef VTC_LOG_LEVEL
#define VTC_LOG_LEVEL 4 // default log level is to print info data
#endif

/// Number of events that can be queued, must be a power of 2
/// An event takes \c sizeof(vtc_evt_t) bytes, \see vtc_evt_t
#ifndef VTC_EVENTS_COUNT
#define VTC_EVENTS_COUNT 16
#endif

//static_assert(VTC_EVENTS_COUNT == 2 || VTC_EVENTS_COUNT == 4 || VTC_EVENTS_COUNT == 8
//                  || VTC_EVENTS_COUNT == 16 || VTC_EVENTS_COUNT == 32 || VTC_EVENTS_COUNT == 64,
//              "VTC_EVENTS_COUNT must be a power of 2, make sure to change the assert if you want to have more than 64 events in the queue.");

/// Optional information passed in transactions
#ifndef OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES
#define OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES     128
#endif

#endif //VERTICES_SDK_INC_VERTICES_CONFIG_H
