#include "BLEConnector.h"
#include <NimBLEDevice.h>
#include <cstdlib>


BLEConnector::BLEConnector(std::string name): m_name(name) {
	BLEDevice::init(m_name);
	m_pServer = BLEDevice::createServer();
	// m_pServer->setCallbacks(new ServerCallbacks());
	m_pService = m_pServer->createService(SERVICE_UUID);
	m_pCharacteristic = m_pService->createCharacteristic(
											CHARACTERISTIC_UUID,
											NIMBLE_PROPERTY::READ   |
											NIMBLE_PROPERTY::WRITE  |
											NIMBLE_PROPERTY::NOTIFY |
											NIMBLE_PROPERTY::INDICATE
										);

	m_pCharacteristic->setCallbacks(&m_chrCallbacks);


    advertisement = nlohmann::json::parse(R"(
		{
			"ID": 0,
			"joints": {
				"joint0": {
					"velocity": null,
					"position": null,
					"torque": null,
					"capabilities": {
						"position": {
							"max": null,
							"min": null
						},
						"speed": {
							"max": null,
							"min": null
						},
						"torque": {
							"max": null
						}
					}
				},
				"joint0": {
					"velocity": null,
					"position": null,
					"torque": null,
					"capabilities": {
						"position": {
							"max": null,
							"min": null
						},
						"speed": {
							"max": null,
							"min": null
						},
						"torque": {
							"max": null
						}
					}
				}
			},
			"connectors": {
				"connector1": {
					"position": null
				}
			}
		}
	)");
}


void BLEConnector::start() {
	m_pCharacteristic->setValue("Hello World says Neil");
	m_pService->start();
	m_pAdvertising = NimBLEDevice::getAdvertising();
	m_pAdvertising->addServiceUUID(SERVICE_UUID);
	m_pAdvertising->setScanResponse(true);
	NimBLEDevice::startAdvertising();
	printf("Characteristic defined! Now you can read it in your phone!\n");
}

void BLEConnector::setValue(std::string value) {
		m_pCharacteristic->setValue(value);
		m_pCharacteristic->notify();
}

void BLEConnector::sendNewValue() {
	m_pCharacteristic->setValue(advertisement.dump());
	m_pCharacteristic->notify();
}


void BLEConnector::setId(int newID) {
    advertisement["ID"] = newID;
}

void BLEConnector::updateJoint(int jointId) {
	auto &joint = advertisement["joints"]["joint" + std::to_string(jointId)];
	joint["velocity"] = rand() % 100;
	joint["position"] = rand() % 100;
	joint["torque"] = rand() % 100;
}

void BLEConnector::updateConnector(int connectorId) {
	auto &connector = advertisement["connectors"]["connector" + std::to_string(connectorId)];
	connector["position"] = rand() % 2 == 0; // return true or false
}

///////////////////////////////////






/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */

void ServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
	printf("Client address: %s\n", connInfo.getAddress().toString().c_str());

	/** We can use the connection handle here to ask for different connection parameters.
	 *  Args: connection handle, min connection interval, max connection interval
	 *  latency, supervision timeout.
	 *  Units; Min/Max Intervals: 1.25 millisecond increments.
	 *  Latency: number of intervals allowed to skip.
	 *  Timeout: 10 millisecond increments, try for 3x interval time for best results.
	 */
	pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 18);
};

void ServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
	printf("Client disconnected - start advertising\n");
	NimBLEDevice::startAdvertising();
};

// void ServerCallbacks::onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) {
// 	printf("MTU updated: %u for connection ID: %u\n", MTU, connInfo.getConnHandle());
// 	m_pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 60);
// };

/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
uint32_t ServerCallbacks::onPassKeyRequest(){
	printf("Server Passkey Request\n");
	/** This should return a random 6 digit number for security
	 *  or make your own static passkey as done here.
	 */
	return 123456;
};

bool ServerCallbacks::onConfirmPIN(uint32_t pass_key){
	printf("The passkey YES/NO number: %" PRIu32"\n", pass_key);
	/** Return false if passkeys don't match. */
	return true;
};

void ServerCallbacks::onAuthenticationComplete(NimBLEConnInfo& connInfo){
	/** Check that encryption was successful, if not we disconnect the client */
	if(!connInfo.isEncrypted()) {
		NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
		printf("Encrypt connection failed - disconnecting client\n");
		return;
	}
	printf("Starting BLE work!");
};



//////////////////////////////////

void CharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
	printf("%s : onRead(), value: %s\n",
			pCharacteristic->getUUID().toString().c_str(),
			pCharacteristic->getValue().c_str());
}

void CharacteristicCallbacks::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
	printf("%s : onWrite(), value: %s\n",
			pCharacteristic->getUUID().toString().c_str(),
			pCharacteristic->getValue().c_str());
}

/** Called before notification or indication is sent,
 *  the value can be changed here before sending if desired.
 */
void CharacteristicCallbacks::onNotify(NimBLECharacteristic* pCharacteristic) {
	printf("Sending notification to clients\n");
}

/**
 *  The value returned in code is the NimBLE host return code.
 */
void CharacteristicCallbacks::onStatus(NimBLECharacteristic* pCharacteristic, int code) {
	printf("Notification/Indication return code: %d, %s\n",
			code, NimBLEUtils::returnCodeToString(code));
}

void CharacteristicCallbacks::onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) {
	std::string str = "Client ID: ";
	str += connInfo.getConnHandle();
	str += " Address: ";
	str += connInfo.getAddress().toString();
	if(subValue == 0) {
		str += " Unsubscribed to ";
	}else if(subValue == 1) {
		str += " Subscribed to notfications for ";
	} else if(subValue == 2) {
		str += " Subscribed to indications for ";
	} else if(subValue == 3) {
		str += " Subscribed to notifications and indications for ";
	}
	str += std::string(pCharacteristic->getUUID());

	printf("%s\n", str.c_str());
}