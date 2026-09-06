# 게임 개발

> 이미지를 클릭하시면 유튜브 시연 링크로 연결됩니다.

<aside>
💡 **인원** : [프로젝트 인원 입력]  
**기간** : [개발 기간 입력]  
**담당** : 게임 콘텐츠·클라이언트·엔진 기능 개발  
**기술** : C++17, DirectX 9, Dear ImGui, nlohmann/json, FMOD
</aside>

## 프로젝트 소개

C++과 DirectX 9을 기반으로 제작한 2D 퍼즐 액션 게임입니다.

플레이어는 횡스크롤 캐릭터 **Toodee**와 탑다운 캐릭터 **Topdee**를 조작하며, 2D와 탑다운 차원을 전환해 서로 다른 규칙으로 스테이지의 퍼즐을 해결합니다.

횡스크롤 차원에서는 이동과 점프를 이용해 지형을 돌파하고, 탑다운 차원에서는 상자를 들거나 연속해서 밀고, 버튼·열쇠·포털·중력 및 전기 블록을 조작합니다. 같은 맵이라도 선택한 차원에 따라 캐릭터의 입력, 카메라, 충돌과 오브젝트 동작이 달라지도록 구성했습니다.

게임 콘텐츠뿐 아니라 GameObject–Component 구조, Prototype 기반 객체 생성, 충돌 레이어, Timer와 Key Context, 차원 전환 Component, 파티클 시스템, 멀티스레드 로딩 및 Dear ImGui 기반 맵 제작 도구도 함께 구현했습니다.

---

## 개발 환경

| 구분 | 사용 기술 |
| --- | --- |
| Language | C++17 |
| Graphics | DirectX 9 / D3DX9 |
| UI / Tool | Dear ImGui |
| Data | nlohmann/json |
| Audio | FMOD |
| IDE / Build | Visual Studio 2022 / v143 |
| Platform | Windows |

### 외부 라이브러리 활용

#### Dear ImGui

Hierarchy, Inspector, Scene과 Insert 창으로 구성된 맵 제작 도구를 구현하는 데 사용했습니다. 오브젝트를 배치하고 Transform과 콘텐츠 속성을 수정하며 결과를 저장·불러올 수 있도록 구성했습니다.

#### nlohmann/json

맵에 배치된 오브젝트의 Prototype, Level, Layer, 위치와 콘텐츠별 속성을 직렬화하고 다시 복원하는 데 사용했습니다.

#### FMOD

캐릭터 이동·점프·착지·상자 상호작용, 퍼즐 오브젝트, 보스 패턴과 UI의 효과음 및 배경음 재생에 사용했습니다. 용도별 Channel을 분리해 사운드를 관리했습니다.

---

# 주요 기능

## 차원 전환 시스템

- 횡스크롤 2D 차원과 탑다운 차원의 실시간 전환
- 차원별 캐릭터 이동 방식 및 입력 Context 전환
- 차원 상태에 따른 Collider 활성화·비활성화
- 전환 비율 기반 카메라 위치·회전 보간
- 캐릭터와 오브젝트의 회전·투명도 전환
- 오브젝트별 차원 진입·유지·이탈 Callback 등록
- 전환 중 상자를 내려놓거나 다시 들어 상태 일관성 유지

## 횡스크롤 캐릭터 조작

- 좌우 이동과 중력 기반 점프
- 발판 이탈 직후 점프를 허용하는 Coyote Time
- 착지 직전 입력을 보존하는 Jump Buffer
- 입력 유지 시간에 따라 높이가 달라지는 가변 점프
- 낙하 속도에 Terminal Velocity 적용
- 상승·정점·하강 상태에 따른 점프 Sprite 전환
- Idle·Run·Jump·Edge·Die 상태 관리
- 점프·착지 먼지 Particle 및 효과음 연동

## 탑다운 캐릭터 조작

- 8방향 이동과 대각선 속도 정규화
- 이동 방향에 따른 캐릭터 회전 및 Animation 전환
- 인접 Grid 검색 기반 상자 선택
- 상자 들기·운반·내려놓기
- 충돌 방향과 입력 방향을 이용한 상자 밀기
- 여러 상자를 재귀적으로 탐색하고 연쇄 이동
- 고정 블록·다른 캐릭터·점유 공간을 반영한 이동 제한

## 퍼즐 블록 시스템

