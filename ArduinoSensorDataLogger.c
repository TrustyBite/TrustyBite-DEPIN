#include <DHT11.h>

// Define analog pins for MQ sensors
#define MQ3_PIN A0      // MQ-3 sensor (Alcohol)
#define MQ4_PIN A2      // MQ-4 sensor (Methane)
#define MQ135_PIN A1    // MQ-135 sensor (Air Quality)
#define MQ8_PIN A3      // MQ-8 sensor (Hydrogen)
#define MQ137_PIN A4    // MQ-137 sensor (Ammonia)

// Define digital pin for DHT11 sensor (using pin 2 as per your setup)
DHT11 dht11(2);

// Define the interval for printing data (15 seconds)
#define PRINT_INTERVAL 15000

// Variables to store sensor values
int mq3Value, mq4Value, mq135Value, mq8Value, mq137Value;
int temperature = 0, humidity = 0;  // Variables for temperature and humidity
unsigned long previousMillis = 0;   // Stores last time a reading was taken

void setup() {
    // Initialize serial communication for debugging and sensor data readout
    Serial.begin(9600);
    Serial.println("Starting Sensor Tracking...");
    
    delay(20000);  // Warm-up time for MQ sensors
}

void loop() {
    unsigned long currentMillis = millis();  // Get the current time

    // Only print every 15 seconds
    if (currentMillis - previousMillis >= PRINT_INTERVAL) {
        previousMillis = currentMillis;  // Update time for the next cycle
        
        // Read MQ sensor values
        mq3Value = analogRead(MQ3_PIN);       // Read MQ-3 sensor (Alcohol)
        mq4Value = analogRead(MQ4_PIN);       // Read MQ-4 sensor (Methane)
        mq135Value = analogRead(MQ135_PIN);   // Read MQ-135 sensor (Air Quality)
        mq8Value = analogRead(MQ8_PIN);       // Read MQ-8 sensor (Hydrogen)
        mq137Value = analogRead(MQ137_PIN);   // Read MQ-137 sensor (Ammonia)

        // Read temperature and humidity from DHT11 sensor
        int result = dht11.readTemperatureHumidity(temperature, humidity);

        // Print MQ sensor values along with gas type
        Serial.println("---- Sensor Data ----");

        Serial.print("MQ-3 (Alcohol): ");
        Serial.print(mq3Value);
        Serial.println(" | Gas: Alcohol");

        Serial.print("MQ-4 (Methane): ");
        Serial.print(mq4Value);
        Serial.println(" | Gas: Methane");

        Serial.print("MQ-135 (Air Quality): ");
        Serial.print(mq135Value);
        Serial.println(" | Gas: CO2, NH3, Alcohol, Benzene, Smoke");

        Serial.print("MQ-8 (Hydrogen): ");
        Serial.print(mq8Value);
        Serial.println(" | Gas: Hydrogen");

        Serial.print("MQ-137 (Ammonia): ");
        Serial.print(mq137Value);
        Serial.println(" | Gas: Ammonia");

        // Print temperature and humidity in the same format as MQ sensors
        if (result == 0) {
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.println(" °C | Data: Temperature");

            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println(" % | Data: Humidity");
        } else {
            // Print error message based on the error code
            Serial.println(DHT11::getErrorString(result));
        }

        Serial.println("---------------------");
    }
}
