//
// Created by Cyril on 16/04/2021.
//

#ifndef VERTICES_SDK_INC_VERTICES_CONFIG_H
#define VERTICES_SDK_INC_VERTICES_CONFIG_H

/// Logs verbosity can be configured using VTC_LOG_LEVEL
/// Set the lower level of logs. Logs with higher severity will be printed.
/// 4 <-> Debug level
/// 3 <-> Info level
/// 2 <-> Warning level
/// 1 <-> Error level
#ifndef VTC_LOG_LEVEL
#define VTC_LOG_LEVEL 3 // default log level is to print info data
#endif

/// Optional information passed in transactions
#ifndef OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES
#define OPTIONAL_TX_FIELDS_MAX_SIZE_BYTES     128
#endif

#endif //VERTICES_SDK_INC_VERTICES_CONFIG_H
