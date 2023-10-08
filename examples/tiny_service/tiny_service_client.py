import RobotRaconteur as RR
import time
import threading
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

# Send overlapping requests
async_errors = []
async_events = [threading.Event() for i in range(10)]

def async_error_handler(i,err):
    print(f"Async error {i}: {err}")
    if err is not None:
        async_errors.append(err)
    async_events[i].set()

for i in range(10):
    c.async_set_d1(i,lambda err, i=i: async_error_handler(i,err))

for i in range(10):
    async_events[i].wait()

assert len(async_errors) == 0

print("Done!")

