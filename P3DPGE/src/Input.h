#pragma once

#include "internal/olcPixelGameEngine.h"
#include "Render.h"
#include "Collider.h" //TODO(i,delle) remove this



//NOTE update InputAction to support ALT when olcPGE does
struct InputAction {
	olc::Key key;
	int mouseButton;
	int inputState; //0 = bPressed, 1 = bHeld, 2 = bReleased
	bool bShiftHeld;
	bool bCtrlHeld;
	//bool bAltHeld;
	std::string name;
	std::string description;
	Action action;

	

	//function, name, key, mouseButton, inputState, shift, ctrl, desc
	InputAction(Action action, std::string name, olc::Key key = olc::Key::NONE, int mouseButton = -1, int inputState = 0,
		bool bShiftHeld = false, bool bCtrlHeld = false, /*bool bAltHeld = false,*/ std::string description = "") {
		this->key = key;
		this->mouseButton = mouseButton;
		this->inputState = inputState;
		this->bShiftHeld = bShiftHeld;
		this->bCtrlHeld = bCtrlHeld;
		//this->bAltHeld = bAltHeld;
		this->name = name;
		this->description = description;
		this->action = *action;
	}

	void CheckKeyboard(olc::PixelGameEngine* p) {
		if (inputState == 1 && p->GetKey(key).bHeld) {
			action(p);
		}
		else if (inputState == 2 && p->GetKey(key).bReleased) {
			action(p);
		}
		else if (p->GetKey(key).bPressed) {
			action(p);
		}
	}

	void CheckMouse(olc::PixelGameEngine* p) {
		if (inputState == 1 && p->GetMouse(mouseButton).bHeld) {
			action(p);
		}
		else if (inputState == 2 && p->GetMouse(mouseButton).bReleased) {

			action(p);
		}
		else if (inputState == 0 && p->GetMouse(mouseButton).bPressed) {
			action(p);
		}
	}

	void Update(olc::PixelGameEngine* p) {
		if (key != olc::NONE) {
			if (bShiftHeld) {
				if (p->GetKey(olc::SHIFT).bHeld) {
					CheckKeyboard(p);
				}
			}
			else if (bCtrlHeld) {
				if (p->GetKey(olc::CTRL).bHeld) {
					CheckKeyboard(p);
				}
				//} else if (bAltHeld) {
				//	if (p->GetKey(olc::ALT).bHeld) {
				//		CheckKeyboard(p);
				//	}
			}
			else if (!(p->GetKey(olc::SHIFT).bHeld || p->GetKey(olc::CTRL).bHeld)) {
				CheckKeyboard(p);
			}
		}
		else if (mouseButton != -1) {
			if (bShiftHeld) {
				if (p->GetKey(olc::SHIFT).bHeld) {
					CheckMouse(p);
				}
			}
			else if (bCtrlHeld) {
				if (p->GetKey(olc::CTRL).bHeld) {
					CheckMouse(p);
				}
				//} else if (bAltHeld) {
				//	if (p->GetKey(olc::ALT).bHeld) {
				//		CheckMouse(p);
				//	}
			}
			else if (!(p->GetKey(olc::SHIFT).bHeld || p->GetKey(olc::CTRL).bHeld)) {
				CheckMouse(p);
			}
		}
	};
};

namespace Input {
	global_variable std::vector<InputAction> inputActions;

	internal Entity* selectedEntity;
	internal UI* selectedUI;
	internal Triangle* selectedTriangle;
	internal Vector3 leftClickPos = V3NULL;
	internal bool DEBUG_INPUT = false;

	static bool drag_latch = false; //for dragging windows

	Timer* timer;

#define DEBUGI DEBUG if(DEBUG_INPUT)

	internal Vector3 GetMousePos(olc::PixelGameEngine* p) {
		return Vector3(p->GetMouseX(), p->GetMouseY());
	}

	internal Vector3 GetMousePosNormalized(olc::PixelGameEngine* p) {
		return Vector3((float)p->GetMouseX() / (float)p->ScreenWidth(), (float)p->GetMouseY() / (float)p->ScreenHeight());
	}

	internal Camera* c;

