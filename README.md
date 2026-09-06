 Todee & Topdee

> DirectX 9로 구현한 2D, 3D 차원을 넘나드는 플랫포머 퍼즐게임

- 플레이 영상: https://www.youtube.com/watch?v=edF-MzIo6zc&t=98s
- 개발 인원: 4인 개발
- 담당 역할: 3D 상태 플레이어 및 보스 1종(2번째 보스), 오브젝트와의 상호작용
- 플랫폼: Windows

- 담당한 코드만 첨부했습니다
## 프로젝트 소개

C++과 DirectX 9을 기반으로 제작한 2D 퍼즐 액션 게임입니다.

플레이어는 횡스크롤 캐릭터 **Toodee**와 탑다운 캐릭터 **Topdee**를 조작하며, 2D와 탑다운 차원을 전환해 서로 다른 규칙으로 스테이지의 퍼즐을 해결합니다.

횡스크롤 차원에서는 이동과 점프를 이용해 지형을 돌파하고, 탑다운 차원에서는 상자를 들거나 연속해서 밀고, 버튼·열쇠·포털·중력 및 전기 블록을 조작합니다. 같은 맵이라도 선택한 차원에 따라 캐릭터의 입력, 카메라, 충돌과 오브젝트 동작이 달라지도록 구성했습니다.

## 개발 환경

| 구분 | 사용 기술 |
| --- | --- |
| Language | C++17 |
| Graphics | DirectX 9 / D3DX9 |
| UI / Tool | Dear ImGui |
| Data | nlohmann/json |
| Audio | FMOD |
| IDE / Build | Visual Studio 2022 / MSVC v143 |
| Platform | Windows |

### 외부 라이브러리 활용

#### Dear ImGui

Hierarchy, Inspector, Scene과 Insert 영역으로 구성된 Map Tool을 구현하는 데 사용했습니다.

오브젝트를 직접 배치하고 Transform과 콘텐츠별 속성을 수정하며, 작성한 맵 데이터를 저장하고 다시 불러올 수 있도록 구성했습니다.

#### nlohmann/json

Map Tool에서 배치한 GameObject의 Prototype, Level, Layer, Transform과 콘텐츠별 데이터를 직렬화하고 다시 복원하는 데 사용했습니다.

#### FMOD

캐릭터의 이동·점프·착지, 상자 상호작용, 퍼즐 오브젝트, 보스 패턴과 UI 등의 효과음 및 배경음 재생에 사용했습니다.

용도별 Channel을 분리해 사운드를 관리했습니다.

---

# 담당 기능

## 탑다운 캐릭터 시스템

- 8방향 이동과 대각선 속도 정규화
- 이동 방향에 따른 캐릭터 회전 및 Animation 전환
- 주변 Grid 검색 기반 상자 선택
- 상자 들기·운반·내려놓기
- 충돌 방향과 입력 방향을 이용한 상자 밀기
- 재귀 탐색을 이용한 다중 상자 연쇄 이동
- 고정 Block·다른 캐릭터·Grid 점유 상태 기반 이동 제한

## 퍼즐 블록 시스템

- 일반 Wall과 이동 가능한 Kinetic Block
- 운반·이동·고정 여부를 구분하는 Block Flag
- 캐릭터와 상자를 감지하는 Button
- Button과 연동해 열리고 닫히는 Wall
- Key 획득과 Lock 해제
- 입구와 출구를 연결하는 Portal Block
- 차원에 따라 낙하 방향이 달라지는 Gravity Block
- 운반 중 전기 상태가 활성화되는 Lightning Block
- Hole·Spike·Lightning 등의 장애물 판정
- Clear Portal을 이용한 Stage 종료

## 몬스터 시스템

- Player 행동을 따라하는 몬스터 구현
- Player 상태에 따른  행동 전환
- Monster별 Sprite Animation과 사망 처리
- Monster별 2D, 3D 상태 구현

## 보스

- Yumbrozi (2번째 원숭이 보스) 구현
- Body·Head·Eye·Arm·Hand·Tail 등의 Part 계층 구성
- Chase·Idle·Hit·Attack·Reset 상태 관리
- 차원 상태 별 투사체 변환
- Timer Callback 기반 단계별 공격 Sequence
- Player와 차원 상태를 반영한 Pattern 선택
- Boss HP와 UI 연동
- Attack과 Particle·Sound·Camera Shake 연동

# 기술 설명

## 1. Grid 기반 상자 상호작용

탑다운 퍼즐의 Block은 자신의 Grid 좌표와 전체 Block을 조회할 수 있는 Pointer Table을 이용해 공간 상태를 관리했습니다.

상자를 들 때는 Player 주변의 8개 Grid를 탐색합니다.

```
주변 Grid 탐색
    ↓
Block Flag 확인
    ↓
Player 방향과 각도 비교
    ↓
가장 적합한 Block 선택
    ↓
Parent 연결
    ↓
기존 Grid 점유 해제
```

후보 Block 중 운반 가능한 객체를 선별하고 Player가 바라보는 방향과 후보 Block 사이의 각도 차이를 비교해 가장 적합한 대상을 선택했습니다.

