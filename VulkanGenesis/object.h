#pragma once

#include "model.h"

#include <memory>

namespace Lorenz
{
	struct Transform2dComponent {
		glm::vec2 translation{};  // (position offset)
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};

	class Object
	{
	public:
		using id_t = unsigned int;

		static Object createObject() {
			static id_t currentId = 0;
			return Object{ currentId++ };
		}


		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;
		Object(Object&&) = default;
		Object& operator=(Object&&) = default;

		id_t getId() { return id; }

		std::shared_ptr<Model> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

	private:
		Object(id_t objId) : id{ objId } {}

		id_t id;
	};
}	// namespace Lorenz