	static void Init() {
		c = &Scene::camera;

		timer = new Timer;

		inputActions = std::vector<InputAction>();
		//NOTE InputAction: function, name, key, mouseButton, inputState, shift, ctrl, description

	//// time control ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Time::deltaTime = 0;
			//TODO(i, sushi) set up pausing to also pause moving/rotating objects manually
			if (DEBUG_INPUT) std::cout << "Pausing the engine" << std::endl;
			}, "pause_game_held", olc::P, -1, 1, 0, 0,
			"Pauses the game while button is held."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Scene::paused = !Scene::paused;
			Physics::paused = !Physics::paused;
			DEBUGI{
				std::string output = (Scene::paused) ? "true" : "false";
				std::cout << "Toggling paused to " + output << std::endl;
			}
			}, "pause_game", olc::SPACE, -1, 0, 0, 0,
			"Pauses the game on press."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Scene::frame = !Scene::frame;
			Physics::frame = !Physics::frame;
			DEBUGI std::cout << "Advancing one frame forward" << std::endl;
			}, "next_frame", olc::F, -1, 0, 0, 0,
			"Advances to the next frame if paused."));

		//// object spawning ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(10, pos);
			sphere->mass = 10;
			sphere->mesh = new CircleMesh(10);
			Physics::AddEntity(sphere);
			Scene::AddEntity(sphere);
			DEBUGI std::cout << "Creating Sphere at: " + pos.str() << std::endl;
			}, "spawn_sphere", olc::N, -1, 0, 0, 0,
			"Spawns a sphere of radius/mass 10 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(100, pos);
			sphere->mass = 100;
			sphere->mesh = new CircleMesh(100);
			Physics::AddEntity(sphere);
			Scene::AddEntity(sphere);
			DEBUGI std::cout << "Creating Large Sphere at: " + pos.str() << std::endl;
			}, "spawn_sphere_large", olc::N, -1, 0, 1, 0,
			"Spawns a large sphere of radius/mass 100 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/bmonkey.obj", Vector3(0, 0, 3));
			selectedEntity = complex;
			Physics::AddEntity(complex);
			Scene::AddEntity(complex);
			DEBUGI std::cout << "Creating " + complex->model_name + " at: " + V3ZERO.str() << std::endl;
			}, "spawn_complex", olc::T, -1, 0, 0, 0,
			"Spawns a complex object at (0,0,3)"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = Vector3(0, 0, 3);
			Box* box = new Box(Vector3(1, 1, 1), pos);
			selectedEntity = box;
			Physics::AddEntity(box);
			Scene::AddEntity(box);
			DEBUGI std::cout << "Creating Box at: " + pos.str() << std::endl;
			}, "spawn_box", olc::B, -1, 0, 0, 0,
			"Spawns a box at (0,0,3)"));

		//// object movement ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddInput(V3FORWARD);
				DEBUGI LOG("Adding force ", V3FORWARD, " to selected object.");
			}
			}, "phys_force_add_+z", olc::I, -1, 1, 0, 0,
			"Add force on selected object in z direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddInput(V3BACKWARD);
				DEBUGI LOG("Adding force ", V3BACKWARD, " to selected object.");
			}
			}, "phys_force_add_-z", olc::K, -1, 1, 0, 0,
			"Add force on selected object in negative z direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddInput(V3RIGHT);
				DEBUGI LOG("Adding force ", V3RIGHT, " to selected object.");
			}
			}, "phys_force_add_+x", olc::L, -1, 1, 0, 0,
			"Add force on selected object in x direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddInput(V3LEFT);
				DEBUGI LOG("Adding force ", V3LEFT, " to selected object.");
			}
			}, "phys_force_add_-x", olc::J, -1, 1, 0, 0,
			"Add force on selected object in negative x direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddInput(V3UP);
				DEBUGI LOG("Adding force ", V3UP, " to selected object.");
			}
			}, "phys_force_add_+y", olc::O, -1, 1, 0, 0,
			"Add force on selected object in y direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddInput(V3DOWN);
				DEBUGI LOG("Adding force ", V3DOWN, " to selected object.");
			}
			}, "phys_force_add_-y", olc::U, -1, 1, 0, 0,
			"Add force on selected object in negative y direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->rotAcceleration.x += 10 * g_fixedDeltaTime;
				DEBUGI LOG("Adding rotational acceleration over +x to selected object.");
			}
			}, "rot_add_+x", olc::K, -1, 1, 1, 0,
			"Add force on selected object in negative y direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->rotAcceleration.x -= 10 * g_fixedDeltaTime;
				DEBUGI LOG("Adding rotational acceleration over -x to selected object.");
			}
			}, "rot_add-+x", olc::H, -1, 1, 1, 0,
			"Add force on selected object in negative y direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Scene::entities.clear();
			}, "clear_entities", olc::C, -1, 1, 1, 0,
			"Delete all entities"));

	//// object selection ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			leftClickPos = GetMousePos(p);
			
			//check ui over entities
			
			bool ui_clicked = false;
			for (UI* ui : Scene::ui_layer) {
				int index = 0;
				if (Menu* m = dynamic_cast<Menu*>(ui)) {
					if (m->Clicked(pos.toVector2(), p)) {
						if (m->ClickedInTitle(pos.toVector2())) {
							selectedUI = m;
							break;
						}
						ui_clicked = true;
					}
					index++;
				}
				if (Button* b = dynamic_cast<Button*>(ui)) {
					if (b->Clicked(pos.toVector2(), p)) {
						ui_clicked = true;
						break;
					}
				}
			}

			if (!ui_clicked) {
				if (selectedEntity) { selectedEntity->ENTITY_DEBUG = false; selectedEntity = nullptr; }
				pos.ScreenToWorld(c->ProjectionMatrix(p), c->MakeViewMatrix(Scene::yaw), p);
				pos.WorldToLocal(c->position);
				pos.normalize();
				pos *= 1000;
				pos.LocalToWorld(c->position);

				//make function somehere that returns a ray cast for ease of use later maybe
				Line3* ray = new Line3(pos, c->position);

				//draw ray if debugging
				DEBUGI Scene::entities.push_back(ray);

				for (Entity* e : Scene::entities) {
					if (e->LineIntersect(&ray->edge)) {
						selectedEntity = e;
						selectedEntity->ENTITY_DEBUG = true;
						break;
					}
				}

				if (selectedEntity == nullptr) { ERROR("No object selected"); }
			}
			
			}, "select_entity", olc::NONE, 0, 0, 0, 0,
			"Selects an entity or ui"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			
			static Vector2 offset;


			if (selectedUI) {
				if (!drag_latch) {
					offset = selectedUI->pos - pos.toVector2();
					drag_latch = true;
				}
				selectedUI->pos = pos.toVector2() + offset;
			}
			else {
				p->DrawLine(leftClickPos.x, leftClickPos.y, p->GetMouseX(), p->GetMouseY(), olc::WHITE);

			}
			//implement entity stuff later

			}, "drag_entity", olc::NONE, 0, 1, 0, 0,
			"Drags an entity or ui"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (selectedUI) {
				selectedUI = nullptr;
				drag_latch = false;
			}
			else if(selectedEntity){
				if (PhysEntity* entity = dynamic_cast<PhysEntity*>(selectedEntity)) {
					Vector3 pos = GetMousePos(p);

					pos.ScreenToWorld(c->ProjectionMatrix(p), c->MakeViewMatrix(Scene::yaw), p);
					leftClickPos.ScreenToWorld(c->ProjectionMatrix(p), c->MakeViewMatrix(Scene::yaw), p);

					entity->AddForce(nullptr, (pos - leftClickPos).normalized() * 5, true);
				}
				leftClickPos = V3NULL;
			}
			}, "stop_drag_entity", olc::NONE, 0, 2, 0, 0,
			""));

		//// camera movement ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position.y -= 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the positive y" << std::endl;
			}, "camera_translate_+y", olc::E, -1, 1, 0, 0,
			"Translates the camera along the positive global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position.y += 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the negative y" << std::endl;
			}, "camera_translate_-y", olc::Q, -1, 1, 0, 0,
			"Translates the camera along the negative global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position += c->lookDir.cross(c->up) * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the negative x" << std::endl;
			}, "camera_translate_left", olc::A, -1, 1, 0, 0,
			"Translates the camera left"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position -= c->lookDir.cross(c->up) * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the positive x" << std::endl;
			}, "camera_translate_right", olc::D, -1, 1, 0, 0,
			"Translates the camera right"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position += c->lookDir * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera forward" << std::endl;
			}, "camera_translate_forward", olc::W, -1, 1, 0, 0,
			"Translates the camera forwards"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position -= c->lookDir * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera backward" << std::endl;
			}, "camera_translate_backward", olc::S, -1, 1, 0, 0,
			"Translates the camera backwards"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Scene::yaw -= 50 * Time::deltaTime;
			DEBUGI std::cout << "Turning the camera right" << std::endl;
			}, "camera_turn_right", olc::RIGHT, -1, 1, 0, 0,
			"Rotates the camera along its local y-axis (yaw) in the positive direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Scene::yaw += 50 * Time::deltaTime;
			DEBUGI std::cout << "Turning the camera left" << std::endl;
			}, "camera_turn_left", olc::LEFT, -1, 1, 0, 0,
			"Rotates the camera along its local y-axis (yaw) in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position = V3ZERO;
			Scene::yaw = 0;
			DEBUGI std::cout << "Resetting camera to pos: (0,0,0) and yaw: 0" << std::endl;
			}, "camera_reset", olc::Z, -1, 0, 0, 0,
			"Resets the camera to position: (0,0,0) and yaw: 0"));

		//// render options ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::WIRE_FRAME = !Render::WIRE_FRAME;
			std::string output = (Render::WIRE_FRAME) ? "true" : "false";
			DEBUGI std::cout << "Toggling wireframe to: " + output << std::endl;
			}, "TOGGLE_WIRE_FRAME", olc::C, -1, 0, 0, 0,
			"Toggles whether the wireframe of objects should be rendered"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::DISP_EDGES = !Render::DISP_EDGES;
			std::string output = (Render::DISP_EDGES) ? "true" : "false";
			DEBUGI std::cout << "Toggling display edge numbers to: " + output << std::endl;
			}, "TOGGLE_DISP_EDGES", olc::V, -1, 0, 0, 0,
			"Toggles whether the edges of objects are numbered"));

		//// temp debugging ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Box* box = new Box(Vector3(5, 5, 5), Vector3(0,0,20));
			box->mass = 100;
			AABBCollider* aabb = new AABBCollider(box, box->halfDims);
			Box* sphere = new Box(Vector3(.3f, .3f, .3f), Vector3(20, 0, 20), V3ZERO, V3ONE, Vector3(-25, -3, 0), V3ZERO, V3ONE);
			SphereCollider* sphereCol = new SphereCollider(sphere, 1);
			Physics::AddEntity(box);
			Scene::AddEntity(box);
			Physics::AddEntity(sphere);
			Scene::AddEntity(sphere);
		}, "test_colliders", olc::F1, -1, 0, 1, 0,
		"n/a"));


		//// input management ////
			//loop through all the input actions and unbind duplicate binds
		for (InputAction& action : inputActions) {
			for (InputAction& action2 : inputActions) {
				if (&action != &action2 && action.key != olc::NONE) {
					if (action.key == action2.key && action.bCtrlHeld == action2.bCtrlHeld && action.bShiftHeld == action2.bShiftHeld) {
						ERROR("Actions " + action.name + " and " + action2.name + " were both bound to " + std::to_string(action.key) + "\n\tBoth have been unbound because of this");
						action.key = olc::NONE; action2.key = olc::NONE;
					}
				}
			}
		}
	}

	static void Update(olc::PixelGameEngine* p, float& deltaTimePtr) {
		for (InputAction action : inputActions) {
			action.Update(p);
		}


		//RMB hold = set the position of selected entity to mouse
		if (selectedEntity && p->GetMouse(1).bHeld) {
			//do nothing for now

			//Vector3 pos = GetMousePos(p);
			//pos.ScreenToWorld(c->ProjectionMatrix(p), c->MakeViewMatrix(Scene::yaw), p);
			//pos.WorldToLocal(c->position);
			//pos.normalize();
			//pos *= 1000;
			//pos.LocalToWorld(c->position);
			//
			//selectedEntity->position.x = pos.x;
			//selectedEntity->position.y = pos.y;
		}
	}

	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};