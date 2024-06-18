import Foundation
import CoreGraphics

class Bitmap {
    static func gbkOffset(bl: UInt8, bh: UInt8) -> Int {
        var offset: Int
        var bh = bh
        print("bh=\(bh), bl=\(bl)")
        if bh > 0x7F {
            bh -= 1 // 去掉7F一条线
        }
        if 0x81 <= bl && bl <= 0xA0 {
            offset = Int(bl - 0x81) * 190 + Int(bh - 0x40)
        } else if 0xA1 <= bl && bl <= 0xA7 {
            offset = Int(bl - 0xA1) * 94 + Int(bh - 0xA0) + 6080
        } else if 0xA8 <= bl && bl <= 0xA9 {
            offset = Int(bl - 0xA8) * 190 + Int(bh - 0x40) + 6738
        } else if 0xAA <= bl && bl <= 0xAF {
            offset = Int(bl - 0xAA) * 96 + Int(bh - 0x40) + 71
        } else if 0xB0 <= bl && bl <= 0xF7 {
            offset = Int(bl - 0xB0) * 190 + Int(bh - 0x40) + 7694
        } else if 0xF8 <= bl && bl <= 0xFE {
            offset = Int(bl - 0xf8) * 96 + Int(bh - 0x40) + 21374
        } else {
            fatalError("Invalid GBK character: \(bl) \(bh)")
        }
        return offset
    }
    
    static func getGBKZM(gbkzm: Data, bl: UInt8, bh: UInt8, size: Int = 32) -> Data {
        let offset = gbkOffset(bl: bl, bh: bh) * size
        return gbkzm.subdata(in: offset..<(offset + size))
    }
    
    static func scanZM(zm: Data, bitoff: Int) -> Bool {
        let byteoff = bitoff / 8
        let bitshift = 7 - (bitoff % 8)
        return ((zm[byteoff] >> bitshift) & 1) == 1
    }
    
    static func drawZM(zm: Data, pt: Int, x: Int, y: Int) -> [[Bool]] {
        var bmp = Array(repeating: Array(repeating: false, count: pt), count: pt)
        for i in 0..<pt {
            for j in 0..<pt {
                let bitoff = i * pt + j
                let val = scanZM(zm: zm, bitoff: bitoff)
                bmp[i][j] = val
            }
        }
        return bmp
    }
    
    static func convertToFullWidth(_ char: Character) -> Character {
        if let scalar = char.unicodeScalars.first, scalar.isASCII {
            if scalar.value >= 0x21 && scalar.value <= 0x7E {
                // 转换 ASCII 字符为全角
                return Character(UnicodeScalar(scalar.value + 0xFEE0)!)
            }
        }
        return char
    }
    
    static func getFullWidthGBKOffset(character: Character) -> (UInt8, UInt8)? {
        let fullWidthCharacter = convertToFullWidth(character)
        guard let data = String(fullWidthCharacter).data(using: .gbk) else {
            return nil
        }
        if data.count == 2 {
            return (data[0], data[1])
        }
        return nil
    }
}
