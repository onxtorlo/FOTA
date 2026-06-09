import asyncio
import logging
import requests
from aiocoap import *

logging.basicConfig(level=logging.INFO)

async def main():
    protocol = await Context.create_client_context()
    # 포트 번호가 56830이 맞는지 다시 확인!
    uri = "coap://localhost:56830/update"
    
    # Observe 옵션을 주어 서버의 변화를 실시간으로 감시
    request = Message(code=GET, uri=uri, observe=0)
    
    try:
        requester = protocol.request(request)
        print(f"[Gateway] Monitoring Cloud Server at {uri}...")

        async for response in requester.observation:
            payload = response.payload.decode('utf-8')
            print(f"[Gateway] Received Update Info: {payload}")
            
            # 예: "v1.1|http://localhost:5000/download/firmware_v1.1.bin"
            if "|" in payload:
                version, url = payload.split("|")
                
                if url.startswith("http"):
                    print(f"[Gateway] Triggering Automatic Download for {version}...")
                    try:
                        # 여기서 실제로 파일을 긁어옵니다.
                        file_res = requests.get(url, timeout=5)
                        if file_res.status_code == 200:
                            filename = f"downloaded_{version}.bin"
                            with open(filename, "wb") as f:
                                f.write(file_res.content)
                            print(f"[Gateway] SUCCESS: {filename} saved automatically!")
                        else:
                            print(f"[Gateway] FAILED: Server returned status {file_res.status_code}")
                    except Exception as e:
                        print(f"[Gateway] Download Error: {e}")
                else:
                    print("[Gateway] No new firmware URL provided yet.")

    except Exception as e:
        print(f"[Gateway] Connection Error: {e}")

if __name__ == "__main__":
    asyncio.run(main())