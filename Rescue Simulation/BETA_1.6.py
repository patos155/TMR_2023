from re import X
from cv2 import contourArea
from matplotlib import container # DESDE RE IMPORTA EL MODULO(ATRIBUTO) X PARA ENTRADA DE DATOS #
from numpy import array # DESDE NUMPY IMPORTA LA FUNCIÓN DE ARRAY PARA ARREGLOS CON DIMENSIONES O MATRICES #

useCV = False #  #
try:
    from controller import Robot # CONEXION DEL CONTROLADOR AL ROBOT #
    import math # PROPORCIONA ACCESO A LAS FUNCIONES MATEMATICAS AVANZADAS #
    import struct # REALIZA CONVERSIONES ENTRE LOS VALORES DE PYTHON Y ESTRUCTURAS DE C COMO OBJETOS BYTES #
    import time # CONTIENE UNA SERIE DE FUNCIONES RELACIONADAS CON LA MEDICIÓN DEL TIEMPO #
    import cv2 # DETECCION DE MOVIMIENTO, RECONOCIMIENTO DE OBJETOS, RECONSTRUCCIÓN 3D A PARTIR DE IMAGENES, ETC #
    useCV = True #  #
    import numpy as np # DA SOPORTE PARA CREAR VECTORES Y MATRICES GRANDES MULTIDIMENSIONALES, JUNTO CON UNA GRAN COLECCION DE FUNCIONES MATEMATICAS DE ALTO NIVEL #
    camara_victima = True #  #
    viewHSV = True #  #
except:
    print("Librerias no instaladas")

#########################################
# CREAR UN OBJETO DE LA CLASE ROBOT #
robot = Robot()

#########################################
# VARIABLE PARA ESTABLECER EL PASO DE TIEMPO PARA LA SIMULACIÓN #
timeStep = 16

#########################################
# VARIABLES DE DISTANCIA DE LOS SENSORES ULTRASONICOS #
d_fd = 0  # distancia a la pared sensor frontal derecho
d_fi = 0  # distancia a la pared sensor frontal izquierdo
d_dd = 0  # distancia a la pared sensor derecho delantero
d_dc = 0  # distancia a la pared sensor derecho central
d_id = 0  # distancia a la pared sensor izquierdo delantero
d_ic = 0  # distancia a la pared sensor izquierdo central

#########################################
# VARIABLES DE VALOR LOGICO DE LOS SENSORES ULSTRASONICOS #
l_fd = 1  # valor logico existe pared sensor frontal derecho
l_fi = 1  # valor logico existe pared frontal derecho
l_dd = 1  # valor logico existe pared sensor  derecho delantero
l_dc = 1  # valor logico existe pared sensor  derecho central
l_id = 1  # valor logico existe pared sensor izquierdo delantero
l_ic = 1  # valor logico existe pared sensor izquierdo central

#########################################
# VARIABLES DE GPS #
cor_x = 0
cor_y = 0
cor_z = 0
x_gps = []
z_gps = []
long_x = 0
long_z = 0
count_gps = 0

#########################################
# VARIABLES PARA LA DISTANCIA IDEAL DE SEPARACION DE LAS PAREDES #
muros = 180  # distancia a los muros para tomar la pared
frente = 50  # distancia al frente para tomar la pared
#########################################
# VARIABLE PARA ENVIO DE MENSAJE
envio_m = False
#########################################
# VARIBLES DE VELOCIDAD #
speed_max = 4.5
speed_med = 4.3
speed_ordynary = 4
#speed_max = 6.28

#########################################
# VARIABLES PARA ESTABLECER CONTADORES DE MOVIMIENTO GENERAL#
count = 1
step_c = 1
#########################################
# VARIABLES PARA TIEMPOS DE GIRO #
ts = 60  # 55 #79
tg = 65  # 100
lg = 70  # 125

#########################################
# VARIABLE PARA DECLARAR EL TIPO DE INERCIA #
""" la inercia del sentido el robot se mueve a:
(F) frente, (I) izquierda, (D) derecha, (U) giro en U, (S) salir, (R),
(TS) terminar_salir, (IT) izquierda_trampa, (DT) derecha_trampa, (SI) """
inercia = 'F'

#########################################
# VARIABLE PARA DECLARAR EL DOBLE DE PASOS EN EL PANTANO #
pantano = 1  # multiplicador por pantano

