#pragma once
#include <imgui.h>

namespace Proto::EditorStyle
{
	// -----------------------------------------------------------------------
	// 전역 설정
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_TEXT             = ImVec4(0.0f,  0.0f,  0.0f, 1.0f); // 전역 기본 텍스트
	static constexpr ImVec4 COLOR_WINDOW_BG        = ImVec4(0.72f, 0.72f, 0.72f, 1.0f); // 패널 배경
	static constexpr ImVec4 COLOR_PANEL_HEADER_BG  = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 패널 헤더 바 배경
	static constexpr ImVec4 COLOR_PANEL_TEXT       = ImVec4(0.0f,  0.0f,  0.0f, 1.0f); // 패널 본문 텍스트
	static constexpr ImVec4 COLOR_POPUP_BG         = ImVec4(1.0f,  1.0f,  1.0f, 1.0f); // 드롭다운 팝업 배경

	static constexpr ImVec4 COLOR_INPUT_BG         = ImVec4(1.0f,  1.0f,  1.0f,  1.0f); // 텍스트 입력칸 배경 (흰색)
	static constexpr ImVec4 COLOR_INPUT_TEXT       = ImVec4(0.0f,  0.0f,  0.0f,  1.0f); // 텍스트 입력칸 글자 (검정)

	static constexpr ImVec4 COLOR_HEADER           = ImVec4(0.80f, 0.80f, 0.80f, 1.0f); // 트리 헤더 (기본)
	static constexpr ImVec4 COLOR_HEADER_HOVER     = ImVec4(0.80f, 0.80f, 0.80f, 1.0f); // 트리 헤더 (호버)
	static constexpr ImVec4 COLOR_HEADER_ACTIVE    = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 트리 헤더 (클릭)

	static constexpr ImVec4 COLOR_BUTTON_HOVERED   = ImVec4(0.68f, 0.68f, 0.68f, 1.0f); // 전역 버튼 호버 (탭 팝업 버튼 등)
	static constexpr ImVec4 COLOR_BUTTON_ACTIVE    = ImVec4(0.62f, 0.62f, 0.62f, 1.0f); // 전역 버튼 클릭 (탭 팝업 버튼 등)

	static constexpr ImVec4 COLOR_BORDER           = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 패널 외곽 테두리
	static constexpr ImVec4 COLOR_BORDER_SHADOW    = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 테두리 그림자
	static constexpr ImVec4 COLOR_SEPARATOR        = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 구분선
	static constexpr ImVec4 COLOR_SEPARATOR_HOVERED= ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 구분선 호버
	static constexpr ImVec4 COLOR_SEPARATOR_ACTIVE = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 구분선 클릭
	// 도킹 분할선 호버 시 ImGui가 내부적으로 SeparatorHovered를 ResizeGripHovered로 덮어씀 — 반드시 함께 설정
	static constexpr ImVec4 COLOR_RESIZE_GRIP        = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 리사이즈 핸들
	static constexpr ImVec4 COLOR_RESIZE_GRIP_HOVERED= ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 리사이즈 핸들 호버
	static constexpr ImVec4 COLOR_RESIZE_GRIP_ACTIVE = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 리사이즈 핸들 클릭

	// 도킹 탭
	static constexpr ImVec4 COLOR_TAB                  = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 탭 기본
	static constexpr ImVec4 COLOR_TAB_HOVERED          = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 탭 호버
	static constexpr ImVec4 COLOR_TAB_ACTIVE           = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 탭 활성
	static constexpr ImVec4 COLOR_TAB_UNFOCUSED        = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 탭 비포커스
	static constexpr ImVec4 COLOR_TAB_UNFOCUSED_ACTIVE = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 탭 비포커스 활성
	static constexpr ImVec4 COLOR_TAB_OVERLINE         = ImVec4(0.23f, 0.45f, 0.69f, 1.0f); // 선택된 탭 강조선
	static constexpr float  TAB_OVERLINE_SIZE          = 2.5f;                               // 강조선 두께 (픽셀)

	// 타이틀바
	static constexpr ImVec4 COLOR_TITLE_BG           = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 배경 (비포커스)
	static constexpr ImVec4 COLOR_TITLE_BG_ACTIVE    = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 배경 (포커스)
	static constexpr ImVec4 COLOR_TITLE_BG_COLLAPSED = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 배경 (접힌 상태)
	static constexpr ImVec4 COLOR_TITLE_TEXT         = ImVec4(0.0f,  0.0f,  0.0f,  1.0f); // 텍스트

