/*
 * File:   bm83.c
 * Author: Ted Salmon <tass2001@gmail.com>
 * Description:
 *     Implementation of the Microchip BM83 Bluetooth UART API
 */
#include "bt_bm83.h"

/**
 * BM83CommandAVRCPGetCapabilities()
 *     Description:
 *         TODO
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandAVRCPGetCapabilities(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_AVC_VENDOR_DEPENDENT_CMD,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        BM83_AVRCP_PDU_GET_CAPABILITIES,
        0x00, // Reserved
        0x00, // LL
        0x01, // LL
        BM83_AVRCP_DATA_CAP_TYPE_EVENTS
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandAVRCPGetElementAttributesAll()
 *     Description:
 *         TODO
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandAVRCPGetElementAttributesAll(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_AVC_VENDOR_DEPENDENT_CMD,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        BM83_AVRCP_PDU_GET_ELEMENT_ATTRIBUTES,
        0x00, // Reserved
        0x00, // LL
        0x09, // LL
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandAVRCPRegisterNotification()
 *     Description:
 *         TODO
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandAVRCPRegisterNotification(BT_t *bt, uint8_t event)
{
    uint8_t command[] = {
        BM83_CMD_AVC_VENDOR_DEPENDENT_CMD,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        BM83_AVRCP_PDU_NOTIFICATION,
        0x00, // Reserved
        0x00, // LL
        0x05, // LL
        event,
        0x00,
        0x00,
        0x00,
        0x00
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandBTMUtilityFunction()
 *     Description:
 *         Run the different BTM functions
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t type - Function to execute
 *         uint8_t param - Parameter for function
 *     Returns:
 *         void
 */
