import RobotRaconteur as RR
import time
RRN = RR.RobotRaconteurNode.s

RRN.SetLogLevel(RR.LogLevel_Debug)

t = RR.TcpTransport()
RRN.RegisterTransport(t)

c = RRN.ConnectService('rr+tcp://localhost:22228?service=tiny_service')

print(c.d1)

c.d1 = 5

time.sleep(2)

print("Client connected!")