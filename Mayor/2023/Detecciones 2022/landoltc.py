import cv2
import numpy as np
import random

cap  = cv2.VideoCapture(1)
#img = cv2.imread(r"F:\kito089\esc\Robotica\major\camara\imagenes\landolt\linear1.png")
#gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
"""medianblur = cv2.medianBlur(gray, 5)
cv2.imshow("blur", medianblur)
cv2.waitKey(0)
cv2.destroyAllWindows()
"""

anterior = []
espacio = []
anteriorA = 0
cont = 0
umbral_bajo_black = (0,0,0) #lineas NEGRO
umbral_alto_black = (180,255,150) #NEGRO

umbral_bajo_white = (0,0,180) #Nlineas blanco
umbral_alto_white = (180,255,255) #NEGRO

def decoLugar(x):
    if x == 0:
        return "T"
    elif x == 1:
        return "B"
    elif x == 2:
        return "L"
    elif x == 3:
        return "R"
    elif x == 4:
        return "TL"
    elif x == 5:
        return "TR"
    elif x == 6:
        return "BL"
    elif x == 7:
        return "BR"

def predeterminado(lista):
    print(lista)
    if lista[0] == "T" and lista[1] == "BL" and lista[2] == "R" and lista[3] == "BR" and lista[4] == "L":
        print("LINEAR TASKS: LEFT PERPENDICULAR")
    elif lista[0] == "T" and lista[1] == "B" and lista[2] == "TR" and lista[3] == "L" and lista[4] == "BL":
        print("LINEAR TASKS: LEFT ANGLED")
    elif lista[0] == "T" and lista[1] == "TL" and lista[2] == "T" and lista[3] == "BL" and lista[4] == "B":
        print("LINEAR TASKS: CENTER")
    elif lista[0] == "T" and lista[1] == "R" and lista[2] == "TL" and lista[3] == "L" and lista[4] == "BR":
        print("LINEAR TASKS: RIGHT ANGLED")
    elif lista[0] == "T" and lista[1] == "TL" and lista[2] == "L" and lista[3] == "BL" and lista[4] == "T":
        print("LINEAR TASKS: RIGHT PERPENDICULAR")
    elif lista[0] == "T" and lista[1] == "T" and lista[2] == "BR" and lista[3] == "L" and lista[4] == "TL":
        print("OMNI TASKS: LEFT FRONT")
    elif lista[0] == "T" and lista[1] == "BR" and lista[2] == "T" and lista[3] == "TL" and lista[4] == "R":
        print("OMNI TASKS: LEFT BACK")
    elif lista[0] == "T" and lista[1] == "R" and lista[2] == "TR" and lista[3] == "L" and lista[4] == "BL":
        print("OMNI TASKS: CENTER")
    elif lista[0] == "T" and lista[1] == "BL" and lista[2] == "R" and lista[3] == "TL" and lista[4] == "B":
        print("OMNI TASKS: RIGHT FRONT")
    elif lista[0] == "T" and lista[1] == "B" and lista[2] == "TL" and lista[3] == "B" and lista[4] == "BL":
        print("OMNI TASKS: RIGHT BACK")
    else:
        print("INDEPENDIENTE")

