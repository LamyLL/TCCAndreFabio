import paho.mqtt.client as mqtt
import json 
import base64
import struct
import time

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

# Device Configuration
DEVICE_ID = 'heatbox'
DEVICE_NAME = 'Heating Box'

# Slider Configuration (Number entity)
SLIDER_NAME = "Heater Level"
SLIDER_OBJECT_ID = "lab_heater_level"
SLIDER_UNIQUE_ID = f"{DEVICE_ID}_heater_level"

# Temperature Sensor Configuration
TEMP_SENSOR_NAME = "Thermistor"
TEMP_SENSOR_OBJECT_ID = "lab_thermistor"
TEMP_SENSOR_UNIQUE_ID = f"{DEVICE_ID}_thermistor"

# MQTT Topics
SLIDER_CONFIG_TOPIC = f"homeassistant/number/{SLIDER_OBJECT_ID}/config"
SLIDER_STATE_TOPIC = f"homeassistant/number/{SLIDER_OBJECT_ID}/state"
SLIDER_COMMAND_TOPIC = f"homeassistant/number/{SLIDER_OBJECT_ID}/set"

TEMP_CONFIG_TOPIC = f"homeassistant/sensor/{TEMP_SENSOR_OBJECT_ID}/config"
TEMP_STATE_TOPIC = f"homeassistant/sensor/{TEMP_SENSOR_OBJECT_ID}/state"


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
        print("Connected to MQTT Broker 2 (Home Assistant) successfully")
        
        publish_discovery_configs(client)
    else:
        print(f"Failed to connect to Broker 2 (Home Assistant), return code: {0}".format(str(rc)))


def on_message(client, userdata, msg):  # The callback for when a PUBLISH message is received from the server.
    global pir_active, client2, last_timestamp, pid

    y = json.loads(msg.payload)
    data_decode = base64.b64decode(y["uplink_message"]["frm_payload"])
    seq_no, temperature = struct.unpack('=Ii', data_decode)
    temp = temperature/100

    print("Leitura", seq_no, ":", temp, "°C")

    now = time.time()
    dt = now - last_timestamp
    last_timestamp = now

    #Decide o valor do PWM do compressor
    warning_status = 0

    compressor_power = int(round(pid.compute(temp, dt)))

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
    #enqueue downlink using MQTT
    client.publish(t,js)
    client2.publish(TEMP_STATE_TOPIC, temp, retain=True)
    client2.publish(SLIDER_STATE_TOPIC, compressor_power, retain=True)


def publish_discovery_configs(client):
    """Publish Home Assistant MQTT discovery configurations"""
    
    # Device information (shared by both entities)
    device_info = {
        "identifiers": [DEVICE_ID],
        "name": DEVICE_NAME,
        "model": 'v1',
        "manufacturer": 'UTFPR'
    }
    
    # Slider discovery configuration
    slider_config = {
        "name": SLIDER_NAME,
        "object_id": SLIDER_OBJECT_ID,
        "unique_id": SLIDER_UNIQUE_ID,
        "state_topic": SLIDER_STATE_TOPIC,
        "command_topic": SLIDER_COMMAND_TOPIC,
        "min": 0,
        "max": 100,
        "step": 1,
        "mode": "slider",
        "device": device_info
    }
    
    client.publish(SLIDER_CONFIG_TOPIC, json.dumps(slider_config), retain=True)
    print(f"Published slider discovery config to: {SLIDER_CONFIG_TOPIC}")

    # Temperature sensor discovery configuration
    temp_config = {
        "name": TEMP_SENSOR_NAME,
        "object_id": TEMP_SENSOR_OBJECT_ID,
        "unique_id": TEMP_SENSOR_UNIQUE_ID,
        "state_topic": TEMP_STATE_TOPIC,
        "unit_of_measurement": "°C",
        "device_class": "temperature",
        "state_class": "measurement",
        "device": device_info
    }
    
    client.publish(TEMP_CONFIG_TOPIC, json.dumps(temp_config), retain=True)
    print(f"Published temperature sensor discovery config to: {TEMP_CONFIG_TOPIC}")


class PIDController:
    def __init__(self, Kp, Ki, Kd, setpoint):
        self.Kp = Kp
        self.Ki = Ki
        self.Kd = Kd
        self.setpoint = setpoint
        self.previous_error = 0
        self.integral = 0
    
    def compute(self, process_variable, dt):
        # Calculate error
        error = self.setpoint - process_variable
        
        # Proportional term
        P_out = self.Kp * error
        
        # Integral term
        self.integral += error * dt
        I_out = self.Ki * self.integral
        
        # Derivative term
        derivative = (error - self.previous_error) / dt
        D_out = self.Kd * derivative
        
        # Compute total output
        output = P_out + I_out + D_out
        
        #Anti-Windup e limites do output
        if output > 100:
            output = 100
            if I_out > 100 - P_out - D_out:
                self.integral = (100 - P_out - D_out) / self.Ki
        elif output < 0:
            output = 0
            if I_out < - P_out - D_out:
                self.integral = (-P_out - D_out) / self.Ki

        # Update previous error
        self.previous_error = error

        # output = max(0, min(100, output))
        
        return output


pid = PIDController(Kp=1.0, Ki=0.1, Kd=0.05, setpoint=25)
last_timestamp = time.time()


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