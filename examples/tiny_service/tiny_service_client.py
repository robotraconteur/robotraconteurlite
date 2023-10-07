import RobotRaconteur as RR
import time
RRN = RR.RobotRaconteurNode.s

RRN.SetLogLevel(RR.LogLevel_Debug)

t = RR.TcpTransport()
RRN.RegisterTransport(t)

c = RRN.ConnectService('rr+tcp://localhost:22228?service=tiny_service')
print(c.d1)
c.d1 = 5

time.sleep(0.5)

c2 = RRN.ConnectService('rr+ws://localhost:22228?service=tiny_service')
print(c2.d1)
c2.d1 = 4