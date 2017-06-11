/**
 * @file    audio-ril.h
 *
 * @brief   SEC Audio-RIL library
 */

#ifndef __AUDIO_RIL_H__
#define __AUDIO_RIL_H__

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#include <secril-client.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

typedef enum _ExtraVolume {
    ORIGINAL_PATH,
    EXTRA_VOLUME_PATH
} ExtraVolume;

typedef int (*RilCallbackHandler)(HRilClient handle, const void *data, size_t datalen);

//---------------------------------------------------------------------------
// APIs
//---------------------------------------------------------------------------

/**
 * Open RILD multi-client.
 * Return is client handle, NULL on error.
 */
HRilClient SecRilOpen(void);
/**
 * Stop RILD multi-client. If client socket was connected,
 * it will be disconnected.
 */
int SecRilClose(HRilClient client);

/**
 * check whether RILD is connected
 * Returns 0 or 1
 */
int SecRilCheckConnection(HRilClient client);

/**
 * check whether RILD supports multisim
 * Returns 0 or 1
 */
int SecRilCheckMultiSim(HRilClient client);

/**
 * Dump (debugging?) info
 */
// int SecRilDump(HRilClient client, int);

/**
 * Initialises dual ril mode
 */
int SecRilDuosInit(HRilClient client);

/**
 * Register callback handler. If handler is NULL,
 * the callback is unregistered.
 * Return is 0 or error code.
 */
int SecRilRegisterCallback(HRilClient client, uint32_t id, RilCallbackHandler handler);

/**
 * Register callback handler. If handler is NULL,
 * the handler for the ID is unregistered.
 * Return is 0 or error code.
 */
int SecRilRegisterCallbackSlot2(HRilClient client, uint32_t id, RilCallbackHandler handler);

/**
 * Set audio mode to what is passed in
 */
int SecRilSetAudioMode(HRilClient client, int mode, bool state);

/**
 * Set modem clock to master or slave.
 */
int SecRilSetCallClockSync(HRilClient client, SoundClockCondition condition);

/**
 * Set call forwarding mode
 */
int SecRilSetCallFowardingMode(HRilClient client, bool mode);

/**
 * Set cover state (?)
 */
int SecRilSetCoverState(HRilClient client, bool state);

/**
 * Set current modem
 */
int SecRilSetCurrentModem(HRilClient client, int modem);

/**
 * DHA Solution Set
 */
// int SecRilSetDhaSolution(HRilClient client, DhaSolMode mode, DhaSolSelect select, char *parameter);

/**
 * Set dual mic
 */
int SecRilSetDualMic(HRilClient client, bool mode);

/**
 * Set emergency mode
 */
int SetRilsetEmergencyMode(HRilClient client, bool mode);

//int SecRilSetEmergencyMode(HRilClient client, bool mode)
//{
//	return SetRilsetEmergencyMode(client, mode);
//}

/**
 * Set extra volume
 */
int SecRilSetExtraVolume(HRilClient client, bool mode);

/**
 * Set HAC mode
 */
int SecRilSetHACMode(HRilClient client, bool mode);

/**
 * Uknown use
 */
// int SecRilSetLoopback(HRilClient, int, int, int);

/**
 * Set mute or unmute
 */
int SecRilSetRxMute(HRilClient client, bool mute);
int SecRilSetTxMute(HRilClient client, bool mute);

/**
 * Set real call status
 */
int SecRilSetRealCallStatus(HRilClient client, bool status);

/**
 * Set sco solution
 */
// int SecRilSetScoSolution(HRilClient client, bool, int);

/**
 * Set sound clk mode
 */
// int SecRilSetSoundClkMode(HRilClient client, int);

/**
 * Set external sound device path for noise reduction.
 */
int SecRilSetVoicePath(HRilClient client, AudioPath path, ExtraVolume mode);

/**
 * Unknown use
 */
// int SecRilSetVoiceSolution(HRilClient);

/**
 * Set in-call volume.
 */
int SecRilSetVoiceVolume(HRilClient client, SoundType type, float vol);

/**
 * Return whether ril supports dual sim
 */
int SecRilSupportDS(HRilClient client);

/*
 * Unknown use/prototype
 */
// int SecRilSetVoLTEState(HRilClient, int);

#ifdef __cplusplus
};
#endif

#endif // __AUDIO_RIL_H__

// end of file

