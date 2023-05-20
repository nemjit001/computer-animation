#pragma once

#include <AnimationClip.hpp>
#include <Mesh.hpp>

class AnimationPlayer {
public:
	double global_time;
	AnimationClip* current_anim;
	Mesh* tgt_mesh;

};