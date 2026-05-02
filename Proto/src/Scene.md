# Scene 계층

게임 객체의 생명주기를 관리하고 상태 및 로직 데이터를 다루는 **GameObject-Component 아키텍처**를 구현합니다.

## 핵심 클래스

1. **Scene**
   - 모든 GameObject의 컨테이너이자 라이프사이클 관리자.
   - 에디터 모드(`OnUpdateEditor`)와 게임 실행 모드(`OnUpdateRuntime`)로 나뉘어 로직 처리.
   - 직렬화(Serialization): 전체 씬 데이터를 YAML 형태로 저장하고 복원.

2. **GameObject**
   - 고유 식별자(ID)와 이름을 가지며 실체 로직을 가지지 않는 빈 컨테이너.
   - 동적으로 여러 Component를 추가/검색할 수 있는 템플릿 인터페이스 제공.

3. **Component 인터페이스**
   - 모든 컴포넌트의 부모 클래스로, `OnStart`, `OnUpdate`, `OnDestroy` 생명주기 제공.
   - `Serialize`, `Deserialize` 순수 가상 함수를 통해 직렬화 규격 정의.

## 주요 내장 Component
- **Transform**: 3D 공간 상의 위치, 회전(오일러각), 스케일과 월드 변환 행렬 캐싱.
- **MeshRenderer**: 렌더링할 3D 메시(VAO)와 사용할 셰이더 보관. 직렬화 시 식별자로 자동 로드.
- **CameraComponent**: 해당 객체를 기준으로 하는 카메라 투영 및 뷰 설정.
- **LightComponent**: 방향성 또는 포인트 조명의 색상 및 강도 설정.

## 데이터 흐름
씬의 Update 단계에서 각 GameObject는 자신에게 부착된 모든 Component의 `OnUpdate(dt)`를 순차적으로 호출하며 자율적인 상태 변화를 진행합니다.
