import socket
import sys
import random
import hashlib
import time
import util as mylib

def main():
    soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = "127.0.0.1"
    port = 8890

    try:
        soc.connect((host, port))
    except:
        print("Connection error")
        sys.exit()

    q=mylib.get_prime()
    g=mylib.primRoots(q)
    a=random.randint(1,q-1)
    print('Prime',q)
    print('Private key',a)
    y1=mylib.power(g,a,q)
    print('y1',y1)
    y2=mylib.power(y1,a,q)
    print('y2',y2)
    # Public key generation

    q_str=str(q)
    g_str=str(g)
    y1_str=str(y1)
    y2_str=str(y2)
    message='10'+' '+q_str+' '+g_str+' '+y1_str+' '+y2_str
    
    #send public key
    soc.sendall(message.encode("utf8"))

    # calculate hash and generate signature
    r=random.randint(1,q-1)
    A=mylib.power(g,r,q)
    B=mylib.power(y1,r,q)
    m="10101010"

    hash=str(A)+str(B)+m

    hash_object = hashlib.sha1(hash.encode())
    hex_dig = hash_object.hexdigest()
    c=int(hex_dig,16)
    print('c',c)

    s=a*c+r%q

    message='20'+' '+str(c)+' '+str(s)+' '+m
    soc.sendall(message.encode("utf8"))

    server_response= soc.recv(5120)
    server_response_size = sys.getsizeof(server_response)

    if server_response_size > 5120:
        print("The input size is greater than expected {}".format(server_response_size))
    decoded_input = server_response.decode("utf8").rstrip()
    print(str(decoded_input))
    
    time.sleep(1)
    soc.send(b'--quit--')

if __name__ == "__main__":
    main()
