import socket
import sys
import traceback
from threading import Thread
import hashlib
import util as mylib

def main():
    start_server()


def start_server():
    host = "127.0.0.1"
    port = 8890 

    soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    soc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  
    print("Socket created")

    try:
        soc.bind((host, port))
    except:
        print("Bind failed. Error : " + str(sys.exc_info()))
        sys.exit()

    soc.listen(5) 
    print("Socket now listening")

    while True:
        connection, address = soc.accept()
        ip, port = str(address[0]), str(address[1])
        print("Connected with " + ip + ":" + port)

        try:
            Thread(target=client_thread, args=(connection, ip, port)).start()
        except:
            print("Thread did not start.")
            traceback.print_exc()

    soc.close()


def client_thread(connection, ip, port, max_buffer_size = 5120):
    is_active = True

    while is_active:
        client_input = receive_input(connection, max_buffer_size)

        if "--quit--" in client_input:
            print("Client is requesting to quit")
            connection.close()
            print("Connection " + ip + ":" + port + " closed")
            is_active = False
        else:
            inputs=client_input.split()
            print(inputs)
            if int(inputs[0])==10:
                print('Request for 10')
                q= int(inputs[1])
                g= int(inputs[2])
                y1= int(inputs[3])
                y2= int(inputs[4])
                print('prime:',q)
                print('g:',g)
                print('y1:',y1)
                print('y2:',y2)
            elif int(inputs[0])==20:
                c=int(inputs[1])
                s=int(inputs[2])
                m=inputs[3]
                print('c',c)
                print('s',s)
                print('m',m)
                print('q',q)
                print('y1',y1)
                print('y2',y2)
                # verify this while debugging
                yA=mylib.modInverse(y1,q)
                A_d=(mylib.power(g,s,q)*mylib.power(yA,c,q))%q

                yB=mylib.modInverse(y2,q)
                B_d=(mylib.power(y1,s,q)*mylib.power(yB,c,q))%q

                hash=str(A_d)+str(B_d)+m
                hash_object = hashlib.sha1(hash.encode())
                hex_dig = hash_object.hexdigest()
                c_d=int(hex_dig,16)
                print('c_d',c_d)
                if c==c_d:
                    print("Matched")
                    message="matched"
                    connection.sendall(message.encode("utf8"))
                else:
                    print("Not matched")
                    message="not matched"
                    connection.sendall(message.encode("utf8"))

def receive_input(connection, max_buffer_size):
    client_input = connection.recv(max_buffer_size)
    client_input_size = sys.getsizeof(client_input)

    if client_input_size > max_buffer_size:
        print("The input size is greater than expected {}".format(client_input_size))

    decoded_input = client_input.decode("utf8").rstrip()
    result = process_input(decoded_input)

    return result


def process_input(input_str):
    print("Processing the input received from client")

    return str(input_str)

if __name__ == "__main__":
    main()