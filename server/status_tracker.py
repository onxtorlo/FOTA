import asyncio
import aiocoap
import aiocoap.resource as resource

class FotaJobResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        self.job_payload = b"NO_JOB"
        print("[Status Tracker] FOTA Job Engine initialized. Control path: /trigger/fota")

    async def render_get(self, request):
        print(f"[Status Tracker] GET/Observe request for FOTA Job. Current Job: {self.job_payload.decode()}")
        return aiocoap.Message(payload=self.job_payload)

    def trigger_fota(self, version, firmware_url):
        self.job_payload = f"{version}|{firmware_url}".encode('ascii')
        print(f"\n[Status Tracker] !!! FOTA JOB CREATED !!! New Targets: {version}")
        self.updated_state()

async def main():
    root = resource.Site()
    fota_job = FotaJobResource()
    root.add_resource(['trigger', 'fota'], fota_job)
    
    # FOTA 트리거용 포트 56832 바인딩
    await aiocoap.Context.create_server_context(root, bind=('127.0.0.1', 56832))
    print("\n[Status Tracker Server] Running on port 56832...")
    
    # 간이 FOTA 트리거 시나리오 (서버 구동 10초 후 v1.1 FOTA Job 강제 발생)
    await asyncio.sleep(10)
    fota_job.trigger_fota("v1.1", "http://127.0.0.1:5000/download/firmware_v1.1.bin")
    
    await asyncio.Future()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass

