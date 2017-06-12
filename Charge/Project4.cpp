#include "Project4.h"
#include "OBJObject.h"
#include "Plane.h"
#include "Skybox.h"
#include "UI_Bar.h"
#include "Actor.h"
#include "Shader.hpp"
#include "Sphere.h"

#include "ServerGame.h"
#include "ClientGame.h"
#include <process.h>

// Server stuff
ServerGame * server;
ClientGame * client;

void serverLoop(void *);
void clientLoop();

void serverLoop(void *)
{
}

void clientLoop()
{
}

// For shader programs
GLuint phongShader, objShader, texShader;
GLuint uiShader, uiRectShader;

// Light properties
const int MAX_LIGHTS = 8;
int numLights;
float *lightPositions;
float *lightColors;

// Material properties
Material *tank_Green;
vec3 tankAmbient = vec3(.04f, .27f, .19f);
vec3 tankDiffuse = vec3(.22f, .83f, .44f);
vec3 tankSpecular = vec3(.19f, .76f, .40f);
float tankShininess = 5.f;

Material *wall_Brown;
vec3 wallAmbient = vec3(.24f, .19f, .02f);
vec3 wallDiffuse = vec3(.79f, .33f, .05f);
vec3 wallSpecular = vec3(.64f, .36f, .04f);
float wallShininess = 1.f;

Material *soldier_Navy;
vec3 soldierAmbient = vec3(.02f, .19f, .26f);
vec3 soldierDiffuse = vec3(.16f, .51f, .85f);
vec3 soldierSpecular = vec3(.11f, .20f, .80f);
float soldierShininess = 3.f;

Material *cannon_Dark;
vec3 cannonAmbient = vec3(.095f, .065f, .025f);
vec3 cannonDiffuse = vec3(.44f, .36f, .19f);
vec3 cannonSpecular = vec3(.75f, .72f, .35f);
float cannonShininess = 15.f;

Material *castle_Sand;
vec3 castleAmbient = vec3(.19f, .24f, .03f);
vec3 castleDiffuse = vec3(.67f, .84f, .15f);
vec3 castleSpecular = vec3(.75f, .90f, .20f);
float castleShininess = 3.f;

Material *sphere_Green;
vec3 sphereAmbient = vec3(.05f, .29f, .08f);
vec3 sphereDiffuse = vec3(.14f, .93f, .22f);
vec3 sphereSpecular = vec3(.10f, .96f, .15f);
float sphereShininess = 3.f;

Material *sphere_Blue;
vec3 spherebAmbient = vec3(.05f, .08f, .29f);
vec3 spherebDiffuse = vec3(.14f, .22f, .93f);
vec3 spherebSpecular = vec3(.10f, .15f, .96f);


// For the ground
vec3 groundColor = vec3(.6f, .6f, .6f);
Plane *ground;

// For the UI
UI_Bar *selfUIL, *selfUIR, *foeUIL, *foeUIR;

// Scene objects
vector<Actor *> selfActors;
vector<Actor *> foeActors;
int objCount = 0;

float selfHP, selfNRG, foeHP;

double lastTime;

// OBJ models
OBJObject* soldierObj, *tankObj, *wallObj, *cannonObj, *castleObj;
Actor* pickedUp;

// For the hand
Sphere *handSphere, *leapSphere;

// OVR input related
ovrInputState inputState;
bool touchInputReceived;
ovrPosef handPose;
vec3 leapHandPos = vec3(0, -1, 0);

// Other variables
bool gameStart;
double lastUpdateTime;
bool wasPickup;