- 일반 벽과 이동 가능한 상자
- 운반·이동·고정 속성을 구분하는 Block Flag
- 캐릭터와 상자를 감지하는 Button
- Button과 연동해 열리고 닫히는 Wall
- Key 획득과 Lock 해제
- 입·출구가 연결된 Portal Block
- 차원에 따라 낙하 방향이 달라지는 Gravity Block
- 운반 중 전기 상태가 활성화되는 Lightning Block
- Hole·Spike·Lightning 등 장애물 판정
- Clear Portal을 이용한 Stage 종료

## 포털 시스템

- Portal 입구와 출구 연결 정보 관리
- 캐릭터와 블록의 위치 이동
- 진입 방향에 대응하는 출구 위치 보정
- 연속 순간이동을 방지하는 Cooldown Timer
- 플레이어 참조를 Portal 관리 객체에서 통합 관리
- 일반 Portal과 Stage Clear Portal 분리

## 체크포인트와 리스폰

- Respawn Point 기반 캐릭터 복귀 위치 관리
- 사망 Animation 종료 후 캐릭터 상태 복원
- Collider와 입력 Context 재활성화
- 운반 중이던 상자의 Parent 관계 해제
- 상자 위치와 Grid 점유 정보 복원
- 함정·몬스터·보스 공격별 사망 판정

## 몬스터 콘텐츠

- 박쥐와 돼지형 몬스터의 이동·추적·공격 구현
- 플레이어 위치 및 거리에 따른 행동 전환
- 차원 상태에 따른 활성화 및 충돌 처리
- Turret의 탐색과 투사체 발사
- Monster별 Sprite Animation 및 사망 처리

## 보스 콘텐츠

- Ordalook, Yumbrozi, Korimlio, Ozraeen 4종 보스 구현
- 몸·머리·눈·팔·손·꼬리 등 Part 단위 계층 구성
- Chase·Idle·Hit·Attack·Reset 등 상태 전환
- FireBall·FirePellet·Laser·Spike·Wave·Giant Hand 공격
- Timer Callback을 연결한 단계별 공격 Sequence
- 플레이어 및 차원 상태를 반영한 패턴 선택
- 보스 체력과 Boss HP UI 연동
- 공격 시 Particle·Sound·Camera Shake 연동

## 카메라 연출

- 2D와 탑다운 시점을 지원하는 Hybrid Camera
- 차원 전환 비율 기반 View 전환
- 원형·Random·상하 Camera Shake
- 진폭·주파수·지속 시간 매개변수화
- 지수 감쇠와 삼각함수를 이용한 충격 표현
- 맵 제작을 위한 Free Camera 이동·회전

## 파티클 시스템

- 점프·착지 Dust
- Rain과 Background Shine
- Portal Shine과 Dimension Transit Shine
- Electric Field와 보스 전용 Particle
- 생성 수·속도·수명·크기·방출 각도 설정
- Timer 기반 Particle 생명주기 관리
- Point Sprite Buffer를 이용한 렌더링

## UI와 게임 진행

- Start·Demo·Edit·Exit Menu
- Boss HP와 Screen UI
- 대화용 Text Box와 Player Text
- Logo·Loading·Stage·Boss·Ending Level 구성
- Clear 및 사망 상태에 따른 화면 전환
- Ending Glitch·축소·Logo Sequence

## 맵 제작 도구

- Dear ImGui DockSpace 기반 Editor UI
- Hierarchy에서 배치 오브젝트 조회 및 선택
- Inspector에서 선택 객체 데이터 확인·편집
- Insert 창에서 Block·Monster·Portal·Puzzle Object 배치
- Scene에서 Click·Drag 기반 오브젝트 편집
- Prototype·Level·Layer 정보와 Object Data 저장
- JSON 기반 Map Data 저장·불러오기
- MapTool에서 저장 결과를 다시 생성해 검증

## 멀티스레드 로딩

- Level별 Texture·GameObject·Component Prototype 로딩
- `_beginthreadex`를 이용한 작업 Thread 구성
- Critical Section을 이용한 Loading 상태 동기화
- Loading 중 Main Thread에서 화면과 진행 상태 갱신
- Stage 및 4종 Boss 리소스의 Level별 분리 로딩

---

# 기술 설명

## 1. GameObject–Component 구조

게임 오브젝트의 변환·렌더링·텍스처·충돌·차원 반응을 각각 Component로 분리했습니다.

