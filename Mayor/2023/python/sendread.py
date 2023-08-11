import serial, time, os

i = 0
arduino = serial.Serial("COM5", 115200)
time.sleep(0.1) #wait for serial to open

if arduino.isOpen():
    print("{} conexion establecida!!".format(arduino.port))
    while True:      
        arduino.write("x12\n".encode('utf-8'))     
        time.sleep(0.1)
        answer=str(arduino.readline().decode('utf-8'))
        #print(answer)   
        datos = [float(x) for x in answer.split(",")]
        print(datos)
        #os.system('cls')    
        #while arduino.inWaiting()==0: pass
        #i += 1
else:
    print("alchile no c q paso :v")

    
    