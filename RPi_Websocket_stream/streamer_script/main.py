from kivy.support import install_twisted_reactor
install_twisted_reactor()
import WSsupport
from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.image import Image
from kivy.uix.button import Button
from kivy.uix.label import Label
from kivy.clock import Clock
from kivy.graphics.texture import Texture
import cv2
import numpy as np

class CamApp(App):

    smlr = np.zeros((128, 128, 3), np.uint8)
    madeConnection = False
    bw = True
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.ws = WSsupport.WStestApp()
        self.conClock = Clock.schedule_interval(self.checkifconnected, 1)
        self.backSub = cv2.createBackgroundSubtractorMOG2()


    def build(self):
        self.img1=Image(size_hint=(1, 1))
        self.layout = FloatLayout()
        self.layout.add_widget(self.img1)
        self.capture = cv2.VideoCapture(0)
        Clock.schedule_interval(self.update, 1/7)
        self.ipOnLay(self.ws.getTheIP())
        self.switchimagetype = Button(text="switch", on_press=self.switchimage, size_hint=(0.1,0.1))
        self.layout.add_widget(self.switchimagetype)
        return self.layout

    def switchimage(self, ins):
        self.bw = not(self.bw)


    def ipOnLay(self, ipstr):
        self.iplbl = Label(text=ipstr, pos_hint=({'top': 1.2, 'right': 1.2}))
        self.layout.add_widget(self.iplbl)

    def update(self, dt):
        ret, frame = self.capture.read()
        if self.bw:
            fgMask = self.backSub.apply(frame)
            cv2.cvtColor(fgMask,cv2.COLOR_GRAY2BGR,frame)
        buf1 = cv2.flip(frame, 0)
        buf = buf1.tostring()
        texture1 = Texture.create(size=(frame.shape[1], frame.shape[0]), colorfmt='bgr')
        texture1.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
        self.img1.texture = texture1
        if self.madeConnection:
            #print("connected")
            im_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            buf1 = cv2.flip(im_rgb, 1)
            cv2.resize(buf1, (128, 128), self.smlr, interpolation=cv2.INTER_AREA)
#            cv2.imshow("de", self.smlr)
#            cv2.waitKey(1000)
            x = self.smlr.tostring()
            try:
                self.ws.sndBinary(x)
            except:
                print("error")



    def sendTest(self, dt):
        print("text")
        self.ws.txtBtn("WORKING?")


    def checkifconnected(self, dt):
        if self.ws.isConnectedToClient == True:
            self.conClock.cancel()
            print("try")
            self.madeConnection = True


if __name__ == '__main__':
    CamApp().run()