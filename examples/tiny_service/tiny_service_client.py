import RobotRaconteur as RR
import time
RRN = RR.RobotRaconteurNode.s

RRN.SetLogLevel(RR.LogLevel_Debug)

t = RR.TcpTransport()
RRN.RegisterTransport(t)

c = RRN.ConnectService('rr+tcp://localhost:22228?service=tiny_service')

time.sleep(30)

print("Client connected!")