#include "SDLAudio.hpp"

SDLAudio::SDLAudio(int frequency, Uint16 format, int channels, int chunksize) 
  : m_frequency(frequency), m_format(format), m_channels(channels), m_chunksize(chunksize)
{
  try {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
      // SDL audio initialization fails
      logError(std::cout, "SDL_Init");
      throw 99;
    }

    if (Mix_OpenAudio(frequency, format, channels, chunksize) < 0) {
      // Cannot open audio
      logError(std::cout, "Mix Open Audio");
      throw 98;
    }
  } catch (int err) {
    exit(EXIT_FAILURE);
  }
}

int SDLAudio::add_chunk(const char* file) {
  Mix_Chunk *chunk = Mix_LoadWAV(file);
  if (!chunk) {
    logError(std::cout, "Add chunk");
    return -1;
  }
  m_chunks.push_back(chunk);
  std::cout << "Added Chunk " << file << std::endl;
  return (int)m_chunks.size() - 1;
}

int SDLAudio::add_music(const char* file) {
  Mix_Music *music = Mix_LoadMUS(file);
  if (!music) {
    logError(std::cout, "Add Wav Music");
    return -1;
  }
  m_musics.push_back(music);
  std::cout << "Added Music " << file << std::endl;
  return (int)m_musics.size() - 1;
}

// Returns an integer for the channel this chunk is playing at
// Returns -1 for errors
int SDLAudio::play_chunk(int index, int loops, int channel, int ticks) {
  Mix_Chunk *chunk = m_chunks[index];
  
  if (channel != -1 && Mix_Playing(channel) != 0) {
    // the channel is already playing
    std::cout << "Warning: Channel " << channel << " is already playing. Will Halt It." << std::endl;
  }
  channel = Mix_PlayChannelTimed(channel, chunk, loops, ticks);
  if (channel == -1) {
    logError(std::cout, "Play Wav Chunk");
    return -1;
  }

  return channel;
}

// Play the loaded music loop times through from start to finish. The previous music will be
// halted, or if fading out it waits (blocking) for that to finish.
// Fade in is true, if want fade in effect, withing ms milliseconds
// Return 1 for success; -1 for error
int SDLAudio::play_music(int index, int loops, bool fadeIn, int ms) {
  Mix_Music *music = m_musics[index];

  if (fadeIn) {
    if (Mix_FadeInMusic(music, loops, ms) == -1) {
      logError(std::cout, "Fade in music");
      return -1;
    }
  } else {
	  if (!Mix_PlayingMusic()) {
		  if (Mix_PlayMusic(music, loops) == -1) {
			  logError(std::cout, "Play Music");
			  return -1;
		  }
	  }
  }
  return 1;
}

void SDLAudio::pause_chunk(int channel) {
  Mix_Pause(channel);
}

void SDLAudio::pause_music() {
  Mix_PauseMusic();
}

void SDLAudio::resume_chunk(int channel) {
  Mix_Resume(channel);
}

void SDLAudio::resume_music() {
  Mix_ResumeMusic();
}

void SDLAudio::halt_chunk(int channel) {
  Mix_HaltChannel(channel);
}

void SDLAudio::halt_music() {
  Mix_HaltMusic();
}

void SDLAudio::group_to(int channel, int tag) {
  Mix_GroupChannel(channel, tag);
}

void SDLAudio::fade_out_group(int tag, int ms) {
  Mix_FadeOutGroup(tag, ms);
}

void SDLAudio::halt_group(int tag) {
  Mix_HaltGroup(tag);
}

int SDLAudio::has_music() {
	return Mix_PlayingMusic();
}

void SDLAudio::logError(std::ostream &os, const std::string &msg) {
  os << msg << " (SDL error): " << SDL_GetError() << std::endl;
  os << msg << " (Mix error): " << Mix_GetError() << std::endl;
}

void SDLAudio::quit() {
  for (int i = 0; i < m_chunks.size(); i++) {
    Mix_FreeChunk(m_chunks[i]);
    m_chunks[i] = nullptr;
  }

  for (int i = 0; i < m_musics.size(); i++) {
    Mix_FreeMusic(m_musics[i]);
    m_musics[i] = nullptr;
  }

  Mix_Quit();
  SDL_Quit();
}
