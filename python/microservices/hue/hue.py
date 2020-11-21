import paho.mqtt.client as mqtt
import time
from datetime import datetime,timedelta
import traceback
import requests
from phue import Bridge  # https://github.com/studioimaginaire/phue
from influxdb import InfluxDBClient
import json


hueDry = 65535
hueWet = 0
min = 800
max = 840

plants = ['yucca', 'amaryllis', 'bonsai', 'aralia']
plantTopics ="home/tele/soilmoisture/livingroom/"

transitionTime = 20

plantSeconds=180
currentPlantCount=0


def getPlantChangeTime():
    return datetime.now() + timedelta(0,plantSeconds)


plantChangeTime= getPlantChangeTime()
print(f"plantChangeTime: {plantChangeTime}")

def getInfluxClient(host='localhost', port=8086):
    user=''
    password=''
    dbname = 'home'
    client = InfluxDBClient(host, port, user, password, dbname)
    return client

def getValue(client, reading, default):

    try:
        query =  f"SELECT MEAN(value) FROM mqtt_consumer   WHERE time > now() - 30s  and topic = 'home/tele/{reading}/livingroom/window'"
        result = client.query(query)
        value = (list(result.get_points(measurement='mqtt_consumer'))[0]['mean'])
        return value
    except:
        return default

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



def postToLights(plantName, reading):
    lightId = 7
    mapped = mapRange(reading,min,max,hueWet,hueDry)
    try:
        temperature = getValue(influxClient, 'temperature', 22)
        mappedTemperature= mapRange(temperature,15,30,230,254)
        print(f"temp: {temperature:.2f}C, mapped: {mappedTemperature}")
        humidity = getValue(influxClient, 'humidity',60)
        mappedHumidity = mapRange(humidity,60,100,200,254)
        print(f"humidity: {humidity:.2f}%, mapped: {mappedHumidity}")
        
        command =  {'transitiontime' : transitionTime,  'hue':  mapped, 'sat':mappedTemperature, 'bri': mappedHumidity}
        print(command)
        b.set_light(lightId,command)
        # b.set_light(2,command)

        lightInfo= str(b.get_light(lightId))
        body={}
        body['hue']=lightInfo['state']
        nextPlantTime=(plantChangeTime - datetime.now()).seconds
        plantInfo ={'name':plantName, 'reading': reading, 'nextPlantAt': nextPlantTime}
        body['plant']=plantInfo
        jsonMsg = json.dumps(body)
        client.publish('home/cmd/hue/tv', jsonMsg)
    except Exception:
        print ("error posting hue data")
        traceback.print_exc()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(f"{plantTopics}#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    # print(f"new message")
    global plantChangeTime 
    global currentPlantCount 
    # print(f"new message {msg.topic}: {str(msg.payload)}")
    plantCount=len(plants)

    if datetime.now() > plantChangeTime:
        plantChangeTime = getPlantChangeTime()
        currentPlantCount+=1
        

    currentPlantIndex = currentPlantCount % plantCount
    currentPlantName = plants[currentPlantIndex]
    currentPlantTopic = f"{plantTopics}{currentPlantName}"
    if currentPlantTopic == str(msg.topic):
        reading =int(msg.payload.decode("utf-8"))
        print (f"currentPlantCount: {currentPlantCount}, currentPlantIndex: {currentPlantIndex}, currentPlantName: {currentPlantName} ")
        
        if reading>min and reading<max:
            nextPlantTime=plantChangeTime - datetime.now()
            print(f"processing {currentPlantName}: {reading}, next plant in {nextPlantTime}")
            postToLights( currentPlantName, reading)
        


client = getMqttClient()
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
