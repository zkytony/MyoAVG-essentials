/*
 * AVG - Audio Venture Game
 */

#ifndef AVG_CONTROLLER_HPP
#define AVG_CONTROLLER_HPP

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "Node.hpp"

// SDL libraries
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDLAudio.hpp"
#include "Events.hpp"

/*
 * This class is the controller for the game
 *
 * Although one could, using Text to Speech API, such
 * as the one provided by Microsoft, to add in various
 * background stories; For the purpose of this project
 * we set the background to be 'Post-Modern Robotics
 * Era', where the player's role is a robotics scientist
 * And his goal is to collect N power rings - known as
 * "The Rings Of the Infinite Energy"
 * He also has an enemy, a robot that he built that has
 * become so intelligent, that it realized the importance
 * of the power rings, and is also trying to collect them
 * in order to give robots infinite energy and rule the
 * earth
 *
 * This adventure game is like those text adventure game,
 * where you need to walk around, and explore. But the
 * messages are provided in a Audio fashion - you hear it.
 * With Myo, it is even more enjoyable, because you can
 * just sit back and close your eyes, and use your brain
 * to imagine, and reason, and play the game.
 *
 * Story is designed by Kaiyu Zheng
 */
class AVGController 
{
public:

  AVGController(); // std::string init_file); 
  enum DIRECTION {
    SOUTH, NORTH, WEST, EAST
  };

  enum MOVE {
    FORWARD, LEFT, RIGHT
  };

  enum STATE {
    UNWALKABLE, NOTHING, SUCCEED, ERROR
  };

  // Audio to play in different situations
  enum AUDIO {
    INTRO, BEFORE_START, BACKGROUND,
    TO_SOUTH, TO_NORTH, TO_WEST, TO_EAST,
    GO_FORWARD, GO_LEFT, GO_RIGHT,
    ITS_UNWALKABLE, ITS_NOTHING, ITS_TREASURE,
    BEEP, WIN
  };

  SDLAudio *audio;

  STATE go_to(MOVE move); // direction is left, right, or forward
  bool has_come(int x, int y);
  bool check_status();
  void quit();
  void print_map();
  std::string current_facing();

  int c_x; // current x pos
  int c_y; // current y pos

  bool initialized = false;
  bool sdl_initialized = false;

  // following functions controls the audio
  // returns 1 for succeed, -1 for error
  int play_audio(AUDIO which, bool is_music, bool infinite);

private:
  int cols;
  int rows;
  Node** nodes; // 2D array of nodes

  std::map<std::string, int> data;
  std::vector<std::string> history; // stores the history of movements

  int collected;
  int steps;
  int health;

  void fill_node(int x, int y, Node::TYPE type, std::string msg);
  void generate_map();

  // direction that you are facing
  DIRECTION facing;

  // Return 0 for unwalkable
  // Return 1 for succeed;
  // Return 2 for nothing there
  // Return -1 for error
  // Also updates current location
  // Also updates facing
  STATE validate_move(DIRECTION direction); // direction is south (0), north (1), east (2), west (3)

  void init_SDLAudio();
  std::map<AUDIO, int> index; // used to store index of the audio data (music and chunk)
};

#endif
