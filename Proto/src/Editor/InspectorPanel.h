/*
 * 선택된 게임 오브젝트의 컴포넌트 정보 및 상세 설정을 표시하는 인스펙터 패널 클래스입니다.
 * 컴포넌트 추가/삭제 및 각 속성(변환, 렌더러, 물리 등)의 실시간 수정을 지원합니다.
 */

#pragma once

namespace Proto
{
	class Scene;
	class GameObject;

	class InspectorPanel
	{
	public:
		InspectorPanel() = default;
		~InspectorPanel() = default;

		void OnImGuiRender(GameObject* selectedContext);

	private:
		void DrawComponents(GameObject* gameObject);
		void DrawAddComponentButton(GameObject* gameObject);
	};
}
