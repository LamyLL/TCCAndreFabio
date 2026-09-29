import paho.mqtt.client as mqtt
import json 
import base64
import struct

#def on_connect(client, userdata, flags, reason_code, properties):
def on_connect(client, userdata, flags, reason_code):
    print("Connected with reason_code {0}".format(str(reason_code)))  # Print result of connection attempt
    client.subscribe("v3/radioandre@ttn/devices/shrek/up")  # Subscribe to the topic “digitest/test1”, receive any messages published on it


def on_message(client, userdata, msg):  # The callback for when a PUBLISH message is received from the server.
    global pir_active

    y = json.loads(msg.payload)

    data_decode = base64.b64decode(y["uplink_message"]["frm_payload"])
    
    seq_no, temperature = struct.unpack('=Ii', data_decode)
    
    temp = temperature/100

    print("Leitura", seq_no, ":", temp, "°C")

    #Decide o valor do PWM do compressor
    setpoint = 40
    warning_status = 0

    if temp >= (setpoint+2):
        compressor_power = 100
    elif temp <= (setpoint-2):
        compressor_power = 0


    #transforms the python variables magic_number and send_value in a byte array named ba
    ba = struct.pack('HB', compressor_power, warning_status)
    m = base64.b64encode(ba)
    payload = m.decode("utf-8")

    #create a python dictionary named resp with the keys required by the chirpstack format
    resp = {"downlinks": [{"fPort": 2, "frm_payload": payload, "priority": "NORMAL"}]}
    
    
    #convert the dictionary in a JSON string named js
    js = json.dumps(resp, indent = 4)
    #gets [ApplicationID] and [DevEUI] from the uplink topic
    topics = msg.topic.split("/")
    #creates the downlink topic
    t = '{:s}/{:s}/{:s}/{:s}/down/push'.format(topics[0],topics[1],topics[2],topics[3])
    h = 'home/sensor1/temperature'
    #enqueue downlink using MQTT
    client.publish(t,js)
    client2.publish(h,temp)


client = mqtt.Client(protocol=mqtt.MQTTv311)  # Create instance of client with client ID “digi_mqtt_test”
client2 = mqtt.Client(protocol=mqtt.MQTTv311)
client.on_connect = on_connect  # Define callback function for successful connection
client2.on_connect = on_connect  # Define callback function for successful connection
client.on_message = on_message  # Define callback function for receipt of a message

client.username_pw_set(\
    username="radioandre@ttn",\
    password="NNSXS.JNCFS5T3FPSMOYCH3X344HA7AVLK4S3VUP7LN5Q.GG4BNLVUUXDMTRIV6SR7R5NSYQ5OGC4AGIXGFCMQABTON4FGOEVQ")
client.connect('nam1.cloud.thethings.network', 1883, 60)

client2.username_pw_set(\
    username="labsc",\
    password="labsc")
client2.connect('localhost', 1883, 60)

client2.loop_start()
client.loop_forever()  # Start networking daemon