	// -----------------------------------------------------------------------
	// 메뉴바
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_MENUBAR_BG   = ImVec4(1.0f,  1.0f,  1.0f, 1.0f); // 메뉴바 배경
	static constexpr ImVec4 COLOR_MENUBAR_TEXT = ImVec4(0.0f,  0.0f,  0.0f, 1.0f); // 메뉴바 텍스트

	// -----------------------------------------------------------------------
	// 툴바
	// -----------------------------------------------------------------------
	static constexpr float TOOLBAR_HEIGHT             = 36.0f; // 툴바 전체 높이
	static constexpr float TOOLBAR_BUTTON_WIDTH       = 70.0f; // Play/Stop 버튼 너비
	static constexpr float TOOLBAR_PAUSE_BUTTON_WIDTH = 70.0f; // Pause/Resume 버튼 너비
	static constexpr float TOOLBAR_BUTTON_HEIGHT      = 25.0f; // 버튼 높이
	static constexpr float TOOLBAR_BUTTON_SPACING     = 8.0f;  // 버튼 간격

	static constexpr ImVec4 COLOR_PLAY_BG  = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 에디터 모드 배경
	static constexpr ImVec4 COLOR_STOP_BG  = ImVec4(0.85f, 0.85f, 0.85f, 1.0f); // 플레이 모드 배경

	static constexpr ImVec4 COLOR_PLAY_BTN         = ImVec4(0.60f,  0.60f,  0.60f,  1.0f); // Play 버튼
	static constexpr ImVec4 COLOR_PLAY_BTN_HOVER   = ImVec4(0.70f,  0.70f,  0.70f,  1.0f); // Play 버튼 호버
	static constexpr ImVec4 COLOR_PLAY_BTN_ACTIVE  = ImVec4(0.75f,  0.75f,  0.75f,  1.0f); // Play 버튼 클릭

	static constexpr ImVec4 COLOR_STOP_BTN        = ImVec4(0.55f,  0.73f,  0.95f,  1.0f); // Stop 버튼
	static constexpr ImVec4 COLOR_STOP_BTN_HOVER  = ImVec4(0.55f,  0.66f,  0.80f,  1.0f); // Stop 버튼 호버
	static constexpr ImVec4 COLOR_STOP_BTN_ACTIVE = ImVec4(0.60f,  0.71f,  0.85f,  1.0f); // Stop 버튼 클릭

	static constexpr ImVec4 COLOR_PAUSE_BTN        = ImVec4(0.60f,  0.60f,  0.60f,  1.0f); // Pause 버튼
	static constexpr ImVec4 COLOR_PAUSE_BTN_HOVER  = ImVec4(0.70f,  0.70f,  0.70f,  1.0f); // Pause 버튼 호버
	static constexpr ImVec4 COLOR_PAUSE_BTN_ACTIVE = ImVec4(0.75f,  0.75f,  0.75f,  1.0f); // Pause 버튼 클릭

	static constexpr ImVec4 COLOR_RESUME_BTN        = ImVec4(0.55f, 0.73f, 0.95f, 1.0f); // Resume 버튼
	static constexpr ImVec4 COLOR_RESUME_BTN_HOVER  = ImVec4(0.55f, 0.66f, 0.80f,  1.0f); // Resume 버튼 호버
	static constexpr ImVec4 COLOR_RESUME_BTN_ACTIVE = ImVec4(0.60f, 0.71f, 0.85f,  1.0f); // Resume 버튼 클릭

	static constexpr ImVec4 COLOR_PLAY_PAUSE_BTN_TEXT  = ImVec4(0.33f, 0.33f, 0.33f, 1.0f); // Play / Pause 버튼 글씨
	static constexpr ImVec4 COLOR_STOP_RESUME_BTN_TEXT = ImVec4(0.0f,  0.0f,  0.0f,  1.0f); // Stop / Resume 버튼 글씨

	// -----------------------------------------------------------------------
	// 씬 패널
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_VIEWPORT_HEADER        = ImVec4(0.75f, 0.75f, 0.75f, 1.0f); // 기즈모 헤더 바 배경

