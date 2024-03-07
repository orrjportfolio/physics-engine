#include "game.hpp"

#include <iostream>
#include <vector>

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
		scorpionPickupTex,
		scorpionTex,
		bannerTex;
	
	static auto white = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(1.0f)
	};
	static auto grey = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.5f)
	};
	static auto red = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(1.0f, 0.7f, 0.7f)
	};
	static auto blue = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.7f, 0.8f, 1.0f)
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
	static auto scorpionMaterial = Material{
		.kind = Material::KIND_LIT,
		.tex = &scorpionTex,
		.colour = glm::vec3(1.0f)
	};
	static auto bannerMaterial = Material{
		.kind = Material::KIND_LIT,
		.tex = &bannerTex,
		.colour = glm::vec3(1.0f)
	};
	
	static Mesh3d
		sphereMesh,
		cubeMesh,
		melonMesh,
		tikiMesh,
		scorpionPickupMesh,
		bannerMesh;
	
	static int carType;
	
	static Entity car;
	
	static int ammo = 0;
	
	static double timer = 0.0;
	
	static std::vector<double> lapTimes;
	static std::vector<int> lapCarTypes;
	
	static int checkpoint = 0;
	
	constexpr glm::vec3 carStartPos = glm::vec3(204.971f, 5.0f, 39.4092f);
	
	static glm::vec3 cpos;
	
	void resetObjects() {
		ammo = 0;
		
		timer = 0.0;
		
		checkpoint = 0;
		
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
			glm::vec3(201.52859497070312, 2.3498709201812744, 13.038581848144531),
			glm::vec3(208.36416625976562, 2.3498709201812744, 13.116134643554688),
			glm::vec3(74.68993377685547, 2.3498709201812744, -66.42782592773438),
			glm::vec3(66.77835083007812, 2.3498709201812744, -66.26666259765625),
			glm::vec3(92.79541015625, -5.385968208312988, 7.681734561920166),
			glm::vec3(87.00019073486328, -6.193933963775635, -5.564462184906006),
			glm::vec3(104.34880065917969, -5.385968208312988, -15.893692016601562),
			glm::vec3(117.0617904663086, -5.385968208312988, -11.665468215942383),
			glm::vec3(99.16342163085938, -6.1848554611206055, -28.1428165435791),
			glm::vec3(122.60000610351562, -5.385968208312988, -27.69451332092285)
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
			if (!o.hidden) {
				e.addMesh(cubeMesh, *o.material, glm::scale(o.halfSize));
			}
		}
		
		for (auto o : scorpionPickups) {
			auto e = Entity::create(o);
			e.addMesh(scorpionPickupMesh, scorpionPickupMaterial);
			e.makeTrigger(ColliderShape::sphere(1.0f));
			e.setUpdateFunc([](Entity e, float dt) {
				e.setRot(glm::mat3(glm::rotate(dt * 7.0f, glm::vec3(0.0f, 1.0f, 0.0f))) * e.rot());
			});
			e.setTriggerFunc([](Entity e, Entity other) {
				if (other.idx == car.idx && other.gen == car.gen) {
					e.destroy();
					ammo += 10;
				}
			});
		}
		
		auto banner = Entity::create(glm::vec3(203.2f, 7.8011f, 26.004f));
		banner.addMesh(bannerMesh, bannerMaterial);
		
		car = Entity::create(carStartPos, glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		if (carType == 0) {
			car.makeDynamic(
				ColliderShape::box(glm::vec3(0.75f, 0.5f, 1.0f)),
				PhysicsMaterial{.sFrict = 0.22f, .dFrict = 0.15f, .bounciness = 0.5f},
				1.0f
			);
			car.addMesh(cubeMesh, purple, glm::scale(glm::vec3(0.75f, 0.5f, 1.0f)));
		}
		else if (carType == 1) {
			car.makeDynamic(
				ColliderShape::box(glm::vec3(1.0f, 0.8f, 1.4f)),
				PhysicsMaterial{.sFrict = 0.4f, .dFrict = 0.3f, .bounciness = 0.2f},
				1.0f
			);
			car.addMesh(cubeMesh, grey, glm::scale(glm::vec3(1.0f, 0.8f, 1.4f)));
		}
		else if (carType == 2) {
			car.makeDynamic(
				ColliderShape::box(glm::vec3(0.375f, 0.25f, 0.5f)),
				PhysicsMaterial{.sFrict = 0.2f, .dFrict = 0.12f, .bounciness = 0.3f},
				1.0f
			);
			car.addMesh(cubeMesh, blue, glm::scale(glm::vec3(0.375f, 0.25f, 0.5f)));
		}
		else if (carType == 3) {
			car.makeDynamic(
				ColliderShape::sphere(0.7f),
				PhysicsMaterial{.sFrict = 0.3f, .dFrict = 0.15f, .bounciness = 0.7f},
				1.0f
			);
			car.addMesh(sphereMesh, red, glm::scale(glm::vec3(0.7f)));
		}
		car.setUpdateFunc([](Entity e, float dt) {
			float thrust =
				(carType == 1)? 100.0f :
				(carType == 2)? 20.0f :
				40.0f;
			float steer =
				(carType == 1)? 30.0f :
				(carType == 2)? 0.5f :
				10.0f;
			float jump =
				(carType == 1)? 6000.0f :
				(carType == 2)? 500.0f :
				2500.0f;
			
			auto keysHeld = SDL_GetKeyboardState(nullptr);
			
			static bool prevSpaceHeld;
			auto spaceHeld = keysHeld[SDL_SCANCODE_SPACE];
			if (spaceHeld && !prevSpaceHeld && glm::abs(e.vel().y) <= 0.5f) {
				e.setPos(e.pos() + glm::vec3(0.0f, 0.05f, 0.0f));
				e.addForce(glm::vec3(0.0f, jump, 0.0f));
			}
			prevSpaceHeld = spaceHeld;
			
			static bool prevEHeld;
			auto eHeld = keysHeld[SDL_SCANCODE_E];
			if (eHeld && !prevEHeld) {
				if (ammo > 0) {
					auto s = Entity::create(e.pos() + e.forward() * 2.5f);
					s.makeDynamic(ColliderShape::sphere(1.0f), PhysicsMaterial{
						.sFrict = 0.2f,
						.dFrict = 0.15f,
						.bounciness = 0.1f
					}, 0.1f);
					s.addForce(e.forward() * 4000.0f);
					s.addMesh(scorpionPickupMesh, scorpionMaterial);
					s.setUpdateFunc([](Entity e, float dt) {
						if (Entity::lifetimes[e.idx] >= 3.0f) { e.destroy(); }
					});
					
					ammo--;
				}
			}
			prevEHeld = eHeld;
			
			if (keysHeld[SDL_SCANCODE_W]) {
				
				e.addForce(e.forward() * thrust);
			}
			if (keysHeld[SDL_SCANCODE_S]) {
				e.addForce(e.forward() * -thrust);
			}
			if ((keysHeld[SDL_SCANCODE_A] && e.up().y > 0.0f) || (keysHeld[SDL_SCANCODE_D] && e.up().y < 0.0f)) {
				e.addForceAt(
					e.right() * steer,
					e.pos() + (e.forward() * 0.7f) + (e.right() * -0.5f)
				);
				e.addForceAt(
					e.right() * -steer,
					e.pos() + (e.forward() * -0.7f) + (e.right() * 0.5f)
				);
			}
			if ((keysHeld[SDL_SCANCODE_D] && e.up().y > 0.0f) || (keysHeld[SDL_SCANCODE_A] && e.up().y < 0.0f)) {
				e.addForceAt(
					e.right() * -steer,
					e.pos() + (e.forward() * 0.7f) + (e.right() * 0.5f)
				);
				e.addForceAt(
					e.right() * steer,
					e.pos() + (e.forward() * -0.7f) + (e.right() * -0.5f)
				);
			}
			
			if (e.pos().y < -50.0f) {
				/*e.setPos(carStartPos);
				e.setVel(glm::vec3(0.0f));
				e.setRot(glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
				e.setRotVel(glm::vec3(0.0f));
				Entity::rotMoms[e.idx] = glm::vec3(0.0f);
				checkpoint = 0;*/
				
				resetObjects();
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
		
		auto cp1 = Entity::create(glm::vec3(70.937f, 11.703f, -60.084f));
		cp1.makeTrigger(ColliderShape::axisAlignedBox(glm::vec3(23.9f, 23.9f, 2.0f) / 2.0f));
		cp1.setTriggerFunc([](Entity e, Entity other) {
			if (other.idx == car.idx && other.gen == car.gen) {
				checkpoint = 1;
			}
		});
		
		auto cp2 = Entity::create(glm::vec3(74.378f, -7.7027f, -14.718f));
		cp2.makeTrigger(ColliderShape::axisAlignedBox(glm::vec3(6.31f, 13.1f, 43.2f) / 2.0f));
		cp2.setTriggerFunc([](Entity e, Entity other) {
			if (other.idx == car.idx && other.gen == car.gen && (checkpoint >= 1)) {
				checkpoint = 2;
			}
		});
		
		auto cp3 = Entity::create(glm::vec3(174.03f, 11.703f, 79.638f));
		cp3.makeTrigger(ColliderShape::axisAlignedBox(glm::vec3(3.37f, 23.9f, 33.4f) / 2.0f));
		cp3.setTriggerFunc([](Entity e, Entity other) {
			if (other.idx == car.idx && other.gen == car.gen && (checkpoint >= 2)) {
				checkpoint = 3;
			}
		});
		
		auto cp4 = Entity::create(glm::vec3(214.34f, 11.703f, 23.699f));
		cp4.makeTrigger(ColliderShape::axisAlignedBox(glm::vec3(79.8f, 23.9f, 2.0f) / 2.0f));
		cp4.setTriggerFunc([](Entity e, Entity other) {
			if (other.idx == car.idx && other.gen == car.gen && (checkpoint == 3)) {
				lapTimes.push_back(timer);
				lapCarTypes.push_back(carType);
				resetObjects();
			}
		});
		
	}
	
	void init() {
		melonTex = Tex::load("assets/textures/melon.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		tikiTex = Tex::load("assets/textures/tiki.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		scorpionPickupTex = Tex::load("assets/textures/scorpionPickup.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		scorpionTex = Tex::load("assets/textures/scorpion.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		bannerTex = Tex::load("assets/textures/banner.png", Tex::FLAG_MIPMAP | Tex::FLAG_FILTER);
		
		sphereMesh = Mesh3d::load("assets/models/sphere.obj");
		cubeMesh = Mesh3d::load("assets/models/cube.obj");
		melonMesh = Mesh3d::load("assets/models/melon.obj");
		tikiMesh = Mesh3d::load("assets/models/tiki.obj");
		scorpionPickupMesh = Mesh3d::load("assets/models/scorpionPickup.obj");
		bannerMesh = Mesh3d::load("assets/models/banner.obj");
		
		Map::init();
		
		resetObjects();
	}
	
	void update(float dt) {
		timer += (double)dt;
		
		glm::vec3 f = car.forward();
		
		cpos = car.pos() - (car.forward() * 10.0f) + glm::vec3(0.0f, 5.0f, 0.0f);
		Scene3d::cam.viewMat =
			glm::lookAt(cpos, car.pos(), glm::vec3(0.0f, 1.0f, 0.0f));
		
		auto keysHeld = SDL_GetKeyboardState(nullptr);
		
		static bool prevRHeld;
		bool rHeld = keysHeld[SDL_SCANCODE_R];
		if (rHeld && !prevRHeld) {
			resetObjects();
		}
		prevRHeld = rHeld;
		
		if (ImGui::Begin("Menu")) {
			int msecs = (int)(timer * 1000.0) % 1000;
			int secs = (int)timer % 60;
			int mins = (int)timer / 60;
			
			ImGui::Text("Time: %d:%.2d.%.3d", mins, secs, msecs);
			
			ImGui::Text("Ammunition: %d", ammo);
			
			ImGui::Text("Switch Car Type:");
			if (ImGui::Button("Classic Original")) { carType = 0; resetObjects(); }
			ImGui::SameLine();
			if (ImGui::Button("Elephant")) { carType = 1; resetObjects(); }
			if (ImGui::Button("Bar of Soap")) { carType = 2; resetObjects(); }
			ImGui::SameLine();
			if (ImGui::Button("Ball")) { carType = 3; resetObjects(); }
			
			ImGui::Checkbox("Use Euler Integration", &useEuler);
		}
		ImGui::End();
		
		if (ImGui::Begin("Lap History")) {
			for (size_t i = 0; i < lapTimes.size(); i++) {
				double t = lapTimes[i];
				int ct = lapCarTypes[i];
				
				int msecs = (int)(t * 1000.0) % 1000;
				int secs = (int)t % 60;
				int mins = (int)t / 60;
				
				ImGui::Text("%d:%.2d.%.3d as %s", mins, secs, msecs,
					(ct == 0)? "Classic Original" :
					(ct == 1)? "Elephant" :
					(ct == 2)? "Bar of Soap" :
					"Ball"
				);
			}
			
		}
		ImGui::End();
		
		if (ImGui::Begin("Controls")) {
			ImGui::Text("W: drive");
			ImGui::Text("S: reverse");
			ImGui::Text("A, D: steer");
			ImGui::Text("SPACE: jump");
			ImGui::Text("E: shoot");
			ImGui::Text("R: reset lap");
			
		}
		ImGui::End();
	}
}
