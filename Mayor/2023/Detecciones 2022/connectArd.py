import serial,time, socket #se importan las librerias serial (conexion) y time(control de tiempo)
import RPi.GPIO as GPIO
import subprocess

HEADER = 64
PORT = 5050
FORMAT = 'UTF-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = "192.168.3.3"
ADDR = (SERVER, PORT)

GPIO.setmode(GPIO.BOARD)
GPIO.setup(3, GPIO.IN, pull_up_down=GPIO.PUD_UP)

arduino1 = serial.Serial('/dev/ttyUSB0', 9600)
arduino2 = serial.Serial('/dev/ttyUSB1', 9600)
arduino3 = serial.Serial('/dev/ttyACM0', 9600)

def send(msg):
    message = msg.encode(FORMAT)
    msg_length = len(message)
    send_length = str(msg_length).encode(FORMAT)
    send_length += b' ' * (HEADER - len(send_length))
    client.send(send_length)
    client.send(message)

if __name__ == '__main__':
    print('Running. Press CTRL-C to exit.')

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(ADDR)

    while True:      
        input_state = GPIO.input(3)   
        if input_state == False:
            print("boton Pulsado")
            send("BOTON PULSADO")
            subprocess.run(['sudo', 'shutdown', '-h', 'now'])
            
        #send("algo")
        answer=str(arduino1.readline())
        #print("---> {}".format(answer))
        answer = answer.replace("b'","")
        answer = answer.replace("\\r\\n'","")
        dataList=answer.split("x")
        #print(dataList)
        print("Gas1d : {}".format(dataList[0]))
        send("Gas1d : {}".format(dataList[0]))
        print("Gas1a : {}".format(dataList[1]))
        send("Gas1a : {}".format(dataList[1]))
        print("Gas2d : {}".format(dataList[2]))
        send("Gas2d : {}".format(dataList[2]))
        print("Gas2a : {}".format(dataList[3]))
        send("Gas2a : {}".format(dataList[3]))
        print("Gas3d : {}".format(dataList[4]))
        send("Gas3d : {}".format(dataList[4]))
        print("Gas3a : {}".format(dataList[5]))
        send("Gas3a : {}".format(dataList[5]))
        print("Humedad : {}".format(dataList[6]))
        send("Humedad : {}".format(dataList[6]))
        print("Temperatura C : {}".format(dataList[7]))
        send("Temperatura C : {}".format(dataList[7]))
            
        arduino1.flushInput()