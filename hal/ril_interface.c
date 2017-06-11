/*
 * Copyright (C) 2013 The CyanogenMod Project
 * Copyright (C) 2017 Andreas Schneider <asn@cryptomilk.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "audio_hw_ril"
/*#define LOG_NDEBUG 0*/

#include <errno.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include <utils/Log.h>
#include <cutils/properties.h>

#include "ril_interface.h"

#define VOLUME_STEPS_DEFAULT  "5"
#define VOLUME_STEPS_PROPERTY "ro.config.vc_call_vol_steps"

static int ril_connect_if_required(struct ril_handle *ril)
{
    int ok;
    int rc;

    if (ril->client == NULL) {
        ALOGE("%s: ril->client is NULL", __func__);
        return -1;
    }

    ok = isConnected_RILD(ril->client);
    if (ok) {
        return 0;
    }

    rc = Connect_RILD(ril->client);
    if (rc != RIL_CLIENT_ERR_SUCCESS) {
        ALOGE("%s: FATAL: Failed to connect to RILD: %s",
              __func__, strerror(errno));
        return -1;
    } else {
        ALOGE("%s: Successfully connected to RILD", __func__);
    }

    ALOGV("%s: Successfully connected to RILD", __func__);
    return 0;
}

int ril_open(struct ril_handle *ril)
{
    char property[PROPERTY_VALUE_MAX];

    if (ril == NULL) {
        return -1;
    }

    /* initialise audio ril client */
    ALOGE("%s: Initialising SecRil client...", __func__);
    ril->client = SecRilOpen();

    if (ril->client == NULL) {
        ALOGE("%s: SecRilOpen() failed", __func__);
    } else {
        ALOGE("%s: SecRilOpen() success", __func__);
    }

    ril->client = OpenClient_RILD();
    if (ril->client == NULL) {
        ALOGE("%s: OpenClient_RILD() failed", __func__);
        return -1;
    } else {
        ALOGE("%s: OpenClient_RILD() success", __func__);
    }

    if (SecRilCheckConnection(ril->client)) {
        ALOGE("%s: SecRil connection failed", __func__);
    } else {
        ALOGE("%s: SecRil connection success", __func__);
    }

    property_get(VOLUME_STEPS_PROPERTY, property, VOLUME_STEPS_DEFAULT);
    ril->volume_steps_max = atoi(property);

    /*
     * This catches the case where VOLUME_STEPS_PROPERTY does not contain
     * an integer
     */
    if (ril->volume_steps_max == 0) {
        ril->volume_steps_max = atoi(VOLUME_STEPS_DEFAULT);
    }

    ALOGV("%s: Successfully opened ril client connection", __func__);
    return 0;
}

int ril_close(struct ril_handle *ril)
{
    int rc;

    if (ril == NULL || ril->client == NULL) {
        return -1;
    }

    rc = Disconnect_RILD(ril->client);
    if (rc != RIL_CLIENT_ERR_SUCCESS) {
        ALOGE("%s: Disconnect_RILD failed", __func__);
        return -1;
    }

    rc = CloseClient_RILD(ril->client);
    if (rc != RIL_CLIENT_ERR_SUCCESS) {
        ALOGE("%s: CloseClient_RILD() failed", __func__);
        return -1;
    }
    ril->client = NULL;

    return 0;
}

int ril_set_call_volume(struct ril_handle *ril,
                        enum _SoundType sound_type,
                        float volume)
{
    int rc;

    rc = ril_connect_if_required(ril);
    if (rc != 0) {
        ALOGE("%s: Failed to connect to RIL (%s)", __func__, strerror(rc));
        return 0;
    }

    ALOGE("%s: Setting audio ril volume...", __func__);
    rc = SecRilSetVoiceVolume(ril->client, sound_type, volume);

    if (rc != 0) {
        ALOGE("%s: SecRilSetVoiceVolume() failed, rc=%d", __func__, rc);
    }

    rc = SetCallVolume(ril->client,
                       sound_type,
                       (int)(volume * ril->volume_steps_max));
    if (rc != 0) {
        ALOGE("%s: SetCallVolume() failed, rc=%d", __func__, rc);
    }

    ALOGV("%s: SetCallVolume() success, vol=%d", __func__, (int)volume*100);
    return rc;
}

int ril_set_call_audio_path(struct ril_handle *ril, enum _AudioPath path)
{
    int rc;

    rc = ril_connect_if_required(ril);
    if (rc != 0) {
        ALOGE("%s: Failed to connect to RIL (%s)", __func__, strerror(rc));
        return 0;
    }

    ALOGE("%s: Setting audio ril client path...", __func__);
    rc = SecRilSetVoicePath(ril->client, path, ORIGINAL_PATH);

    if (rc != 0) {
        ALOGE("%s: SecRilSetVoicePath() failed, rc=%d", __func__, rc);
    }

    rc = SetCallAudioPath(ril->client, path);
    if (rc != 0) {
        ALOGE("%s: SetCallAudioPath() failed, rc=%d", __func__, rc);
    }

    return rc;
}

int ril_set_call_clock_sync(struct ril_handle *ril,
                            enum _SoundClockCondition condition)
{
    int rc;

    rc = ril_connect_if_required(ril);
    if (rc != 0) {
        ALOGE("%s: Failed to connect to RIL (%s)", __func__, strerror(rc));
        return 0;
    }

    rc = SetCallClockSync(ril->client, condition);
    if (rc != 0) {
        ALOGE("%s: SetCallClockSync() failed, rc=%d", __func__, rc);
    }

    return rc;
}

int ril_set_mute(struct ril_handle *ril, enum _MuteCondition condition)
{
    int rc;

    rc = ril_connect_if_required(ril);
    if (rc != 0) {
        ALOGE("%s: Failed to connect to RIL (%s)", __func__, strerror(rc));
        return 0;
    }

    switch (condition) {
        case TX_UNMUTE:
            rc = SecRilSetTxMute(ril->client, false);
            break;
        case TX_MUTE:
            rc = SecRilSetTxMute(ril->client, true);
            break;
        case RX_UNMUTE:
            rc = SecRilSetRxMute(ril->client, false);
            break;
        case RX_MUTE:
            rc = SecRilSetRxMute(ril->client, true);
            break;
        case RXTX_UNMUTE:
            rc = SecRilSetRxMute(ril->client, false);
            rc = SecRilSetTxMute(ril->client, false);
            break;
        case RXTX_MUTE:
            rc = SecRilSetRxMute(ril->client, true);
            rc = SecRilSetTxMute(ril->client, true);
            break;
        default:
            break;
    }

    if (rc != 0) {
        ALOGE("%s: SecRilSetXxMute() failed, rc=%d", __func__, rc);
    }

    rc = SetMute(ril->client, condition);
    if (rc != 0) {
        ALOGE("%s: SetMute() failed, rc=%d", __func__, rc);
    }

    ALOGV("%s: SetMute() success, rc=%d", __func__, rc);
    return rc;
}
