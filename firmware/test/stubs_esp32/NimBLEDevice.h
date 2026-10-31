// Platzhalter für NimBLE-Arduino: merkt sich Name, UUIDs und gesendete Werte. Für den PC-Testbuild.
#pragma once
#include <string>
#include <vector>
namespace NIMBLE_PROPERTY { enum : unsigned { READ = 1, NOTIFY = 16 }; }
struct NimBLEUUID { std::string s; };
struct NimBLECharacteristic {
  std::string uuid, value; unsigned props = 0; std::vector<std::string> sent;
  void setValue(const char* v) { value = v; }
  void notify() { sent.push_back(value); }
};
struct NimBLEService {
  std::string uuid; std::vector<NimBLECharacteristic*> chars; bool started = false;
  NimBLECharacteristic* createCharacteristic(const char* u, unsigned p) { auto* c = new NimBLECharacteristic; c->uuid = u; c->props = p; chars.push_back(c); return c; }
  void start() { started = true; }
  NimBLEUUID getUUID() const { return {uuid}; }
};
struct NimBLEServer {
  std::vector<NimBLEService*> services;
  NimBLEService* createService(const char* u) { auto* s = new NimBLEService; s->uuid = u; services.push_back(s); return s; }
};
struct NimBLEAdvertising {
  std::vector<std::string> uuids; bool running = false;
  void addServiceUUID(const NimBLEUUID& u) { uuids.push_back(u.s); }
  void start() { running = true; }
};
struct NimBLEDevice {
  static inline std::string name; static inline NimBLEServer server; static inline NimBLEAdvertising adv;
  static void init(const char* n) { name = n; }
  static NimBLEServer* createServer() { return &server; }
  static NimBLEAdvertising* getAdvertising() { return &adv; }
};