#########################################
# VARIABLES PARA LA DETECCION DE VICTIMAS
contCb = 0 # contador de contornos central
contCl = 0 # contador de contornos izquierda
contCr = 0 # contador de contornos derecha
contCbc = 0
# RANGO DE COLORES PARA LA DETECCION DE VICTIMAS
umbral_bajo_black = (0,0,0) #NEGRO
umbral_alto_black = (179,5,137) #NEGRO 0 0 9(no detecta sombras pero para detectar victimas se tiene que acercar mucho)
umbral_bajo_ama = (20,50,170) #AMARILLO CLARO
umbral_alto_ama = (35,255,255) #AMARILLO
umbral_bajo_red = (163,80,100) #ROJO CLARO
umbral_alto_red = (177,255,212) #ROJO
# AREA DE LOS COLORES
contAreaA = 0
contAreaB = 0
contAreaR = 0
contAreaAc = 0
contAreaBc = 0
contAreaRc = 0
ratiobc = 0
contCbc

#########################################
# DEFINICION DE LAS RUEDAS DEL ROBOT #
rueda_rdf = robot.getDevice("wheel2 motor")
rueda_rif = robot.getDevice("wheel1 motor")
rueda_rdf.setPosition(float("inf"))
rueda_rif.setPosition(float("inf"))

#########################################
# DECLARACION DE LOS SENSORES DE DISTANCIA #
sensor_fd = robot.getDevice("ps0")
sensor_fd.enable(timeStep)
sensor_fi = robot.getDevice("ps7")
sensor_fi.enable(timeStep)
sensor_dd = robot.getDevice("ps1")
sensor_dd.enable(timeStep)
sensor_dc = robot.getDevice("ps2")
sensor_dc.enable(timeStep)
sensor_id = robot.getDevice("ps6")
sensor_id.enable(timeStep)
sensor_ic = robot.getDevice("ps5")
sensor_ic.enable(timeStep)

#########################################
# DECLARACION DE LAS 3 CAMARAS #
camera = robot.getDevice("camera_centre")
camera.enable(timeStep)
camerar = robot.getDevice("camera_right")
camerar.enable(timeStep)
cameral = robot.getDevice("camera_left")
cameral.enable(timeStep)

#########################################
# DECLARACION DEL SENSOR DE COLOR #
c_color = robot.getDevice("colour_sensor")
c_color.enable(timeStep)

#########################################
# DECLARACION DEL EMISOR #
emitter = robot.getDevice("emitter")

#########################################
# DECLARACION DEL GPS #
gps = robot.getDevice("gps")
gps.enable(timeStep)

#########################################
# METODO PARA LA LECTURA DE LOS SENSORES DE DISTANCIA #
def lee_distancia():
    # paso 1: declaramos variables globales #
    global d_fd, d_fi, d_dd, d_dc, d_id, d_ic # valores de distancias
    global l_fd, l_fi, l_dd, l_dc, l_id, l_ic # valores logicos

    # paso 2: analiza la informacion para determinar la distancia de los sensores#
    d_fd = int(sensor_fd.getValue()*1000)
    d_fi = int(sensor_fi.getValue()*1000)
    d_dd = int(sensor_dd.getValue()*1000)
    d_dc = int(sensor_dc.getValue()*1000)
    d_id = int(sensor_id.getValue()*1000)
    d_ic = int(sensor_ic.getValue()*1000)

    """ paso 3: Si determina que las distancias se encuantran en un rango
    determinado: entonces sabra cuando hay una pared(0) y cuando no hay una pared(1) """
    # caso unico: esta serie de condicionales determinan si hay o no una pared #
    if (d_ic > 500 and d_id < frente):
        print("estoy pegado a la pared izquierda")
        d_ic = 0

    if (d_dc > 500 and d_dd < frente):
        print("estoy pegado a la pared derecha")
        d_dc = 0

    if (d_fd > 600 and d_dd < frente):
        print("estoy pegado a la pared frontal a la derecha")
        d_fd = 0

    if (d_fi > 600 and d_id < frente):
        print("estoy pegado a la pared frontal a la izquierda")
        d_fi = 0

    # logicos al frente
    if (d_fd > frente):
        l_fd = 1  # no hay pared al frente
    else:
        l_fd = 0  # hay pared al frente

    if (d_fi > frente):
        l_fi = 1  # no hay pared al frente
    else:
        l_fi = 0  # hay pared al frente

    # logicos a las paredes
    if (d_dd > muros):
        l_dd = 1  # no hay pared a la derecha
    else:
        l_dd = 0  # hay pared a la derecha

    if (d_dc > muros):
        l_dc = 1  # no hay pared a la derecha
    else:
        l_dc = 0  # hay pared a la derecha

    if (d_id > muros):
        l_id = 1  # no hay pared a la izquierda
    else:
        l_id = 0  # hay pared a la izquierda

    if (d_ic > muros):
        l_ic = 1  # no hay pared a la izquierda
    else:
        l_ic = 0  # hay pared a la izquierda

    # paso 4: imprimimos los resultados de las distancias y sus valores en logicos #
    print('Distancias: fd ', d_fd, '  fi ', d_fi, ' derecha delantero ', d_dd,
        ' derecha central ', d_dc, " izquierda delantero ", d_id, " izquierda central ", d_ic)
    print('V_logicos: fd ', l_fd, '  fi ', l_fi, ' derecha delantero ', l_dd,
        ' derecha central ', l_dc, " izquierda delantero ", l_id, " izquierda central ", l_ic)

