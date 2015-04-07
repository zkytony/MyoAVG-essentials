#ifndef SDL_AUDIO_HPP
#define SDL_AUDIO_HPP

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <iostream>
#include <string>
#include <vector>

class SDLAudio 
{
public:
  SDLAudio(int frequency = 44100, Uint16 format = MIX_DEFAULT_FORMAT, 
	   int channels = 2, int chunksize = 2048); // initialize
  int add_chunk(const char* file);   // returns a index for that added chunk
  int add_music(const char* file);   // returns a index for that added muxic

  int play_chunk(int index, int loops = 0, int channel = -1, int ticks = -1); // channel -1 for automatic chosen channel; ticks means millisecond limit to play sample, at most; ticks = -1 means play forever
  int play_music(int index, int loops = -1, bool fadeIn = true, int ms = 2000); // loop is -1 means forever

  // The following are mostly 1-line functions to keep consistency
  void pause_chunk(int channel); // index does not matter - channel is needed; -1 for all
  void pause_music();

  void resume_chunk(int channel); // -1 for all channels
  void resume_music();
  
  void halt_chunk(int channel); // -1 for all
  void halt_music();

  void group_to(int channel, int tag); // tag is like the ID for that group
  void fade_out_group(int tag, int ms);
  void halt_group(int tag);

  int has_music();

  void quit();
  
private:
  std::vector<Mix_Chunk*> m_chunks;
  std::vector<Mix_Music*> m_musics; // music is not played on a normal mixer channel

  int m_frequency;
  int m_channels;
  int m_chunksize;
  Uint16 m_format;

  void logError(std::ostream &os, const std::string &msg);

};

#endif
