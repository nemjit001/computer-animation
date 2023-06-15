#include <AnimationClip.hpp>

AnimationClip::AnimationClip(std::string nameID, int n_bones, int max_frames, double duration, double ticks_per_second, std::map<std::string, AnimationPose> poseSamples)
{
	this->nameID = nameID;
	this->n_bones = n_bones;
	this->max_frames = max_frames;
	this->duration = duration;
	this->ticks_per_second = ticks_per_second;
	this->poseSamples = poseSamples;
}

glm::mat4 AnimationClip::Evaluate(int frame)
{
	return glm::mat4(1.0f);
}

int AnimationClip::GetFrameNum()
{
	return max_frames;
}