import paho.mqtt.client as mqtt
import time
from datetime import datetime,timedelta
import traceback
import requests
from phue import Bridge  # https://github.com/studioimaginaire/phue
from influxdb import InfluxDBClient


hueDry = 65535
hueWet = 0
min = 800
max = 840

transitionTime = 20

def getInfluxClient(host='localhost', port=8086):
    user=''
    password=''
    dbname = 'home'
    client = InfluxDBClient(host, port, user, password, dbname)
    return client

def getValue(client, reading):
    query =  f"SELECT MEAN(value) FROM mqtt_consumer   WHERE time > now() - 30s  and topic = 'home/tele/{reading}/livingroom/window'"
    result = client.query(query)
    value = list(result.get_points(measurement='mqtt_consumer'))[0]['mean']
    return value

host = '192.168.0.14'

b = Bridge(host)

# If the app is not registered and the button is not pressed, press the button and call connect() (this only needs to be run a single time)
b.connect()

# Get the bridge state (This returns the full dictionary that you can explore)
b.get_api()

influxClient = getInfluxClient()

def getMqttClient():
    client = mqtt.Client()
    while (True):
        try:
            client.connect("192.168.0.63", 1883, 60)
            break
        except Exception:
            print ("error connecting, pausing")
            traceback.print_exc()
            time.sleep(5)
    return client

def mapRange( x,  in_min,  in_max,  out_min,  out_max):
  return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)



def postToLights(hueReading):
    
    key='Zk16ZQhoxu1MHAJskpApN8i-y8xg0EfGULyBMHS7'
    lightId = 7
    
    # uri =f"https://{host}/api/{key}/lights/{lightId}/state"
    # print(uri)

    try:
        # if hueReading > hueWet and  hueReading < hueDry:
        # b.set_light(lightId, 'hue', hueReading)
        temperature = getValue(influxClient, 'temperature')
        mappedTemperature= mapRange(temperature,15,35,230,254)
        print(f"temp: {temperature}C, mapped: {mappedTemperature}")
        # b.set_light(lightId, 'sat', mappedTemperature)
        humidity = getValue(influxClient, 'humidity')
        mappedHumidity = mapRange(humidity,60,100,200,254)
        print(f"humidity: {humidity}C, mapped: {mappedHumidity}")
        # b.set_light(lightId, 'bri', mappedHumidity) 
        
        command =  {'transitiontime' : transitionTime,  'hue':  hueReading, 'sat':mappedTemperature, 'bri': mappedHumidity}
        b.set_light(lightId,command)
        light= b.get_light(lightId)
        print(light)
    except Exception:
        print ("error posting hue data")
        traceback.print_exc()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("home/tele/soilmoisture/livingroom/yucca")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    # print(f"new message")
    print(f"new message {msg.topic}: {str(msg.payload)}")
    reading =int(msg.payload.decode("utf-8"))
    if reading>min and reading<max:


        # hueReading = getHue(reading)
        mapped = mapRange(reading,min,max,hueWet,hueDry)
        client.publish('home/cmd/hue/tv/', mapped)
        print (f"reading : {reading} =  {mapped} ")
        postToLights(mapped)
        


client = getMqttClient()
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
