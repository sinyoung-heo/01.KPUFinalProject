[ Prototype ���� ��� ]
- ��� ���� �߰���, ���� �����ڿ��� ���ڵ��� �߰�.
- ��ü ���� ����
1. Create_Prototype()�� ���� ������Ÿ���� ����.
2. ObjectMgr�� ������Ÿ�� �߰�. Add_GameObjectProtjtype("Tag")
3. ��ü ������ ObjectMgr���� Tag���� Ž���ϰ�, Clone�� ���� ����.
4. Ŭ���� ��ü�� �ʱ�ȭ�� Ready_GameObjectClone()�� ���� ����.
5. �ʱ�ȭ�� ��, Layer�� �߰�.

[ Scene�� �Ҹ��ڿ� ObjectMgr->Clear_Layer() ȣ�� X ]
- ���� ������ ��ü�� �߰��ص�, ��ü���� ��� �����.
- ���ο� Scene�� �����Ҵ� �ϱ� ���� ObjectMgr->Clear_Layer()�� ȣ���ϰ� ���� ��ȯ�Ѵ�.

[ Component Clone�� AddRef �ʼ� ]
- Release ��忡�� Prototype Clone�ÿ� ������ �ּҰ��� �Ȱ��� ��.
- Safe_Release�� �������� �����Ǿ� �����߻�.
- Ref Cnt�� ���� ���� ȸ���� ������� �Ѵ�.


[ Key Input ]
Tab	ī�޶� ��� ���� (DynamicCamera <-> DebugCamera)
F1	Render On/Off	RenderTarget 
F2	Render On/Off	Font
F3	Render On/Off	Collider