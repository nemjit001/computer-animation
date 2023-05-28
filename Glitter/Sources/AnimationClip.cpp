#include <AnimationClip.hpp>

AnimationClip::AnimationClip(std::string nameID, int n_bones, double duration, double ticks_per_second, std::vector<AnimationPose> poseSamples)
{
	this->nameID = nameID;
	this->n_bones = n_bones;
	this->duration = duration;
	this->ticks_per_second = ticks_per_second;
	this->poseSamples = poseSamples;
}