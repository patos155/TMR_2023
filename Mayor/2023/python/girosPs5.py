from pydualsense import *
import serial
import time
import os

gyro_data = [0,0,0]
gyro_offsets = [0, 0, 0]

ejeX = 0
ejeY = 0

inicio = ""

datos = [0,0,0,0] # maxX minX maxY minY

try:
    arduino = serial.Serial("COM4", 9600) ##COM5
    time.sleep(0.1) 
    if arduino.isOpen():
        print("{} conexion establecida!!".format(arduino.port))
    else:
        print("alchile no c q paso :v")

    def maxmin(ini = 1):
        global datos, inicio
        input()
        print("Obteniendo datos...")

        for i in range(10000):
            print("ejeX ",ejeX," ejeY ",ejeY)
            if datos[0] > ejeX:
                datos[0] = ejeX
            if datos[1] < ejeX:
                datos[1] = ejeX
            if datos[2] > ejeY:
                datos[2] = ejeY
            if datos[3] < ejeY:
                datos[3] = ejeY
            if ini == 4:
                time.sleep(0.001)
        print(datos)
        for i in range(4):
            if ini:
                if i/2 == 0 or i/2 == 1:
                    inicio += str(datos[i]+5)+"|"
                else:
                    inicio += str(datos[i]-5)+"|"
            else:
                inicio += str(datos[i])+"|"

    def calibracion():
        global gyro_offsets
        print("Coloca el giroscopio en una posición estable y presiona Enter para comenzar la calibración.")
        input()
        print("Calibrando...")

        gyro_offsets = [0, 0, 0]  # Valores de compensación para cada eje (x, y, z)

    # Realizar varias lecturas y calcular el promedio para obtener los valores de compensación
        num_samples = 100
        for _ in range(num_samples):
            gyro_offsets[0] += gyro_data[0]
            gyro_offsets[1] += gyro_data[1]
            gyro_offsets[2] += gyro_data[2]
            time.sleep(0.01)

        gyro_offsets[0] /= num_samples
        gyro_offsets[1] /= num_samples
        gyro_offsets[2] /= num_samples

        print("Calibración completa.")
        print("Valores de compensación del giroscopio (x, y, z):", gyro_offsets)

    def giroscopio_changed(pitch, yaw, roll):
        global gyro_data
        gyro_data[0] = pitch
        gyro_data[1] = yaw
        gyro_data[2] = roll        

    def jr(stateX, stateY):
        global ejeX, ejeY
        #print(f'joystick {stateX} {stateY}')
        ejeX = stateX
        ejeY = stateY

    dualsense = pydualsense()
    dualsense.init()

    dualsense.gyro_changed += giroscopio_changed
    dualsense.right_joystick_changed += jr

    print("Deja quieto el control y presiona Enter para establecer la zona muerta: ")
    maxmin()

    print("Zona muerta establecida :D\nDa Enter y Gira el joystick hasta que aparesca la notificacion para establecer maximos y minimos")
    maxmin(0)

    print("Datos de inicio establecidos.....")
    #inicio = "5|-5|5|-5|128|-127|128|-127|"
    print(inicio)
    arduino.write(inicio.encode('utf-8'))
    time.sleep(1)

    print("programa iniciado....")

    while not dualsense.state.R1:
        '''calibrated_gyro = [
            gyro_data[0] - gyro_offsets[0],
            gyro_data[1] - gyro_offsets[1],
            gyro_data[2] - gyro_offsets[2]
        ]

        print(calibrated_gyro)
        os.system('cls')'''
        msg = str(ejeX)+"|"+str(ejeY)
        arduino.write(msg.encode('utf-8'))
        time.sleep(0.1)

    '''determinar ejes'''

    dualsense.close()
    #arduino.close()

except Exception as e:
    print("Error: {}".format(e))
