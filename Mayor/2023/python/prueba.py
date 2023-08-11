import serial
from OpenGL.GL import * # opengl basico
from OpenGL.GLU import * # no c :v
import pygame # que te puedo decir es pygame XD
from pygame.locals import * # por si acaso (^///^)
import time
import os

x = y = z = 0

verticies = (
    (1, -1, -1),
    (1, 1, -1),
    (-1, 1, -1),
    (-1, -1, -1),
    (1, -1, 1),
    (1, 1, 1),
    (-1, -1, 1),
    (-1, 1, 1)
)

edges = (
    (0, 1),
    (0, 3),
    (0, 4),
    (2, 1),
    (2, 3),
    (2, 7),
    (6, 3),
    (6, 4),
    (6, 7),
    (5, 1),
    (5, 4),
    (5, 7)
)

surfaces = (
    (0, 1, 2, 3),
    (3, 2, 7, 6),
    (6, 7, 5, 4),
    (4, 5, 1, 0),
    (1, 5, 7, 2),
    (4, 0, 3, 6)
)

colores = (
    (1.0, 1.0, 1.0), # blanco --
    (0.0, 1.0, 0.0), # verde --
    (1.0, 1.0, 0.0), # amarillo --
    (0.0, 0.0, 1.0), # azul --
    (1.0, 0.0, 0.0), # rojo --
    (1.0, 0.5, 0.0) # naranja --
)

arduino = serial.Serial("COM5", 115200)
time.sleep(0.1) #wait for serial to open

def init():
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def resize(width, height):
    if height==0:
        height=1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def draw():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
    
    glLoadIdentity()
    glTranslatef(0,0.0,-7.0)

    # grados de 0 a 360 donde tambien hay negativos
    glRotatef(x*-1, 1, 0, 0) # X
    glRotatef(z*-1, 0, 1, 0) # Y
    glRotatef(y*-1, 0, 0, 1) # Z

    glBegin(GL_QUADS)
    for color, surface in zip(colores, surfaces):
        glColor3fv(color)
        for vertex in surface:
            glVertex3fv(verticies[vertex])
    glEnd()

    glColor3f(1.0, 1.0, 1.0)
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(verticies[vertex])
    glEnd()

def main():
    global x, y, z

    while True:
        answer=str(arduino.readline().decode('utf-8'))
        print(answer)
        if "Calibrando" in answer:
            arduino.write("x".encode('utf-8'))
        if "Terminado" in answer:
            break
        
    arduino.flush()

    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, OPENGL|DOUBLEBUF)
    pygame.display.set_caption("Press Esc to quit")
    resize(800,600)
    init()

    frames = 0
    ticks = pygame.time.get_ticks()

    while True:        
        answer=str(arduino.readline().decode('utf-8'))
        datos = [float(x) for x in answer.split(",")]
        print(datos)
        
        x = datos[2]
        y = datos[1]
        z = datos[0]

        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            pygame.quit()
            break    

        draw()

        pygame.display.flip()
        frames = frames+1

        print ("fps:  %d" % ((frames*1000)/(pygame.time.get_ticks()-ticks)))
        #os.system('cls') ##Baja mucho rendimiento

if arduino.isOpen(): 
    print("{} conexion establecida!!".format(arduino.port))
    main()
