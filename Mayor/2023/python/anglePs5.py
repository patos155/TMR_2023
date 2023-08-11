from pydualsense import *
import math
import os

FREQ = 30.0
gSensitivity = 65.5
accel_data = [0,0,0]
gyro_data = [0,0,0]
gyro_offsets = [0, 0, 0]
pitch = 0
yaw = 0
roll = 0

def calculate_angles(gyro_data, accel_data):
    global pitch, yaw, roll
    # Convert raw gyro data to radians per second
    #gyro_x, gyro_y, gyro_z = [math.radians(g) for g in gyro_data]

    gyro_x, gyro_y, gyro_z = gyro_data

    # Convert raw accel data to g-force
    accel_x, accel_y, accel_z = accel_data

    # Calculate pitch (rotation around X-axis)
    ax = math.atan2(accel_y, math.sqrt(accel_x ** 2 + accel_z ** 2))*180/3.1415926535897932384626433832795
    #pitch = math.degrees(pitch)

    # Calculate roll (rotation around Y-axis)
    ay = math.atan2(accel_x, math.sqrt(accel_y ** 2 + accel_z ** 2))*180/3.1415926535897932384626433832795
    #roll = math.degrees(roll)

    pitch = pitch + gyro_x / FREQ
    roll = roll - gyro_y / FREQ
    yaw = yaw + gyro_z / FREQ

    pitch = pitch * 0.96 + ax * 0.04
    roll = roll * 0.96 + ay * 0.04

    # Calculate yaw (rotation around Z-axis)
    yaw = -math.atan2(-gyro_z, math.sqrt(gyro_x ** 2 + gyro_y ** 2)) #% 360
    yaw = math.degrees(yaw)

    return pitch, roll, yaw

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
        #time.sleep(0.01)

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

def acelerometro_changed(x, y, z):
    global accel_data
    accel_data[0] = x 
    accel_data[1] = y 
    accel_data[2] = z 


def main():
    global gyro_data, accel_data
    dualsense = pydualsense()
    dualsense.init()

    dualsense.gyro_changed += giroscopio_changed
    dualsense.accelerometer_changed += acelerometro_changed

    calibracion()

    while not dualsense.state.R1:
        calibrated_gyro = [
            (gyro_data[0] - gyro_offsets[0])/ gSensitivity,
            (gyro_data[1] - gyro_offsets[1])/ gSensitivity,
            (gyro_data[2] - gyro_offsets[2])/ gSensitivity
        ]

        #print("\t\t",gyro_data)
        #print(accel_data)

        pitch, roll, yaw = calculate_angles(calibrated_gyro, accel_data)
        print(f"Pitch: {pitch:.2f}°, Roll: {roll:.2f}°, Yaw: {yaw:.2f}°")
        os.system('cls')

if __name__ == '__main__':
    main()