```text
GameObject
├─ Transform
├─ Renderer
├─ Texture / AnimTexture
├─ VIBuffer
├─ Collider
└─ DimensionSwitcher
```

GameObject는 필요한 Component를 문자열 Tag로 등록해 조합하고, 실제 플레이어·블록·몬스터·보스 로직만 파생 클래스에서 구현했습니다.

이를 통해 Transform, Collider와 Renderer 같은 공통 기능을 여러 콘텐츠에서 재사용하고 객체마다 필요한 기능만 선택적으로 구성했습니다.

---

## 2. Prototype 기반 객체 생성

GameObject와 Component의 원형을 Level별 Prototype으로 등록하고 `Clone()`을 통해 실제 객체를 생성했습니다.

```text
Prototype 등록
    ↓
Level / Tag 기반 검색
    ↓
Clone()
    ↓
객체별 초기 데이터 적용
    ↓
Layer 등록
```

Texture, VIBuffer와 GameObject의 공통 초기 설정을 Prototype에 보관하고, 실제 배치 시에는 위치와 콘텐츠 데이터만 전달했습니다.

MapTool에서 저장한 Prototype Tag와 Layer 정보를 이용해 동일한 생성 경로로 오브젝트를 복원할 수 있도록 구성했습니다.

---

## 3. Layer 기반 충돌 시스템

Collider는 소유 GameObject, Transform, 충돌 Layer Tag와 충돌 Callback을 관리하도록 구성했습니다.

충돌 결과에는 충돌 방향과 Overlap Depth를 포함하고, 객체는 전달받은 정보를 이용해 위치를 보정했습니다.

```text
Collider 충돌
    ↓
Layer Tag 확인
    ↓
충돌 방향 판정
    ↓
Overlap Depth만큼 위치 보정
    ↓
객체별 이벤트 실행
```

Wall·Kinetic Block·Hole·Spike·Boss Attack·Clear Portal 등 Layer별 반응을 구분해 동일한 Collider 구조에서 물리 충돌과 Trigger 콘텐츠를 처리했습니다.

---

## 4. Timer 시스템

지속 시간, 반복 여부, 활성 상태와 종료 Callback을 가진 Timer를 생성해 게임 콘텐츠의 시간 흐름을 관리했습니다.

Timer는 다음 기능에 사용했습니다.

- Coyote Time과 Jump Buffer
- 점프 입력 유지 시간
- 발걸음 및 Animation Frame
- Portal 재진입 Cooldown
- Particle Life Time
- Camera Shake
- 보스 공격 Sequence
- Clear 및 Ending 연출

보스 패턴은 하나의 Update 함수에 모든 경과 시간을 직접 비교하기보다, 각 단계의 Timer Callback에서 다음 Timer를 시작하는 방식으로 순서를 연결했습니다.

---

## 5. Key Context 입력 구조

입력을 `Enter·Stay·Exit` 단계로 나누고 기능별 Key Context에 Callback을 등록했습니다.

```text
Key Context
├─ Enter : 입력 시작
├─ Stay  : 입력 유지
└─ Exit  : 입력 종료
```

횡스크롤 이동, 탑다운 이동, 오브젝트 회전과 MapTool 조작을 서로 다른 Context로 구성했습니다.

차원이 전환되거나 캐릭터가 사망하면 해당 Context를 비활성화해 같은 키가 여러 캐릭터나 도구에 동시에 전달되는 문제를 방지했습니다.

---

## 6. 차원 상태 Callback Component

차원 상태별 `Enter·Stay·Exit` Callback을 등록할 수 있는 `DimensionSwitcher` Component를 구성했습니다.

```text
Dimension State 변경
    ↓
이전 State Exit Callback
    ↓
새 State Enter Callback
    ↓
매 Frame Stay Callback
```

플레이어, 블록과 보스가 차원 관리자를 직접 참조해 조건문을 반복하기보다, 필요한 반응을 Component에 등록할 수 있도록 했습니다.

전환 진행 비율은 Camera, 투명도와 회전 보간에도 함께 사용해 게임 규칙의 전환과 시각적 연출이 동일한 상태를 기준으로 동작하도록 구성했습니다.

---

## 7. Coyote Time·Jump Buffer·가변 점프

정확한 한 Frame에만 점프 입력을 요구하면 조작이 답답하게 느껴질 수 있어 입력 허용 구간을 Timer로 보완했습니다.

