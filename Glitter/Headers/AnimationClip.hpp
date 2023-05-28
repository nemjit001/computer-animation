#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>

struct SQT {
	double time;				// Time of Keyframe

	// Keyframe Data
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
};

struct AnimationPose {
	std::vector<SQT> bonePoses;
	std::string bone_name;
};

class AnimationClip {
public:
	AnimationClip(std::string nameID, int n_bones, double duration, double ticks_per_second, std::vector<AnimationPose> poseSamples);

	// TODO: Implement Functions
	void Evaluate(double time, AnimationPose& tgt_pose);
	void Play();
	void Pause();
	void Reset();
	 
private:
	std::string nameID;
	int n_bones;
	double duration;
	double ticks_per_second;
	std::vector<AnimationPose> poseSamples;
};