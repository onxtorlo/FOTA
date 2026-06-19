import asyncio
import aiocoap
import aiocoap.resource as resource

class DeviceShadowResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        self.current_version = b"v1.0"
        print("[Device Management] Shadow initialized. Track path: /management/version")

    async def render_get(self, request):
        print(f"[Device Management] GET received. Current Shadow Version: {self.current_version.decode()}")
        return aiocoap.Message(payload=self.current_version)

    async def render_put(self, request):
        self.current_version = request.payload
        print(f"[Device Management] Shadow Updated! Device reported version: {self.current_version.decode()}")
        self.updated_state()
        return aiocoap.Message(code=aiocoap.CHANGED, payload=b"Shadow Update Success")

async def main():
    root = resource.Site()
    device_shadow = DeviceShadowResource()
    root.add_resource(['management', 'version'], device_shadow)
    
    # 상시 관제용 포트 56831 바인딩
    await aiocoap.Context.create_server_context(root, bind=('127.0.0.1', 56831))
    print("\n[Device Management Server] Running on port 56831...")
    
    await asyncio.Future()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass

