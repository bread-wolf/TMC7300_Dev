/*
 * ComStack.hpp
 *
 *  Created on: 31.05.2021
 *      Author: AB
 */

#ifndef COMSTACK_HPP_
#define COMSTACK_HPP_

#include<stdint.h>
#include<stdbool.h>

/* ComStack command numbers */
#define COMSTACK_CMD_VERSION                0x00
#define COMSTACK_CMD_TMC_WRITE              0x10
#define COMSTACK_CMD_TMC_READ               0x11

/* ComStack status codes */
#define COMSTACK_REPLY_OK                   0x20
#define COMSTACK_REPLY_INVALID_CMD          0x21
#define COMSTACK_REPLY_CMD_UNAVAILABLE      0x22
#define COMSTACK_REPLY_INVALID_CHANNEL      0x23
#define COMSTACK_REPLY_INVALID_DATA         0x24

/* ComStack Request Struct */
typedef struct
{
    uint8_t slaveAddr;
    uint8_t jobID;
    uint8_t comStackCmd;
    uint8_t channel;
    uint8_t registerAddr;
    union
    {
        uint32_t data;
        uint8_t bytes[4];
    };
} ComStack_Request;


/* ComStack Reply Struct */
typedef struct
{
    uint8_t masterAddr;
    uint8_t jobID;
    uint8_t status;
    union
    {
        uint32_t data;
        uint8_t bytes[4];
    };
} ComStack_Reply;

#endif /* COMSTACK_HPP_ */