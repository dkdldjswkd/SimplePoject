# 프로젝트 모음

C++와 네트워크 관련 개념을 공부하며 구현한 프로젝트 모음입니다.  
각 프로젝트는 학습 목적 또는 [락프리 게임서버 프로젝트](https://github.com/dkdldjswkd/lockfree-gameserver-core-cpp)를 위해 작성된 코드로서, 네트워크, 자료구조, 알고리즘 등의 다양한 주제를 다룹니다.

## 주요 프로젝트

### 1. **AsyncSelect 그림판 (AsyncSelect I/O 모델을 사용한 비동기 I/O 처리)**
`AsyncSelect` I/O 모델을 사용한 그림판 서버/클라이언트 프로젝트입니다.  
클라이언트가 그리는 그림을 서버에 연결된 다른 클라이언트에게 브로드캐스트합니다.

---

### 2. **A\* (A 스타 알고리즘)**
최단 경로를 찾는 알고리즘인 A\* 알고리즘을 구현했습니다.  
A\* 알고리즘은 출발 지점에서 목적 지점까지 가는 과정에서 새로운 지점을 Priority Queue에 추가하고, 가장 우선순위가 높은 지점부터 탐색을 반복하는 길찾기 알고리즘입니다.  
[![A\* 알고리즘 동영상](https://img.youtube.com/vi/trbj_MLWv2k/0.jpg)](https://youtu.be/trbj_MLWv2k)  
*(이미지 클릭 시 동영상으로 연결됩니다.)*

---

### 3. **JPS (Jump Point Search)**
A\* 알고리즘을 개선한 JPS 알고리즘 예제입니다.  
A\*가 인접한 모든 지점을 Priority Queue에 추가하는 반면, JPS는 충돌 정보를 기반으로 중요한 분기점만 추가하여, 탐색 거리가 멀어질수록 A\*보다 빠른 경로 탐색 성능을 보입니다.  
[![JPS 알고리즘 동영상](https://img.youtube.com/vi/sMQnVRWcX2Y/0.jpg)](https://youtu.be/sMQnVRWcX2Y)  
*(이미지 클릭 시 동영상으로 연결됩니다.)*

---

### 4. **LockFreeStack (락프리 스택)**
멀티 스레드 환경에서 락 없이 안전하게 사용할 수 있는 락프리 스택을 구현했습니다.

---

### 5. **MMO TCPFighter Server**
간단한 MMO 프로젝트로, 섹터 기법을 활용하여 맵을 분할함으로써, 공격 시 인접 섹터의 플레이어만 검색해 탐색 비용을 줄였으며,  
send 시에도 인접 섹터의 플레이어에게만 송신하여 시스템 콜 오버헤드를 최소화했습니다.  
또한, 클라이언트가 전송한 이동 좌표와 서버의 좌표 간 오차가 허용 범위를 초과하면 위치 동기화 패킷을 송신하여 정확성을 유지하도록 구현했습니다.  
[![MMO TCPFighter Server 동영상](https://img.youtube.com/vi/HaJeUk6SPZY/0.jpg)](https://youtu.be/HaJeUk6SPZY)  
*(이미지 클릭 시 동영상으로 연결됩니다.)*
