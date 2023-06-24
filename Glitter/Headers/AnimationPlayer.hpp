#pragma once

#include <AnimationClip.hpp>
#include <Mesh.hpp>

class AnimationPlayer {
public:
	double animation_time = 0.0;
	bool is_playing = true;
	
	int current_anim;				// Animation index
	Mesh* tgt_mesh;					// Mesh

	AnimationPlayer(int anim_index, Mesh* mesh);

	/// <summary>
	/// Uses global time to set new animation time, while checking that it doesn't surpass its duration, in which case it resets
	/// </summary>
	/// <param name="global_time">: time from global timer</param>
	double UpdateTime(double global_time, float animation_speed);

	/// <summary>
	/// Resets time
	/// </summary>
	inline void ResetTime()
	{
		animation_time = 0.0;			// Unexpected functionality is unexpected
	}

	/// <summary>
	/// Updates the animation player with another mesh and its animation
	/// </summary>
	/// <param name="anim"></param>
	/// <param name="mesh"></param>
	void SetValues(int anim_index, Mesh* mesh);
};