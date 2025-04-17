import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time

# MQTT server configuration
BROKER = "broker.emqx.io"
LED_GPIO_PIN = 19  # BCM pin number for the LED

# Setup GPIO configuration
GPIO.setmode(GPIO.BCM)  # Use Broadcom SOC channel numbering
GPIO.setup(LED_GPIO_PIN, GPIO.OUT)  # Configure LED pin as output

def on_message(client, userdata, msg):
    """Process incoming MQTT messages and control LED accordingly"""
    received_msg = msg.payload.decode("utf-8")
    print("Message received:", received_msg)

    # Pattern for wave detection (4 quick blinks)
    if received_msg == "IsuruWave":
        for i in range(4):  # Repeat the pattern 4 times
            GPIO.output(LED_GPIO_PIN, GPIO.HIGH)  # Illuminate LED
            time.sleep(0.5)  # Maintain illumination for 0.5s
            GPIO.output(LED_GPIO_PIN, GPIO.LOW)  # Turn off LED
            time.sleep(0.5)  # Pause between blinks
            
    # Pattern for pat detection (5 slow blinks)
    elif received_msg == "IsuruPat":
        for i in range(5):  # Repeat the pattern 5 times
            GPIO.output(LED_GPIO_PIN, GPIO.HIGH)  # Illuminate LED
            time.sleep(1)  # Maintain illumination for 1s
            GPIO.output(LED_GPIO_PIN, GPIO.LOW)  # Turn off LED
            time.sleep(1)  # Pause between blinks
            
def handle_broker_connection(client, userdata, flags, return_code):
    """Manage MQTT broker connection status"""
    if return_code == 0:
        print("Successfully connected to MQTT server!")
        # Subscribe to both gesture detection channels
        client.subscribe("SIT210/waveS")  # Wave notifications
        client.subscribe("SIT210/pat")    # Pat notifications
    else:
        print(f"Connection failed with error code: {return_code}")
        sys.exit(1)  # Terminate on connection failure

# Create MQTT client instance
mqtt_client = mqtt.Client()
# Assign callback functions
mqtt_client.on_connect = handle_broker_connection
mqtt_client.on_message = handle_incoming_message

try:
    # Establish connection with broker (60s timeout)
    mqtt_client.connect(BROKER_ADDRESS, 1883, 60)
    # Maintain continuous network connection
    mqtt_client.loop_forever()  
    
except KeyboardInterrupt:
     print("Execution stopped by user input")
except Exception as e:
      print(f"Unexpected error: {str(e)}")
finally:
     # Cleanup resources before exiting
     GPIO.cleanup()  # Reset GPIO configuration
     mqtt_client.disconnect()  # Close network connection
     print("System resources released and connection terminated")
