import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time

# MQTT network configuration
BROKER = "broker.emqx.io"
TOPIC = "SIT210/waves"
LED_PIN = 19  # GPIO pin connected to the LED

# Configure Raspberry Pi GPIO settings
GPIO.setmode(GPIO.BCM)  # Use Broadcom pin numbering
GPIO.setup(LED_PIN, GPIO.OUT)  # Set LED pin as output

# Ensure LED starts in off state
GPIO.output(LED_PIN, GPIO.LOW)

# Callback function for incoming messages
def on_message(client, userdata, msg):
    message = msg.payload.decode("utf-8")
    print("Received:", message)

    # If specific message received, activate LED pattern
    if message == "Isuru":
        for _ in range(4):  # Repeat blinking sequence 4 times
            GPIO.output(LED_PIN, GPIO.HIGH)  # Turn LED on
            time.sleep(0.5)  # Keep on for half second
            GPIO.output(LED_PIN, GPIO.LOW)  # Turn LED off
            time.sleep(0.5)  # Keep off for half second

# Connection establishment handler
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Successfully connected to MQTT server!")
        client.subscribe(TOPIC)  # Begin listening to the topic
    else:
        print(f"Connection unsuccessful, error code {rc}")
        sys.exit(1)  # Terminate if connection fails

# Create MQTT client instance
client = mqtt.Client()
# Assign callback functions
client.on_connect = on_connect
client.on_message = on_message

try:
    # Establish connection with broker
    client.connect(BROKER, 1883, 60)
    # Continuous network loop (blocks execution)
    client.loop_forever()  
except KeyboardInterrupt:
    print("User terminated the program")
except Exception as e:
    print(f"Unexpected error: {str(e)}")
finally:
    # Cleanup resources before exiting
    GPIO.cleanup()  # Reset GPIO configuration
    client.disconnect()  # Close MQTT connection
    print("System resources released and connection closed")
