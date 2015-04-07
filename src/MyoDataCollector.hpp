#ifndef MYO_AVG_CORE_CONTROLLER_HPP
#define MYO_AVG_CORE_CONTROLLER_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <map>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>

// AVG controller
#include "AVGController.hpp"

class MyoAVGCoreController : public myo::DeviceListener {
public:

	enum GAME {
		INTRO, BEFORE_START, STARTED, END 
	};

	AVGController *avgc;

	bool onArm;
	myo::Arm whichArm;

	bool isUnlocked;

	int roll_w, pitch_w, yaw_w;
	myo::Pose currentPose;

	// contructor goes here
	MyoAVGCoreController()
		: onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
	{
		this->avgc = new AVGController();
	}

	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		roll_w = 0;
		pitch_w = 0;
		yaw_w = 0;
		onArm = false;
		isUnlocked = false;
	}

	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
		// Convert the floating point angles in radians to a scale from 0 to 18.
		roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
	}


	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;
		if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
			myo->unlock(myo::Myo::unlockHold);

			myo->notifyUserAction();

			if (this->avgc->initialized) {
				
				AVGController::STATE validity = AVGController::STATE::ERROR;
				AVGController::AUDIO move_succeed_audio;

				switch (progress) {
				case GAME::INTRO:
					if (!intro_finished) {
						this->avgc->play_audio(AVGController::AUDIO::INTRO, true, false);
					}
					if (this->avgc->audio->has_music() == 0) {
						progress = GAME::BEFORE_START;
					}
					else if (pose == myo::Pose::waveIn) {
						this->avgc->audio->halt_music();
						progress = GAME::BEFORE_START;
					}
					break;
				case GAME::BEFORE_START:
					progress = GAME::STARTED;
					break;
				case GAME::STARTED:
					// Custom code goes here:
					//pose = myo::Pose::waveOut;
					if (pose == myo::Pose::waveIn) {
						// left
						validity = this->avgc->go_to(AVGController::MOVE::LEFT);
						move_succeed_audio = AVGController::AUDIO::GO_LEFT;
					}
					else if (pose == myo::Pose::waveOut) {
						// right
						AVGController::STATE validity = this->avgc->go_to(AVGController::MOVE::RIGHT);
						move_succeed_audio = AVGController::AUDIO::GO_RIGHT;
					}
					else if (pose == myo::Pose::fingersSpread) {
						// forward
						AVGController::STATE validity = this->avgc->go_to(AVGController::MOVE::FORWARD);
						move_succeed_audio = AVGController::AUDIO::GO_FORWARD;
					}
					break;
				case GAME::END:
					break;
				}

				if (validity == AVGController::STATE::SUCCEED) {
					this->avgc->play_audio(move_succeed_audio, false, false);
				}
				else if (validity == AVGController::STATE::NOTHING) {
					this->avgc->play_audio(AVGController::AUDIO::ITS_NOTHING, false, false);
				}
				else if (validity == AVGController::STATE::UNWALKABLE) {
					this->avgc->play_audio(AVGController::AUDIO::ITS_UNWALKABLE, false, false);
				}
			}
		}
		else {
			myo->unlock(myo::Myo::unlockTimed);
		}
	}
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
	{
		onArm = true;
		whichArm = arm;
	}

	void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}

	void onUnlock(myo::Myo* myo, uint64_t timestamp)
	{
		isUnlocked = true;
	}

	void onLock(myo::Myo* myo, uint64_t timestamp)
	{
		isUnlocked = false;
	}

	void print() {
		std::cout << std::string(25, '\n') << std::endl; // act as if the screen is refreshed
		std::cout << '\r'; // clear the current line;
		std::cout << "Welcome to MyoAVG! Myo found and connected!" << std::endl;
		if (onArm) {
			std::string poseString = currentPose.toString();

			std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
				<< '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';

			std::cout << std::endl;
			if (this->avgc->initialized) {
				this->avgc->print_map();
			}
		}
		std::cout << "Your position: " << avgc->c_x << ", " << avgc->c_y << std::endl;
		std::cout << "Your facing: " << avgc->current_facing() << std::endl;
		std::cout << "Your Progress: " << this->game_progress() << std::endl;
		std::cout << std::flush;
	}

	void eco_cycle() {
		if (!avgc->check_status()) {
			avgc->initialized = false;
			std::cout << "You Won!" << std::endl;
			progress = GAME::END;
		}
	}

private:
	GAME progress = GAME::INTRO;
	bool intro_finished = false;

	std::string game_progress() {
		switch (this->progress) {
		case GAME::INTRO:
			return "Introduction";
			break;
		case GAME::BEFORE_START:
			return "Before start";
			break;
		case GAME::STARTED:
			return "Started";
			break;
		case GAME::END:
			return "Ended";
			break;
		}
	}
};

#endif