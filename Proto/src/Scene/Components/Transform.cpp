#include "Transform.h"

namespace Proto {

	glm::mat4 Transform::GetTransform() const {
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}

}