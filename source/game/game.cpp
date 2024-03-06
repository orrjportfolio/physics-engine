#include "game.hpp"

#include <iostream>

#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "entity/entity.hpp"
#include "entity/octree.hpp"
#include "gfx/gfx.hpp"
#include "gfx/scene3d.hpp"
#include "imgui/imgui.h"
#include "map.hpp"

namespace Game {
	static Tex
		melonTex,
		tikiTex,
		scorpionPickupTex;
	
	static auto white = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(1.0f)
	};
	static auto grey = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.5f)
	};
	static auto purple = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.85f, 0.7f, 1.0f)
	};
	static auto green = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.7f, 1.0f, 0.7f)
	};
	static auto melonMaterial = Material{
		.kind = Material::KIND_LIT,
		.tex = &melonTex,
		.colour = glm::vec3(1.0f)
	};
	static auto tikiMaterial = Material{
		.kind = Material::KIND_LIT,
		.tex = &tikiTex,
		.colour = glm::vec3(1.0f)
	};
	static auto scorpionPickupMaterial = Material{
		.kind = Material::KIND_LIT,
		.tex = &scorpionPickupTex,
		.colour = glm::vec3(1.0f)
	};
	
	static Mesh3d
		sphereMesh,
		cubeMesh,
		melonMesh,
		tikiMesh,
		scorpionPickupMesh;
	
	static Entity car;
	
	static int ammo = 0;
	
	constexpr glm::vec3 carStartPos = glm::vec3(204.971f, 5.0f, -27.3629f);
	
	void resetObjects() {
		struct DynObject {
			glm::vec3 pos;
			float rot;
			glm::vec3 scale;
		};
		
		Entity::destroyAll();
		
		DynObject melons[] = {
			DynObject{.pos = glm::vec3(195.56199645996094, 2.5587499141693115, 16.896799087524414), .rot = 1.8073756694793701},
			DynObject{.pos = glm::vec3(193.63999938964844, 2.5587499141693115, 19.457300186157227), .rot = 2.884610414505005},
			DynObject{.pos = glm::vec3(191.99234008789062, 2.5587542057037354, 15.373064041137695), .rot = 0.9151316285133362},
			DynObject{.pos = glm::vec3(215.9249725341797, 2.5587539672851562, 16.462865829467773), .rot = 0.6750636100769043},
			DynObject{.pos = glm::vec3(213.1174774169922, 2.558755874633789, 20.533123016357422), .rot = 2.2457358837127686},
			DynObject{.pos = glm::vec3(219.2461700439453, 2.5587539672851562, 18.645273208618164), .rot = 1.338787317276001},
			DynObject{.pos = glm::vec3(61.20112609863281, 2.5587544441223145, -88.84671020507812), .rot = 1.807375431060791},
			DynObject{.pos = glm::vec3(57.89752960205078, 2.5587544441223145, -82.83638000488281), .rot = 3.2678909301757812},
			DynObject{.pos = glm::vec3(81.68724060058594, 2.8225958347320557, -70.3031234741211), .rot = 5.408358097076416},
			DynObject{.pos = glm::vec3(172.3517608642578, 2.5587549209594727, -44.119956970214844), .rot = 2.005850076675415},
			DynObject{.pos = glm::vec3(180.33810424804688, 2.5587549209594727, -42.0538215637207), .rot = 4.99378776550293},
			DynObject{.pos = glm::vec3(183.0315704345703, 2.5587549209594727, -39.30659103393555), .rot = 3.9174020290374756},
			DynObject{.pos = glm::vec3(212.13287353515625, 2.5587549209594727, -56.15932083129883), .rot = 2.005850076675415},
			DynObject{.pos = glm::vec3(152.7491455078125, 2.5587549209594727, -87.91552734375), .rot = 3.8358020782470703},
			DynObject{.pos = glm::vec3(159.75875854492188, 2.5587549209594727, -88.402587890625), .rot = 1.049845576286316},
			DynObject{.pos = glm::vec3(125.81708526611328, 2.5587539672851562, -73.73522186279297), .rot = 3.2153127193450928},
			DynObject{.pos = glm::vec3(92.82307434082031, 2.5587539672851562, -105.46566772460938), .rot = 3.3485958576202393},
			DynObject{.pos = glm::vec3(104.90440368652344, -5.412690162658691, 0.6267492175102234), .rot = 2.005850076675415},
			DynObject{.pos = glm::vec3(92.03630828857422, -5.412690162658691, -19.602306365966797), .rot = 2.005850076675415},
			DynObject{.pos = glm::vec3(81.92384338378906, -5.412690162658691, 0.5278053283691406), .rot = 2.005850076675415},
			DynObject{.pos = glm::vec3(85.0608901977539, -5.412690162658691, 0.72517991065979), .rot = 2.005850076675415},
			DynObject{.pos = glm::vec3(133.7743682861328, 2.5587549209594727, -85.63771057128906), .rot = 2.8928747177124023},
			DynObject{.pos = glm::vec3(71.89749145507812, 2.8931689262390137, -9.977951049804688), .rot = 4.266375541687012},
			DynObject{.pos = glm::vec3(69.41834259033203, 2.8859164714813232, -6.824255466461182), .rot = 4.266375541687012},
			DynObject{.pos = glm::vec3(68.09882354736328, 2.8931689262390137, -11.192522048950195), .rot = 4.266375541687012},
			DynObject{.pos = glm::vec3(65.03960418701172, 2.8859164714813232, -6.768028736114502), .rot = 4.266375541687012},
			DynObject{.pos = glm::vec3(63.38663101196289, 2.8931689262390137, -10.392108917236328), .rot = 4.266375541687012}
		};
		
		DynObject tikis[] = {
			DynObject{.pos = glm::vec3(157.3822784423828, 2.3107011318206787, -64.76368713378906), .rot = -0.3916512131690979},
			DynObject{.pos = glm::vec3(158.59002685546875, 2.3107001781463623, -66.96183013916016), .rot = -0.7454231977462769},
			DynObject{.pos = glm::vec3(127.15621948242188, 2.3107006549835205, -86.6084213256836), .rot = -0.19565707445144653},
			DynObject{.pos = glm::vec3(128.58604431152344, 2.3107006549835205, -89.24311828613281), .rot = -0.5028544068336487},
			DynObject{.pos = glm::vec3(81.10911560058594, -6.253407955169678, -3.3783116340637207), .rot = 2.6897647380828857},
			DynObject{.pos = glm::vec3(78.46258544921875, -6.253407955169678, -2.18179988861084), .rot = 2.6897647380828857},
			DynObject{.pos = glm::vec3(78.83413696289062, -4.253407955169678, 0.6530666351318359), .rot = 2.6897647380828857},
			DynObject{.pos = glm::vec3(88.89607238769531, -6.253407955169678, -20.77820587158203), .rot = 3.3386998176574707},
			DynObject{.pos = glm::vec3(90.71540069580078, -4.253408908843994, -22.749988555908203), .rot = 3.3386998176574707},
			DynObject{.pos = glm::vec3(104.13651275634766, -5.404483318328857, -2.3615455627441406), .rot = 2.498479127883911},
			DynObject{.pos = glm::vec3(102.71137237548828, -5.404483318328857, -0.605941116809845), .rot = 2.498479127883911},
			DynObject{.pos = glm::vec3(103.53640747070312, -3.4044830799102783, 2.4146857261657715), .rot = 2.498479127883911},
			DynObject{.pos = glm::vec3(77.57347106933594, 2.3107006549835205, -91.65721130371094), .rot = 0.6375841498374939},
			DynObject{.pos = glm::vec3(74.44206237792969, 2.3107001781463623, -76.2093276977539), .rot = 1.5264313220977783}
		};
		
		DynObject blocks[] = {
			DynObject{.pos = glm::vec3(97.64014434814453, -5.401905059814453, -13.047822952270508), .scale = glm::vec3(5.372400283813477, 0.9997929334640503, 2.0)},
			DynObject{.pos = glm::vec3(98.10885620117188, -5.401905059814453, -10.557787895202637), .scale = glm::vec3(2.7599456310272217, 2.8986408710479736, 1.225079894065857)},
			DynObject{.pos = glm::vec3(101.3646469116211, -5.401905059814453, -10.340733528137207), .scale = glm::vec3(0.9533631205558777, 0.9533630609512329, 0.9533630609512329)},
			DynObject{.pos = glm::vec3(98.69625091552734, -2.5392417907714844, -11.110014915466309), .scale = glm::vec3(2.0, 2.0, 2.0)},
			DynObject{.pos = glm::vec3(148.35694885253906, 4.881471633911133, 46.733734130859375), .scale = glm::vec3(2.781856060028076, 4.9436516761779785, 1.4799209833145142)},
			DynObject{.pos = glm::vec3(122.51792907714844, 3.5918595790863037, 55.81604766845703), .scale = glm::vec3(3.198075294494629, 1.5938507318496704, 5.825438499450684)}
		};
		
		glm::vec3 scorpionPickups[] = {
			glm::vec3(204.17f, 3.3143f, 20.632f)
		};
		
		for (auto o : melons) {
			auto e = Entity::create(
				o.pos,
				glm::rotate(o.rot, glm::vec3(0.0f, 1.0f, 0.0f))
			);
			e.makeDynamic(ColliderShape::sphere(1.315f), PhysicsMaterial{
				.sFrict = 0.2f,
				.dFrict = 0.15f,
				.bounciness = 0.8f
			}, 0.04f);
			e.addMesh(melonMesh, melonMaterial);
		}
		
		for (auto o : tikis) {
			auto e = Entity::create(
				o.pos,
				glm::rotate(o.rot, glm::vec3(0.0f, 1.0f, 0.0f))
			);
			e.makeDynamic(ColliderShape::box(glm::vec3(1.0f)), PhysicsMaterial{
				.sFrict = 0.8f,
				.dFrict = 0.6f,
				.bounciness = 0.2f
			}, 0.07f);
			e.addMesh(tikiMesh, tikiMaterial);
		}
		
		for (auto o : blocks) {
			auto e = Entity::create(o.pos);
			e.makeDynamic(ColliderShape::axisAlignedBox(o.scale / 2.0f), PhysicsMaterial{
				.sFrict = 1.5f,
				.dFrict = 1.2f,
				.bounciness = 0.0f
			}, 0.2f);
			e.addMesh(cubeMesh, green, glm::scale(o.scale / 2.0f));
		}
		
		for (auto o : Map::objects) {
			auto e = Entity::create(
				o.pos,
				glm::orthonormalize(glm::mat3(
					glm::rotate(o.rot.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::rotate(o.rot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::rotate(o.rot.x, glm::vec3(1.0f, 0.0f, 0.0f))
				))
			);
			if (o.colliderKind == COLLIDER_KIND_KINEMATIC) {
				e.makeKinematic(
					ColliderShape::box(o.halfSize),
					o.physMaterial
				);
			}
			else if (o.colliderKind == COLLIDER_KIND_DYNAMIC) {
				e.makeDynamic(
					ColliderShape::box(o.halfSize),
					o.physMaterial,
					o.density
				);
			}
			e.addMesh(cubeMesh, *o.material, glm::scale(o.halfSize));
		}
		
		for (auto o : scorpionPickups) {
			auto e = Entity::create(o);
			e.addMesh(scorpionPickupMesh, scorpionPickupMaterial);
		}
		
		car = Entity::create(carStartPos, glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		car.makeDynamic(
			ColliderShape::box(glm::vec3(0.75f, 0.5f, 1.0f)),
			PhysicsMaterial{.sFrict = 0.3f, .dFrict = 0.15f, .bounciness = 0.5f},
			1.0f
		);
		car.addMesh(cubeMesh, purple, glm::scale(glm::vec3(0.75f, 0.5f, 1.0f)));
		car.setUpdateFunc([](Entity e, float dt) {
			auto keysHeld = SDL_GetKeyboardState(nullptr);
			
			static bool prevSpaceHeld;
			auto spaceHeld = keysHeld[SDL_SCANCODE_SPACE];
			if (spaceHeld && !prevSpaceHeld) {
				e.setPos(e.pos() + glm::vec3(0.0f, 0.05f, 0.0f));
				e.addForce(glm::vec3(0.0f, 2500.0f, 0.0f));
			}
			prevSpaceHeld = spaceHeld;
			
			if (keysHeld[SDL_SCANCODE_W]) {
				e.addForce(e.forward() * 40.0f);
			}
			if (keysHeld[SDL_SCANCODE_S]) {
				e.addForce(e.forward() * -40.0f);
			}
			if (keysHeld[SDL_SCANCODE_A]) {
				e.addForceAt(
					e.right() * 10.0f,
					e.pos() + (e.forward() * 0.7f) + (e.right() * -0.5f)
				);
				e.addForceAt(
					e.right() * -10.0f,
					e.pos() + (e.forward() * -0.7f) + (e.right() * 0.5f)
				);
			}
			if (keysHeld[SDL_SCANCODE_D]) {
				e.addForceAt(
					e.right() * -10.0f,
					e.pos() + (e.forward() * 0.7f) + (e.right() * 0.5f)
				);
				e.addForceAt(
					e.right() * 10.0f,
					e.pos() + (e.forward() * -0.7f) + (e.right() * -0.5f)
				);
			}
			
			if (e.pos().y < -50.0f) {
				e.setPos(carStartPos);
				e.setVel(glm::vec3(0.0f));
				e.setRot(glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
				e.setRotVel(glm::vec3(0.0f));
				Entity::rotMoms[e.idx] = glm::vec3(0.0f);
			}
		});
		
		auto updraft = Entity::create(glm::vec3(133.89f, 0.0f, 9.0864f), glm::rotate(glm::radians(-30.981f), glm::vec3(0.0f, 1.0f, 0.0f)));
		updraft.makeTrigger(ColliderShape::box(glm::vec3(23.6f, 43.2f, 28.2f) / 2.0f));
		updraft.setUpdateFunc([](Entity e, float dt) {
			if (rand() / (float)RAND_MAX <= 0.05f) {
				auto particle = Entity::create(e.pos() + glm::vec3(
					(rand() / (float)RAND_MAX) * 16.0f - 8.0f,
					(rand() / (float)RAND_MAX) * 16.0f - 8.0f,
					(rand() / (float)RAND_MAX) * 16.0f - 8.0f
				));
				particle.addMesh(sphereMesh, white);
				particle.makeDynamic(ColliderShape::sphere(0.1f), PhysicsMaterial{
					.sFrict = 0.0f,
					.dFrict = 0.0f,
					.bounciness = 0.5f
				}, 20.0f);
				particle.setUpdateFunc([](Entity e, float dt) {
					if (rand() / (float)RAND_MAX <= 0.01f) { e.destroy(); }
				});
			}
		});
		updraft.setTriggerFunc([](Entity e, Entity other) {
			other.addForce(glm::vec3(0.0f, 80.0f, 0.0f));
		});
		//updraft.addMesh(cubeMesh, purple, glm::scale(glm::vec3(23.6f, 43.2f, 28.2f) / 2.0f));
	}
	
	void init() {
		melonTex = Tex::load("assets/textures/melon.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		tikiTex = Tex::load("assets/textures/tiki.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		scorpionPickupTex = Tex::load("assets/textures/scorpionPickup.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		
		sphereMesh = Mesh3d::load("assets/models/sphere.obj");
		cubeMesh = Mesh3d::load("assets/models/cube.obj");
		melonMesh = Mesh3d::load("assets/models/melon.obj");
		tikiMesh = Mesh3d::load("assets/models/tiki.obj");
		scorpionPickupMesh = Mesh3d::load("assets/models/scorpionPickup.obj");
		
		Map::init();
		
		resetObjects();
	}
	
	void update(float dt) {
		glm::vec3 f = car.forward();
		
		glm::vec3 cpos = car.pos() - (car.forward() * 10.0f) + glm::vec3(0.0f, 5.0f, 0.0f);
		Scene3d::cam.viewMat =
			glm::lookAt(cpos, car.pos(), glm::vec3(0.0f, 1.0f, 0.0f));
		
		auto keysHeld = SDL_GetKeyboardState(nullptr);
		
		static bool prevRHeld;
		bool rHeld = keysHeld[SDL_SCANCODE_R];
		if (rHeld && !prevRHeld) {
			resetObjects();
		}
		prevRHeld = rHeld;
		
		ImGui::Begin("Status"); {
			ImGui::Text("Ammunition: %d", ammo);
			
			ImGui::End();
		}
	}
}