def partirImagen():
    parte = []
    ind = 0
    global espacio

    parte.append(img[0:129,129:479])
    parte.append(img[479:600,129:479])
    parte.append(img[129:479,0:129])
    parte.append(img[129:479,479:600])

    parte.append(img[0:200,0:200])
    parte.append(img[0:200,400:600])
    parte.append(img[400:600,0:200])
    parte.append(img[400:600,400:600])

    for i in parte:
        cont, _ = cv2.findContours(i, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        contg = sorted(cont, key=lambda x: cv2.contourArea(x), reverse=True)

        for co in contg:
            area = cv2.contourArea(co)
            epsilon = 0.02*cv2.arcLength(co,True)
            approx = cv2.approxPolyDP(co,epsilon,True)

            print("area: ", area)
            print("approx: ", len(approx))

            if area <=  22000 and len(contg) > 1:
                print("cuadrado?....")
                espacio.append(decoLugar(ind))
                break

        if ind <= 8:        
            ind = ind+1
        else:
            ind=0

        if (len(espacio) == 5):
            break

        print("---------------------------------------------------------------")
        cv2.imshow("ParteImagen", i)
        cv2.waitKey(0)

def conjuntoArea(imga, col):
    area = 0
    comp = imga[260:340,330:400]
    concomp, _ = cv2.findContours(comp, cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)

    for c in concomp:
        area = area+cv2.contourArea(c)
        x,y,w,h = cv2.boundingRect(c)
        epsilon = 0.02*cv2.arcLength(c,True)
        approx = cv2.approxPolyDP(c,epsilon,True)
        aspect_ratio = float(w/h)
        print(";;;;;;;;;;;;;;;;;;;;;;;Lados: ",len(approx)," aspec_ratio: ",aspect_ratio," color: ", col)

    print(";;;;;;;;;;;;;;;;;;;;;;;area "+ col+": ",area," no.Contornos: ",len(concomp))

    cv2.imshow("comp", comp)
    cv2.waitKey(0)

    
    return(len(concomp))

while True:
    _, img = cap.read()
    img2 = img.copy()

    while True:
        canny = cv2.Canny(img,0,255)
        #canny = cv2.blur(canny,(2,2))
        canny = cv2.dilate(canny,None,iterations=1)#4
        cv2.imshow("algo", canny)
        cv2.waitKey(0)

        cnts, _ = cv2.findContours(canny,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
        contornos = sorted(cnts, key=lambda x: cv2.contourArea(x), reverse=True)
        img2 = cv2.drawContours(img2, contornos, -1,(0,255,0), 3)
        cv2.imshow("img2",img2)

        for c in contornos:
            area = cv2.contourArea(c)
            x,y,w,h = cv2.boundingRect(c)
            epsilon = 0.02*cv2.arcLength(c,True)
            approx = cv2.approxPolyDP(c,epsilon,True)
            aspect_ratio = float(w/h)

            print("approx ", len(approx))
            print("aspect ratio ", aspect_ratio)
            print(">///< area ", area)

            if cont == 0 and len(approx) >= 8:
                cont = cont +1
                print("circulo.......")
                img = img[y:y+h, x:x+w]
                anteriorC = c
                anteriorA = area
                print("anterior a ", anteriorA)
                img = cv2.resize(img, (600, 600), interpolation=cv2.INTER_AREA)
                img = cv2.medianBlur(img, 3)
                hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
                negro = cv2.inRange(hsv, umbral_bajo_black, umbral_alto_black)
                blanco = cv2.inRange(hsv, umbral_bajo_white, umbral_alto_white)
                print("----------------------------------------------",cont)
                if conjuntoArea(negro,"lineas negro") > conjuntoArea(blanco,"lineas blanco"):
                    img = negro
                else:
                    img = blanco
                cv2.imshow("img",img)
                cv2.waitKey(0)
                partirImagen()
                break
            elif len(approx) >= 8 and area < anteriorA:
                #cv2.drawContours(img2,[c],-1,(255,0,0),1)
                print("circulo.......")
                img = img[y:y+h, x:x+w]
                anterior = c
                print("anterior a ", anteriorA)
                print("area: ",area)
                anteriorA = area
                cont = cont +1
                img = cv2.resize(img, (600, 600), interpolation=cv2.INTER_AREA)
                img = cv2.medianBlur(img, 3)
                print("----------------------------------------------",cont)
                cv2.imshow("img",img)
                cv2.waitKey(0)
                partirImagen()
                break

        print(espacio)

        if(cv2.waitKey(1)==ord("e")):
            break
    if(cv2.waitKey(1)==ord("e")):
        break


cv2.destroyAllWindows()

