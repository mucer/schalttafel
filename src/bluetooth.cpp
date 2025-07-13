#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <vector>
#include "element.h"

#define BLE_NAME "Schalttafel"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c3319141"
#define CHARACTERISTIC_NAMES "4fafc201-1fb5-459e-8fcc-c5c9c3319142"
#define CHARACTERISTIC_ALL "4fafc201-1fb5-459e-8fcc-c5c9c3319143"
#define CHARACTERISTIC_WRITE "4fafc201-1fb5-459e-8fcc-c5c9c3319144"

class BluetoothManager : public BLEServerCallbacks, public BLECharacteristicCallbacks
{
private:
    std::vector<Element *> *_states;
    BLECharacteristic *_names = nullptr;
    BLECharacteristic *_values = nullptr;
    BLECharacteristic *_write = nullptr;
    bool _deviceConnected = false;

    // uint8_t statusValue(const ElementStatus &status)
    // {
    //     switch (status)
    //     {
    //     case ElementStatus::INACTIVE:
    //         return 0x00;
    //     case ElementStatus::ACTIVE:
    //         return 0x01;
    //     case ElementStatus::ERROR:
    //         return 0x02;
    //     default:
    //         return 0x03;
    //     }
    // }

    void setNames()
    {
        String names;
        for (Element *state : *_states)
        {
            if (!names.isEmpty())
            {
                names += "|";
            }
            names += state->getName();
        }

        _names->setValue(names.c_str());
    }

    void setValues()
    {
        size_t size = _states->size();
        uint8_t temp[size];
        for (size_t i = 0; i < size; ++i)
        {
            temp[i] = static_cast<uint8_t>((*_states)[i]->getStatus());
        }

        _values->setValue(temp, size);
    }

public:
    BluetoothManager(std::vector<Element *> *states)
        : _states(states)
    {
    }

    void init()
    {
        Serial.println("Initializing BLE...");
        BLEDevice::init(BLE_NAME);

        BLEServer *pServer = BLEDevice::createServer();
        pServer->setCallbacks(this);
        BLEService *pService = pServer->createService(SERVICE_UUID);

        _names = pService->createCharacteristic(
            CHARACTERISTIC_NAMES, BLECharacteristic::PROPERTY_READ);

        _values = pService->createCharacteristic(
            CHARACTERISTIC_ALL, BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_NOTIFY);
        _values->addDescriptor(new BLE2902());

        _write = pService->createCharacteristic(
            CHARACTERISTIC_WRITE,
            BLECharacteristic::PROPERTY_WRITE);
        _write->setCallbacks(this);

        setNames();
        pService->start();
        pServer->getAdvertising()->start();
        Serial.println("BLE service started and advertising.");
    }

    void onConnect(BLEServer *pServer) override
    {
        Serial.println("BLE connected");
        _deviceConnected = true;
        setValues();
    }

    void onDisconnect(BLEServer *pServer) override
    {
        Serial.println("BLE disconnected");
        _deviceConnected = false;
        pServer->getAdvertising()->start();
    }

    void onWrite(BLECharacteristic *pCharacteristic) override
    {
        size_t length = pCharacteristic->getLength();

        if (length == 2)
        {
            uint8_t *data = pCharacteristic->getData();
            if (data[0] < _states->size())
            {
                Element *element = (*_states)[data[0]];
                ElementStatus newStatus = static_cast<ElementStatus>(data[1]);
                if (element->setStatus(newStatus))
                {
                    Serial.printf("Element %s status changed to %d\n", element->getName().c_str(), static_cast<int>(newStatus));
                    updateElements();
                }
            }
        }
    }

    void updateElements()
    {
        if (_deviceConnected)
        {
            setValues();
            _values->notify();
        }
    }

    bool isDeviceConnected() const
    {
        return _deviceConnected;
    }
};