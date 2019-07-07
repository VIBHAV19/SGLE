#include "stdafx.h"
#include "SoundObject.h"

SDL_AudioSpec SoundObject::wantedSpec;
std::vector<SoundObject*> SoundObject::sounds;

SoundObject::SoundObject(const char* filename, bool loop) {
	if (SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length) == NULL)
		return;

	SDL_AudioCVT cvter;
	SDL_BuildAudioCVT(&cvter, wav_spec.format, wav_spec.channels, wav_spec.freq, wantedSpec.format, wantedSpec.channels, wantedSpec.freq);

	//Do we need to convert the file?
	if (cvter.needed) {
		cvter.len = wav_length;
		cvter.buf = (Uint8 *)SDL_malloc(cvter.len * cvter.len_mult);

		SDL_memcpy(cvter.buf, wav_buffer, cvter.len);
		SDL_ConvertAudio(&cvter);
		SDL_FreeWAV(wav_buffer);

		wav_buffer = cvter.buf;
		wav_length = cvter.len_cvt;
	}

	volume = SDL_MIX_MAXVOLUME;
	looping = loop;
	playing = false;
	audio_pos = 0;
	sounds.push_back(this);
}


SoundObject::~SoundObject() {
	SDL_FreeWAV(wav_buffer);
	sounds.erase(std::remove(sounds.begin(), sounds.end(), this), sounds.end());
}

void SoundObject::audMix(void *userdata, Uint8 *stream, int len) {
	//Clear buffer
	SDL_memset(stream, 0, len);

	//TODO: May need a mutex

	//Go through all the sounds
	for (std::vector<SoundObject*>::iterator i = sounds.begin(); i != sounds.end(); i++) {
		SoundObject *sound = *i;
		if (sound->playing) {
			if (sound->audio_pos >= sound->wav_length) {
				if (!sound->looping)
					sound->playing = false;
				sound->audio_pos = 0;
			} else {
				SDL_MixAudio(stream, sound->wav_buffer + sound->audio_pos, len, sound->volume);
				sound->audio_pos += len;
			}
		}
	}
}


int SoundObject::initDevice() {
	if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
			return 1;
	}

	/* Set the audio format */
	wantedSpec.freq = 48000;
	wantedSpec.format = AUDIO_F32;
	wantedSpec.channels = 2;
	wantedSpec.samples = 1024;
	wantedSpec.callback = audMix;
	wantedSpec.userdata = NULL;

	if (SDL_OpenAudio(&wantedSpec, NULL) < 0) {
		std::cout << "Couldn't open audio: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_PauseAudio(0);

	return 0;
}

void SoundObject::freeDevice() {
	SDL_CloseAudio();
}


bool SoundObject::isPlaying() {
	return playing && audio_pos < wav_length;
}

void SoundObject::loop(bool doLoop) {
	looping = doLoop;
}

void SoundObject::setVolume(int vol) {
	if (vol > SDL_MIX_MAXVOLUME) vol = SDL_MIX_MAXVOLUME;
	volume = vol;
}

void SoundObject::play(bool restart) {
	playing = true;
	if (restart || audio_pos >= wav_length)
		audio_pos = 0;
}

void SoundObject::playWait(bool restart) {
	if (!isPlaying())
		play(restart);
}

void SoundObject::pause() {
	playing = false;
}