#########################################
# METODO PARA DETERMINAR LA LECTURA DEL SENSOR DE COLOR #
def prueba_color():
    # paso 1: declaramos variables globales #
    global ts, tg, lg
    global step_c
    global speed_ordynary
    global pantano

    # paso 2: analiza la informacion para determinar el color de la imagen que transmite #
    color = c_color.getImage()
    """ paso 3: imprime el codigo del color, aumentamos la velocidad y declaramos el pantano en 1
    esto es para que cuando se encuentre en el pantano sepa cuando o no redoblar los pasos """
    print("Color del suelo: ", color)
    speed_ordynary = 4
    pantano = 1

    """ paso 4: si esta en algunos de estos tonos de color, entonces sabe si esta en un
    pantano y luego redobla los pasos que tiene que dar a traves de la variable pantano """
    # caso 1: esta condicional es para determinar si se encuentra en un pantano #
    if color in [b'\x8e\xde\xf4\xff', b'\x8c\xdc\xf3\xff', b'\x8d\xdd\xf4\xff',
                b'\x81\xd1\xed\xff', b'\x81\xd2\xed\xff', b'\x81\xd1\xec\xff',
                b'\x80\xd0\xec\xff', b'\x7f\xd0\xec\xff', b'\x82\xd3\xee\xff',
                b'\x83\xd3\xee\xff', b'\x83\xd4\xee\xff', b'\x89\xd9\xf2\xff',
                b'\x7f\xcf\xeb\xff', b'\xfe\xfe\xfe\xff', b'\x84\xd4\xef\xff',
                b'~\xce\xea\xff', b'\x7f\xd0\xeb\xff', b'\x88\xd8\xf1\xff',
                b'}\xcd\xea\xff', b'\x80\xd0\xec\xff', b'\x82\xd2\xed\xff']:
        print("------------------------------------------------------PANTANO")
        pantano = 2.5
        speed_ordynary = 2
        """ paso 4.5: si esta en algunos de estos tonos de color, entonces sabe si esta en una
        trampa despues de determinar el paso 4.5 declara 55 pasos en el metodo sal_trampa """
    # caso 2: esta condicional determina si se encuentra en una trampa #
    elif color in [b';;;\xff', b';;@\xff', b'ooo\xff', b'---\xff', b'<<<\xff']:
        print("TRAMPA")
        step_c = 55
        sal_trampa()