```text
발판 이탈
    ↓
Coyote Timer 유지
    ↓
Timer 안에서 Jump 입력 시 점프 허용
```

공중에서 점프 키를 먼저 누르면 Jump Buffer를 시작하고, Buffer가 유지되는 동안 착지하면 즉시 점프하도록 구성했습니다.

또한 점프 키를 짧게 놓으면 강한 중력을 적용하고 일정 시간 유지하면 약한 중력을 적용해 입력 시간에 따라 점프 높이를 조절했습니다.

---

## 8. Grid 기반 상자 상호작용

탑다운 퍼즐의 Block은 현재 Grid 좌표와 전체 Block Pointer Table을 함께 관리합니다.

상자를 들 때는 캐릭터 주변 8개 Grid를 검색하고, 캐릭터가 바라보는 방향과 후보 Block의 각도 차이를 비교해 대상 Block을 선택했습니다.

```text
주변 Grid 탐색
    ↓
Block Flag 확인
    ↓
방향과 각도 비교
    ↓
가장 적합한 Block 선택
    ↓
Parent 연결 및 Grid 점유 해제
```

상자를 내려놓을 때는 목표 Grid의 점유 상태를 검사하고 빈 공간인 경우에만 Parent를 해제한 뒤 Grid Table에 다시 등록했습니다.

---

## 9. 재귀 기반 다중 상자 밀기

상자가 일렬로 연결된 경우 마지막 상자 앞의 공간까지 확인해야 전체 이동 가능 여부를 판단할 수 있습니다.

`Search_Block()`은 다음 Grid의 상태를 재귀적으로 탐색합니다.

```text
다음 Grid
├─ 빈 공간       → 이동 가능
├─ 이동 가능 상자 → 다음 상자 재귀 탐색
└─ 고정 물체      → 이동 불가
```

탐색에 성공하면 `Push_Block()`을 호출합니다. 먼저 다음 상자의 `Push_Block()`을 호출하고 재귀가 반환되는 과정에서 현재 상자를 이동해 가장 뒤쪽 상자부터 순서대로 공간을 확보했습니다.

```text
배치 : A → B → C → 빈 공간
탐색 : A → B → C → 빈 공간
이동 : C → B → A
```

이동 시 기존 Grid Pointer를 제거한 뒤 갱신한 좌표에 다시 등록해 논리 좌표와 실제 오브젝트 상태를 일치시켰습니다.

---

## 10. 계층형 캐릭터·보스 구성

캐릭터와 보스를 몸·머리·눈·팔·손 등의 Part GameObject로 분리하고 Parent–Child Transform으로 연결했습니다.

```text
Boss Root
├─ Body
├─ Head
│  ├─ Eye
│  └─ Mouth
├─ Left Arm / Hand
└─ Right Arm / Hand
```

각 Part는 Root의 위치와 상태를 공유하면서 개별 회전, 흔들림과 Sprite Animation을 적용할 수 있도록 구성했습니다.

이를 통해 단일 Sprite 교체만으로 표현하기 어려운 눈 깜빡임, 손 공격, 꼬리 움직임과 부위별 연출을 조합했습니다.

---

## 11. 보스 패턴 Sequence

각 보스는 플레이어 추적, 대기, 피격과 공격 상태를 관리하며 패턴별 Timer를 이용해 공격 단계를 연결했습니다.

```text
패턴 선택
    ↓
준비 Animation
    ↓
공격 Object 생성
    ↓
판정 활성화
    ↓
회수·소멸
    ↓
상태 Reset
```

FireBall은 생성 지연과 추적 시간을 분리했고, Giant Hand는 등장·대기·당기기·낙하·충돌·소멸 단계를 각각 Timer로 구성했습니다.

보스 본체와 공격 Object를 분리해 각 공격의 이동, Collider와 생명주기를 독립적으로 관리했습니다.

---

## 12. Camera Shake

전투와 충돌 상황에 따라 서로 다른 형태의 Camera Shake를 적용했습니다.

- Random Shake : 임의 각도와 진폭 Offset
- Up–Down Shake : Sin 파형 기반 상하 진동
- Round Shake : 원형 Offset 기반 흔들림

상하 흔들림에는 지수 감쇠를 적용해 시작 시 충격이 크고 시간이 지날수록 자연스럽게 줄어들도록 구성했습니다.

