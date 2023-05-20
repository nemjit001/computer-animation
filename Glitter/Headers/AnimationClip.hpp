#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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
};

class AnimationClip {
public:
	int nameID;
	double duration;
	double ticks_per_second;
	AnimationPose poseSamples;

	// TODO: Implement Functions
	void Evaluate(double time, AnimationPose& tgt_pose);
	void Play();
	void Pause();
	void Reset();
};