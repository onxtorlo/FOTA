import asyncio
import aiocoap.resource as resource
import aiocoap

# 1. 업데이트 정보를 담을 리소스 정의 (RFC 9124 매니페스트 개념 반영)
class UpdateResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        # 초기 상태: 버전 1.0, 펌웨어 경로 없음
        self.payload = b"v1.0|none"
        print("[Cloud] Server Started. Resource /update is ready.")

    # Gateway(Client)가 GET 요청을 보냈을 때 실행
    async def render_get(self, request):
        print(f"[Cloud] GET request received. Sending current state: {self.payload.decode()}")
        return aiocoap.Message(payload=self.payload)

    # 연구자가 서버 내부에서 트리거를 발생시킬 때 사용하는 함수
    def trigger_fota(self, version, firmware_url):
        # 예: v1.1|http://localhost:5000/firmware.bin
        self.payload = f"{version}|{firmware_url}".encode('ascii')
        print(f"\n[Cloud] !!! FOTA TRIGGERED !!! New Version: {version}")
        
        # RFC 7641(Observe) 방식에 따라 구독 중인 게이트웨이들에게 알림 전송
        self.updated_state()

# 2. 서버 메인 루프
async def main():
    # 리소스 트리 생성 및 경로 등록
    root = resource.Site()
    update_res = UpdateResource()
    root.add_resource(['update'], update_res)

    # CoAP 서버 컨텍스트 생성 (기본 포트 5683) -> 기존
    # await aiocoap.Context.create_server_context(root)
    # 수정 코드 (비표준 포트 56830 사용 및 루프백 주소 명시)
    await aiocoap.Context.create_server_context(root, bind=('127.0.0.1', 56830))

    # 간이 트리거 시나리오 (실행 10초 후 v1.1 업데이트 발생 가정)
    await asyncio.sleep(10)
    update_res.trigger_fota("v1.1", "http://127.0.0.1:5000/download/firmware_v1.1.bin")

    # 서버 유지
    await asyncio.Future()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass