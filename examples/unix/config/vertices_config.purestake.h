//
// Created by Cyril on 17/03/2021.
//

#ifndef VERTICES_CONFIG_VERTICES_CONFIG_H
#define VERTICES_CONFIG_VERTICES_CONFIG_H

#ifndef SERVER_URL
#define SERVER_URL "https://testnet-algorand.api.purestake.io/ps2"
#endif

/// Port to be used when communicating with the remote API
/// Standard HTTP port is 80
#ifndef SERVER_PORT
#define SERVER_PORT 0
#endif

#ifndef SERVER_TOKEN
#define SERVER_TOKEN 0
#endif

#if SERVER_TOKEN==0
#error Please define a token string with: #define SERVER_TOKEN "...". Get your token: [https://developer.purestake.io/home]
#endif

#endif //VERTICES_CONFIG_VERTICES_CONFIG_H
