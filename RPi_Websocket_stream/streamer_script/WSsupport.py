from kivy.uix.widget import Widget
import socket
from autobahn.twisted.websocket import WebSocketServerProtocol, WebSocketServerFactory, listenWS


wsStr = "Wait..."

class BroadcastServerProtocol(WebSocketServerProtocol):

    def onOpen(self):
        self.factory.register(self)
        print("connected")
        WStestApp.isConnectedToClient = True
#        self.factory.broadcast("nunu")

    def onMessage(self, payload, isBinary):
        if not isBinary:

            print("broadcasting prepared message '{}' ..".format(payload))

    def connectionLost(self, reason):
        WebSocketServerProtocol.connectionLost(self, reason)
        self.factory.unregister(self)

class BroadcastServerFactory(WebSocketServerFactory):

    def __init__(self, url):
        WebSocketServerFactory.__init__(self, url)
        self.clients = []

    def register(self, client):
        if client not in self.clients:
            self.clients.append(client)

    def unregister(self, client):
        if client in self.clients:
            self.clients.remove(client)

    def broadcast(self, msg):
        for c in self.clients:
            c.sendMessage(msg.encode('utf8'))

    def broadcastBinary(self, msg):
        for c in self.clients:
            c.sendMessage(msg, isBinary=True)

class BroadcastPreparedServerFactory(BroadcastServerFactory):

    """
    Functionally same as above, but optimized broadcast using
    prepareMessage and sendPreparedMessage.
    """

    def broadcast(self, msg):
        print("broadcasting prepared message '{}' ..".format(msg))
        preparedMsg = self.prepareMessage(msg)
        for c in self.clients:
            c.sendPreparedMessage(preparedMsg)
            print("prepared message sent to {}".format(c.peer))


class WStestApp(Widget):

    isConnectedToClient = False

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.wsStr = "ws://" + self.get_ip() + ":9000"
        ServerFactory = BroadcastServerFactory
        self.factory = ServerFactory(self.wsStr)
        self.factory.protocol = BroadcastServerProtocol
        listenWS(self.factory)
        print(self.wsStr)


    def build(self):
        print("OK")

    def iscon(self):
        return self.isConnectedToClient

    def txtBtn(self, instance):
        T = instance
        print(T)
        self.factory.broadcast(T)

    def getTheIP(self):
        return self.wsStr

    def sndBinary(self, instance):
        T = instance
        self.factory.broadcastBinary(T)

    def change_label(self, msg):
        print(msg)

    def get_ip(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            # doesn't even have to be reachable
            s.connect(('10.255.255.255', 1))
            IP = s.getsockname()[0]
        except Exception:
            IP = '127.0.0.1'
        finally:
            s.close()
        return IP

if __name__ == '__main__':
    WStestApp().run()

