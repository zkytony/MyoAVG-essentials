#include "AVGController.hpp"
#include <assert.h>

// init_file: the file path to the initialization configuration file (ini file)
AVGController::AVGController() { //std::string init_file) {
  data["rows"] = 5;
  data["cols"] = 5;
  data["number_of_targets"] = 4;
  data["health"] = 100;
  data["difficulty"] = 3; // 1 - 5
  // difficulty : num of conflicts = difficulty * (cols / 2)

  this->facing = DIRECTION::EAST;
  this->c_x = 0;
  this->c_y = 0;
  this->health = data["health"];

  std::cout << "row " << this->rows << std::endl;
  std::cout << "col " << this->cols << std::endl;

  int r = data["rows"];
  int c = data["cols"];

  this->rows = data["rows"];
  this->cols = data["cols"];

  this->nodes = new Node*[r];
  for (int i = 0; i < r; ++i) {
    nodes[i] = new Node[c];
  }

  this->generate_map();

  // initialize SDLAudio
  this->init_SDLAudio();

  if (sdl_initialized) {
    this->initialized = true;
  }
  else {
    std::cout << "Loading Music...Please Wait......" << std::endl;
  }
}

// 0 for start; 
// 1 for end; 
// 2 for normal; 
// 3 for nothing; 
// 4 for conflict;
// 5 for treasure
void AVGController::generate_map() {
  // The map is generated in such a way that a path from
  // starting node to ending node can be formed
  // Given x cols and y rows, first set the nodes (0,0) to
  // starting, and (x-1, y-1) to ending. Then randomly, for
  // the middle columns, select one node (xi, y_rand) for each
  // column; Then, add the nodes in necessary to connect
  // the two adjacent selected nodes.
  // The type of node is random; And whether or not there 
  // will be conflicts/awards is also random


  for (int i = 0; i < this->rows; ++i) {
    for (int j = 0; j < this->cols; ++j) {
      this->fill_node(i, j, Node::TYPE::NOTHING, "Nothing Bro");
    }
  }

  this->fill_node(0, 0, Node::TYPE::START, "START"); // start
  this->fill_node(this->rows-1, this->cols-1, Node::TYPE::END, "END"); // end

  // keep track of which row index does the node goes to
  int *path_node = new int[this->cols];
  path_node[0] = 0; // set the first column
  path_node[this->cols-1] = this->rows - 1; // set the last column

  // TODO: Random type improvement. Make the game more playable
  for (int j = 0; j < this->cols; ++j) {
    if (j != 0 && j != this->cols - 1) {
      int i = rand() % (this->rows - 1); // random number between 0 and rows - 1

      this->fill_node(i, j, Node::TYPE::NORMAL, "There is nothing special there"); // 2 is normal
      path_node[j] = i;

    }
  }

  // TODO: Improve the way the map is generated
  for (int j = 0; j < this->cols; ++j) {
    if (j != 0) {
      int diff = path_node[j] - path_node[j-1];
      for (int k = 1; k <= abs(diff); ++k) {
	if (diff > 0) {
	  this->fill_node(path_node[j - 1] + k, j - 1, Node::TYPE::NORMAL, "There is nothing special there");
	} else {
	  // opposite
	  this->fill_node(path_node[j - 1] - k, j - 1, Node::TYPE::NORMAL, "There is nothing special there");
	}
      }
    }
  }
}

// define direction:
// 0 - forward
// 1 - left
// 2 - right
// Return 0 for unwalkable
// Return 1 for succeed;
// Return 2 for nothing there
// Return -1 for error
//
// types:
// 0 for start; 
// 1 for end; 
// 2 for normal; 
// 3 for nothing; 
// 4 for conflict;
// 5 for treasure
AVGController::STATE AVGController::go_to(AVGController::MOVE move) {
  switch (facing) {
  case DIRECTION::SOUTH: // south
    if (move == MOVE::LEFT) { // left --> east (3)
      return this->validate_move(DIRECTION::EAST);
    }
    else if (move == MOVE::RIGHT) {
      return this->validate_move(DIRECTION::WEST);
    }
    else if (move == MOVE::FORWARD) {
      return this->validate_move(DIRECTION::SOUTH);
    }
    else {
      return STATE::ERROR;
    }
    break;
  case DIRECTION::NORTH: // north
    if (move == MOVE::LEFT) { // left --> east (3)
      return this->validate_move(DIRECTION::WEST);
    }
    else if (move == MOVE::RIGHT) {
      return this->validate_move(DIRECTION::EAST);
    }
    else if (move == MOVE::FORWARD) {
      return this->validate_move(DIRECTION::NORTH);
    }
    else {
      return STATE::ERROR;
    }
    break;
  case DIRECTION::WEST: // west
    if (move == MOVE::LEFT) { // left --> east (3)
      return this->validate_move(DIRECTION::SOUTH);
    }
    else if (move == MOVE::RIGHT) {
      return this->validate_move(DIRECTION::NORTH);
    }
    else if (move == MOVE::FORWARD) {
      return this->validate_move(DIRECTION::WEST);
    }
    else {
      return STATE::ERROR;
    }
    break;
  case DIRECTION::EAST: // east
    if (move == MOVE::LEFT) { // left --> east (3)
      return this->validate_move(DIRECTION::NORTH);
    }
    else if (move == MOVE::RIGHT) {
      return this->validate_move(DIRECTION::SOUTH);
    }
    else if (move == MOVE::FORWARD) {
      return this->validate_move(DIRECTION::EAST);
    }
    else {
      return STATE::ERROR;
    }
    break;
  }
  return STATE::ERROR;
}

