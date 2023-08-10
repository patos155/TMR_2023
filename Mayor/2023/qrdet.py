import cv2
import numpy as np
#import pyautogui
from tkinter import *

cap = cv2.VideoCapture(1)
cv2.namedWindow("Grabando", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Grabando", 960, 540) #Los ultimos dos argunmentos son las dimensiones de la ventana de grabación

detector = cv2.QRCodeDetector()

datos = ""

def mensaje():
    global datos

    if not(datos == data):
        carcasa = Tk()
        carcasa.title("Mensaje:")
        carcasa.geometry("250x100")
        Label(carcasa, text=data).pack(expand=1)
        carcasa.mainloop()
    datos = data

while True:
    _, frame = cap.read()
    # get bounding box coords and data
    data, bbox, _ = detector.detectAndDecode(frame)
    
    # if there is a bounding box, draw one, along with the data
    if(bbox is not None):
        bb_pts = bbox.astype(int).reshape(-1, 2)
        for i in range(len(bb_pts)):
            cv2.line(frame, tuple(bb_pts[i]), tuple(bb_pts[(i+1) % len(bb_pts)]), color=(255,
                     0, 255), thickness=2)
        cv2.putText(frame, data, (bb_pts[0][0], bb_pts[0][1] - 10), cv2.FONT_HERSHEY_SIMPLEX,
                    0.5, (0, 255, 0), 2)
        if data:
            print("data found: ", data)
    # display the image preview
            mensaje()
    cv2.imshow("Grabando", frame)
    if(cv2.waitKey(1)==ord("e")):
        break
# free camera object and exit
cv2.destroyAllWindows()

