# 🎮 Bar-Game

벽돌 대신 **패들·공·낙하 장애물(붉은 원)**로 플레이하는 미니 아케이드. 공은 벽/패들에 튕기고, **점수 150점** 달성 시 승리. **바닥으로 떨어지면 라이프 감소, 0이면 패배**.

## 🕹️ 조작법
- **← / →**: 패들 이동  
- **(Secret)** `P`: 라이프 +5 (디버그용)

## 게임 장면

##빨간 폭탄 피하기
<img width="410" height="353" alt="image" src="https://github.com/user-attachments/assets/34cc8111-04c7-4a72-8c84-6b986b77c55b" />

##막대 색과 같은 공 튕기기 + 색 변화
<img width="407" height="353" alt="image" src="https://github.com/user-attachments/assets/098a2efb-7904-4c5f-8b5d-dc1975784bc0" />

##빨간 폭탄 맞으면 폭발
<img width="410" height="356" alt="image" src="https://github.com/user-attachments/assets/fa63318a-ef49-4c59-b48a-b4ca0539c317" />

##승리 화면
<img width="405" height="341" alt="image" src="https://github.com/user-attachments/assets/3826641c-6fa2-46e7-beea-981a705ed855" />

##패배 화면
<img width="401" height="347" alt="image" src="https://github.com/user-attachments/assets/45674a0a-d208-463a-bd17-8219fe834dd5" />

## ✨ 핵심 기능
- 공 반사 물리(상·하·좌·우), 패들 충돌 시 **가속도 점증**(점수 50마다 속도 업)  
- 주기적 **붉은 폭탄** 스폰 & 하강, 패들과 충돌 시 라이프 감소/폭발 이펙트  
- HUD: `Score / Life`, 컬러 변화, 짧은 충돌 이펙트(1초 표시)  
- **승/패 시퀀스**: 승리 이미지 순차 표시+박수 사운드 / 실패 이미지+경고음

## 🧪 기술(Tech)
- **C++17**, **SFML (Graphics/Audio/Window/System)**  
- 렌더링 루프(60 FPS), 이벤트 처리, **Update→Draw** 분리  
- 효과 타이밍: `sf::Clock`로 1초 이펙트 제어  
- 리소스 로드 실패 시 콘솔 경고

## 🎯 프로젝트 의의 & 소감
- 실시간 루프, 충돌 판정, 사운드/그래픽 자원 관리까지 **게임의 뼈대**를 구현해 OOP 감각 강화  
- 규칙(점수·라이프·속도 상승)을 스스로 설계/튜닝하며 **시스템 사고** 향상  
- **게임 형식으로 C++를 배우니까 재밌었다** **즉각 피드백으로 개념 체화가 빨랐음**