상자를 내려놓을 때는 목표 Grid가 비어 있는지 먼저 확인하고, 유효한 위치인 경우에만 Parent를 해제한 뒤 Block Pointer Table에 다시 등록했습니다.

이를 통해 화면상의 위치와 퍼즐에서 사용하는 논리 Grid 상태를 함께 관리했습니다.

---

## 2. 재귀 기반 다중 상자 밀기

여러 개의 상자가 일렬로 연결되어 있을 경우 첫 번째 상자의 다음 공간만 검사해서는 전체 상자가 이동 가능한지 판단할 수 없었습니다.

이에 상자의 이동 가능 여부를 확인하는 탐색과 실제 이동 과정을 분리하고 재귀를 적용했습니다.

```
다음 Grid
├─ 빈 공간        → 이동 가능
├─ 이동 가능한 상자 → 다음 상자 재귀 탐색
└─ 고정 객체       → 이동 불가
```

`Search_Block()`은 진행 방향의 다음 Grid를 확인하고 이동 가능한 상자가 존재하면 해당 상자의 `Search_Block()`을 다시 호출합니다.

전체 탐색이 성공한 경우에만 실제 이동을 시작합니다.

```
배치 : A → B → C → Empty

탐색 : A → B → C → Empty

이동 : C → B → A
```

실제 이동 과정에서는 다음 상자의 `Push_Block()`을 먼저 호출하고, 재귀가 반환되는 과정에서 현재 상자를 이동시켰습니다.

이를 통해 가장 뒤쪽 상자부터 공간을 확보하며 이동하도록 구성해 Grid 점유 정보가 서로 충돌하는 문제를 방지했습니다.

---

## 3. 계층형 캐릭터·보스 구성

캐릭터와 Boss를 Body·Head·Eye·Arm·Hand 등의 여러 Part GameObject로 분리하고 Parent–Child Transform 관계로 연결했습니다.

```
Boss Root
├─ Body
├─ Head
│  ├─ Eye
│  └─ Mouth
├─ Left Arm / Hand
└─ Right Arm / Hand
```

각 Part는 Root의 위치와 상태를 공유하면서 개별적인 회전, 흔들림과 Sprite Animation을 적용할 수 있도록 구성했습니다.

이를 통해 하나의 Sprite만 교체하는 방식보다 눈 깜빡임, 손 공격, 꼬리 움직임 등의 여러 Animation을 조합할 수 있도록 했습니다.

---

## 4. 보스 패턴 Sequence

Boss는 Player 추적, Idle, Hit과 Attack 상태를 관리하며 각 Pattern의 진행 과정은 Timer를 이용해 단계별로 구성했습니다.

```
Pattern 선택
    ↓
준비 Animation
    ↓
Attack Object 생성
    ↓
Collision 활성화
    ↓
회수·소멸
    ↓
State Reset
```

FireBall은 생성 지연과 Player 추적 시간을 분리했고, Giant Hand는 등장·대기·당기기·낙하·충돌·소멸 과정을 각각 별도의 Timer 단계로 구성했습니다.

Boss 본체와 실제 Attack Object도 분리해 각 공격의 위치, Collider와 생명주기를 독립적으로 관리했습니다.

이를 통해 Pattern의 실행 순서와 실제 공격 객체의 책임을 분리했습니다.

---

# 문제 해결 경험

## 1. 여러 상자가 연결되면 이동 가능 여부와 이동 순서를 관리하기 어려운 문제

### 문제

Player가 상자를 미는 기능을 구현했지만 진행 방향에 다른 상자가 존재하면 첫 번째 상자의 다음 공간만 검사하는 방식으로는 여러 상자를 연속해서 밀 수 없었습니다.

연결된 상자의 개수는 상황마다 달라졌으며 마지막 상자 앞의 공간이 비어 있는지 확인하기 전에 일부 상자를 움직이면 Grid 점유 정보가 서로 어긋날 수 있었습니다.

### 원인

- 현재 상자의 바로 다음 Grid만 검사
- 연결된 상자 개수가 가변적
- 마지막 공간이 막혀 있다면 전체 이동을 취소해야 함
- 앞쪽 상자부터 이동하면 아직 점유 중인 Grid와 충돌
- 고정 Block과 다른 캐릭터를 종료 조건으로 처리해야 함

### 해결

탐색과 실제 이동 과정을 분리하고 두 과정에 재귀를 적용했습니다.

- `Search_Block()`에서 진행 방향의 다음 Grid를 검사했습니다.
- 빈 공간이면 이동 가능을 반환했습니다.
- 이동 가능한 상자라면 해당 상자의 `Search_Block()`을 재귀 호출했습니다.
- 고정 Block이나 다른 캐릭터가 있으면 이동 불가를 반환했습니다.
- 전체 탐색에 성공한 이후에만 `Push_Block()`을 실행했습니다.
- `Push_Block()`에서는 다음 상자를 먼저 재귀 호출했습니다.
- 재귀가 반환되면서 가장 뒤쪽 상자부터 현재 상자까지 순서대로 이동했습니다.
- 이동 전후 Grid Pointer를 갱신해 실제 위치와 논리 Grid를 일치시켰습니다.

