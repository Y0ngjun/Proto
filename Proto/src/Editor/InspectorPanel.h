#pragma once

#include "../Scene/Scene.h"

namespace Proto
{
	class InspectorPanel
	{
	public:
		InspectorPanel() = default;

		void OnImGuiRender(GameObject* selectedContext);

	private:
		void DrawComponents(GameObject* gameObject);
	};
}