void Project4::initGl() {
	RiftApp::initGl();
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Disable backface culling to render both sides of polygons
	glDisable(GL_CULL_FACE);
	// Set clear color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// Recenter the tracking origin now that the app is running
	ovr_RecenterTrackingOrigin(_session);

	// Project specific initializations// Create the models
	soldierObj = new OBJObject("objects/soldier.obj");
	tankObj = new OBJObject("objects/tank_T72.obj");
	cannonObj = new OBJObject("objects/cannon.obj");
	wallObj = new OBJObject("objects/wall.obj");
	castleObj = new OBJObject("objects/castle.obj");

	// Lights
	numLights = 2;
	lightPositions = new float[4 * MAX_LIGHTS];
	lightColors = new float[3 * MAX_LIGHTS];
	// Light 0 - directional
	lightPositions[0] = .1f; lightPositions[1] = -.1f;
	lightPositions[2] = 1.f; lightPositions[3] = 0.f;
	lightColors[0] = 1.f; lightColors[1] = 1.f; lightColors[2] = 1.f;
	// Test light 1 - negation of light 0
	lightPositions[4] = -.1f; lightPositions[5] = -.1f;
	lightPositions[6] = -1.f; lightPositions[7] = 0.f;
	lightColors[3] = 1.f; lightColors[4] = 1.f; lightColors[5] = 1.f;

	// Initialize shaders
	objShader = phongShader = LoadShaders("shaders/basic.vert", "shaders/phong.frag");
	// Phong shading and the regular materials
	glUseProgram(phongShader);
	tank_Green = new RegMaterial();
	((RegMaterial*)tank_Green)->setMaterial(tankAmbient, tankDiffuse, tankSpecular, tankShininess);
	tank_Green->getUniformLocs(phongShader);

	wall_Brown = new RegMaterial();
	((RegMaterial*)wall_Brown)->setMaterial(wallAmbient, wallDiffuse, wallSpecular, wallShininess);
	wall_Brown->getUniformLocs(phongShader);

	soldier_Navy = new RegMaterial();
	((RegMaterial*)soldier_Navy)->setMaterial(soldierAmbient, soldierDiffuse, soldierSpecular, soldierShininess);
	soldier_Navy->getUniformLocs(phongShader);

	cannon_Dark = new RegMaterial();
	((RegMaterial*)cannon_Dark)->setMaterial(cannonAmbient, cannonDiffuse, cannonSpecular, cannonShininess);
	cannon_Dark->getUniformLocs(phongShader);

	castle_Sand = new RegMaterial();
	((RegMaterial*)castle_Sand)->setMaterial(castleAmbient, castleDiffuse, castleSpecular, castleShininess);
	castle_Sand->getUniformLocs(phongShader);

	sphere_Green = new RegMaterial();
	((RegMaterial*)sphere_Green)->setMaterial(sphereAmbient, sphereDiffuse, sphereSpecular, sphereShininess);
	sphere_Green->getUniformLocs(phongShader);

	sphere_Blue = new RegMaterial();
	((RegMaterial*)sphere_Blue)->setMaterial(spherebAmbient, spherebDiffuse, spherebSpecular, sphereShininess);
	sphere_Blue->getUniformLocs(phongShader);

	soldierObj->setMaterial(soldier_Navy);
	soldierObj->setModel(translate(mat4(1.f), vec3(0, -.15f, 0)) * scale(mat4(1.f), vec3(1.2f, .7f, 1.f)));

	tankObj->setMaterial(tank_Green);
	tankObj->setModel(translate(mat4(1.f), vec3(0, -.2f, 0)) * scale(mat4(1.f), vec3(1.2f, 2.4f, 1.2f)));

	cannonObj->setMaterial(cannon_Dark);
	cannonObj->setModel(translate(mat4(1.f), vec3(0, -.25f, 0)) * scale(mat4(1.f), vec3(1.f, 1.2f, 0.9f)) * rotate(mat4(1.f), PI, vec3(0, 1, 0)));

	wallObj->setMaterial(wall_Brown);
	wallObj->setModel(translate(mat4(1.f), vec3(0, -.1f, 0)) * scale(mat4(1.f), vec3(1.f, 1.f, 4.f)));

	castleObj->setMaterial(castle_Sand);
	castleObj->setModel(translate(mat4(1.f), vec3(0, 1.3f, -3.f)) * scale(mat4(1.f), vec3(14.f, 5.f, 5.f)));

	// Create towers
	Actor *a1 = new Tower(castleObj);
	objCount++;
	a1->setID(-objCount);
	a1->setPosition(0, 0, 0);
	a1->toggleActive();
	a1->togglePlacing();
	a1->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
	selfActors.push_back(a1);

	Actor *a2 = new Tower(castleObj);
	objCount++;
	a2->setID(objCount);
	a2->setPosition(0, 0, -10);
	a2->toggleActive();
	a2->togglePlacing();
	foeActors.push_back(a2);

	selfNRG = 0.f;
	lastUpdateTime = lastTime = glfwGetTime();

	// Create the ground
	texShader = LoadShaders("shaders/texture.vert", "shaders/texture.frag");
	glUseProgram(texShader);
	ground = new Plane(texShader);
	ground->setColor(groundColor);
	ground->setModel(translate(mat4(1), vec3(0, -.5f, -5))
		* rotate(mat4(1), PI / 2.f, vec3(1, 0, 0))
		* scale(mat4(1), vec3(14, 12, 1)));

	// UI shaders and related components
	uiShader = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	uiRectShader = LoadShaders("shaders/ui_rect.vert", "shaders/ui_rect.frag");
	selfUIL = new UI_Bar(true, true);
	foeUIL = new UI_Bar(false, true);
	selfUIR = new UI_Bar(true, false);
	foeUIR = new UI_Bar(false, false);
	selfUIL->fetchUniforms(uiShader, uiRectShader);
	foeUIL->fetchUniforms(uiShader, uiRectShader);
	selfUIR->fetchUniforms(uiShader, uiRectShader);
	foeUIR->fetchUniforms(uiShader, uiRectShader);

	// The hand sphere
	glUseProgram(phongShader);
	handSphere = new Sphere(20, 20);
	handSphere->setMaterial(sphere_Green);
	leapSphere = new Sphere(20, 20);
	leapSphere->setMaterial(sphere_Blue);

	// Misc initializations
	touchInputReceived = false;

	// Start the server and client
	server = new ServerGame();
	client = new ClientGame();
	gameStart = true;
	wasPickup = false;
}

