#pragma once

#include <algorithm>
#include <format>

#include <imgui/imgui.h>

#include "entity.hpp"

static void debuguiInit() {
	
}

static void debuguiUpdate(float dt) {
	ImGui::Begin("Entities");
	
	for (size_t i = 0; i < (size_t)numEntitySlots; i++) {
		if (ImGui::TreeNode(std::format("Entity {}", i).c_str())) {
			queueDrawDebugPoint(entityTransforms[i].pos, glm::vec3(1.0f, 1.0f, 0.0f), true, 0.0f);
			
			ImGui::SeparatorText("Transform");
			
			ImGui::DragFloat3("Position", (float*)&entityTransforms[i].pos, 0.2f);
			
			glm::mat3 const &rot = entityTransforms[i].rot;
			glm::vec3 rotEuler = glm::vec3(
				glm::degrees(atan2(rot[1][2], rot[2][2])),
				glm::degrees(atan2(-rot[0][2], glm::sqrt((rot[1][2] * rot[1][2]) + (rot[2][2] * rot[2][2])))),
				glm::degrees(atan2(rot[0][1], rot[0][0]))
			);
			
			glm::vec3 prevRotEuler = rotEuler;
			ImGui::DragFloat3("Rotation", (float*)&rotEuler);
			if (prevRotEuler != rotEuler) {
				entityTransforms[i].rot =
					glm::rotate(glm::radians(rotEuler.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::rotate(glm::radians(rotEuler.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::rotate(glm::radians(rotEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			
			if (entitySlots[i].flags & ENTITY_HAS_BODY) {
				Body &body = entityBodies[i];
				
				ImGui::SeparatorText("Body");
				
				char const *bodyKindStr[] = {
					"Sphere",
					"AABB",
					"OBB"
				};
				
				if (ImGui::BeginCombo("Type", bodyKindStr[body.kind])) {
					if (ImGui::Selectable("Sphere")) {
						if (body.kind != BODY_SPHERE) {
							body.kind = BODY_SPHERE;
							body.radius = std::min({body.extents.x, body.extents.y, body.extents.z});
						}
					}
					else if (ImGui::Selectable("AABB")) {
						if (body.kind == BODY_SPHERE) {
							body.extents = glm::vec3(body.radius);
						}
						body.kind = BODY_AABB;
					}
					else if (ImGui::Selectable("OBB")) {
						if (body.kind == BODY_SPHERE) {
							body.extents = glm::vec3(body.radius);
						}
						body.kind = BODY_OBB;
					}
					
					ImGui::EndCombo();
				}
				
				if (body.kind == BODY_SPHERE) {
					ImGui::DragFloat("Radius", &body.radius, 0.1f);
				}
				else {
					ImGui::DragFloat3("Extents", (float*)&body.extents, 0.1f);
				}
				
				float prevMass = body.mass;
				ImGui::DragFloat("Mass", &body.mass);
				if (body.mass != prevMass) {
					body.massInv = 1.0f / body.mass;
				}
				
				ImGui::DragFloat("Static Friction", &body.sFriction, 0.02f);
				ImGui::DragFloat("Dynamic Friction", &body.dFriction, 0.02f);
				ImGui::DragFloat("Restitution", &body.restitution, 0.02f);
				
				ImGui::DragFloat3("Velocity", (float*)&body.vel, 0.1f);
				//ImGui::DragFloat3("Velocity", (float*)&body.vel, 0.1f);
			}
			
			ImGui::TreePop();
		}
	}
	
	ImGui::End();
	
	
	//ImGui::ShowDemoWindow();
}
