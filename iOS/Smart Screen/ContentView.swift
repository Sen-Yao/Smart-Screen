import SwiftUI
import Combine

let MAX_ROWS = 10
let WORD_PER_LINE = 9

class BitmapViewModel: ObservableObject {
    @Published var inputText: String = ""
    @Published var bitmapData: [Data] = []
    @Published var speed: String = ""
    @Published var selectedColor: Color = .white
    
    func convertTextToBitmap() {
        print("Starting text to bitmap conversion")
        
        guard let resourcePath = Bundle.main.path(forResource: "GBK16", ofType: "bin") else {
            print("Failed to find gbk16.bin path")
            return
        }
        
        print("Found gbk16.bin at path: \(resourcePath)")
        
        guard let gbk16zm = try? Data(contentsOf: URL(fileURLWithPath: resourcePath)) else {
            print("Failed to load gbk16.bin")
            return
        }
        
        var row = 0
        var col = 0
        var bitmaps: [Data] = []
        
        for char in inputText {
            if char == "\n" {
                row += 1
                col = 0
                if row >= MAX_ROWS {
                    print("Exceeded maximum number of rows")
                    break
                }
                continue
            }
            
            guard let (bl, bh) = Bitmap.getFullWidthGBKOffset(character: char) else {
                print("Invalid character: \(char)")
                continue
            }
            
            if col >= WORD_PER_LINE {
                col = 0
                row += 1
                if row >= MAX_ROWS {
                    print("Exceeded maximum number of rows")
                    break
                }
            }
            
            let zm = Bitmap.getGBKZM(gbkzm: gbk16zm, bl: bl, bh: bh)
            let positionData = Data([UInt8(row), UInt8(col)])
            let fullData = positionData + zm
            
            print("Row: \(row), Col: \(col), Data: \(fullData.map { String(format: "%02X", $0) }.joined())")
            bitmaps.append(fullData)
            
            col += 1
        }
        
        self.bitmapData = bitmaps
    }
    
    func convertColorToBytes(color: Color) -> (UInt8, UInt8) {
        let components = color.cgColor?.components ?? [1, 1, 1]
        let red = UInt16(components[0] * 31) & 0x1F
        let green = UInt16(components[1] * 63) & 0x3F
        let blue = UInt16(components[2] * 31) & 0x1F
        let colorValue = (blue << 11) | (green << 5) | red
        let highByte = UInt8((colorValue & 0xFF00) >> 8)
        let lowByte = UInt8(colorValue & 0x00FF)
        return (highByte, lowByte)
    }
}

struct ContentView: View {
    @ObservedObject var viewModel = BitmapViewModel()
    @ObservedObject var bleManager = BLEManager()
    
    var body: some View {
        ZStack {
            Color.white.ignoresSafeArea()
                .onTapGesture {
                    UIApplication.shared.endEditing()
                }
            
            VStack(alignment: .center, spacing: 20) {
                Image(systemName: "antenna.radiowaves.left.and.right")
                    .imageScale(.large)
                    .foregroundColor(.blue)
                    .padding(.top, 20)
                Text("Senyao Smart Screen")
                    .font(.title)
                    .padding()
                
                if !bleManager.isConnected {
                    Text(bleManager.statusMessage)
                        .padding()
                }
                
                Text("请输入汉字:")
                    .font(.headline)
                
                TextEditor(text: $viewModel.inputText)
                    .padding()
                    .background(Color.gray.opacity(0.2))
                    .cornerRadius(10)
                    .frame(height: 150)
                    .overlay(RoundedRectangle(cornerRadius: 10).stroke(Color.gray, lineWidth: 1))
                    .padding(.horizontal, 20)
                
                VStack {
                    Button(action: {
                        print("Up button pressed")
                        bleManager.writeDirectionToFFE2(direction: 3)
                    }) {
                        Image(systemName: "arrow.up.circle")
                            .imageScale(.large)
                    }
                    HStack {
                        Button(action: {
                            print("Left button pressed")
                            bleManager.writeDirectionToFFE2(direction: 1)
                        }) {
                            Image(systemName: "arrow.left.circle")
                                .imageScale(.large)
                        }
                        
                        Spacer().frame(width: 40)
                        
                        Button(action: {
                            print("Right button pressed")
                            bleManager.writeDirectionToFFE2(direction: 0)
                        }) {
                            Image(systemName: "arrow.right.circle")
                                .imageScale(.large)
                        }
                    }
                    Button(action: {
                        print("Down button pressed")
                        bleManager.writeDirectionToFFE2(direction: 2)
                    }) {
                        Image(systemName: "arrow.down.circle")
                            .imageScale(.large)
                    }
                }
                
                HStack {
                    Text("输入速度:")
                        .font(.headline)
                    TextField("速度 (ms)", text: $viewModel.speed)
                        .keyboardType(.numberPad)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .frame(width: 100)
                    Button(action: {
                        guard let speed = UInt8(viewModel.speed), speed <= 0xFF else {
                            print("Invalid speed input")
                            return
                        }
                        bleManager.writeSpeedToFFE2(speed: speed)
                    }) {
                        Text("设置速度")
                            .padding()
                            .background(Color.blue)
                            .foregroundColor(.white)
                            .cornerRadius(10)
                    }
                }
                .padding(.horizontal, 20)
                
                ColorPicker("选择颜色", selection: $viewModel.selectedColor)
                    .padding()
                    .onChange(of: viewModel.selectedColor) { newColor in
                        let (highByte, lowByte) = viewModel.convertColorToBytes(color: newColor)
                        bleManager.writeColorToFFE2(highByte: highByte, lowByte: lowByte)
                    }
                
                Button(action: {
                    UIApplication.shared.endEditing()
                    print("Text committed: \(self.viewModel.inputText)")
                    self.viewModel.convertTextToBitmap()
                    
                    // 写入数据到 BLE 设备
                    if self.bleManager.isConnected {
                        for data in self.viewModel.bitmapData {
                            self.bleManager.writeDataToFFE1(data: data)
                        }
                    } else {
                        print("BLE device is not connected.")
                    }
                }) {
                    Text("投射到 Smart Screen")
                        .padding()
                        .background(Color.blue)
                        .foregroundColor(.white)
                        .cornerRadius(10)
                }
                .padding(.top, 10)
            }
            .padding()
        }
        .onAppear {
            bleManager.startScanning()
        }
    }
}

extension String.Encoding {
    static let gbk = String.Encoding(rawValue: CFStringConvertEncodingToNSStringEncoding(CFStringEncoding(CFStringEncodings.GB_18030_2000.rawValue)))
}

extension UIApplication {
    func endEditing() {
        sendAction(#selector(UIResponder.resignFirstResponder), to: nil, from: nil, for: nil)
    }
}
