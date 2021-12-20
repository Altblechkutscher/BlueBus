/*
 * File: handler_bc127.h
 * Author: Ted Salmon <tass2001@gmail.com>
 * Description:
 *     Implement the logic to have the BC127 and I/K-Bus communicate
 */
#ifndef HANDLER_BC127_H
#define HANDLER_BC127_H
#define _ADDED_C_LIB 1
#include <stdio.h>
#include "../lib/bt/bt_bc127.h"
#include "../lib/bt/bt_bm83.h"
#include "../lib/bt/bt_common.h"
#include "../lib/log.h"
#include "../lib/event.h"
#include "../lib/ibus.h"
#include "../lib/timer.h"
#include "../lib/utils.h"
#include "../ui/bmbt.h"
#include "../ui/cd53.h"
#include "../ui/mid.h"
#include "handler_common.h"

void HandlerBTInit(HandlerContext_t *);
void HandlerBTCallStatus(void *, unsigned char *);
void HandlerBTDeviceFound(void *, unsigned char *);
void HandlerBTCallerID(void *, unsigned char *);
void HandlerBTDeviceLinkConnected(void *, unsigned char *);
void HandlerBTDeviceDisconnected(void *, unsigned char *);
void HandlerBTPlaybackStatus(void *, unsigned char *);
void HandlerUICloseConnection(void *, unsigned char *);
void HandlerUIInitiateConnection(void *, unsigned char *);

void HandlerBTBC127Boot(void *, unsigned char *);
void HandlerBTBC127BootStatus(void *, unsigned char *);

void HandlerBTBM83AVRCPUpdates(void *, unsigned char *);
void HandlerBTBM83Boot(void *, unsigned char *);
void HandlerBTBM83BootStatus(void *, unsigned char *);
void HandlerBTBM83LinkBackStatus(void *, unsigned char *);

void HandlerTimerBTVolumeManagement(void *);

void HandlerTimerBTBC127State(void *);
void HandlerTimerBTBC127DeviceConnection(void *);
void HandlerTimerBTBC127OpenProfileErrors(void *);
void HandlerTimerBTBC127ScanDevices(void *);

void HandlerTimerBTBM83AVRCPRegisterStatusNotifier(void *);
void HandlerTimerBTBM83ManagePowerState(void *);
#endif /* HANDLER_BC127_H */
