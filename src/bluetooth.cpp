#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class MyServerCallbacks : public BLEServerCallbacks {
    private:
        // Reference to a boolean flag in BluetoothManager to update connection status
        bool& _deviceConnected;
    
    public:
        // Constructor: Takes a reference to the connection status flag
        MyServerCallbacks(bool& deviceConnectedFlag) : _deviceConnected(deviceConnectedFlag) {}
    
        // Called when a BLE client connects
        void onConnect(BLEServer* pServer) override {
            Serial.println("BLE connected");
            _deviceConnected = true;
        }
    
        // Called when a BLE client disconnects
        void onDisconnect(BLEServer* pServer) override {
            Serial.println("BLE disconnected");
            _deviceConnected = false;
            // Restart advertising to allow new connections
            pServer->getAdvertising()->start();
        }
    };
    
    // Callback class for BLE Characteristic write events (from client to server)
    class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    private:
        // Callback function to be invoked when a state change request is received via BLE
        // This function will typically be set by the LightControlApp to handle the request.
        std::function<void(const String&, bool)> _onCommandReceived;
    
    public:
        // Constructor: Takes a function pointer/lambda for handling received commands
        MyCharacteristicCallbacks(std::function<void(const String&, bool)> onCommand)
            : _onCommandReceived(onCommand) {}
    
        // Called when a client writes to this characteristic
        void onWrite(BLECharacteristic *pCharacteristic) override {
            std::string rxValue = pCharacteristic->getValue(); // Get the received value
    
            if (rxValue.length() > 0) {
                Serial.print("Received BLE command: ");
                for (int i = 0; i < rxValue.length(); i++) {
                    Serial.print(rxValue[i]);
                }
                Serial.println();
    
                // Example: Parse command "NAME,STATE" (e.g., "Toplicht,ON" or "Navilicht,OFF")
                String command = String(rxValue.c_str());
                int commaIndex = command.indexOf(',');
                if (commaIndex != -1) {
                    String name = command.substring(0, commaIndex);
                    String stateStr = command.substring(commaIndex + 1);
                    bool newState = (stateStr.equalsIgnoreCase("ON") || stateStr.equalsIgnoreCase("ACTIVE"));
    
                    if (_onCommandReceived) {
                        _onCommandReceived(name, newState);
                    }
                }
            }
        }
    };
    
    
    // Manages all Bluetooth Low Energy (BLE) communication
    class BluetoothManager {
    private:
        BLECharacteristic* _pCharacteristic; // The BLE characteristic for communication
        bool _deviceConnected; // Flag to track BLE connection status
        MyServerCallbacks* _serverCallbacks; // Instance of server callbacks
        MyCharacteristicCallbacks* _characteristicCallbacks; // Instance of characteristic callbacks
    
    public:
        // Constructor: Initializes the connection flag and sets up callback instances
        BluetoothManager() : _pCharacteristic(nullptr), _deviceConnected(false) {
            _serverCallbacks = new MyServerCallbacks(_deviceConnected);
        }
    
        // Destructor: Cleans up dynamically allocated callback objects
        ~BluetoothManager() {
            delete _serverCallbacks;
            delete _characteristicCallbacks;
        }
    
        // Initializes the BLE service
        void init(const char* bleName, const char* serviceUUID, const char* characteristicUUID,
                  std::function<void(const String&, bool)> onCommand) {
            Serial.println("Initializing BLE...");
            BLEDevice::init(bleName); // Initialize BLE device with a name
            BLEServer* pServer = BLEDevice::createServer(); // Create a BLE server
            pServer->setCallbacks(_serverCallbacks); // Set server callbacks
    
            BLEService* pService = pServer->createService(serviceUUID); // Create a BLE service
            _pCharacteristic = pService->createCharacteristic(
                characteristicUUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE |
                BLECharacteristic::PROPERTY_NOTIFY
            );
            _pCharacteristic->addDescriptor(new BLE2902()); // Add a standard descriptor
            
            // Set characteristic callbacks for incoming writes
            _characteristicCallbacks = new MyCharacteristicCallbacks(onCommand);
            _pCharacteristic->setCallbacks(_characteristicCallbacks);
    
            pService->start(); // Start the BLE service
            pServer->getAdvertising()->start(); // Start advertising to allow connections
            Serial.println("BLE service started and advertising.");
        }
    
        // Sends a state update string over BLE
        void sendStateUpdate(const String& message) {
            if (_deviceConnected && _pCharacteristic) {
                _pCharacteristic->setValue(message.c_str()); // Set the characteristic value
                _pCharacteristic->notify(); // Notify connected clients
                // Serial.println("BLE Notified: " + message); // Uncomment for debugging
            }
        }
    
        // Checks if a device is currently connected
        bool isDeviceConnected() const {
            return _deviceConnected;
        }
    };