Intensity, Frequency, Duration과 Damping Ratio를 매개변수로 전달해 보스 낙하, 충돌과 연출별로 재사용할 수 있도록 했습니다.

---

## 13. 맵 데이터 저장·불러오기

MapTool에서 배치한 GameObject의 원형 정보와 객체별 데이터를 분리해 저장했습니다.

```text
Object Original Info
├─ Prototype Level
├─ Prototype Tag
├─ Layer Level
└─ Layer Tag

Object Info
├─ Position
├─ Scale / Rotation
└─ 콘텐츠별 속성
```

Load 시 Prototype과 Layer 정보를 이용해 객체를 Clone하고 저장된 Object Data를 초기화 인자로 전달했습니다.

Editor와 실제 게임이 같은 Prototype 생성 구조를 사용하도록 해 MapTool에서 확인한 결과를 Stage에서도 재현할 수 있도록 구성했습니다.

---

## 14. 멀티스레드 리소스 로딩

Level 전환 시 Texture와 Prototype을 동기적으로 모두 생성하면 화면이 정지할 수 있어 작업 Thread에서 다음 Level의 리소스를 준비했습니다.

```text
Main Thread
├─ Loading UI Update
├─ Loading Screen Render
└─ 완료 상태 확인

Worker Thread
├─ Texture Prototype 등록
├─ Component Prototype 등록
└─ GameObject Prototype 등록
```

공유되는 Loading 상태는 Critical Section으로 보호하고, 작업 완료 후 준비된 Level로 전환했습니다.

전체 로딩 시간이 반드시 감소한다고 표현하기보다 로딩 중 화면의 응답성을 유지하는 것을 목표로 구현했습니다.

---

# 문제 해결 경험

## 1. 여러 상자가 연결되면 밀기 가능 여부와 이동 순서를 관리하기 어려운 문제

### 문제

플레이어가 상자를 미는 기능을 구현했지만 진행 방향에 다른 상자가 있으면 첫 번째 상자만 확인하는 방식으로는 여러 상자를 연속해서 밀 수 없었습니다.

연결된 상자의 개수는 상황마다 달랐고, 마지막 상자 앞에 빈 공간이 있는지 확인하기 전에 일부 상자를 이동시키면 Grid 점유 정보가 서로 어긋날 수 있었습니다.

### 원인

- 현재 상자의 바로 다음 Grid만 검사했음
- 연결된 상자의 개수가 가변적이었음
- 마지막 공간이 막혀 있으면 전체 이동을 취소해야 했음
- 앞쪽 상자부터 이동하면 아직 점유 중인 Grid로 이동하게 됐음
- 고정 Block과 다른 캐릭터를 종료 조건으로 처리해야 했음

### 해결

탐색과 실제 이동을 분리하고 두 과정에 재귀를 적용했습니다.

- `Search_Block()`에서 진행 방향의 다음 Grid를 검사했습니다.
- 빈 공간이면 이동 가능을 반환했습니다.
- 이동 가능한 상자라면 해당 상자의 `Search_Block()`을 재귀 호출했습니다.
- 고정 Block이나 다른 캐릭터가 있으면 이동 불가를 반환했습니다.
- 전체 탐색 성공 후에만 `Push_Block()`을 실행했습니다.
- `Push_Block()`에서는 다음 상자를 먼저 재귀 호출했습니다.
- 재귀가 반환되면서 가장 뒤쪽 상자부터 현재 상자까지 차례대로 이동했습니다.
- 이동할 때 이전 Grid Pointer를 제거하고 새로운 Grid에 다시 등록했습니다.

```text
Search : A → B → C → Empty
Push   : C → B → A
```

### 결과

상자 개수를 미리 제한하지 않고도 연결된 여러 상자의 이동 가능 여부를 검사하고 안전하게 연쇄 이동시킬 수 있었습니다.

마지막 공간이 막힌 경우에는 어떤 상자도 이동하지 않으며, 뒤쪽부터 공간을 확보해 Grid 점유 상태가 중간에 충돌하는 문제를 방지했습니다.

재귀를 단순 반복 처리에 사용하는 것이 아니라 **가변 길이의 연결 구조를 탐색하고 반환 순서로 역방향 갱신을 수행하는 방법**을 경험했습니다.

---

## 2. 외부 리소스의 축과 회전을 올바르게 해석하기 어려운 문제

