#include "map.hpp"

namespace Map {
	static Tex
		sandTex,
		roadTex;
	
	static auto
		//sandMaterial = Material{.kind = Material::KIND_LIT_UNTEXED, .colour = glm::vec3(0.8f, 0.56f, 0.23f)};
		sandMaterial = Material{.kind = Material::KIND_LIT, .tex = &sandTex, .colour = glm::vec3(1.0f)},
		roadMaterial = Material{.kind = Material::KIND_LIT, .tex = &roadTex, .colour = glm::vec3(1.0f)};
	
	static auto
		sandPhysMaterial = PhysicsMaterial{.sFrict = 1.0f, .dFrict = 0.8f, .bounciness = 0.1f},
		roadPhysMaterial = PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f};
	
	Object objects[] = {
		Object{
			.pos = glm::vec3(194.664f, -12.6758f, 6.68387f),
			.halfSize = glm::vec3(85.7936f, 27.8352f, 251.66f) / 2.0f,
			.rot = glm::vec3(0.0f, 0.0f, 0.0f),
			.material = &sandMaterial, .physMaterial = sandPhysMaterial
		},
		Object{
			.pos = glm::vec3(205.048f, 0.399536f, -12.9622f),
			.halfSize = glm::vec3(11.6224f, 1.82233f, 98.3853f) / 2.0f,
			.rot = glm::vec3(0.0f, 0.0f, 0.0f),
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		},
		Object{
			.pos = glm::vec3(197.795f, 0.399537f, 41.8688f),
			.halfSize = glm::vec3(14.3095f, 1.82233f, 24.5965f) / 2.0f,
			.rot = glm::vec3(0.0f, glm::radians(36.4638f), 0.0f),
			.material = &roadMaterial, .physMaterial = roadPhysMaterial
		}
	};
	
	void init() {
		roadTex = Tex::load("assets/textures/road.png", Tex::FLAG_FILTER | Tex::FLAG_MIPMAP);
		sandTex = Tex::load("assets/textures/sand.png", Tex::FLAG_FILTER | Tex::FLAG_MIPMAP);
	}
}