	static constexpr ImVec4 COLOR_GIZMO_BTN             = ImVec4(0.89f, 0.89f, 0.89f, 1.0f); // 기즈모 버튼
	static constexpr ImVec4 COLOR_GIZMO_BTN_HOVERED     = ImVec4(0.93f, 0.93f, 0.93f, 1.0f); // 기즈모 버튼 호버
	static constexpr ImVec4 COLOR_GIZMO_BTN_ACTIVE      = ImVec4(0.93f, 0.93f, 0.93f, 1.0f); // 기즈모 버튼 클릭
	static constexpr ImVec4 COLOR_GIZMO_BTN_TEXT        = ImVec4(0.33f, 0.33f, 0.33f, 1.0f); // 기즈모 버튼 텍스트

	static constexpr ImVec4 COLOR_GIZMO_BTN_SELECTED    = ImVec4(0.55f, 0.73f, 0.95f, 1.0f); // 선택된 기즈모 버튼
	static constexpr ImVec4 COLOR_GIZMO_BTN_SEL_HOVERED = ImVec4(0.55f, 0.73f, 0.95f, 1.0f); // 선택된 기즈모 버튼 호버
	static constexpr ImVec4 COLOR_GIZMO_BTN_SEL_ACTIVE  = ImVec4(0.55f, 0.73f, 0.95f, 1.0f); // 선택된 기즈모 버튼 클릭
	static constexpr ImVec4 COLOR_GIZMO_BTN_SELECTED_TEXT = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 선택된 기즈모 버튼 텍스트

	// -----------------------------------------------------------------------
	// 계층 패널
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_HIERARCHY_SELECTED         = ImVec4(0.227f, 0.447f, 0.690f, 1.0f); // 선택된 오브젝트
	static constexpr ImVec4 COLOR_HIERARCHY_SELECTED_HOVERED = ImVec4(0.227f, 0.447f, 0.690f, 1.0f); // 선택 + 호버
	static constexpr ImVec4 COLOR_HIERARCHY_SELECTED_ACTIVE  = ImVec4(0.227f, 0.447f, 0.690f, 1.0f); // 선택 + 클릭
	static constexpr ImVec4 COLOR_HIERARCHY_HOVERED          = ImVec4(0.60f,  0.60f,  0.60f,  1.0f); // 비선택 호버/클릭

	// -----------------------------------------------------------------------
	// 인스펙터 패널
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_INSPECTOR_FIELD_BG = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 값 수정 필드 배경 (흰색)

	static constexpr ImVec4 COLOR_STATUS_ERROR   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 오류 (Red)
	static constexpr ImVec4 COLOR_STATUS_WARNING = ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // 경고 (Orange)
	static constexpr ImVec4 COLOR_STATUS_SUCCESS = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // 정상 (Green)

	static constexpr float  ADD_COMPONENT_BTN_WIDTH = 240.0f; // Add Component 버튼 너비

	// -----------------------------------------------------------------------
	// 콘텐츠 브라우저 패널
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_FILE_FOLDER  = ImVec4(0.3f, 0.5f, 0.8f, 1.0f); // 폴더        (Blue)
	static constexpr ImVec4 COLOR_FILE_PROJECT = ImVec4(0.8f, 0.5f, 0.2f, 1.0f); // .proto 파일 (Orange)
	static constexpr ImVec4 COLOR_FILE_DEFAULT = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 일반 파일   (Grey)

	// -----------------------------------------------------------------------
	// 콘솔 패널
	// -----------------------------------------------------------------------
	static constexpr ImVec4 COLOR_LOG_INFO    = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Info  (Black)
	static constexpr ImVec4 COLOR_LOG_WARN    = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Warn  (Yellow)
	static constexpr ImVec4 COLOR_LOG_ERROR   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Error (Red)
	static constexpr ImVec4 COLOR_LOG_DEFAULT = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 기본  (White)

	// 필터 버튼 활성 상태 색 (별도 지정 가능)
	static constexpr ImVec4 COLOR_CONSOLE_FILTER_ACTIVE         = ImVec4(0.60f, 0.60f, 0.60f, 1.0f); // 활성 버튼
	static constexpr ImVec4 COLOR_CONSOLE_FILTER_ACTIVE_HOVERED = ImVec4(0.70f, 0.70f, 0.70f, 1.0f); // 활성 버튼 호버
	static constexpr ImVec4 COLOR_CONSOLE_FILTER_ACTIVE_PRESSED = ImVec4(0.65f, 0.65f, 0.65f, 1.0f); // 활성 버튼 클릭
}
