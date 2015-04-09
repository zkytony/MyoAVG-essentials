// Copyright Kaiyu Zheng

#define _USE_MATH_DEFINES
#include <cmath>
#include <map>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>

// Myo
#include <myo/myo.hpp>
#include "MyoDataCollector.hpp"

// SDL libraries
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDLAudio.hpp"

// AVG controller
#include "AVGController.hpp"
#include "Events.hpp"

int main(int argc, char** argv)
{
  try {

    myo::Hub hub("com.example.hello-myo"); // not planning to change this yet
    std::cout << "Attempting to find a Myo..." << std::endl;
    myo::Myo* myo = hub.waitForMyo(10000);

    if (!myo) {
      throw std::runtime_error("Unable to find a Myo!");
    }

    std::cout << "Connected to a Myo armsband!" << std::endl << std::endl;

    MyoAVGCoreController collector;

    // init the AVGController
    hub.addListener(&collector);

    while (1) {
      hub.run(1000 / 10);
      collector.print();
      collector.eco_cycle();
    }
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "Press enter to continue.";
    std::cin.ignore();
    return 1;
  }
}
