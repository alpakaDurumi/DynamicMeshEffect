# DynamicMeshEffect

Personal project for practicing Direct3D 11 and HLSL

Content is based on lecture : [홍정모의 그래픽스 새싹코스 - 파트 2](https://honglab.co.kr/courses/graphicspt2)

## Goals

### Review lecture

- D3D graphics pipeline
- Vertex Shader and Pixel Shader
- Drawing 3D models
- Texturing
- environment mapping and basic IBL
- Post processing

## Additional Implementaions

- A separate mesh corresponding to the shell is created for the original model, and when the mouse is brought close, the shell separates in the direction of the vertex normals

## What used

- C++17
- Direct3D 11
- HLSL
- [Dear ImGui](https://github.com/ocornut/imgui)
- [assimp](https://github.com/assimp/assimp)
- [stb](https://github.com/nothings/stb)
- [DirectXTK](https://github.com/microsoft/DirectXTK)

---

# DynamicMeshEffect

Direct3D 11과 쉐이더 프로그래밍을 연습하기 위한 프로젝트입니다.

내용은 [홍정모의 그래픽스 새싹코스 - 파트 2](https://honglab.co.kr/courses/graphicspt2) 강의를 기반으로 합니다.

## 목표

### 강의 복습

- D3D 그래픽스 파이프라인
- 버텍스 쉐이더와 픽셀 쉐이더
- 3D 모델 그리기
- 텍스처링
- 환경 매핑과 기초적인 IBL
- 후처리

### 추가 구현 사항

- 원본 모델에 대해 껍질에 해당하는 별도의 메쉬를 생성하고, 마우스를 가까이 가져다 대면 껍질이 버텍스 노멀 방향으로 분리되는 효과


## 사용된 자료 및 기술

- C++17
- Direct3D 11
- HLSL
- [Dear ImGui](https://github.com/ocornut/imgui)
- [assimp](https://github.com/assimp/assimp)
- [stb](https://github.com/nothings/stb)
- [DirectXTK](https://github.com/microsoft/DirectXTK)
