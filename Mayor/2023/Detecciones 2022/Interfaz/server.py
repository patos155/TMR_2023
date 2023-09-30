from flask import *
import cv2
import numpy as np
from tkinter import *
from flask_socketio import *


cap = cv2.VideoCapture(0)
detector = cv2.QRCodeDetector()


dato = ""


def generar():
    global dato
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
                if not(dato == data):
                    socketio.emit('dato', data)
                dato=data
        (flag, encodeimage) = cv2.imencode(".jpg",frame)
        if not flag:
            continue
        yield(b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' +
              bytearray(encodeimage) + b'\r\n') 
        
        
app = Flask(__name__)

app.config['SECRET_KEY'] = 'secret'
socketio = SocketIO(app)

@app.route('/')
def index():
    return render_template('interfaz.html')

@app.route('/video')
def video():
    return Response(generar(),
                    mimetype="multipart/x-mixed-replace; boundary=frame")


if __name__ == "__main__":
    app.run(debug=False)