### 문제

외부 제작 리소스를 엔진에서 표현했을 때 위치는 비슷하지만 일부 축이 반전되거나 회전 방향이 원본과 다르게 나타났습니다.

특정 축의 부호만 바꾸면 일부 리소스는 맞았지만 다른 방향의 리소스와 부모–자식 구조에서는 결과가 다시 틀어졌습니다. 코드의 숫자만 확인해서는 제작 도구가 Transform을 어떤 기준으로 해석하는지 판단하기 어려웠습니다.

### 확인한 원인

- Blender와 Engine의 Up·Forward Axis 차이
- Coordinate System Handedness 차이
- Matrix 저장·해석 방식 차이
- Euler Rotation 적용 순서
- Rotation Axis의 부호 차이
- Local·World Transform 혼용
- Parent·Child Matrix 결합 순서

### 해결

아트 작업자와 Blender의 기준 장면을 함께 확인하며 Engine 결과와 비교했습니다.

- 원점에 배치한 기준 Object를 만들었습니다.
- X·Y·Z축 이동과 각 축 90도 회전을 각각 적용했습니다.
- 비균일 Scale과 Parent–Child 구조를 가진 비교 Object를 만들었습니다.
- Blender Transform 수치, 추출 데이터와 Engine 계산 결과를 함께 비교했습니다.
- 축 교환과 부호 반전을 각각 분리해 검증했습니다.
- Euler Rotation 순서와 Matrix 곱셈 순서를 하나씩 변경해 결과를 확인했습니다.
- Parent Reference를 따라 Local Transform을 재귀적으로 결합했습니다.
- 검증된 변환 규칙을 공통 리소스 해석 과정에 적용했습니다.

```text
Blender 기준 장면
    ↓
Transform 수치와 화면 방향 확인
    ↓
추출 데이터 비교
    ↓
Engine 변환 단계별 검증
    ↓
공통 해석 규칙 확정
```

### 협업 과정

프로그래머는 행렬, 좌표계와 엔진의 계산 과정을 공유하고 아트 작업자는 Blender의 축, 회전 방식과 Export 결과를 공유했습니다.

단순히 화면이 틀어졌다는 결과만 전달하지 않고 다음 정보를 공통 기준으로 비교했습니다.

- Blender의 Transform 값
- Export된 실제 데이터
- Engine에서 해석한 값
- 각 단계의 화면 결과

이를 통해 제작 리소스, Export 과정과 Engine 해석 중 어느 단계에서 차이가 발생했는지 범위를 좁혔습니다.

### 결과

특정 리소스에만 적용되는 임시 보정값이 아니라 여러 리소스에 공통으로 적용할 수 있는 Transform 해석 규칙을 정립했습니다.

서로 사용하는 도구와 용어가 다른 상황에서도 **동일한 기준 장면과 수치 데이터를 바탕으로 의견을 교환하며 원인을 검증하는 협업 경험**을 얻었습니다.

---

## 3. 점프 입력이 누락되어 조작이 답답하게 느껴지는 문제

### 문제

지면에 닿아 있는 정확한 시점에만 점프를 허용하면 발판에서 아주 조금 벗어난 뒤의 입력과 착지 직전의 입력이 무시됐습니다.

코드상 충돌 판정은 정상이어도 플레이어 입장에서는 점프 키를 눌렀는데 반응하지 않은 것처럼 느껴졌습니다.

### 해결

- 지면에 있을 때 Coyote Timer를 갱신했습니다.
- 발판 이탈 후 Timer가 유지되는 동안 점프를 허용했습니다.
- 공중에서 입력된 점프는 Jump Buffer Timer에 저장했습니다.
- Buffer가 유지되는 동안 착지하면 즉시 점프를 실행했습니다.
- 점프 성공 시 관련 Timer를 초기화해 중복 점프를 방지했습니다.
- 입력을 놓는 시점에 중력을 크게 적용해 짧은 점프도 지원했습니다.

### 결과

발판 끝과 착지 구간에서 발생하던 입력 누락을 줄이고, 입력 유지 시간에 따라 점프 높이를 조절할 수 있는 반응성 있는 조작을 구현했습니다.

물리 판정의 정확성뿐 아니라 플레이어가 의도한 입력을 어느 범위까지 허용할지 설계하는 것이 조작감에 중요하다는 점을 경험했습니다.

---

