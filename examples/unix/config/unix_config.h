//
// Created by Cyril on 17/03/2021.
//

#ifndef VERTICES_CONFIG_VERTICES_CONFIG_H
#define VERTICES_CONFIG_VERTICES_CONFIG_H

#define ALGOEXPLORER    1
#define PURESTAKE       2
#define LOCAL           3

#ifndef API_PROVIDER
#define API_PROVIDER    ALGOEXPLORER
#endif

#define TESTNET_ALGOEXPLORER_API            "https://api.testnet.algoexplorer.io"
#define TESTNET_PURESTAKE_API               "https://testnet-algorand.api.purestake.io/ps2"
#define TESTNET_LOCAL_API                   "localhost"

#define TESTNET_ALGOEXPLORER_PORT       0
#define TESTNET_PURESTAKE_PORT          0
#define TESTNET_LOCAL_PORT              8080

#define TESTNET_ALGOEXPLORER_AUTH_HEADER    ""
#define TESTNET_PURESTAKE_AUTH_HEADER       "x-api-key:"
#define TESTNET_LOCAL_AUTH_HEADER           "X-Algo-API-Token:"

#define  TESTNET_ALGOEXPLORER_API_TOKEN    ""

#if API_PROVIDER==PURESTAKE
#include "custom_config.h"
#define SERVER_URL              TESTNET_PURESTAKE_API
#define SERVER_PORT             TESTNET_PURESTAKE_PORT
#define SERVER_TOKEN_HEADER     (TESTNET_PURESTAKE_AUTH_HEADER TESTNET_PURESTAKE_API_TOKEN)
#elif API_PROVIDER==LOCAL
#include "custom_config.h"
#define SERVER_URL              TESTNET_LOCAL_API
#define SERVER_PORT             TESTNET_LOCAL_PORT
#define SERVER_TOKEN_HEADER     (TESTNET_LOCAL_AUTH_HEADER TESTNET_LOCAL_API_TOKEN)
#else
// default provider is AlgoExplorer
#define SERVER_URL              TESTNET_ALGOEXPLORER_API
#define SERVER_PORT             TESTNET_ALGOEXPLORER_PORT
#define SERVER_TOKEN_HEADER     (TESTNET_ALGOEXPLORER_AUTH_HEADER TESTNET_ALGOEXPLORER_API_TOKEN)
#endif

#endif //VERTICES_CONFIG_VERTICES_CONFIG_H