#########################################
# METODO PARA DETERMINAR LA POSICION DEL ROBOT CON EL GPS #
# CON LA IMPLEMENTACION DE MOVERSE CUANDO ESTA ATORADO #
def lee_gps():
    # paso 1: declaramos variables globales #
    global lect_gps
    global x_gps, z_gps
    global cor_x, cor_y, cor_z
    global long_x, long_z
    global inercia
    global count_gps

    # paso 2: analiza la informacion para determinar las coordenadas x, y, z #
    lect_gps = gps.getValues()
    cor_x = int(lect_gps[0]*1000)
    cor_y = int(lect_gps[1]*1000)
    cor_z = int(lect_gps[2]*1000)

    """ paso 3: agregamos los datos de las coordenadas "x" y "z" en los arreglos x_gps y z_gps
    y a su vez esos datos se guardar en long_x y long_Z para determinar el numero de datos """
    x_gps.append(cor_x) #agregamos los valores a la lista x
    z_gps.append(cor_z) #agregamos los valores a la lista z
    long_x = len(x_gps)
    long_z = len(z_gps)

    # paso 4: determinamos un rango de 50 datos en las variables long_x y long_z #
    if (long_x>=50 and long_z>=50):
        print("evaluando sensores -----------------")
        """ paso 5: si los 50 datos son iguales, tanto en los datos x como en los de z: #
        # entonces contara 55 pasos con inercia hacia la izquierda junto con las velocidades """
        # caso 1: esta condicional es en caso de que el robot se quede estatico #
        if (x_gps[0] == x_gps[1] == x_gps[2] == x_gps[3] == x_gps[4] == x_gps[5] == x_gps[6] == x_gps[7] == x_gps[8] == x_gps[9] == x_gps[10] == x_gps[11] == x_gps[12] == x_gps[13] == x_gps[14] == x_gps[15] == x_gps[16] == x_gps[17] == x_gps[18] == x_gps[19] == x_gps[20] == x_gps[21] == x_gps[22] == x_gps[23] ==x_gps[24] ==x_gps[25] == x_gps[26] == x_gps[27] ==x_gps[28] ==x_gps[29] ==x_gps[30] ==x_gps[31] == x_gps[32] == x_gps[33] == x_gps[34] ==x_gps[35] == x_gps[36] ==x_gps[37] ==x_gps[38] == x_gps[39] ==x_gps[40] == x_gps[41] == x_gps[42] == x_gps[43] == x_gps[44] ==x_gps[45] == x_gps[46] == x_gps[47] ==x_gps[48] ==x_gps[49]) and (z_gps[0] == z_gps[1] == z_gps[2] == z_gps[3] == z_gps[4] == z_gps[5] == z_gps[6] == z_gps[7] == z_gps[8] == z_gps[9] == z_gps[10] == z_gps[11] == z_gps[12] == z_gps[13] == z_gps[14] == z_gps[15] == z_gps[16] == z_gps[17] == z_gps[18] == z_gps[19] == z_gps[20] == z_gps[21] == z_gps[22] == z_gps[23] ==z_gps[24] ==z_gps[25] == z_gps[26] == z_gps[27] ==z_gps[28] ==z_gps[29] ==z_gps[30] ==z_gps[31] == z_gps[32] == z_gps[33] == z_gps[34] ==z_gps[35] == z_gps[36] ==z_gps[37] ==z_gps[38] == z_gps[39] ==z_gps[40] == z_gps[41] == z_gps[42] == z_gps[43] == z_gps[44] ==z_gps[45] == z_gps[46] == z_gps[47] ==z_gps[48] ==z_gps[49]):
            print("estoy atorado XD")
            if count_gps<=55:
                inercia="I"
                paro()
                rueda_rdf.setVelocity(speed_max*-1)
                rueda_rif.setVelocity(speed_max*1)
                count_gps=count_gps+1
            else:
                """ paso 6: Una vez que termina el paso 5, se ira de frente y limpiara los 50 datos,
                esto es para que no se sobre carguen los datos y/o falle la programación del robot """
                print("todo correcto no me atore uwu")
                inercia="F"
                x_gps.clear()
                z_gps.clear()
            """ caso 2: esta condicional es en caso de que hayan pasado 50 pasos y sean
            diferentes esto hace que los datos se eliminen como dicta en el paso 6 """
        elif(long_x>=50 and long_z>=50):
            print("borrando listas")
            x_gps.clear()
            z_gps.clear()
            count_gps=0

    # paso 7: imprimimos los resultados del gps, junto con el numero de datos de los arreglos #
    print("Posición GPS[0](X):", cor_x, "----- Posición GPS[1](Y):", cor_y, "----- Posición GPS[2](Z):", cor_z)
    print("Longitud del GPS X:", len(x_gps),"----- Longitud del GPS Z:", len(z_gps))

#################################################################################

def delay(ms):
    initTime = robot.getTime()      # Store starting time (in seconds)
    while robot.step(timeStep) != -1:
        if (robot.getTime() - initTime) * 1000.0 > ms: # If time elapsed (converted into ms) is greater than value passed in
            break
            
