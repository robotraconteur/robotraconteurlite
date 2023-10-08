import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s

tiny_service_robdef = "service example.tiny_service\n\n" \
    "option version 0.10\n\n" \
    "object tiny_object\n" \
    "property double d1\n" \
    "end\n\n"

class tiny_object:
    @property
    def d1(self):
        return 3.14
    @d1.setter
    def d1(self, value):
        print(f"Setting d1 to {value}")

obj = tiny_object()

RRN.SetLogLevel(RR.LogLevel_Debug)
t = RR.TcpTransport()
t.StartServer(22229)
RRN.RegisterTransport(t)

RRN.SetNodeName("tiny_client_service_node")

RRN.RegisterServiceType(tiny_service_robdef)

RRN.RegisterService("tiny_service", "example.tiny_service.tiny_object", obj)

input("Press enter to quit...\n")
