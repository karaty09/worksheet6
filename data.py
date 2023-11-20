import paho.mqtt.client as mqtt
import json
import requests
def on_connect(client, userdata, flags, rc):
    print("Connected to broker " + str(rc))
    client.subscribe("dht11")

def on_message(client, userdata, msg):
    # print(msg.payload.decode("utf-8", "strict"))
    payload = msg.payload.decode()
    print(f"message on topic {msg.topic}: {payload}")
    try:
        d = json.loads(payload)
        print("Decoded JSON data:", d)

        server_url = "http://192.168.0.119:3000/DataWeb"

        response = requests.post(server_url, json=d)
        print(f"HTTP Response: {response.status_code}")

        if response.status_code == 200:
            print("Data sent successfully")
    except json.JSONDecodeError as e:(
        print(f"Error decoding JSON: {e}"))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.0.119", port=1883)
client.loop_forever()