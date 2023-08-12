#include <NimBLEDevice.h>
#include <string.h>
#include <string>
#include <json.hpp>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static BLEServer* m_pServer = NULL;

/** Handler class for characteristic actions */
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo);

    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo);

    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic);

    /**
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, int code);

    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue);
};




/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo);
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason);
    // void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo);

	/********************* Security handled here **********************
	****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest();
    bool onConfirmPIN(uint32_t pass_key);
    void onAuthenticationComplete(NimBLEConnInfo& connInfo);
};



class BLEConnector {
	public:
		BLEConnector(std::string name);
		void start();
		void stop();

		// template <typename T>
		// void setValue(T value);
		void setValue(std::string value);
        void sendNewValue();
        void setId(int newID);

        void updateJoint(int jointId);
        void updateConnector(int connectorId);


	private:
		std::string m_name;
		BLEService *m_pService = NULL;
		BLECharacteristic* m_pCharacteristic = NULL;
		NimBLEAdvertising* m_pAdvertising = NULL;
		CharacteristicCallbacks m_chrCallbacks;

        nlohmann::json advertisement;
};