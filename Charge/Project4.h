#ifndef _VR_PROJECT_4_H_
#define _VR_PROJECT_4_H_

// Definitions to be used for Project 3

#include "Globals.h"
#include "Oculus.cpp"
#include "Skybox.h"

extern enum ACTOR_TYPE;

class Project4 : public RiftApp {
private:
	Skybox *wallSky, *caveSkybox;

protected:
	void initGl() override;
	void update(mat4, mat4);
	void shutdownGl() override;
	void renderScene(const mat4&, const mat4&) {};
	void renderScene(const mat4&, const mat4&, ovrEyeType);
	void draw() override;
};

void updateLeapPos(vec3);
void createNewUnit(ACTOR_TYPE, int);

#endif