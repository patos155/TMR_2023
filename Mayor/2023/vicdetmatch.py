import cv2
import numpy as np
from tkinter import *

datos = ""

def mensaje(data):
    global datos

    if not(datos == data):
        carcasa = Tk()
        carcasa.title("Mensaje:")
        carcasa.geometry("250x100")
        Label(carcasa, text=data).pack(expand=1)
        carcasa.mainloop()
    datos = data

def ordenar_puntos(puntos):
    n_puntos = np.concatenate([puntos[0], puntos[1], puntos[2], puntos[3]]).tolist()
    y_order = sorted(n_puntos, key=lambda n_puntos: n_puntos[1])
    x1_order = y_order[:2]
    x1_order = sorted(x1_order, key=lambda x1_order: x1_order[0])
    x2_order = y_order[2:4]
    x2_order = sorted(x2_order, key=lambda x2_order: x2_order[0])
    
    return [x1_order[0], x1_order[1], x2_order[0], x2_order[1]]

def tip_victim(indice):
    if indice <= 5:
        tipo = "EXPLOSIVES"
    elif indice <= 11:
        tipo = "POISON"
    elif indice <= 17:
        tipo = "FLAMMABLE SOLID"
    elif indice <= 23:
        tipo = "CORROSIVE"
    elif indice <= 29:
        tipo = "FLAMMABLE GAS"
    elif indice <= 35: 
        tipo = "BLASTING AGENTS"
    elif indice <= 41: 
        tipo = "SPONTANEOUSLY COMBUSTIBLE"
    elif indice <= 47: 
        tipo = "OXIDIZER"
    elif indice <= 53: 
        tipo = "NON-FLAMMABLE GAS"
    elif indice <= 59: 
        tipo = "OXYGEN"
    elif indice <= 65: 
        tipo = "FUEL OIL"
    elif indice <= 71: 
        tipo = "DANGEROUS WHEN WET"
    elif indice <= 77: 
        tipo = "INHALATION HAZARD"
    elif indice <= 83: 
        tipo = "ORGANIC PEROXIDE"
    elif indice <= 89: 
        tipo = "RADIOACTIVE"

    print(tipo,"   ",indice)

    return tipo

h, w = 59 , 59
cap = cv2.VideoCapture(1)
cv2.namedWindow("Grabando", cv2.WINDOW_NORMAL)
cv2.namedWindow("canny", cv2.WINDOW_NORMAL)
cv2.namedWindow("giro", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Grabando", 960, 540)
cv2.resizeWindow("canny", 960, 540)
#cv2.resizeWindow("giro", 960, 540)

a = 0
victims = []

for i in range(90):
    ej = cv2.imread(r"F:\kito089\esc\Robotica\major\camara\imagenes\comp\vic_"+str(i)+".png", 0)
    victims.append(ej)

while True:
    _, img = cap.read()
    orig = img.copy()
    
    canny = cv2.Canny(img,0,255)
    canny = cv2.dilate(canny,None,iterations=2)

    cnts,_ = cv2.findContours(canny,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)

    for c in cnts:
        area = cv2.contourArea(c)
        x,y,w,h = cv2.boundingRect(c)
        epsilon = 0.09*cv2.arcLength(c,True)
        approx = cv2.approxPolyDP(c,epsilon,True)
        aspect_ratio = float(w/h)

        if len(approx)==4 and aspect_ratio>0.7 and aspect_ratio<1.2 and area > 7000 and area < 90000:
            cv2.drawContours(img, [approx], 0, (0,255,255),2)

            puntos = ordenar_puntos(approx)
            pts1 = np.float32(puntos)
            pts2 = np.float32([[0,0], [w,0], [0,h], [w,h]])
            M = cv2.getPerspectiveTransform(pts1,pts2)
            giro = cv2.warpPerspective(orig, M, (w,h))
            girogr = cv2.cvtColor(giro, cv2.COLOR_BGR2GRAY)
            cv2.imshow("giro",giro)
            #cv2.imwrite(r"F:\kito089\esc\Robotica\major\camara\imagenes\vic\vic_"+str(a)+".png" , giro)
            #cv2.waitKey(0)
            #a = a+1
            for v in victims:
                v = cv2.resize(v, (girogr.shape[1], girogr.shape[0]))
                res = cv2.matchTemplate(girogr, v, cv2.TM_CCOEFF_NORMED)
                min, max, pos_min, pos_max = cv2.minMaxLoc(res)
                print(max,"       ",min)
                print("imagen ", a)
                maxde = max*100
                print(maxde)
                if maxde > 83:
                    mensaje(tip_victim(a))
                a = a + 1
            a = 0

    cv2.imshow("Grabando",img)
    cv2.imshow("canny",canny)

    if(cv2.waitKey(1)==ord("e")):
        break