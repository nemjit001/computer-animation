#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>
#include <map>

struct SQT {
	double time;					// Time of Keyframe

	// Keyframe Data
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
};

struct AnimationPose {
	std::vector<SQT> bonePoses;		// SQTs for each keyframe
	std::string bone_name;			// Name of bone
};

class AnimationClip {
public:
	double duration;									// Animation duration
	double ticks_per_second;							// Ticks per second
	std::map<std::string, AnimationPose> poseSamples;	// Map from bone name to AnimationPose

	AnimationClip(std::string nameID, int n_bones, int max_frames, double duration, double ticks_per_second, std::map<std::string, AnimationPose > poseSamples);

	// TODO: Implement Functions
	glm::mat4 Evaluate(double time, AnimationPose& tgt_pose);
	glm::mat4 Evaluate(int frame);
	void Play();
	void Pause();
	void Reset();
	int GetFrameNum();
	 
private:
	std::string nameID;											// Name of animation (currently not used)
	int n_bones;												// Number of bones in animation
	int max_frames;												// Maximum number of keyframes in a channel
};