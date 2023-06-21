#include <AnimationPlayer.hpp>

AnimationPlayer::AnimationPlayer(AnimationClip* anim, Mesh* mesh) : current_anim(anim), tgt_mesh(mesh)
{

}

double AnimationPlayer::UpdateTime(double global_time)
{
	// If animation is paused, return
	if (!is_playing)
		return animation_time;

	double new_time = animation_time + global_time;

	// Check whether time exceeds animation duration, then reset
	if (new_time > current_anim->duration)
	{
		ResetTime();

		return animation_time;
	}

	// Update time
	animation_time = new_time;

	return animation_time;
}