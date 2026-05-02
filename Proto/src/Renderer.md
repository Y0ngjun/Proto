# Renderer 계층

OpenGL 그래픽스 API를 추상화하고, 화면 렌더링 파이프라인을 책임집니다.

## 핵심 개념 및 클래스

1. **Renderer (Static API)**
   - 실제 드로우 콜(`glDrawElements`)을 감싸고 있는 정적 유틸리티.
   - `Submit(vao, shader)` 형태로 메시와 셰이더를 받아 렌더링 실행.

2. **Shader**
   - GLSL 프로그램 컴파일, 링크 및 유니폼(Uniform) 데이터 전송.
   - 행렬, 색상, 스칼라 등 데이터를 GPU로 업로드.

3. **VertexArray, Buffer (VBO, IBO)**
   - 정점 데이터(위치, 법선, UV)와 인덱스 버퍼를 캡슐화.
   - `MeshLoader`를 통해 Cube, Plane 등 기본 프리미티브를 손쉽게 생성.

4. **Framebuffer**
   - 오프스크린 렌더링 타겟 관리.
   - 에디터의 뷰포트에 텍스처 형태로 렌더링된 결과를 전달하거나, Object Picking용 ID 버퍼 저장에 활용.

5. **Camera (EditorCamera / SceneCamera)**
   - View, Projection 행렬 계산.
   - 에디터 카메라는 마우스/키보드 입력으로 자유 이동 지원.

## 렌더링 파이프라인 흐름
1. Framebuffer 바인딩 및 화면 클리어.
2. 사용할 Shader 바인딩 후 전역/조명 Uniform 업데이트.
3. 씬의 각 객체를 순회하며 VertexArray 바인딩 후 Renderer::Submit 호출.
4. Framebuffer 언바인딩 후 ImGui에 결과 출력.
