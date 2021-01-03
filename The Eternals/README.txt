[ Prototype 패턴 사용 ]
- 멤버 변수 추가시, 복사 생성자에도 인자들을 추가.
- 객체 생성 순서
1. Create_Prototype()을 통해 프로토타입을 생성.
2. ObjectMgr에 프로토타입 추가. Add_GameObjectProtjtype("Tag")
3. 객체 생성시 ObjectMgr에서 Tag값을 탐색하고, Clone을 통한 생성.
4. 클론한 객체의 초기화는 Ready_GameObjectClone()을 통해 수행.
5. 초기화한 후, Layer에 추가.

[ Scene의 소멸자에 ObjectMgr->Clear_Layer() 호출 X ]
- 다음 씬에서 객체를 추가해도, 객체들이 모두 사라짐.
- 새로운 Scene을 동적할당 하기 전에 ObjectMgr->Clear_Layer()을 호출하고 씬을 전환한다.

[ Component Clone시 AddRef 필수 ]
- Release 모드에서 Prototype Clone시에 원본이 주소값이 똑같이 들어감.
- Safe_Release시 이중으로 해제되어 에러발생.
- Ref Cnt를 통한 참조 회수로 지워줘야 한다.


[ Key Input ]
Tab	카메라 모드 변경 (DynamicCamera <-> DebugCamera)
F1	Render On/Off	RenderTarget 
F2	Render On/Off	Font
F3	Render On/Off	Collider