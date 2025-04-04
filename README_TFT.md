# 2.4寸TFT触屏驱动技术文档

## 硬件规格
### 主控芯片
- 型号: ESP32-S3
- 工作电压: 3.3V

### 显示屏参数
- 型号: ILI9341 (SKU: MAR2406)
- 类型: TFT
- 尺寸: 2.4英寸
- 分辨率: 320×240像素
- 显示颜色: RGB 65K彩色
- 有效显示区域: 48.96×36.72mm
- 模块PCB尺寸: 72.20×52.7mm
- 接口类型: 8-bit parallel interface
- 工作电压: 5V/3.3V
- 工作温度: -20°C~60°C
- 存储温度: -30°C~70°C
- 工作电压: 3.3V
- 工作电流: 120mA (最大)
- 视角: 70°/70°/70°/70° (上/下/左/右)
- 接口类型: 4线SPI
- 色彩深度: 262K色
- 响应时间: 5ms
- 分辨率: 240x320
- 接口: SPI
- 触摸芯片: XPT2046

## 硬件连接
| TFT引脚 | ESP32引脚 | 功能 |
|---------|----------|------|
| VCC     | 3.3V     | 电源 |
| GND     | GND      | 地线 |
| D0      | GPIO4    | 数据线0 |
| D1      | GPIO5    | 数据线1 |
| D2      | GPIO6    | 数据线2 |
| D3      | GPIO7    | 数据线3 |
| D4      | GPIO8    | 数据线4 |
| D5      | GPIO9    | 数据线5 |
| D6      | GPIO10   | 数据线6 |
| D7      | GPIO11   | 数据线7 |
| RST     | GPIO12   | 复位 |
| CS      | GPIO13   | 片选 |
| RS      | GPIO14   | 寄存器选择 |
| WR      | GPIO15   | 写控制 |
| RD      | GPIO16   | 读控制 |
| LED     | 3.3V     | 背光 |

## 软件设计
1. SPI配置
   - 主机模式
   - 时钟频率: 40MHz (最大)
   - 数据位宽: 8位
   - 模式: 0 (CPOL=0, CPHA=0)
   - 命令/数据切换: DC线控制
   - 时序要求:
     - 片选建立时间: 10ns
     - 数据保持时间: 10ns

2. ILI9341初始化序列
   - 复位脉冲: 低电平至少10ms
   - 初始化命令序列:
     1. 0xEF (启动命令)
     2. 0xCF (电源控制B)
     3. 0xED (电源控制A)
     4. 0xE8 (驱动时序控制A)
     5. 0xCB (电源控制A)
     6. 0xF7 (泵比控制)
     7. 0xEA (驱动时序控制B)
     8. 0xC0 (电源控制1)
     9. 0xC1 (电源控制2)
     10. 0xC5 (VCOM控制1)
     11. 0xC7 (VCOM控制2)
     12. 0x36 (内存访问控制)
     13. 0x3A (像素格式设置)
     14. 0xB1 (帧率控制)
     15. 0xB6 (显示功能控制)
     16. 0xF2 (3G控制)
     17. 0x26 (Gamma设置)
     18. 0xE0 (正极性Gamma校正)
     19. 0xE1 (负极性Gamma校正)
     20. 0x11 (退出睡眠模式)
     21. 0x29 (开启显示)

3. 触摸校准参数
   - X轴校准: 0.0-1.0 (默认1.0)
   - Y轴校准: 0.0-1.0 (默认1.0)
   - 旋转方向: 0° (可设置为90°/180°/270°)
   - 初始化序列
   - 显示缓冲区配置

3. 触摸驱动
   - 触摸校准
   - 中断处理
   - 坐标转换

## 依赖库
- LVGL (轻量级图形库)
- SPI驱动
- XPT2046触摸驱动