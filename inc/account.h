//
// Created by Cyril on 17/03/2021.
//

#ifndef C_VERTICES_SDK_SRC_ACCOUNT_H
#define C_VERTICES_SDK_SRC_ACCOUNT_H

#define ADDRESS_LENGTH  64

typedef struct
{
    char partpkb64[ADDRESS_LENGTH];
    char vrfpkb64[ADDRESS_LENGTH];
    long votefst;
    long votelst;
    int votekd;
} participation_t;

typedef struct
{
    long round;
    char address[ADDRESS_LENGTH];
    long long amount;
    long long pendingrewards;
    long long amountwithoutpendingrewards;
    long long rewards;
    participation_t participation;
} account_information_t;

#endif //C_VERTICES_SDK_SRC_ACCOUNT_H
