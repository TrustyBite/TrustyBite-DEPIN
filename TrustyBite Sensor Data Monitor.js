const { SerialPort, ReadlineParser } = require('serialport');
const { exec } = require('child_process');

// Create a port connection
const port = new SerialPort({ path: '/dev/ttyACM0', baudRate: 9600 });

// Use a parser to handle incoming data
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

// Initialize sensor data with default values of 0
let sensorData = {
    Temperature: 0,
    Humidity: 0,
    "MQ-3": 0,
    "MQ-4": 0,
    "MQ-135": 0,
    "MQ-8": 0,
    "MQ-137": 0
};
let lastSentTime = Date.now();

// Function to parse and update sensor data
function updateSensorData(line) {
    const regexes = {
        "Temperature": /Temperature:\s*([\d.]+)/,
        "Humidity": /Humidity:\s*([\d.]+)/,
        "MQ-3": /MQ-3 \(Alcohol\):\s*([\d.]+)/,
        "MQ-4": /MQ-4 \(Methane\):\s*([\d.]+)/,
        "MQ-135": /MQ-135 \(Air Quality\):\s*([\d.]+)/,
        "MQ-8": /MQ-8 \(Hydrogen\):\s*([\d.]+)/,
        "MQ-137": /MQ-137 \(Ammonia\):\s*([\d.]+)/
    };

    for (let key in regexes) {
        const match = line.match(regexes[key]);
        if (match) {
            sensorData[key] = parseFloat(match[1]);
        }
    }
}

// Function to send data to NEAR smart contract
function sendDataToSmartContract() {
    // Check if all sensor values are different from 0
    const allDataAvailable = Object.values(sensorData).every(value => value !== 0);
    
    if (!allDataAvailable) {
        console.log("Not all sensor data is available. Waiting for next reading...");
        return; // Do not send if any value is still 0
    }

    const command = `near call trustybite.testnet add_sensor_data '{
        "restaurant_id": "veggiehaven",
        "category": "vegetables",
        "humidity": ${sensorData.Humidity},
        "temperature": ${sensorData.Temperature},
        "mq3": ${sensorData['MQ-3']},
        "mq4": ${sensorData['MQ-4']},
        "mq8": ${sensorData['MQ-8']},
        "mq135": ${sensorData['MQ-135']},
        "mq137": ${sensorData['MQ-137']}
    }' --accountId veggiehaven.testnet --gas 300000000000000`;

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error sending data: ${error}`);
            return;
        }
        console.log(`Data sent to NEAR smart contract:\n${stdout}`);
    });
}

// Event listener for data from Arduino
parser.on('data', (data) => {
    console.log(`Received from Arduino: ${data.trim()}`);
    updateSensorData(data);

    // Check if 40 seconds have passed since the last data sent
    const now = Date.now();
    if (now - lastSentTime >=  900000) {  // 900000 ms = 15 minutes
        sendDataToSmartContract();
        lastSentTime = now;

        // Reset sensor data to 0 after checking
        sensorData = {
            Temperature: 0,
            Humidity: 0,
            "MQ-3": 0,
            "MQ-4": 0,
            "MQ-135": 0,
            "MQ-8": 0,
            "MQ-137": 0
        };
    }
});