void Project4::shutdownGl() {
	if (soldierObj) delete soldierObj;
	if (tankObj) delete tankObj;
	if (cannonObj) delete cannonObj;
	if (wallObj) delete wallObj;
	if (castleObj) delete castleObj;
	if (tank_Green) delete tank_Green;
	if (sphere_Green) delete sphere_Green;
	if (sphere_Blue) delete sphere_Blue;
	if (wall_Brown) delete wall_Brown;
	if (soldier_Navy) delete soldier_Navy;
	if (cannon_Dark) delete cannon_Dark;
	if (castle_Sand) delete castle_Sand;
	if (lightPositions) delete[] lightPositions;
	if (lightColors) delete[] lightColors;
	if (ground) delete ground;
	if (selfUIL) delete selfUIL;
	if (foeUIL) delete foeUIL;
	if (selfUIR) delete selfUIR;
	if (foeUIR) delete foeUIR;
	if (handSphere) delete handSphere;
	if (leapSphere) delete leapSphere;
	selfActors.clear();
	foeActors.clear();

	// Kill the server and client
	if(server) delete server;
	if(client) delete client;
}

void updateLeapPos(vec3 p) {
	leapHandPos = p;
}

void Project4::update(mat4 left, mat4 right) {
	double displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, frame);
	ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);
	handPose = trackState.HandPoses[ovrHand_Right].ThePose;

	vec3 handPos = ovr::toGlm(handPose.Position);

	// Check for server response
	double currTime = glfwGetTime();
	if (!gameStart) lastTime = currTime;
	if (currTime - lastUpdateTime > 0.04) {
		server->update();
		if(gameStart)
			client->sendHandPos(handPos.x, handPos.y, handPos.z);
		client->update();
		lastUpdateTime = currTime;
	}

	// Update the game objects
	if (isGameOver) return;
	if (gameStart) {
		selfNRG += .045f * float(currTime - lastTime);
		lastTime = currTime;
		if (selfNRG > 1.f)
			selfNRG = 1.f;
		for (Actor *a : selfActors) {
			a->update();
		}
		for (Actor *b : foeActors) {
			b->update();
		}
		selfUIL->update(float(selfActors[0]->getHealthRatio()), selfNRG, float(foeActors[0]->getHealthRatio()), true);
		foeUIL->update(float(selfActors[0]->getHealthRatio()), selfNRG, float(foeActors[0]->getHealthRatio()), true);
		selfUIR->update(float(selfActors[0]->getHealthRatio()), selfNRG, float(foeActors[0]->getHealthRatio()), false);
		foeUIR->update(float(selfActors[0]->getHealthRatio()), selfNRG, float(foeActors[0]->getHealthRatio()), false);

		// Now check for inputs
		//cout << "Update hand" << endl;
		handSphere->setModel(translate(mat4(1.f), handPos) * scale(mat4(1.f), vec3(.2f, .2f, .2f)));
		leapSphere->setModel(translate(mat4(1.f), leapHandPos) * scale(mat4(1.f), vec3(.2f, .2f, .2f)));

		if (pickedUp) {
			pickedUp->setPosition(handPos.x, handPos.y, handPos.z);
		}

		// Check for any Touch input
		if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState))) {
			// A button
			if (inputState.Buttons & ovrButton_A) {
				if (!touchInputReceived) {
					cout << "A button pressed" << endl;
					touchInputReceived = true;
					// Check if we are currently holding an actor
					if (pickedUp) {
						if (pickedUp->getType() == a_Soldier || wasPickup) {
							pickedUp->setPosition(handPos.x, 0, handPos.z);
							pickedUp->togglePlacing();
							pickedUp->toggleActive();
							pickedUp = 0;
							wasPickup = false;
						}
						else {
							size_t actorsSize = selfActors.size();
							Actor *oldChoice = selfActors.at(actorsSize - 1);
							delete(oldChoice);
							pickedUp = new Soldier(soldierObj);
							pickedUp->setID(-objCount);
							pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
							selfActors[actorsSize - 1] = pickedUp;
						}
					}
					else {
						pickedUp = new Soldier(soldierObj);
						objCount++;
						pickedUp->setID(-objCount);
						pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
				}
			}
			// B button
			else if (inputState.Buttons & ovrButton_B) {
				if (!touchInputReceived) {
					cout << "B button pressed" << endl;
					touchInputReceived = true;
					// Check if we are currently holding an actor
					if (pickedUp) {
						if (pickedUp->getType() == a_Tank || wasPickup) {
							pickedUp->setPosition(handPos.x, 0, handPos.z);
							pickedUp->togglePlacing();
							pickedUp->toggleActive();
							pickedUp = 0;
							wasPickup = false;
						}
						else {
							size_t actorsSize = selfActors.size();
							Actor *oldChoice = selfActors.at(actorsSize - 1);
							delete(oldChoice);
							pickedUp = new Tank(tankObj);
							pickedUp->setID(-objCount);
							pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
							selfActors[actorsSize - 1] = pickedUp;
						}
					}
					else {
						pickedUp = new Tank(tankObj);
						objCount++;
						pickedUp->setID(-objCount);
						pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
				}
			}
			// Right thumbstick
			else if (inputState.Buttons & ovrButton_RThumb) {
				if (!touchInputReceived) {
					cout << "Thumb button pressed" << endl;
					touchInputReceived = true;
					// Check if we are currently holding an actor
					if (pickedUp) {
						pickedUp->setPosition(handPos.x, 0, handPos.z);
						pickedUp->togglePlacing();
						pickedUp->toggleActive();
						pickedUp = 0;
						wasPickup = false;
					}
					// No actor picked up, check if we selected one instead
					else {
						for (Actor *a : selfActors) {
							if (a->getType() == a_Tower) continue;
							if (length(a->getPosition() - handPos) < .5f) {
								pickedUp = a;
								break;
							}
						}
						if (!pickedUp) {
							for (Actor *a : foeActors) {
								if (a->getType() == a_Tower) continue;
								if (length(a->getPosition() - handPos) < .5f) {
									pickedUp = a;
									break;
								}
							}
						}
						if (pickedUp) {
							pickedUp->toggleActive();
							pickedUp->togglePlacing();
							wasPickup = true;
						}
					}
				}
			}
			// Right shoulder button
			else if (inputState.HandTrigger[ovrHand_Right] > .7f) {
				if (!touchInputReceived) {
					cout << "Shoulder button pressed" << endl;
					touchInputReceived = true;
					// Check if we are currently holding an actor
					if (pickedUp) {
						if (pickedUp->getType() == a_Cannon || wasPickup) {
							pickedUp->setPosition(handPos.x, 0, handPos.z);
							pickedUp->togglePlacing();
							pickedUp->toggleActive();
							pickedUp = 0;
							wasPickup = false;
						}
						else {
							size_t actorsSize = selfActors.size();
							Actor *oldChoice = selfActors.at(actorsSize - 1);
							delete(oldChoice);
							pickedUp = new Cannon(cannonObj);
							pickedUp->setID(-objCount);
							pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
							selfActors[actorsSize - 1] = pickedUp;
						}
					}
					else {
						pickedUp = new Cannon(cannonObj);
						objCount++;
						pickedUp->setID(-objCount);
						pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
				}
			}
			else if (inputState.IndexTrigger[ovrHand_Right] > .7f) {
				if (!touchInputReceived) {
					cout << "Trigger pressed" << endl;
					touchInputReceived = true;
					// Check if we are currently holding an actor
					if (pickedUp) {
						if (pickedUp->getType() == a_Wall || wasPickup) {
							pickedUp->setPosition(handPos.x, 0, handPos.z);
							pickedUp->togglePlacing();
							pickedUp->toggleActive();
							pickedUp = 0;
							wasPickup = false;
						}
						else {
							size_t actorsSize = selfActors.size();
							Actor *oldChoice = selfActors.at(actorsSize - 1);
							delete(oldChoice);
							pickedUp = new Wall(wallObj);
							pickedUp->setID(-objCount);
							pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
							selfActors[actorsSize - 1] = pickedUp;
						}
					}
					else {
						pickedUp = new Wall(wallObj);
						objCount++;
						pickedUp->setID(-objCount);
						pickedUp->setModel(rotate(mat4(1.f), PI, vec3(0, 1, 0)));
						selfActors.push_back(pickedUp);
					}
				}
			}
			else {
				touchInputReceived = false;
			}
		}
	}
}

void Project4::renderScene(const mat4& projection, const mat4& headPose, ovrEyeType eye) {
	glUseProgram(texShader);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "view"), 1, GL_FALSE, &(headPose[0][0]));
	ground->draw();

	glUseProgram(objShader);
	glUniform1i(glGetUniformLocation(objShader, "numLights"), numLights);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, &(headPose[0][0]));
	glUniform3f(glGetUniformLocation(objShader, "camPos"), headPose[3][0], headPose[3][1], headPose[3][2]);
	glUniform4fv(glGetUniformLocation(objShader, "lights"), numLights, lightPositions);
	glUniform3fv(glGetUniformLocation(objShader, "lightCols"), numLights, lightColors);

	for (Actor *a : selfActors)
		a->draw(objShader);
	for (Actor *b : foeActors)
		b->draw(objShader);
	handSphere->draw(objShader);
	leapSphere->draw(objShader);
}

