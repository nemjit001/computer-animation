#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>

/// <summary>
/// Scale, Rotation, Time and Translation data for a keyframe
/// </summary>
struct SQT {
	double time;				// Time of Keyframe

	// Keyframe Data
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
};

/// <summary>
/// The Pose list for a single Bone
/// 
/// XXX: The Bone is referred to by name
/// </summary>
struct AnimationPose {
	std::vector<SQT> bonePoses;
	std::string bone_name;
};

/// <summary>
/// The AnimationClip class contains all data associated with an animation.
/// </summary>
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