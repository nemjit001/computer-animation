#pragma once

#include <AnimationClip.hpp>
#include <Mesh.hpp>

class AnimationPlayer {
public:
	double animation_time = 0.0;
	bool is_playing = true;

	AnimationClip* current_anim;
	Mesh* tgt_mesh;

	AnimationPlayer(AnimationClip* anim, Mesh* mesh);

	/// <summary>
	/// Uses global time to set new animation time, while checking that it doesn't surpass its duration, in which case it resets
	/// </summary>
	/// <param name="global_time">: time from global timer</param>
	double UpdateTime(double global_time);

	/// <summary>
	/// Resets time
	/// </summary>
	inline void ResetTime()
	{
		animation_time = 0.0;			// Unexpected functionality is unexpected
	}
};