void Project4::draw() {
	ovrPosef eyePoses[2];
	ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyeOffset, eyePoses, &_sceneLayer.SensorSampleTime);

	int curIndex;
	ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
	GLuint curTexId;
	ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto& vp = _sceneLayer.Viewport[ovrEye_Left];
	glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
	_sceneLayer.RenderPose[ovrEye_Left] = eyePoses[ovrEye_Left];
	renderScene(_eyeProjections[ovrEye_Left], ovr::toGlm(eyePoses[ovrEye_Left]), ovrEye_Left);

	const auto& vp2 = _sceneLayer.Viewport[ovrEye_Right];
	glViewport(vp2.Pos.x, vp2.Pos.y, vp2.Size.w, vp2.Size.h);
	_sceneLayer.RenderPose[ovrEye_Right] = eyePoses[ovrEye_Right];
	renderScene(_eyeProjections[ovrEye_Right], ovr::toGlm(eyePoses[ovrEye_Right]), ovrEye_Right);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ovr_CommitTextureSwapChain(_session, _eyeTexture);
	ovrLayerHeader* headerList = &_sceneLayer.Header;
	ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

	GLuint mirrorTextureId;
	ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
	glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	update(ovr::toGlm(eyePoses[ovrEye_Left]), ovr::toGlm(eyePoses[ovrEye_Right]));
}



// MAIN CALL TO PROJECT 3. DO NOT MODIFY UNDER THIS LINE.
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Allocate a console for debugging
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stdout);
	// Attempt to run the project
	int result = -1;
	try {
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		result = Project4().run();
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	ovr_Shutdown();
	return result;
}