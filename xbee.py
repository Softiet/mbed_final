import paho.mqtt.client as paho
import serial
import time

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)


mqttc = paho.Client()


# Settings for connection
# TODO: revise host to your ip
host = "localhost"
topic = "BBcar"

data = []
# Callbacks
def on_connect(self, mosq, obj, rc):
      print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
      print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")


def on_subscribe(mosq, obj, mid, granted_qos):
      print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
      print("Unsubscribed OK")

# Set callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

# Connect and subscribe
print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)

print("ALL SYSTEM GO")
f = open("log.txt","w+")
f.close()
while True:
    line = s.readline()
    f = open("log.txt","a+")
    f.write(line.decode("UTF-8") + "\n")
    f.close()
    print(line.decode("UTF-8"))
    mqttc.publish(topic, line)
    if line == "end":
        f.close()
        break