#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <vector>
#include <iostream>
#include <algorithm>

class SoundObject {
	Uint32 wav_length;
	Uint8 *wav_buffer;
	SDL_AudioSpec wav_spec;

	unsigned long audio_pos;

	bool playing;
	bool looping;
	int volume;

	static SDL_AudioSpec wantedSpec;
	static std::vector<SoundObject*> sounds;

	static void audMix(void *userdata, Uint8 *stream, int len);

public:
	SoundObject(const char* filename, bool loop = false);
	~SoundObject();

	void loop(bool doLoop);
	void setVolume(int vol);
	bool isPlaying();
	void play(bool restart = false);
	void playWait(bool restart = false);
	void pause();

	static int initDevice();
	static void freeDevice();
};