// Return 0 for unwalkable
// Return 1 for succeed;
// Return 2 for nothing there
// Return -1 for error
// direction that you are facing
// 0 - south
// 1 - north
// 2 - west
// 3 - east
AVGController::STATE AVGController::validate_move(AVGController::DIRECTION direction) {
  switch (direction) {
  case DIRECTION::SOUTH: // south
    if (c_x + 1 > this->cols - 1) return STATE::UNWALKABLE;
    if (nodes[c_x + 1][c_y].type == Node::TYPE::NOTHING) return STATE::NOTHING;
		
    nodes[c_x + 1][c_y].has_come = true;
    c_x++;
    steps++;
		
    facing = DIRECTION::SOUTH;
    return STATE::SUCCEED;
    break;
  case DIRECTION::NORTH: // north
    if (c_x - 1 < 0) return STATE::UNWALKABLE;
    if (nodes[c_x - 1][c_y].type == Node::TYPE::NOTHING) return STATE::NOTHING;
		
    nodes[c_x - 1][c_y].has_come = true;
    c_x--;
    steps++;

    facing = DIRECTION::NORTH;
    return STATE::SUCCEED;
    break;
  case DIRECTION::WEST: // west
    if (c_y - 1 < 0) return STATE::UNWALKABLE;
    if (nodes[c_x][c_y - 1].type == Node::TYPE::NOTHING) return STATE::NOTHING;
		
    nodes[c_x][c_y - 1].has_come = true;
    c_y--;
    steps++;

    facing = DIRECTION::WEST;
    return STATE::SUCCEED;
    break;
  case DIRECTION::EAST: // east
    if (c_y + 1 > this->rows - 1) return STATE::UNWALKABLE;
    if (nodes[c_x][c_y + 1].type == Node::TYPE::NOTHING) return STATE::NOTHING;
		
    nodes[c_x][c_y + 1].has_come = true;
    c_y++;
    steps++;
		
    facing = DIRECTION::EAST;
    return STATE::SUCCEED;
    break;
  }
  return STATE::ERROR;
}

std::string AVGController::current_facing() {
  switch (facing) {
  case DIRECTION::SOUTH: // south
    return "South";
    break;
  case DIRECTION::NORTH: // north
    return "North";
    break;
  case DIRECTION::WEST: // west
    return "West";
    break;
  case DIRECTION::EAST: // east
    return "East";
    break;
  }
  return "Unknown";
}

bool AVGController::has_come(int x, int y) {
  return nodes[x][y].has_come;
}

bool AVGController::check_status() {
  return (health > 0 && (c_x != cols - 1 || c_y != rows - 1));
}

void AVGController::quit() {
  delete nodes;
}

// 0 for start; 
// 1 for end; 
// 2 for normal; 
// 3 for nothing; 
// 4 for conflict;
// 5 for treasure
void AVGController::print_map() {
  // prints a 2 dimensional rectangular like text, each point is the type of node it has
  for (int i = 0; i < this->rows; ++i) {
    for (int j = 0; j < this->cols; ++j) {
      if (i == c_x && j == c_y) {
	std::cout << "@";
      } else if (nodes[i][j].type == 3) {
	std::cout << "-";
      } else if (nodes[i][j].type == 2) {
	std::cout << "o";
      } else {
	std::cout << "*";
      }
    }
    std::cout << std::endl;
  }
  
}

// type - 0 for start; 1 for end; 2 for normal; 3 for nothing; 4 for conflict; 5 for treasure
void AVGController::fill_node(int x, int y, Node::TYPE type, std::string msg) {
  nodes[x][y].type = type; // start
  nodes[x][y].has_come = false;
  nodes[x][y].msg = msg;
  nodes[x][y].x = x;
  nodes[x][y].y = y;
}

void AVGController::init_SDLAudio() {
  this->audio = new SDLAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
  index[AUDIO::INTRO] = audio->add_music(INTRO_WAV);
  index[AUDIO::BACKGROUND] = audio->add_music(BGM_WAV);

  index[AUDIO::BEFORE_START] = audio->add_chunk(RULES_WAV);
  index[AUDIO::BEEP] = audio->add_chunk(BEEP_WAV);
  index[AUDIO::ITS_NOTHING] = audio->add_chunk(THERES_NOTHING_WAV);
  //index[AUDIO::ITS_TREASURE] = audio->add_chunk(THERES_TREASURE_WAV);
  index[AUDIO::ITS_UNWALKABLE] = audio->add_chunk(ITS_UNWALKABLE_WAV);
  index[AUDIO::WIN] = audio->add_chunk(YOU_WIN_WAV);
  index[AUDIO::TO_EAST] = audio->add_chunk(TURN_EAST_WAV);
  index[AUDIO::TO_WEST] = audio->add_chunk(TURN_WEST_WAV);
  index[AUDIO::TO_NORTH] = audio->add_chunk(TURN_NORTH_WAV);
  index[AUDIO::TO_SOUTH] = audio->add_chunk(TURN_SOUTH_WAV);
  index[AUDIO::GO_FORWARD] = audio->add_chunk(MOVE_FORWARD_WAV);
  index[AUDIO::GO_LEFT] = audio->add_chunk(MOVE_LEFT_WAV);
  index[AUDIO::GO_RIGHT] = audio->add_chunk(MOVE_RIGHT_WAV);

  sdl_initialized = true;
}

int AVGController::play_audio(AUDIO which, bool is_music, bool infinite) {
  int loops = infinite ? -1 : 0;
  if (sdl_initialized) {
    if (is_music) {
      if (this->audio->has_music()) {
	this->audio->halt_music();
      }
      this->audio->play_music(index[which], loops, false, 0); 
      return 1;
    }
    else {
      this->audio->play_chunk(index[which], loops, -1, -1);
      return 1;
    }
  }
  else {
    std::cout << "You have not initilized the SDL." << std::endl;
    return -1;
  }
}

