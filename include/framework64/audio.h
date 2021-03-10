#ifndef FW64_AUDIO_H
#define FW64_AUDIO_H

#include <stdint.h>

typedef struct {
    uint32_t sound_count;
    uint32_t music_track_count;
} Audio;

typedef enum {
    SOUND_STATE_STOPPED,
    SOUND_STATE_PLAYING
} SoundStatus;

void audio_init(Audio* audio);

/**
 * Loads a sound effect bank.  The number of sounds contained in the bank will be placed in audio->sound_count.
 * \return nonzero value if the bank was successfully loaded, otherwise zero if an error occurred.
*/
int audio_load_soundbank(Audio* audio, int asset_id);

int audio_play_sound(Audio* audio, uint32_t sound_num);
int audio_stop_sound(Audio* audio, int handle);
SoundStatus audio_get_sound_status(Audio* audio, int handle);

/**
 * Loads a music (sequence) bank.  The number of tracks (sequences) contained in the bank will be placed in audio->music_track_count.
 * \returns nonzero value if the bank was successfully loaded, otherwise zero if an error occurred.
 */
int audio_load_music(Audio* audio, int asset_id);
int audio_play_music(Audio* audio, uint32_t track_num);
int audio_stop_music(Audio* audio);

#endif