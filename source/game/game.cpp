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
#include "map.hpp"

namespace Game {
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
	
	static Mesh3d
		sphereMesh,
		cubeMesh;
	
	static Entity car;
	
	void addStaticObb(glm::vec3 pos, glm::vec3 halfExtents, glm::vec3 rot) {
		auto obj = Entity::create(
			pos,
			glm::rotate(rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(rot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(rot.z, glm::vec3(0.0f, 0.0f, 1.0f))
		);
		obj.makeKinematic(
			ColliderShape::box(halfExtents),
			PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f}
		);
		obj.addMesh(cubeMesh, grey, glm::scale(halfExtents));
	}
	
	void init() {
		sphereMesh = Mesh3d::load("assets/models/sphere.obj");
		cubeMesh = Mesh3d::load("assets/models/cube.obj");
		
		Map::init();
		
		for (auto o : Map::objects) {
			auto e = Entity::create(
				o.pos,
				glm::orthonormalize(glm::mat3(
					glm::rotate(o.rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::rotate(o.rot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::rotate(o.rot.z, glm::vec3(0.0f, 0.0f, 1.0f))
				))
			);
			e.makeKinematic(
				ColliderShape::box(o.halfSize),
				o.physMaterial
			);
			e.addMesh(cubeMesh, *o.material, glm::scale(o.halfSize));
		}
		
		car = Entity::create(glm::vec3(204.971f, 5.0f, -27.3629f));
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
		});
	}
	
	void update(float dt) {
		glm::vec3 f = car.forward();
		
		glm::vec3 cpos = car.pos() - (car.forward() * 10.0f) + glm::vec3(0.0f, 5.0f, 0.0f);
		Scene3d::cam.viewMat =
			glm::lookAt(cpos, car.pos(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
