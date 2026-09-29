import paho.mqtt.client as mqtt
import json 
import base64
import struct


# Create MQTT client
client = mqtt.Client(protocol=mqtt.MQTTv311)
client2 = mqtt.Client(protocol=mqtt.MQTTv311)

# MQTT Configuration
MQTT_BROKER = 'nam1.cloud.thethings.network'  # Change to your MQTT broker address
MQTT_PORT = 1883
MQTT_USERNAME = 'radioandre@ttn'  # Set to your MQTT username if needed
MQTT_PASSWORD = 'NNSXS.YPNVTZZUUL6SFQUD7JD6BP6NJRHCCZYW7KCVVZY.M7XU46PGTOHE6THZNUGM724GEAHGTAZJV253TMYNZNAJ5EP6EHEA'  # Set to your MQTT password if needed

# MQTT2 Configuration
MQTT_BROKER2 = 'localhost'  # Change to your MQTT broker address
MQTT_PORT2 = 1883
MQTT_USERNAME2 = 'labsc'  # Set to your MQTT username if needed
MQTT_PASSWORD2 = 'labsc'  # Set to your MQTT password if needed


#Connection declarations
def on_connect(client, userdata, flags, rc):
    """Callback when connected to MQTT Broker 2"""
    if rc == 0:
        print("Connected to MQTT Broker (TTN) successfully with code {0}".format(str(rc)))
        
        client.subscribe("v3/radioandre@ttn/devices/radio-tcc-9048/up")
    else:
        print(f"Failed to connect to Broker (TTN), return code: {0}".format(str(rc)))


def on_connect2(client, userdata, flags, rc):
    """Callback when connected to MQTT Broker 2"""
    if rc == 0:
        print("Connected to MQTT Broker 2 (Home Assistant) successfully with code {0}".format(str(rc)))
        
        #publish_discovery_configs(client)
    else:
        print(f"Failed to connect to Broker 2 (Home Assistant), return code: {0}".format(str(rc)))    


# Message received
def on_message(client, userdata, msg):  # The callback for when a PUBLISH message is received from the server.
    global pir_active

    y = json.loads(msg.payload)

    data_decode = base64.b64decode(y["uplink_message"]["frm_payload"])
    
    seq_no, nivel_mm, ph_x100, turbidez_x10, temperatura_x10, vazao_x10 = struct.unpack('=HHHHHH', data_decode)

    ph = ph_x100/100
    turbidez = turbidez_x10/10
    temperatura = temperatura_x10/10
    vazao = vazao_x10/10

    print(f"Leitura N°{seq_no}")
    print(f"  Nível:       {nivel_mm} mm")
    print(f"  pH:          {ph:.2f}")
    print(f"  Turbidez:    {turbidez:.1f} NTU")
    print(f"  Temperatura: {temperatura:.1f} °C")
    print(f"  Vazão:       {vazao:.1f} L/min")

    #Decide o valor do PWM do compressor
    # setpoint = 40
    # warning_status = 0

    # if temp >= (setpoint+2):
        # compressor_power = 100
    # elif temp <= (setpoint-2):
        # compressor_power = 0


    #transforms the python variables magic_number and send_value in a byte array named ba

    #ba = struct.pack('HB', compressor_power, warning_status)
    #m = base64.b64encode(ba)
    #payload = m.decode("utf-8")


    #create a python dictionary named resp with the keys required by the chirpstack format

    # resp = {"downlinks": [{"fPort": 2, "frm_payload": payload, "priority": "NORMAL"}]}
    
    
    #convert the dictionary in a JSON string named js
    
    # js = json.dumps(resp, indent = 4)


    #gets [ApplicationID] and [DevEUI] from the uplink topic

    # topics = msg.topic.split("/")


    #creates the downlink topic
    
    #t = '{:s}/{:s}/{:s}/{:s}/down/push'.format(topics[0],topics[1],topics[2],topics[3])
    #h = 'home/sensor1/temperature'


    #enqueue downlink using MQTT
    # client.publish(t,js)
    client2.publish("home/sensor1/nivel", nivel_mm)
    client2.publish("home/sensor1/ph", ph)
    client2.publish("home/sensor1/turbidez", turbidez)
    client2.publish("home/sensor1/temperature", temperatura)
    client2.publish("home/sensor1/vazao", vazao)


def main():
    """Main function"""
    print("Starting Home Assistant MQTT Auto-Discovery and Communication Script")
    print(f"MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    if MQTT_BROKER2 and MQTT_PORT2:
        print(f"MQTT Broker 2: {MQTT_BROKER2}:{MQTT_PORT2}")
    
    # Call MQTT clients
    global client
    global client2
    
    # Set username and password if provided
    if MQTT_USERNAME and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    if MQTT_USERNAME2 and MQTT_PASSWORD2:
        client2.username_pw_set(MQTT_USERNAME2, MQTT_PASSWORD2)
    
    # Set callbacks
    client.on_connect = on_connect
    client.on_message = on_message
    client2.on_connect = on_connect2
    
    # Connect to broker
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
    except Exception as e:
        print(f"Error connecting to MQTT Broker: {e}")
        return
    
    try:
        client2.connect(MQTT_BROKER2, MQTT_PORT2, 60)
    except Exception as e:
        print(f"Error connecting to MQTT Broker 2: {e}")
        return
    
    # Start the network loop in a background thread
    client2.loop_start()
    client.loop_forever()

if __name__ == "__main__":
    main()