void BM83CommandBTMUtilityFunction(BT_t *bt, uint8_t type, uint8_t param)
{
    uint8_t command[] = {BM83_CMD_BTM_UTILITY_FUNCTION, type, param};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandCallAccept()
 *     Description:
 *         Accept the incomming call (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandCallAccept(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_MMI_ACTION,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        0x04 // Accept Call
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandCallEnd()
 *     Description:
 *         End the ongoing call (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandCallEnd(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_MMI_ACTION,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        0x06 // End Call
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandConnect()
 *     Description:
 *         Link back to a paired device (Profiles_Link_Back -> 0x17)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandConnect(BT_t *bt, BTPairedDevice_t *dev)
{
    uint8_t command[] = {
        BM83_CMD_PROFILES_LINK_BACK,
        BM83_DATA_LINK_BACK_MAC_ID,
        dev->number - 1,
        0x04, // Link back A2DP only
        dev->macId[5],
        dev->macId[4],
        dev->macId[3],
        dev->macId[2],
        dev->macId[1],
        dev->macId[0]
    };
    // Make this the active device
    memcpy(bt->activeDevice.macId, dev->macId, BT_MAC_ID_LEN);
    bt->status = BT_STATUS_CONNECTING;
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandDisconnect()
 *     Description:
 *         Disconnect Devices (Disconnect -> 0x18)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandDisconnect(BT_t *bt, uint8_t disconenctFlags)
{
    uint8_t command[] = {BM83_CMD_DISCONNECT, disconenctFlags};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandLinkBackLastDevice()
 *     Description:
 *         Attempt to connect to the last device (Profiles_Link_Back -> 0x17)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandLinkBackLastDevice(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_PROFILES_LINK_BACK,
        BM83_DATA_LINK_BACK_LAST_DEVICE
    };
    bt->status = BT_STATUS_CONNECTING;
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandLinkBackProfile()
 *     Description:
 *         Attempt to connect to the given BTPairingProfile_t (Profiles_Link_Back -> 0x17)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t profileIdx - The profile to connect to
 *     Returns:
 *         void
 */
void BM83CommandLinkBackProfile(BT_t *bt, BTPairedDevice_t *dev)
{
    uint8_t command[] = {
        BM83_CMD_PROFILES_LINK_BACK,
        BM83_DATA_LINK_BACK_MAC_ID,
        dev->number - 1,
        BM83_DATA_LINK_BACK_PROFILES_A2DP_HF,
        dev->macId[5],
        dev->macId[4],
        dev->macId[3],
        dev->macId[2],
        dev->macId[1],
        dev->macId[0]
    };
    bt->status = BT_STATUS_CONNECTING;
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandMusicControl()
 *     Description:
 *         Perform a music control action (Music_Control -> 0x04)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t action - The playback action to take
 *     Returns:
 *         void
 */
void BM83CommandMusicControl(BT_t *bt, uint8_t action)
{
    uint8_t command[] = {BM83_CMD_MUSIC_CONTROL, 0x00, action};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandPairingEnable()
 *     Description:
 *         Enable Pairing Mode (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandPairingEnable(BT_t *bt)
{
    bt->discoverable = BT_STATE_ON;
    uint8_t command[] = {BM83_CMD_MMI_ACTION, 0x00, 0x5D};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandPairingDisable()
 *     Description:
 *         Disable Pairing Mode (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandPairingDisable(BT_t *bt)
{
    bt->discoverable = BT_STATE_OFF;
    uint8_t command[] = {BM83_CMD_MMI_ACTION, 0x00, 0x6B};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandPowerOn()
 *     Description:
 *         Power the Module on (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandPowerOn(BT_t *bt)
{
    uint8_t powerPress[] = {0x02, 0x00, 0x51};
    BM83SendCommand(bt, powerPress, sizeof(powerPress));
    uint8_t powerRelease[] = {0x02, 0x00, 0x52};
    BM83SendCommand(bt, powerRelease, sizeof(powerRelease));
}

/**
 * BM83CommandReadLinkStatus()
 *     Description:
 *         Request the current link status
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t queryType - The query type
 *     Returns:
 *         void
 */
void BM83CommandReadLinkStatus(BT_t *bt)
{
    uint8_t command[] = {BM83_CMD_READ_LINK_STATUS};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandReadPairedDevices()
 *     Description:
 *         Request the paired devices list
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t queryType - The query type
 *     Returns:
 *         void
 */
void BM83CommandReadLinkedDeviceInformation(BT_t *bt, uint8_t queryType)
{
    uint8_t command[] = {
        BM83_CMD_READ_LINKED_DEVICE_INFORMATION,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble,
        queryType
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandReadPairedDevices()
 *     Description:
 *         Request the paired devices list
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandReadPairedDevices(BT_t *bt)
{
    uint8_t command[] = {BM83_CMD_READ_PAIRED_DEVICE_RECORD};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandStatus()
 *     Description:
 *         Get the Link Status (Read_Link_Status -> 0x0D)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandStatus(BT_t *bt)
{
    uint8_t command[] = {0x0D};
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandVendorATCommand()
 *     Description:
 *         Run an AT Command (Vendor_AT_Cmd -> 0x0A)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         char *cmd - The AT command ASCII to send
 *     Returns:
 *         void
 */
void BM83CommandVendorATCommand(BT_t *bt, char *cmd)
{
    uint8_t command[17] = {0};
    command[0] = BM83_CMD_VENDOR_AT_COMMAND;
    command[1] = bt->activeDevice.deviceId & 0xF; // Linked Database, the lower nibble
    uint8_t cmdLen = strlen(cmd);
    if (cmdLen <= 15) {
        uint8_t i = 0;
        for (i = 0; i < cmdLen; i++) {
            command[i + 2] = cmd[i];
        }
        BM83SendCommand(bt, command, cmdLen + 2);
    } else {
        LogError("BT: AT Command too long for buffer");
    }
}

/**
 * BM83CommandVoiceRecognitionClose()
 *     Description:
 *         Close Voice Recognition (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandVoiceRecognitionClose(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_MMI_ACTION,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        BM83_CMD_MMI_ACTION_VR_CLOSE
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83CommandVoiceRecognitionOpen()
 *     Description:
 *         Open Voice Recognition (MMI_Action -> 0x02)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83CommandVoiceRecognitionOpen(BT_t *bt)
{
    uint8_t command[] = {
        BM83_CMD_MMI_ACTION,
        bt->activeDevice.deviceId & 0xF, // Linked Database, the lower nibble
        BM83_CMD_MMI_ACTION_VR_OPEN
    };
    BM83SendCommand(bt, command, sizeof(command));
}

/**
 * BM83ProcessEventAVCSpecificRsp()
 *     Description:
 *         Process AVRCP Updates
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83ProcessEventAVCSpecificRsp(BT_t *bt, uint8_t *data, uint16_t length)
{
    uint8_t pduId = 0;
    if (length > 7) {
        pduId = data[BM83_FRAME_DB7];
    }
    switch (data[BM83_FRAME_DB1]) {
        case BM83_DATA_AVC_RSP_NOT_IMPL: {
            LogWarning("BT: AVRCP Not Implemented: %02X", pduId);
            break;
        }
        case BM83_DATA_AVC_RSP_ACCEPT: {
            LogDebug(LOG_SOURCE_BT, "BT: AVRCP Accept: %02X", pduId);
            break;
        }
        case BM83_DATA_AVC_RSP_REJECT: {
            LogWarning("BT: AVRCP Reject: %02X", pduId);
            break;
        }
        case BM83_DATA_AVC_RSP_STABLE: {
            LogDebug(LOG_SOURCE_BT, "BT: AVRCP Stable: %02X", pduId);
            if (pduId == BM83_AVRCP_PDU_GET_CAPABILITIES &&
                data[BM83_FRAME_DB11] == BM83_AVRCP_DATA_CAP_TYPE_EVENTS
            ) {
                // Clear the available capabilities
                memset(&bt->activeDevice.avrcpCaps, 0, sizeof(BTConnectionAVRCPCapabilities_t));
                uint8_t length = data[BM83_FRAME_DB12];
                uint8_t i = 0;
                for (i = 0; i < length; i++) {
                    switch (data[BM83_FRAME_DB13 + i]) {
                        case BM83_AVRCP_EVT_PLAYBACK_STATUS_CHANGED:
                            bt->activeDevice.avrcpCaps.playbackChanged = 1;
                            break;
                        case BM83_AVRCP_EVT_PLAYBACK_TRACK_CHANGED:
                            bt->activeDevice.avrcpCaps.trackChanged = 1;
                            break;
                        case BM83_AVRCP_EVT_TRACK_REACHED_END:
                            bt->activeDevice.avrcpCaps.trackReachedEnd = 1;
                            break;
                        case BM83_AVRCP_EVT_TRACK_REACHED_START:
                            bt->activeDevice.avrcpCaps.trackReachedStart = 1;
                            break;
                        case BM83_AVRCP_EVT_PLAYBACK_POS_CHANGED:
                            bt->activeDevice.avrcpCaps.playbackPosChanged = 1;
                            break;
                        case BM83_AVRCP_EVT_NOW_PLAYING_CONTENT_CHANGED:
                            bt->activeDevice.avrcpCaps.nowPlayingChanged = 1;
                            break;
                        case BM83_AVRCP_EVT_VOLUME_CHANGED:
                            bt->activeDevice.avrcpCaps.volumeChanged = 1;
                            break;
                    }
                }
                uint8_t updateData[2] = {BM83_AVRCP_PDU_GET_CAPABILITIES, 0x00};
                EventTriggerCallback(BT_EVENT_AVRCP_PDU_CHANGE, updateData);
            } else if (pduId == BM83_AVRCP_PDU_GET_ELEMENT_ATTRIBUTES) {
                // Clear Metadata since we're receiving new data
                BTClearMetadata(bt);
                bt->metadataStatus = BT_METADATA_STATUS_NEW;
                uint8_t attributeCount = data[BM83_FRAME_DB11];
                uint16_t bytePos = BM83_FRAME_DB12;
                uint8_t i = 0;
                for (i = 0; i < attributeCount; i++) {
                    // Skip the 0 pads
                    bytePos = bytePos + 3;
                    uint8_t attributeType = data[bytePos];
                    // Skip The zero pad and the encoding type (always assume UTF-8)
                    bytePos = bytePos + 3;
                    uint16_t attributeLen = (data[bytePos + 1] & 0xFF) | (data[bytePos] << 8);
                    // Skip over the length and to the beginning of the data
                    bytePos = bytePos + 2;
                    char tempString[BT_METADATA_MAX_SIZE] = {0};
                    switch (attributeType) {
                        case BM83_AVRCP_DATA_ELEMENT_TYPE_TITLE: {
                            uint16_t j = 0;
                            for (j = 0; j < attributeLen; j++) {
                                tempString[j] = data[bytePos];
                                bytePos++;
                            }
                            strncpy(bt->title, tempString, BT_METADATA_FIELD_SIZE);
                            break;
                        }
                        case BM83_AVRCP_DATA_ELEMENT_TYPE_ARTIST: {
                            uint16_t j = 0;
                            for (j = 0; j < attributeLen; j++) {
                                tempString[j] = data[bytePos];
                                bytePos++;
                            }
                            strncpy(bt->artist, tempString, BT_METADATA_FIELD_SIZE);
                            break;
                        }
                        case BM83_AVRCP_DATA_ELEMENT_TYPE_ALBUM: {
                            uint16_t j = 0;
                            for (j = 0; j < attributeLen; j++) {
                                tempString[j] = data[bytePos];
                                bytePos++;
                            }
                            strncpy(bt->album, tempString, BT_METADATA_FIELD_SIZE);
                            break;
                        }
                        default:
                            bytePos = bytePos + attributeLen;
                            break;
                    }
                }
                LogDebug(
                    LOG_SOURCE_BT,
                    "BT: title=%s,artist=%s,album=%s",
                    bt->title,
                    bt->artist,
                    bt->album
                );
                EventTriggerCallback(BT_EVENT_METADATA_UPDATE, 0);
            }
            break;
        }
        case BM83_DATA_AVC_RSP_INTERIM: {
            LogDebug(LOG_SOURCE_BT, "BT: AVRCP Interim: %02X", pduId);
            uint8_t updateType = data[BM83_FRAME_DB11];
            if (updateType == BM83_AVRCP_EVT_PLAYBACK_TRACK_CHANGED ||
                updateType == BM83_AVRCP_EVT_NOW_PLAYING_CONTENT_CHANGED
            ) {
                uint8_t updateData[2] = {updateType, 0x01};
                EventTriggerCallback(BT_EVENT_AVRCP_PDU_CHANGE, updateData);
            }
            break;
        }
        case BM83_DATA_AVC_RSP_CHANGED: {
            uint8_t pduId = data[BM83_FRAME_DB7];
            LogDebug(LOG_SOURCE_BT, "BT: AVRCP Changed: %02X", pduId);
            if (pduId == BM83_AVRCP_PDU_NOTIFICATION) {
                uint8_t updateType = data[BM83_FRAME_DB11];
                uint8_t status = data[BM83_FRAME_DB12];
                if (updateType == BM83_AVRCP_EVT_PLAYBACK_STATUS_CHANGED) {
                    if (status == BM83_AVRCP_DATA_PLAYBACK_STATUS_PAUSED &&
                        bt->playbackStatus != BT_AVRCP_STATUS_PAUSED
                    ) {
                        bt->playbackStatus = BT_AVRCP_STATUS_PAUSED;
                        EventTriggerCallback(BT_EVENT_PLAYBACK_STATUS_CHANGE, 0);
                    } else if (status == BM83_AVRCP_DATA_PLAYBACK_STATUS_PLAYING &&
                        bt->playbackStatus != BT_AVRCP_STATUS_PLAYING
                    ) {
                        bt->playbackStatus = BT_AVRCP_STATUS_PLAYING;
                        EventTriggerCallback(BT_EVENT_PLAYBACK_STATUS_CHANGE, 0);
                    }
                } else if (updateType == BM83_AVRCP_EVT_PLAYBACK_TRACK_CHANGED) {
                    status = 0x00;
                }
                uint8_t updateData[2] = {updateType, status};
                EventTriggerCallback(BT_EVENT_AVRCP_PDU_CHANGE, updateData);
            }
            break;
        }
    }
}

/**
 * BM83ProcessEventBTMStatus()
 *     Description:
 *         Process updates to the BTM Status
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83ProcessEventBTMStatus(BT_t *bt, uint8_t *data, uint16_t length)
{
    switch (data[BM83_FRAME_DB0]) {
        case BM83_DATA_BTM_STATUS_POWER_OFF: {
            LogDebug(LOG_SOURCE_BT, "BT: Power Off");
            bt->powerState = BT_STATE_OFF;
            bt->status = BT_STATUS_OFF;
            break;
        }
        case BM83_DATA_BTM_STATUS_PAIRING_ON: {
            LogDebug(LOG_SOURCE_BT, "BT: Pairing On");
            bt->discoverable = BT_STATE_ON;
            break;
        }
        case BM83_DATA_BTM_STATUS_POWER_ON: {
            LogDebug(LOG_SOURCE_BT, "BT: Power On");
            bt->powerState = BT_STATE_ON;
            bt->status = BT_STATUS_DISCONNECTED;
            EventTriggerCallback(BT_EVENT_BOOT_STATUS, 0);
            break;
        }
        case BM83_DATA_BTM_STATUS_PAIRING_OK: {
            LogDebug(LOG_SOURCE_BT, "BT: Pairing Ok");
            bt->discoverable = BT_STATE_OFF;
            bt->status = BT_STATUS_CONNECTED;
            break;
        }
        case BM83_DATA_BTM_STATUS_PAIRING_NOK: {
            LogDebug(LOG_SOURCE_BT, "BT: Pairing Error");
            bt->discoverable = BT_STATE_OFF;
            break;
        }
        case BM83_DATA_BTM_STATUS_HFP_CONN: {
            LogDebug(LOG_SOURCE_BT, "BT: HFP Open");
            bt->activeDevice.deviceId = data[BM83_FRAME_DB1];
            bt->status = BT_STATUS_CONNECTED;
            bt->activeDevice.hfpId = 1;
            uint8_t linkType = BT_LINK_TYPE_HFP;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            break;
        }
        case BM83_DATA_BTM_STATUS_A2DP_CONN: {
            LogDebug(LOG_SOURCE_BT, "BT: A2DP Open");
            bt->activeDevice.deviceId = data[BM83_FRAME_DB1];
            bt->status = BT_STATUS_CONNECTED;
            bt->activeDevice.a2dpId = 1;
            uint8_t linkType = BT_LINK_TYPE_A2DP;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            break;
        }
        case BM83_DATA_BTM_STATUS_HFP_DISCO: {
            LogDebug(LOG_SOURCE_BT, "BT: HFP Closed");
            bt->status = BT_STATUS_DISCONNECTED;
            bt->activeDevice.hfpId = 0;
            uint8_t linkType = BT_LINK_TYPE_HFP;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            break;
        }
        case BM83_DATA_BTM_STATUS_A2DP_DISCO: {
            LogDebug(LOG_SOURCE_BT, "BT: A2DP Closed");
            bt->status = BT_STATUS_DISCONNECTED;
            bt->activeDevice.deviceId = 0;
            bt->activeDevice.a2dpId = 0;
            uint8_t linkType = BT_LINK_TYPE_A2DP;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            break;
        }
        case BM83_DATA_BTM_STATUS_SCO_CONN: {
            LogDebug(LOG_SOURCE_BT, "BT: SCO Open");
            bt->scoStatus = BT_CALL_SCO_OPEN;
            break;
        }
        case BM83_DATA_BTM_STATUS_SCO_DISCO: {
            LogDebug(LOG_SOURCE_BT, "BT: SCO Closed");
            bt->scoStatus = BT_CALL_SCO_CLOSE;
            break;
        }
        case BM83_DATA_BTM_STATUS_AVRCP_CONN: {
            LogDebug(LOG_SOURCE_BT, "BT: ARVCP Open");
            bt->status = BT_STATUS_CONNECTED;
            bt->activeDevice.avrcpId = 1;
            uint8_t linkType = BT_LINK_TYPE_AVRCP;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            break;
        }
        case BM83_DATA_BTM_STATUS_AVRCP_DISCO: {
            LogDebug(LOG_SOURCE_BT, "BT: ARVCP Closed");
            bt->status = BT_STATUS_DISCONNECTED;
            bt->activeDevice.avrcpId = 0;
            uint8_t linkType = BT_LINK_TYPE_AVRCP;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            break;
        }
        case BM83_DATA_BTM_STATUS_STANDARD_SPP_CONN: {
            LogDebug(LOG_SOURCE_BT, "BT: SPP Open");
            break;
        }
        case BM83_DATA_BTM_STATUS_STANDARD_SPP_IAP_DISCO: {
            break;
        }
        case BM83_DATA_BTM_STATUS_STANDBY_ON: {
            LogDebug(LOG_SOURCE_BT, "BT: Standby On");
            if (bt->activeDevice.deviceId != 0) {
                bt->status = BT_STATUS_DISCONNECTED;
                memset(&bt->activeDevice, 0, sizeof(BTConnection_t));
                LogDebug(LOG_SOURCE_BT, "BT: Device Disconnected [BTM Standby]");
                EventTriggerCallback(BT_EVENT_DEVICE_DISCONNECTED, 0);
            }
            bt->powerState = BT_STATE_STANDBY;
            EventTriggerCallback(BT_EVENT_BOOT_STATUS, 0);
            break;
        }
        case BM83_DATA_BTM_STATUS_IAP_CONN: {
            // We do not support iAP
            break;
        }
        case BM83_DATA_BTM_STATUS_ACL_DISCO: {
            bt->status = BT_STATUS_DISCONNECTED;
            memset(&bt->activeDevice, 0, sizeof(BTConnection_t));
            LogDebug(LOG_SOURCE_BT, "BT: Device Disconnected");
            uint8_t linkType = BT_LINK_TYPE_ACL;
            EventTriggerCallback(BT_EVENT_DEVICE_LINK_CONNECTED, &linkType);
            EventTriggerCallback(BT_EVENT_DEVICE_DISCONNECTED, 0);
            break;
        }
        case BM83_DATA_BTM_STATUS_MAP_CONN: {
            break;
        }
        case BM83_DATA_BTM_STATUS_MAP_OPERATION_FORBIDDEN: {
            break;
        }
        case BM83_DATA_BTM_STATUS_MAP_DISCONN: {
            break;
        }
        case BM83_DATA_BTM_STATUS_ACL_CONN: {
            bt->activeDevice = BTConnectionInit();
            bt->status = BT_STATUS_CONNECTED;
            LogDebug(LOG_SOURCE_BT, "BT: Device Connected");
            EventTriggerCallback(BT_EVENT_DEVICE_CONNECTED, 0);
            break;
        }
        case BM83_DATA_BTM_STATUS_SPP_IAP_DISCONN_NO_OTHER_PROFILE: {
            break;
        }
        case BM83_DATA_BTM_STATUS_LINK_BACK_ACL: {
            LogDebug(LOG_SOURCE_BT, "BT: Link Back Pairing");
            bt->status = BT_STATUS_CONNECTED;
            break;
        }
        case BM83_DATA_BTM_STATUS_INQUIRY_ON: {
            LogDebug(LOG_SOURCE_BT, "BT: Scan On");
            break;
        }
        case BM83_DATA_BTM_STATUS_AUDIO_SRC_NOT_AUX_NOT_A2DP: {
            LogDebug(LOG_SOURCE_BT, "BT: Audio Source Not A2DP/AUX");
            break;
        }
        case BM83_DATA_BTM_STATUS_AUDIO_SRC_AUX_IN: {
            LogDebug(LOG_SOURCE_BT, "BT: Audio Source AUX");
            break;
        }
        case BM83_DATA_BTM_STATUS_AUDIO_SRC_A2DP: {
            LogDebug(LOG_SOURCE_BT, "BT: Audio Source A2DP");
            break;
        }
    }
}

/**
 * BM83ProcessEventCallStatus()
 *     Description:
 *         Process Call Status Updates
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83ProcessEventCallStatus(BT_t *bt, uint8_t *data, uint16_t length)
{
    switch (data[BM83_FRAME_DB1]) {
        case BM83_DATA_CALL_STATUS_IDLE:
            bt->callStatus = BT_CALL_INACTIVE;
            break;
        case BM83_DATA_CALL_STATUS_VR:
            bt->callStatus = BT_CALL_VR;
            break;
        case BM83_DATA_CALL_STATUS_INCOMMING:
            bt->callStatus = BT_CALL_INCOMING;
            break;
        case BM83_DATA_CALL_STATUS_OUTGOING:
            bt->callStatus = BT_CALL_OUTGOING;
            break;
        case BM83_DATA_CALL_STATUS_ACTIVE:
        case BM83_DATA_CALL_STATUS_ACTIVE_CALL_WAITING:
        case BM83_DATA_CALL_STATUS_ACTIVE_CALL_HOLD:
            bt->callStatus = BT_CALL_ACTIVE;
            break;
    }
}

/**
 * BM83ProcessEventCallerID()
 *     Description:
 *         Process Caller ID Updates
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83ProcessEventCallerID(BT_t *bt, uint8_t *data, uint16_t length)
{
    char callerId[length];
    memset(callerId, 0, length);
    uint16_t i = 0;
    for (i = 0; i < length; i++) {
        callerId[i] = data[i + BM83_FRAME_DB1];
    }
    memset(bt->callerId, 0, BT_CALLER_ID_FIELD_SIZE);
    strncpy(bt->callerId, callerId, BT_CALLER_ID_FIELD_SIZE);
    EventTriggerCallback(BT_EVENT_CALLER_ID_UPDATE, 0);
}

/**
 * BM83ProcessEventReadLinkStatus()
 *     Description:
 *         Process Link Status Information
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t *data - The data portion of the frame,
 *             beginning with the byte after the event code
 *         uint16_t length - The length of the data
 *     Returns:
 *         void
 */
void BM83ProcessEventReadLinkStatus(BT_t *bt, uint8_t *data, uint16_t length)
{
    switch (data[BM83_FRAME_DB0]) {
        case BM83_DATA_LINK_STATE_POWER_OFF:
            LogDebug(LOG_SOURCE_BT, "BT: Powered Off");
            bt->status = BT_STATUS_OFF;
            bt->powerState = BT_STATE_OFF;
            // Call the boot event so we can turn the BT Module back on
            EventTriggerCallback(BT_EVENT_BOOT, 0);
            break;
        case BM83_DATA_LINK_STATE_PAIRING_ON:
            LogDebug(LOG_SOURCE_BT, "BT: Pairing On");
            bt->discoverable = BT_STATE_ON;
            break;
        case BM83_DATA_LINK_STATE_STANDBY:
            LogDebug(LOG_SOURCE_BT, "BT: Standby On");
            bt->status = BT_STATUS_DISCONNECTED;
            bt->powerState = BT_STATE_STANDBY;
            EventTriggerCallback(BT_EVENT_BOOT_STATUS, 0);
            break;
        case BM83_DATA_LINK_STATE_HF_CONNECTED_ONLY:
        case BM83_DATA_LINK_STATE_A2DP_CONNECTED_ONLY:
        case BM83_DATA_LINK_STATE_SPP_CONNECTED_ONLY:
        case BM83_DATA_LINK_STATE_MULTI_PROFILES_CONNECTED:
            bt->connectable = BT_STATE_ON;
            bt->discoverable = BT_STATE_OFF;
            bt->status = BT_STATUS_CONNECTED;
            break;
    }
}

/**
 * BM83ProcessEventReadLinkedDeviceInformation()
 *     Description:
 *         Process Linked Device Information (Remote device name)
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t *data - The data portion of the frame,
 *             beginning with the byte after the event code
 *         uint16_t length - The length of the data
 *     Returns:
 *         void
 */
void BM83ProcessEventReadLinkedDeviceInformation(BT_t *bt, uint8_t *data, uint16_t length)
{
    uint8_t dataType = data[BM83_FRAME_DB1];
    switch (dataType) {
        case BM83_LINKED_DEVICE_QUERY_NAME: {
            char deviceName[BT_DEVICE_NAME_LEN + 1] = {0};
            uint8_t i = 0;
            for (i = 0; i < length && i < BT_DEVICE_NAME_LEN; i++) {
                deviceName[i] = data[i + BM83_FRAME_DB2];
            }
            LogDebug(LOG_SOURCE_BT, "Connected Device: %s", deviceName);
            strncpy(bt->activeDevice.deviceName, deviceName, BT_DEVICE_NAME_LEN);
            EventTriggerCallback(BT_EVENT_DEVICE_CONNECTED, 0);
            break;
        }
    }
}

/**
 * BM83ProcessEventReadPairedDeviceRecord()
 *     Description:
 *         Process updates to the BTM Status
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83ProcessEventReadPairedDeviceRecord(BT_t *bt, uint8_t *data, uint16_t length)
{
    uint8_t pairedDevices = data[BM83_FRAME_DB0];
    uint16_t dataPos = BM83_FRAME_DB1;
    while (pairedDevices > 0) {
        uint8_t macId[6] = {0};
        uint8_t number = data[dataPos++];
        int8_t i;
        for (i = 6; i > 0; i--) {
            macId[i - 1] = data[dataPos++];
        }
        BTPairedDeviceInit(bt, macId, "", number);
        pairedDevices--;
    }
    EventTriggerCallback(BT_EVENT_DEVICE_FOUND, 0);
}

/**
 * BM83ProcessEventReportLinkBackStatus()
 *     Description:
 *         Process the link back status report
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t *data - The data portion of the frame,
 *             beginning with the byte after the event code
 *         uint16_t length - The length of the data
 *     Returns:
 *         void
 */
void BM83ProcessEventReportLinkBackStatus(BT_t *bt, uint8_t *data, uint16_t length)
{
    uint8_t eventData[2] = {data[BM83_FRAME_DB0], data[BM83_FRAME_DB1]};
    EventTriggerCallback(BT_EVENT_LINK_BACK_STATUS, eventData);
}

/**
 * BM83Process()
 *     Description:
 *         Read the RX queue and process the messages into meaningful data
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *     Returns:
 *         void
 */
void BM83Process(BT_t *bt)
{
    if (bt->uart.rxQueue.size >= 3) {
        uint8_t startw = CharQueueGetOffset(&bt->uart.rxQueue, 0);
        if (startw != 0xAA) {
            // Trash the byte
            uint8_t byte = CharQueueNext(&bt->uart.rxQueue);
            LogWarning("BT: Invalid Start Word [%02X]", byte);
        } else {
            uint8_t lengthHigh = CharQueueGetOffset(&bt->uart.rxQueue, 1);
            uint8_t lengthLow = CharQueueGetOffset(&bt->uart.rxQueue, 2);
            uint16_t frameLength = (lengthLow & 0xFF) | (lengthHigh << 8);
            if (bt->uart.rxQueue.size >= frameLength && frameLength > 0) {
                long long unsigned int ts = (long long unsigned int) TimerGetMillis();
                LogRawDebug(LOG_SOURCE_BT, "[%llu] DEBUG: BM83: RX: ", ts);
                uint8_t startWord = CharQueueNext(&bt->uart.rxQueue);
                LogRawDebug(LOG_SOURCE_BT, "%02X ", startWord);
                // Drop the 16-bit frame length
                LogRawDebug(LOG_SOURCE_BT, "%02X ", CharQueueNext(&bt->uart.rxQueue));
                LogRawDebug(LOG_SOURCE_BT, "%02X ", CharQueueNext(&bt->uart.rxQueue));
                uint8_t event = CharQueueNext(&bt->uart.rxQueue);
                LogRawDebug(LOG_SOURCE_BT, "%02X ", event);
                frameLength--;
                uint8_t eventData[frameLength];
                memset(eventData, 0, frameLength);
                uint16_t dataLength;
                for (dataLength = 0; dataLength < frameLength; dataLength++) {
                    eventData[dataLength] = CharQueueNext(&bt->uart.rxQueue);
                    LogRawDebug(LOG_SOURCE_BT, "%02X ", eventData[dataLength]);
                }
                uint8_t checksum = CharQueueNext(&bt->uart.rxQueue);
                LogRawDebug(LOG_SOURCE_BT, "%02X\r\n", checksum);
                // Always acknowledge reception of the frame first
                if (event != BM83_EVT_COMMAND_ACK) {
                    uint8_t ack[] = {BM83_CMD_EVENT_ACK, event};
                    BM83SendCommand(bt, ack, sizeof(ack));
                }
                if (event == BM83_EVT_AVC_SPECIFIC_RSP) {
                    BM83ProcessEventAVCSpecificRsp(bt, eventData, dataLength);
                }
                if (event == BM83_EVT_BTM_STATUS) {
                    BM83ProcessEventBTMStatus(bt, eventData, dataLength);
                }
                if (event == BM83_EVT_CALL_STATUS) {
                    BM83ProcessEventCallStatus(bt, eventData, dataLength);
                }
                if (event == BM83_EVT_CALLER_ID) {
                    BM83ProcessEventCallerID(bt, eventData, dataLength);
                }
                if (event == BM83_EVT_READ_LINK_STATUS_REPLY) {
                    BM83ProcessEventReadLinkStatus(bt, eventData, dataLength);
                }
                if (event == BM83_EVT_READ_LINKED_DEVICE_INFORMATION_REPLY) {
                    BM83ProcessEventReadLinkedDeviceInformation(
                        bt,
                        eventData,
                        dataLength
                    );
                }
                if (event == BM83_EVT_READ_PAIRED_DEVICE_RECORD_REPLY) {
                    BM83ProcessEventReadPairedDeviceRecord(
                        bt,
                        eventData,
                        dataLength
                    );
                }
                if (event == BM83_EVT_REPORT_BTM_INITIAL_STATUS) {
                    if (eventData[BM83_FRAME_DB0] ==
                        BM83_DATA_BTM_INITIAL_STATUS_BOOT_COMPLETE
                    ) {
                        EventTriggerCallback(BT_EVENT_BOOT, 0);
                    }
                }
                if (event == BM83_EVT_REPORT_LINK_BACK_STATUS) {
                    BM83ProcessEventReportLinkBackStatus(
                        bt,
                        eventData,
                        dataLength
                    );
                }
            }
        }
    }
    UARTReportErrors(&bt->uart);
}

/**
 * BM83SendCommand()
 *     Description:
 *         Send data over UART
 *     Params:
 *         BT_t *bt - A pointer to the module object
 *         uint8_t *targetData - A command to send along with its data
 *         size_t size - The target length of the frame
 *     Returns:
 *         void
 */
void BM83SendCommand(
    BT_t *bt,
    uint8_t *targetData,
    size_t size
) {
    uint8_t idx = 0;
    long long unsigned int ts = (long long unsigned int) TimerGetMillis();
    LogRawDebug(
        LOG_SOURCE_BT,
        "[%llu] DEBUG: BM83: TX: AA 00 ",
        ts
    );
    uint8_t checksum = 0xFF;
    // Send the startword
    CharQueueAdd(&bt->uart.txQueue, 0xAA);
    // Send the length
    CharQueueAdd(&bt->uart.txQueue, 0x00);
    CharQueueAdd(&bt->uart.txQueue, size);
    LogRawDebug(LOG_SOURCE_BT, "%02X ", size);
    checksum = checksum - size;
    for (idx = 0; idx < size; idx++) {
        CharQueueAdd(&bt->uart.txQueue, targetData[idx]);
        checksum = checksum - targetData[idx];
        LogRawDebug(LOG_SOURCE_BT, "%02X ", targetData[idx]);
    }
    checksum++;
    LogRawDebug(LOG_SOURCE_BT, "%02X\r\n", checksum);
    CharQueueAdd(&bt->uart.txQueue, checksum);
    // Set the interrupt flag
    SetUARTTXIE(bt->uart.moduleIndex, 1);
}
