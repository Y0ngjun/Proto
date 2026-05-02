# TODO (향후 작업 목록)

## 1. 에디터 고도화 (Editor Polish)
- [ ] **컴포넌트 추가/삭제**: Inspector 패널에서 "Add Component" 버튼 및 개별 컴포넌트 삭제 기능 구현
- [ ] **오브젝트 관리**: Hierarchy 패널에서 우클릭 메뉴(Create Empty, Delete, Rename) 구현
- [ ] **기본 프리미티브 추가**: Cube, Sphere, Plane 등 기본 메쉬 오브젝트 생성 단축 기능
- [ ] **에셋 드래그 앤 드롭**: Content Browser에서 Viewport나 Inspector로 에셋(메쉬, 씬) 드래그 연동

## 2. 씬 및 에셋 시스템 (Scene & Asset)
- [ ] **씬 전환 UI**: 에디터 내에서 여러 씬을 자유롭게 열고 닫는 탭 또는 다이얼로그 시스템
- [ ] **메테리얼 에디터 기초**: 컴포넌트에서 셰이더 파라미터(색상, 텍스처) 수정 기능
- [ ] **자동 리로드**: 외부에서 에셋이 변경되었을 때 에디터에서 실시간으로 감지하여 리로드

## 3. 리팩토링 및 구조 개선 (Refactoring)
- [ ] **파일 분리**: 500라인이 넘어가는 `Application.cpp`, `Scene.cpp` 등을 기능 단위로 쪼개기
- [ ] **단일 책임 원칙(SRP) 점검**: UI 로직과 엔진 핵심 로직의 완전한 분리
- [ ] **Public API 정립**: 외부 모듈(스크립트 등)에서 접근 가능한 헤더와 내부 헤더 명확히 구분
- [ ] **스마트 포인터 전면 도입**: 원시 포인터(`new/delete`) 사용처를 `unique_ptr`/`shared_ptr`로 완전 대체

## 4. 문서화 (Documentation)
- [ ] **아키텍처 다이어그램**: Core/Scene/Renderer 간의 데이터 흐름 가시화
- [ ] **스크립트 가이드**: `NativeScriptComponent`를 사용하여 게임 로직을 작성하는 방법 문서화