```
Search : A → B → C → Empty

Push   : C → B → A
```

### 결과

상자의 개수를 미리 제한하지 않고도 연결된 여러 상자의 이동 가능 여부를 검사하고 연쇄적으로 이동시킬 수 있었습니다.

마지막 공간이 막혀 있으면 어떤 상자도 이동하지 않도록 했으며, 가장 뒤쪽 상자부터 공간을 확보해 Grid 점유 상태가 중간에 충돌하는 문제를 방지했습니다.

재귀를 단순 반복 처리에 사용하는 것이 아니라 **가변 길이의 연결 구조를 탐색하고 반환 순서를 이용해 역방향으로 상태를 갱신하는 경험**을 얻었습니다.

---

## 2. 3D 연출 리소스 구조를 해석하기 어려운 문제

### 문제

Topdee 캐릭터와 차원 전환 과정에서 사용되는 리소스를 처음 확인했을 때, 화면에서는 입체적인 형태로 표현되지만 제공된 리소스만으로는 어떤 방식으로 3D 공간을 구성하고 있는지 정확한 구조를 파악하기 어려웠습니다.

비슷한 모작 프로젝트와 자료도 찾아보았지만 구현 방식을 명확하게 확인할 수 없었으며, 단순한 3D Mesh인지 여러 Sprite를 조합한 방식인지 판단할 수 있는 기준도 부족했습니다.

### 해결

팀장과 이틀 동안 여러 가능성을 가설로 세우고 실제 리소스와 원본 게임 화면을 비교하며 하나씩 검증했습니다.

- 리소스의 이미지 크기와 배치 형태를 분석했습니다.
- 단일 Plane, 3D Mesh, 여러 Sprite 배치 등의 가능성을 각각 검토했습니다.
- 원본 게임의 카메라 각도와 캐릭터가 회전할 때 나타나는 단면 변화를 비교했습니다.
- Blender에 Sprite를 직접 배치해 각 가설의 결과를 재현했습니다.
- 서로 다른 각도에서 원본과 결과를 반복적으로 비교했습니다.
- 검증 결과, 여러 장의 Sprite를 깊이 방향으로 일정 간격 쌓아 입체감을 표현하는 **Sprite Stack 방식**임을 확인했습니다.
- 확인한 구조를 기준으로 엔진에서 동일한 리소스 배치와 차원 전환 연출을 구현했습니다.

### 결과

!SpriteStack.gif

명확한 구현 자료가 없는 상황에서도 팀원과 가설을 공유하고 직접 검증하는 과정을 반복해 원본의 **Sprite Stack 기반 리소스 구성 방식**을 해석할 수 있었습니다.

제가 확인한 당시 모작 사례들에서는 동일한 구조를 정확하게 재현한 사례를 찾기 어려웠지만, 리소스 구조를 직접 분석한 결과 원작에 가까운 3D 표현과 차원 전환 연출을 구현할 수 있었고 프로젝트의 시각적 완성도를 높일 수 있었습니다.

또한 정답이 주어지지 않은 문제에서도 **가설을 세우고 팀원과 근거를 공유하며 실제 결과로 검증해 나가는 문제 해결 및 협업 경험**을 얻었습니다.

---

# 프로젝트를 통해 배운 점

- 2D와 Top Down이라는 서로 다른 게임 규칙을 하나의 Dimension State로 연결하며 입력·Collider·Camera와 Object 상태의 일관성을 관리하는 경험을 얻었습니다.
- Coyote Time, Jump Buffer와 가변 Jump를 구현하면서 물리적으로 정확한 판정뿐 아니라 Player의 입력 의도를 보완하는 과정이 조작감에 중요하다는 점을 배웠습니다.
- Grid Pointer Table과 재귀 탐색을 이용해 가변 길이의 상자 연결 구조를 탐색하고 재귀 반환 순서로 상태를 갱신하는 경험을 얻었습니다.
- Block·Button·Key·Lock·Portal·Gravity·Lightning 등의 퍼즐 콘텐츠를 구현하며 공통 Block 기능과 콘텐츠별 특수 동작의 책임을 분리했습니다.
- GameObject–Component와 Prototype–Clone 구조를 활용하면서 객체 기능의 조합과 Resource 생성 과정을 관리하는 경험을 얻었습니다.
- Layer 기반 Collision과 Overlap Depth 보정을 구현하며 충돌 결과를 실제 Gameplay 규칙과 연결했습니다.
- Timer Callback과 상태를 조합해 여러 단계로 구성된 Boss Attack Pattern과 Ending Sequence를 구현했습니다.
- Parent–Child Transform을 이용해 여러 Sprite Part로 구성된 캐릭터와 Boss의 복합 Animation을 제작했습니다.
- Dear ImGui 기반 Map Tool과 JSON 직렬화를 구현하며 콘텐츠를 빠르게 제작하고 반복 검증할 수 있는 Tool의 필요성을 경험했습니다.
- 팀 작업 시 문서화를 통한 서로의 작업 기준과 진행 상황을 맞춰 원활한 협업을 경험함으로써 문서화의 중요함을 다시 한번 느꼈습니다.