def checkVic(img):
    global victimType, contAreaAc, contAreaBc, contAreaRc, ratiobc, contCbc

    img = np.frombuffer(img, np.uint8).reshape((camera.getHeight(), camera.getWidth(), 4))  # Convert img to RGBA format (for OpenCV)
    img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    mask_red = cv2.inRange(img_hsv, umbral_bajo_red, umbral_alto_red)
    mask_ama = cv2.inRange(img_hsv, umbral_bajo_ama, umbral_alto_ama)
    mask_black = cv2.inRange(img_hsv, umbral_bajo_black, umbral_alto_black)
    cntsr,_ = cv2.findContours(mask_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)   
    cntsa,_ = cv2.findContours(mask_ama, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
    cntsb,_ = cv2.findContours(mask_black, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
    for a in cntsa:
        xa, ya, wa, ha = cv2.boundingRect(a)
        contAreaAc = cv2.contourArea(a)
        ratioac = wa / ha
        contCac = len(a)
        print("contour area cac ", contAreaAc, " contCac ", contCac, " Ratio cac ", ratioac)
        if contAreaAc > 100:
            return True    
    for b in cntsr:
        xr, yr, wr, hr = cv2.boundingRect(b)   # Find width and height of contour
        contAreaRc = cv2.contourArea(b)   # Area covered by the shape
        ratiorc = wr / hr    # Calculate width to height ratio of contour
        contCrc = len(b)
        print("contour area crc ", contAreaRc," contCrc ",contCrc," Ratio crc ", ratiorc) 
        if contAreaRc > 100:
            return True
    for c in cntsb:
        xb, yb, wb, hb = cv2.boundingRect(c)
        contAreaBc = cv2.contourArea(c)
        ratiobc = wb / hb
        contCbc = len(c)
        print("contour area cb ", contAreaBc," contCb ", contCbc," Ratio cb ", ratiobc)
        if contAreaBc > 50:
            return True


def checkVic_izq(imgl):
    global cnts, inercia, contAl, victimType 

    imgl = np.frombuffer(imgl, np.uint8).reshape((cameral.getHeight(), cameral.getWidth(), 4))  # Convert img to RGBA format (for OpenCV)
    gaussianal = cv2.GaussianBlur(imgl, (17,17), 0)
    grayl = cv2.cvtColor(gaussianal, cv2.COLOR_BGR2GRAY) # Grayscale image
    _, threshl = cv2.threshold(grayl, 125, 255, cv2.THRESH_BINARY_INV) # Inverse threshold image (0-80 -> white; 80-255 -> black)
    #cannyl = cv2.Canny(threshl, 50, 150) #Se determina la imagen a analizar con canny  y se le da un valor minimo y maximo para decidir que bordes se toman en cuenta
    cntsl,_ = cv2.findContours(threshl, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)    
    for cnt in cntsl:
        x, y, w, h = cv2.boundingRect(cnt)   # Find width and height of contour
        contArea = cv2.contourArea(cnt)   # Area covered by the shape
        ratio = w / h    # Calculate width to height ratio of contour
        # if the contour area and width to height ratio are within certain ranges
        contCl = len(cnt)
        print("contour area l ", contArea,"contl ",contCl,"Ratio l ", ratio)
        if contArea <= 1700 and ratio >= 0.4 and ratio <= 1.3:
            paro()
            print("Area_l: ", contArea," Razon_l: ", ratio,"contCl ", contCl)
            victimType = bytes('H', "utf-8")
            return True
        else:
            contCl = 0
            return False         

def checkVic_der(imgr):
    global cnts, inercia, contAr, contCr, victimType 

    imgr = np.frombuffer(imgr, np.uint8).reshape((camerar.getHeight(), camerar.getWidth(), 4))  # Convert img to RGBA format (for OpenCV)
    gaussianar = cv2.GaussianBlur(imgr, (17,17), 0)
    grayr = cv2.cvtColor(gaussianar, cv2.COLOR_BGR2GRAY) # Grayscale image
    _, threshr = cv2.threshold(grayr, 125, 255, cv2.THRESH_BINARY_INV) # Inverse threshold image (0-80 -> white; 80-255 -> black)
    #cannyr = cv2.Canny(threshr, 50, 150) #Se determina la imagen a analizar con canny  y se le da un valor minimo y maximo para decidir que bordes se toman en cuenta
    cntsr,_ = cv2.findContours(threshr, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)    
    for cnt in cntsr:
        x, y, w, h = cv2.boundingRect(cnt)   # Find width and height of contour
        contArea = cv2.contourArea(cnt)   # Area covered by the shape
        ratio = w / h    # Calculate width to height ratio of contour
        # if the contour area and width to height ratio are within certain ranges
        contCr = len(cnt)
        print("contour area r ", contArea,"contr ",contCr,"Ratio r ", ratio)
        if contArea <= 1700 and ratio >= 0.4 and ratio <= 1.3:
            paro()
            print("Area r: ", contArea," Razon r: ", ratio,"contCr ", contCr)
            victimType = bytes('H', "utf-8")
            return True
        else:
            contCr = 0 
            return False  

def analisisvictym(contAreaA, contAreaB, contAreaR, ratiob, contCb):
    global victimType

    if contAreaR > 75:
        if contAreaA > 100:
            print("\t\t VICTIMA O ENCONTRADA------------------")
            victimType = bytes('O', "utf-8")
            #report()
        elif contAreaA == 0 and contAreaR > 100:
            print("\t\t VICTIMA F ENCONTRADA------------------")
            victimType = bytes('F', "utf-8")
            #report()
    if contAreaB > 50 and ratiob > 0.7 and ratiob < 1.4:
        if contCb >= 15 and contCb <= 25:
            print("\t\t VICTIMA H ENCONTRADA------------------")
            victimType = bytes('H', "utf-8")
            #report()
        elif contCb >= 26 and contCb <= 40:
            print("\t\t VICTIMA U ENCONTRADA------------------")
            victimType = bytes('U', "utf-8")
            #report()
        elif contCb >= 41 and contCb <= 78:
            print("\t\t VICTIMA S ENCONTRADA------------------")
            victimType = bytes('S', "utf-8")  
            #report()

def report():
    global envio_m, inercia
    print("se envio el mensaje: ", envio_m)
    if envio_m == False:
        paro()
        delay(1300)
        posX = int(gps.getValues()[0] * 100)
        posZ = int(gps.getValues()[2] * 100)
        message = struct.pack("i i c", posX, posZ, victimType)
        print(message)
        emitter.send(message)
        robot.step(timeStep)    
        envio_m = True

    else:
        inercia="F"
        delay(500)
        envio_m = False

def leer_camaras():
    global contAreaAc, contAreaBc, contAreaRc, ratiobc, contCbc
    img = camera.getImage()
    imgr = camerar.getImage()
    imgl = cameral.getImage()
    victimac = checkVic(img)
    victimai = checkVic_izq(imgl)
    victimad = checkVic_der(imgr)
    print("victimac ", victimac," victimai ", victimai," victimad ", victimad)
    if victimac == True:
        analisisvictym(contAreaAc, contAreaBc, contAreaRc, ratiobc, contCbc)
    #elif victimai == True:
    #    analisisvictym(contAreaAi, contAreaBi, contAreaRi, ratiobi, contCb)
    #elif victimad == True:    
    #    analisisvictym(contAreaAd, contAreaBd, contAreaRd, ratiobd, contCb)
    
#################################################################################

#########################################
# METODO PARA PARAR EL ROBOT #
def paro():
    print("-------- paro ------")
    rueda_rdf.setVelocity(0)
    rueda_rif.setVelocity(0)

#########################################
# METODO PARA IR HACIA ADELANTE #
def adelante():
    """ paso 1: declaramos una variable global y declarar la inercia F y
    declaramos una nueva variable para la diferenciación del centrado """
    global inercia
    inercia = 'F'
    print("adelante------")
    diferencia = d_id-d_ic

    # paso 2: el robot comienza a traves de condicionales a verificar el centrado #
    """ nota: los rango establecidos son los siguientes de 0 a 100 de termina la inclinacion.
    la inclinacion de 50 hacia abajo representa una inclinación a la derecha y de 70 hacia
    arriba representa una inclinación a la izquierda """
    # caso 1: si el robot se encuentra en un rango de 50 a 70 entonces sigue de frente #
    if (diferencia >= 50 and diferencia <= 70):
        # y id es correcto seguimos de frente normal
        rueda_rdf.setVelocity(speed_ordynary)
        rueda_rif.setVelocity(speed_ordynary)
        print("sin ajuste")
    # caso 2: si el robot se encuentra en un rango menor a 50 entonces se inclina hacia la derecha #
    elif (diferencia < 50):
        rueda_rdf.setVelocity(speed_ordynary)
        rueda_rif.setVelocity(speed_max)
        print("ajuste a la derecha")
    # caso 3: si el robot se encuentra en un rango mayo mayor a 70 entonces se inclina hacia la izquierda #
    elif (diferencia > 70):
        rueda_rdf.setVelocity(speed_max)
        rueda_rif.setVelocity(speed_ordynary)
        print("ajuste a la izquierda")

#########################################
# METODO PARA SALIR EN CASOS NECESARIOS #
def salir():
    """ paso 1: declaramos variables globales de la inercia contador
    e imprimimos la salida con su respectivo conteo de pasos """
    global inercia
    global count
    print("salir ------", count)

    """ paso 2: declaramos que los motores vallan a la misma velocidad y establecemos
    un contador con el cual veremos si esta en terreno normal o en un pantano """
    rueda_rdf.setVelocity(speed_ordynary)
    rueda_rif.setVelocity(speed_ordynary)
    count = count+(1/pantano)

#########################################
# METODO PARA TERMINAR DE SALIR DE UNA TRAMPA #
def term_salir():
    # paso 1: declaramos variables globales y la inercia la fijamos como TS #
    global inercia
    global count
    inercia = 'TS'
    print("Terminar de salir ------", count)

    """ paso 2: declaramos que los motores vallan a la misma velocidad y establecemos
    un contador con el cual veremos si esta en terreno normal o en un pantano """
    rueda_rdf.setVelocity(speed_ordynary)
    rueda_rif.setVelocity(speed_ordynary)
    count = count+(1/pantano)

#########################################
# METODO PARA DEZPLAZAR EL ROBOT HACIA LA IZQUIERDA #
def izquierda():
    # paso 1: declaramos variables globales y la inercia la fijamos como I #
    global inercia
    global count
    inercia = 'I'

    """ paso 2: declaramos que los motores vallan orientados hacia la izquierda y establecemos
    un contador con el cual veremos si esta en terreno normal o en un pantano """
    rueda_rdf.setVelocity(speed_ordynary)
    rueda_rif.setVelocity(speed_ordynary * -1)
    count = count+(1/pantano)

#########################################
# METODO PARA DESPLAZAR EL ROBOT HACIA LA DERECHA #
def derecha():
    # paso 1: declaramos variables globales y la inercia la fijamos como D #
    global inercia
    global count
    inercia = 'D'

    """ paso 2: declaramos que los motores vallan orientados hacia la derecha y establecemos
    un contador con el cual veremos si esta en terreno normal o en un pantano """
    rueda_rdf.setVelocity(speed_ordynary * -1)
    rueda_rif.setVelocity(speed_ordynary)
    count = count+(1/pantano)

#########################################
# METODO PARA DEZPLAZAR EL ROBOT EN UN GIRO DE 180°(GIRO_U) #
def giro_u():
    """ paso 1: declaramos variables globales y la inercia la fijamos como U y establecemos
    un contador con el cual veremos si esta en terreno normal o en un pantano """
    global inercia
    global count
    inercia = "U"
    count = count+(1/pantano)

    """ paso 2: establecemos una serie de condicionales para determinar la cantidad de
    pasos que debe de realizar en una vuelta en U """
    # caso 1: si el contador es menor o mayor a tg=65 entonces dara un giro en u orientado a la derecha #
    if (count <= tg):
        print("Inicia el Giro ", count, "tg ", tg)
        rueda_rdf.setVelocity(speed_ordynary)
        rueda_rif.setVelocity(speed_ordynary * -1)
        """ caso 2: si la dis_frontal izquierda es mayor o igual a la dis_frontal derecha menos 100
        y si el contador es menor o igual a lg=70 entonces dara un giro en u orientado a la izquierda """
    elif (d_fi >= d_fd-100 and count <= lg):
        print("Termina el giro ", count, "lg ", lg, "Diferencia ", d_fi-d_fd)
        rueda_rdf.setVelocity(speed_ordynary)
        rueda_rif.setVelocity(speed_ordynary * -1)
        """ caso 3: si el contador es menor o igual a ts=60 entonces seguira de frente """
    elif (count <= ts):
        print("Salir ", count, " ts ", ts)
        rueda_rdf.setVelocity(speed_ordynary)
        rueda_rif.setVelocity(speed_ordynary)
    else:
        # caso 4: si se llega o no a cumplir los 3 casos, entonces seguira hacia adelante #
        adelante()
        print("Terminar de salir ", count," Pasos ",step_c)

#########################################
# METODO PARA PODER SALIR DE UNA TRAMPA #
def sal_trampa():
    """ paso 1: declaramos variables globales, declaramos los motores a maxima velocidad hacia atras,
    la inercia la fijamos como R y establecemos un contador con el cual veremos si esta en terreno
    normal o en un pantano """  #
    global inercia
    global step_c
    global ts, tg, lg
    global count
    rueda_rdf.setVelocity(speed_max*-1)
    rueda_rif.setVelocity(speed_max*-1)
    inercia = "R"
    count = count + (1/pantano)
    print("Salir de trampa  ", count, " Pasos ", step_c)

    """ paso 2: establecemos una serie de condicionales para determinar
    el tipo de giro que debe de realizar para salir de la trampa"""
    if count > 25:
        inercia = "U"
        if (l_dc == 0 and l_ic == 0):
            inercia = "U"
        elif (l_dc == 0 and l_ic == 1):
            count = 1
            step_c = 90  # PASOS PARA LA VUELTA
            giro_u() # izquierda()
        elif (l_dc == 1 and l_ic == 0):
            count = 1
            step_c = 35  # PASOS PARA LA VUELTA
            derecha()
        else:
            count = 1
            step_c = 90  # PASOS PARA LA VUELTA
            giro_u() # izquierda()

#########################################
# METODOS PARA REALIZAR UN AJUSTE A LA IZQUIERDA #
def alinear_izquierda():
    print("alinear a la izquierda")
    rueda_rdf.setVelocity(speed_ordynary)
    rueda_rif.setVelocity(speed_ordynary * -1)

#########################################
# METODO PARA REALIZAR UN AJUSTE A LA DERECHA #
def alinear_derecha():
    print("alinear a la derecha")
    rueda_rdf.setVelocity(speed_ordynary * -1)
    rueda_rif.setVelocity(speed_ordynary)

#########################################
# METODO PARA AJUSTAR EL CENTRADO CON LOS SENSORES FRONTALES #
def ajustar():
    ajustar = False
    if (d_fd != 0):
        multiplo = d_fi/d_fd
        if (multiplo < 0.8 and l_fi == 1 and l_fd == 1 and l_ic == 1 and l_id == 1 and l_dc == 1 and l_dd == 1):
            # paro()
            print("Estoy perdido me alineo a la izquierda", multiplo)
            alinear_izquierda()
            ajustar = True
        elif (multiplo > 1.2 and l_fi == 1 and l_fd == 1 and l_ic == 1 and l_id == 1 and l_dc == 1 and l_dd == 1):
            # paro()
            print("Estoy perdido me alineo a la derecha", multiplo)
            alinear_izquierda()
            # alinear_derecha()
            ajustar = True
    return ajustar

#########################################
# METODO PARA DETERMINAR LOS MOVIMIENTOS DEL ROBOT EN LA PISTA #
def direccion():
    global inercia
    global step_c, count
    ins = 2
    print("--------------", inercia)
    # no hay paredes al frente
    if (inercia == 'F'):
        if (ajustar() == False):
            # no hay pared al frente y hay pared a la izquierda
            if (l_fd == 1 and l_fi == 1 and (l_id == 0 or l_ic == 0)):
                adelante()
            # Hay pared al frente y no hay pared a la izquierda(or o and)
            elif ((l_fd == 0 and l_fi == 0) and (l_id == 1 or l_ic == 1)):
                izquierda()
            elif ((l_fd == 0 and l_fi == 0) and (l_id == 1 or l_ic == 1)):
                izquierda()
            # No hay pared al frente y no hay pared a la izquierda
            elif ((l_fd == 1 and l_fi == 1) and (l_id == 1 or l_ic == 1)):
                step_c = 50
                count = 0
                inercia = 'SI'
                salir()
            elif ((l_fd == 0 and l_fi == 0) and (l_dc == 1)):
                step_c = 35
                count = 0
                derecha()
            elif (l_fd == 0 and l_fi == 0 and l_id == 0 and l_ic == 0 and l_dd == 0 and l_dc == 0):
                step_c = 35
                count = 0
                giro_u()
            else:
                adelante()
    elif inercia == 'S':
        salir()
        print("Salir", count)
        if (count > step_c or l_fd == 0 or l_fi == 0):
            inercia = "F"
            adelante()
    elif inercia == 'I':
        izquierda()
        print("Izquierda", count)
        if (count > step_c):
            step_c = 40
            count = 0
            term_salir()
    elif inercia == 'D':
        derecha()
        print("Derecha", count)
        if (count > step_c):
            step_c = 40
            count = 0
            term_salir()
    elif inercia == 'SI':
        if (count < step_c):
            salir()
        else:
            step_c = 35
            count = 0
            izquierda()
    elif inercia == "R":
        sal_trampa()
    elif inercia == "DT":
        derecha()
        print("Salir de derecha trampa", count)
        if (count > step_c):
            step_c = 40
            count = 0
            term_salir()
    elif inercia == "IT":
        izquierda()
        print("Salir de izquierda trampa", count)
        if (count > step_c):
            step_c = 40
            count = 0
            term_salir()
    elif inercia == "TS":
        term_salir()
        print("Terminar de salir", count)
        if (count > step_c or l_fd == 0 or l_fi == 0):
            step_c = 0
            count = 0
            adelante()
    elif inercia == 'U':
        giro_u()

#########################################
# PROGRAMA PRINCIPAL (MAIN) #
while robot.step(timeStep) != -1:
    print("---------------------------------------------------------------------")
    #print("Count: ", count, "Pantano: ", pantano, "Contador gps: ",count_gps)
    #lee_gps()
    #prueba_color()
    #lee_distancia()
    leer_camaras()
    #direccion()
