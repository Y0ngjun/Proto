# TODO (향후 작업 목록)

## 1. 에디터 고도화 (Editor Polish)
- [ ] **컴포넌트 추가/삭제**: Inspector 패널에서 "Add Component" 버튼 및 개별 컴포넌트 삭제 기능 구현
- [ ] **오브젝트 관리**: Hierarchy 패널에서 우클릭 메뉴(Create Empty, Delete, Rename) 구현
- [ ] **기본 프리미티브 추가**: Cube, Sphere, Plane 등 기본 메쉬 오브젝트 생성 단축 기능
- [ ] **에셋 드래그 앤 드롭**: Content Browser에서 Viewport나 Inspector로 에셋(메쉬, 씬) 드래그 연동

## 2. 리팩토링 및 구조 개선 (Refactoring)
- [ ] **파일 분리**: 500라인이 넘어가는 `Application.cpp`, `Scene.cpp` 등을 기능 단위로 쪼개기
- [ ] **단일 책임 원칙(SRP) 점검**: UI 로직과 엔진 핵심 로직의 완전한 분리
- [ ] **Public API 정립**: 외부 모듈(스크립트 등)에서 접근 가능한 헤더와 내부 헤더 명확히 구분
- [ ] **스마트 포인터 전면 도입**: 원시 포인터(`new/delete`) 사용처를 `unique_ptr`/`shared_ptr`로 완전 대체

- new, open, save, save as, (scene, proj 별로 구현)