## 4. 차원 전환 중 객체 상태가 서로 어긋나는 문제

### 문제

2D와 탑다운 차원이 전환될 때 캐릭터 입력만 교체하면 비활성 캐릭터의 Collider가 남거나, 들고 있던 상자의 Parent 및 Grid 정보가 이전 차원의 상태로 유지될 수 있었습니다.

Camera, 투명도, 입력과 충돌이 서로 다른 조건을 기준으로 갱신하면 전환 중 화면과 게임 규칙도 일치하지 않았습니다.

### 해결

- 차원 상태와 전환 비율을 Dimension Manager에서 통합 관리했습니다.
- 객체가 상태별 Enter·Stay·Exit Callback을 등록하도록 DimensionSwitcher Component를 구성했습니다.
- 차원 진입과 이탈 시 Collider 및 Key Context를 전환했습니다.
- 동일한 전환 비율로 Camera, 캐릭터 회전과 투명도를 계산했습니다.
- 상자를 든 상태에서 비활성 차원으로 전환되면 상자를 유효한 Grid에 내려놓았습니다.
- 다시 활성화될 때 필요한 경우 상자와 캐릭터 상태를 복원했습니다.

### 결과

차원 전환과 관련된 동작을 하나의 상태 기준으로 연결해 입력·충돌·카메라·오브젝트가 서로 다른 시점에 변경되는 문제를 줄였습니다.

객체별 반응은 Callback으로 분리해 새로운 오브젝트가 추가되더라도 차원 관리자 자체를 수정하지 않고 전환 동작을 등록할 수 있게 했습니다.

---

## 5. 맵 제작 시 반복적인 코드 수정과 빌드가 필요한 문제

### 문제

스테이지의 블록, 포털, 함정과 몬스터 위치를 코드에 직접 작성하면 배치를 수정할 때마다 다시 빌드해야 했습니다.

오브젝트 종류가 많아질수록 Prototype과 Layer 정보를 수동으로 입력하는 과정에서 누락이나 오타도 발생할 수 있었습니다.

### 해결

- Dear ImGui 기반 MapTool을 제작했습니다.
- Hierarchy, Inspector, Scene과 Insert 역할을 분리했습니다.
- 오브젝트를 선택하고 Click·Drag 방식으로 배치하도록 구성했습니다.
- Prototype·Level·Layer 정보와 Object Data를 함께 저장했습니다.
- JSON Data를 Load해 동일한 Prototype 생성 경로로 객체를 복원했습니다.
- Editor에서 저장한 맵을 다시 불러와 배치 결과를 확인했습니다.

### 결과

맵 배치를 변경할 때 게임 코드와 Level 초기화 함수를 반복해서 수정하는 작업을 줄였습니다.

게임 콘텐츠를 구현하는 것뿐 아니라 데이터를 반복적으로 제작·확인할 수 있는 작업 환경이 전체 개발 효율에 중요하다는 점을 경험했습니다.

---

# 향후 개선 방향

## Grid 접근 안정성 강화

현재 Block 탐색은 좌표를 정수 Index로 변환해 Pointer Table에 접근합니다. 향후에는 다음 항목을 보완하고 싶습니다.

- Grid 범위 검사 함수 통합
- 음수 및 최대 Index 접근 방어
- 재귀 탐색 중 방문 집합을 이용한 순환 방어
- 탐색 결과와 이동 대상 목록의 명시적 분리
- 이동 Transaction을 통한 일괄 적용과 실패 시 Rollback

이를 통해 잘못된 맵 데이터나 특수 Block이 추가되더라도 안전하게 연쇄 이동을 처리할 수 있도록 개선하고 싶습니다.

---

## 차원 전환 규칙의 데이터화

현재 객체별 Callback으로 전환 동작을 구성했지만 오브젝트 종류가 늘어나면 코드 등록도 함께 증가합니다.

향후 다음 데이터를 외부 설정으로 분리하고 싶습니다.

- 차원별 Collider 활성 여부
- 렌더링 및 투명도 정책
- 입력 가능 여부
- 중력과 이동 규칙
- 전환 시 위치 보정 방식

공통 규칙은 데이터로 설정하고 특수한 동작만 Callback으로 구현해 차원 콘텐츠 확장 비용을 줄이고 싶습니다.

---

## 충돌 및 공간 탐색 구조 개선

