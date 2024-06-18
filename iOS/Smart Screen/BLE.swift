import Foundation
import CoreBluetooth
import Combine

class BLEManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    var centralManager: CBCentralManager?
    var discoveredPeripheral: CBPeripheral?
    var targetCharacteristicFFE1: CBCharacteristic?
    var targetCharacteristicFFE2: CBCharacteristic?
    
    @Published var isConnected = false
    @Published var statusMessage = "Scanning for devices..."
    
    let targetDeviceName = "Senyao Smart Screen" // 目标设备的名称
    private var shouldStartScanning = false // 标志变量
    private var ffe2Data: [UInt8] = [0, 0x01, 0xFF, 0xFF] // 初始数据

    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        print("centralManagerDidUpdateState: \(central.state.rawValue)")
        if central.state == .poweredOn {
            // 蓝牙已打开，开始扫描设备（如果标志为真）
            if shouldStartScanning {
                startScanning()
            }
        } else {
            // 处理蓝牙未打开的情况
            statusMessage = "Bluetooth is not available."
            print(statusMessage)
        }
    }
    
    func startScanning() {
        print("Start scanning for devices...")
        if centralManager?.state == .poweredOn {
            statusMessage = "Scanning for devices..."
            centralManager?.scanForPeripherals(withServices: nil, options: nil)
        } else {
            shouldStartScanning = true // 设置标志，在状态更新后开始扫描
            print("Bluetooth is not powered on yet.")
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        print("Did discover peripheral: \(peripheral.name ?? "unknown")")
        if let name = peripheral.name, name == targetDeviceName {
            // 发现目标设备，停止扫描并连接
            statusMessage = "Found target device: \(name)"
            print(statusMessage)
            centralManager?.stopScan()
            discoveredPeripheral = peripheral
            discoveredPeripheral?.delegate = self
            centralManager?.connect(peripheral, options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        // 已连接设备
        statusMessage = "Connected to \(peripheral.name ?? "unknown")"
        print(statusMessage)
        isConnected = true
        peripheral.discoverServices(nil)
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        // 处理连接失败
        statusMessage = "Failed to connect to \(peripheral.name ?? "unknown"). Retrying..."
        print(statusMessage)
        if let error = error {
            print("Connection error: \(error.localizedDescription)")
        }
        startScanning() // 重新开始扫描
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        // 处理断开连接
        statusMessage = "Disconnected from \(peripheral.name ?? "unknown")."
        print(statusMessage)
        if let error = error {
            print("Disconnection error: \(error.localizedDescription)")
        }
        isConnected = false
        // 可以选择重新连接
        startScanning() // 重新开始扫描
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let error = error {
            print("Error discovering services: \(error.localizedDescription)")
            return
        }
        
        guard let services = peripheral.services else { return }
        for service in services {
            print("Discovered service: \(service)")
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if let error = error {
            print("Error discovering characteristics: \(error.localizedDescription)")
            return
        }
        
        guard let characteristics = service.characteristics else { return }
        for characteristic in characteristics {
            print("Discovered characteristic: \(characteristic)")
            
            // 找到目标特征
            if characteristic.uuid == CBUUID(string: "FFE1") {
                print("Target characteristic FFE1 found")
                targetCharacteristicFFE1 = characteristic
            }
            
            if characteristic.uuid == CBUUID(string: "FFE2") {
                print("Target characteristic FFE2 found")
                targetCharacteristicFFE2 = characteristic
                // 读取 FFE2 特征值
                peripheral.readValue(for: characteristic)
            }
            
            if characteristic.properties.contains(.read) {
                print("Reading value for characteristic: \(characteristic)")
                peripheral.readValue(for: characteristic)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if let error = error {
            print("Error updating value: \(error.localizedDescription)")
            return
        }
        
        if let value = characteristic.value {
            print("Updated value for characteristic \(characteristic): \(value)")
            
            if characteristic.uuid == CBUUID(string: "FFE2") {
                // 覆盖默认的初始数据
                ffe2Data = [UInt8](value)
                print("Updated ffe2Data: \(ffe2Data)")
            }
        }
    }
    
    func writeDataToFFE1(data: Data) {
        guard let peripheral = discoveredPeripheral else {
            print("Peripheral not available")
            return
        }
        
        guard let characteristic = targetCharacteristicFFE1 else {
            print("Target characteristic FFE1 not available")
            return
        }
        
        print("Writing data to FFE1: \(data.map { String(format: "%02X", $0) }.joined())")
        peripheral.writeValue(data, for: characteristic, type: .withResponse)
        peripheral.readValue(for: characteristic)
    }
    
    func writeDirectionToFFE2(direction: UInt8) {
        guard let peripheral = discoveredPeripheral else {
            print("Peripheral not available")
            return
        }
        
        guard let characteristic = targetCharacteristicFFE2 else {
            print("Target characteristic FFE2 not available")
            return
        }
        
        ffe2Data[0] = direction
        let data = Data(ffe2Data)
        print("Writing direction to FFE2: \(data.map { String(format: "%02X", $0) }.joined())")
        peripheral.writeValue(data, for: characteristic, type: .withResponse)
        peripheral.readValue(for: characteristic)
    }
    
    func writeSpeedToFFE2(speed: UInt8) {
        guard let peripheral = discoveredPeripheral else {
            print("Peripheral not available")
            return
        }
        
        guard let characteristic = targetCharacteristicFFE2 else {
            print("Target characteristic FFE2 not available")
            return
        }
        
        ffe2Data[1] = speed
        let data = Data(ffe2Data)
        print("Writing speed to FFE2: \(data.map { String(format: "%02X", $0) }.joined())")
        peripheral.writeValue(data, for: characteristic, type: .withResponse)
    }
    
    func writeColorToFFE2(highByte: UInt8, lowByte: UInt8) {
        guard let peripheral = discoveredPeripheral else {
            print("Peripheral not available")
            return
        }
        
        guard let characteristic = targetCharacteristicFFE2 else {
            print("Target characteristic FFE2 not available")
            return
        }
        
        ffe2Data[2] = highByte
        ffe2Data[3] = lowByte
        let data = Data(ffe2Data)
        print("Writing color to FFE2: \(data.map { String(format: "%02X", $0) }.joined())")
        peripheral.writeValue(data, for: characteristic, type: .withResponse)
    }
}
