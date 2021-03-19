//
// Created by Cyril on 17/03/2021.
//

#ifndef C_VERTICES_SDK_SRC_ACCOUNT_H
#define C_VERTICES_SDK_SRC_ACCOUNT_H

#include <vertices_types.h>
#include <stdio.h>
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
    account_info_t * info;
    long round;
    char address[ADDRESS_LENGTH];
    long long amount;
    long long pendingrewards;
    long long amountwithoutpendingrewards;
    long long rewards;
    participation_t participation;
} account_details_t;

err_code_t
account_add(account_info_t *account, size_t * id);

err_code_t
account_delete(size_t id);

err_code_t
account_init();

#endif //C_VERTICES_SDK_SRC_ACCOUNT_H