현재 퍼즐은 Grid Table을 사용하고 일반 충돌은 Layer 기반으로 검사합니다. 오브젝트 수가 증가하는 상황을 고려해 다음 구조를 적용하고 싶습니다.

- Spatial Hash 또는 Quad Tree
- Trigger와 물리 충돌 Pair 분리
- 충돌 Mask의 명시적 데이터화
- Debug Collider와 Grid 점유 상태 시각화
- 충돌 횟수와 비용 측정

---

## 성능 수치화

멀티스레드 로딩과 파티클 수명 관리 등을 구현했지만 적용 전후 성능을 체계적으로 기록하지 못했습니다.

향후 다음 항목을 측정하고 문서화하고 싶습니다.

- 동기·비동기 로딩의 Main Thread 정지 시간
- Level별 Prototype 생성 시간
- 활성 Collider 수와 충돌 검사 횟수
- Particle 수에 따른 Frame Time
- 보스 패턴별 활성 GameObject 수
- Map Data 저장·불러오기 시간

단순히 기법을 적용했다는 설명을 넘어 측정 결과로 개선 효과를 확인할 수 있도록 보완하고 싶습니다.

---

# 프로젝트를 통해 배운 점

- 2D와 탑다운이라는 서로 다른 게임 규칙을 하나의 차원 상태로 연결하며 입력, 충돌, Camera와 오브젝트 상태의 일관성을 관리하는 경험을 얻었습니다.
- Coyote Time, Jump Buffer와 가변 점프를 구현하며 정확한 물리 판정뿐 아니라 플레이어의 입력 의도를 허용하는 설계가 조작감에 미치는 영향을 배웠습니다.
- Grid Pointer Table과 재귀 탐색을 이용해 여러 상자의 이동 가능 여부를 판단하고 재귀 반환 순서로 안전하게 갱신하는 경험을 얻었습니다.
- 상자, Button, Lock, Key, Portal, Gravity와 Lightning Block을 조합하며 공통 Block 구조와 콘텐츠별 특수 동작의 책임을 나누는 방법을 고민했습니다.
- GameObject–Component와 Prototype–Clone 구조를 직접 사용하며 객체 기능의 조합과 리소스 생성 과정을 관리했습니다.
- Layer 기반 충돌과 Overlap Depth 보정을 구현하며 충돌 결과를 실제 게임플레이 규칙으로 연결했습니다.
- Timer Callback과 상태를 조합해 여러 단계로 구성된 보스 공격 패턴과 Ending Sequence를 구현했습니다.
- 계층형 Transform을 이용해 여러 Sprite Part로 구성된 캐릭터와 보스의 복합 Animation을 제작했습니다.
- Dear ImGui 기반 MapTool과 JSON 직렬화를 구현하며 콘텐츠 제작과 반복 검증을 지원하는 도구의 필요성을 경험했습니다.
- 작업 Thread와 Main Thread를 분리해 리소스를 준비하고 Loading 화면의 응답성을 유지하는 구조를 경험했습니다.
- Blender와 Engine의 Transform 결과를 함께 비교하며 서로 다른 직군이 수치와 기준 장면을 바탕으로 문제를 해결하는 협업 방식을 배웠습니다.

---

# 포트폴리오 편집 시 추가할 자료

아래 자료를 각 항목 바로 아래에 배치하면 구현 내용이 더 명확하게 전달됩니다.

1. **프로젝트 소개**: 2D와 탑다운 화면이 함께 보이는 대표 이미지 및 전체 시연 영상
2. **차원 전환 시스템**: 전환 전·중·후를 보여주는 짧은 GIF
3. **캐릭터 조작**: Coyote Time과 Jump Buffer 비교 GIF
4. **상자 상호작용**: 들기·놓기·여러 상자 연쇄 밀기 영상
5. **퍼즐 블록**: Button·Key·Portal·Gravity·Lightning Block 조합 영상
6. **보스 콘텐츠**: 4종 보스의 대표 패턴을 한 장씩 배치
7. **맵 제작 도구**: Hierarchy·Inspector·Scene·Insert가 보이는 Editor 화면
8. **문제 해결 경험**: 재귀 탐색 및 역순 이동 도식
9. **리소스 해석 협업**: Blender 기준 장면과 Engine 결과 비교 이미지

> 상단의 프로젝트 인원, 개발 기간, 담당 범위와 YouTube URL은 실제 정보로 교체해 사용합니다.
