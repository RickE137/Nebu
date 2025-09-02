# Hardware Personalizado: ESP32S3-WROOM-1 + INMP441 + MAX98357

## Descripción del Hardware

Este ejemplo ha sido modificado para funcionar con tu hardware personalizado:

- **Microcontrolador**: ESP32S3-WROOM-1 (16MB Flash + 8MB PSRAM)
- **Micrófono**: INMP441 (I2S digital, mono)
- **Amplificador**: MAX98357 (I2S, mono)
- **Speaker**: Conectado al MAX98357

## Configuración de Pines

### I2S (Inter-IC Sound)
- **BCLK (Bit Clock)**: GPIO 41
- **WS/LRC (Word Select/Left Right Clock)**: GPIO 42
- **SD (Data IN - Micrófono)**: GPIO 2
- **DIN (Data OUT - Speaker)**: GPIO 1

### Características del Audio
- **Formato**: I2S estándar
- **Canales**: Mono (1 canal)
- **Sample Rate**: 16kHz (configurado para LiveKit)
- **Bits por Sample**: 16
- **Codec**: Opus (configurado en LiveKit)

## Modificaciones Realizadas

### 1. Board Configuration (`board_cfg.txt`)
- Agregada nueva configuración `ESP32S3_WROOM_CUSTOM`
- Configurados pines I2S específicos para tu hardware
- Uso de codec `DUMMY` (sin codec I2C)

### 2. Board Initialization (`board.c`)
- Cambiado tipo de board a `ESP32S3_WROOM_CUSTOM`
- Configurado modo I2S stereo (compatible con mono)
- Deshabilitado modo TDM

### 3. Media Configuration (`media.c`)
- Configurado audio de entrada a 1 canal (mono)
- Configurado audio de salida a 1 canal (mono)
- Mantenida compatibilidad con LiveKit

### 4. LiveKit Configuration (`example.c`)
- Audio ya configurado para mono (`channel_count = 1`)
- Codec Opus a 16kHz
- Compatible con tu hardware

## Configuración del Sistema

### PSRAM
- **Tamaño**: 8MB
- **Modo**: Oct (8 líneas)
- **Velocidad**: 80MHz
- **Configuración**: Auto-detección

### Flash
- **Tamaño**: 16MB
- **Modo**: QIO
- **Frecuencia**: 80MHz

### WiFi
- Buffers optimizados para LiveKit
- Configuración de rendimiento mejorada

## Uso

1. **Compilar**: `idf.py build`
2. **Flash**: `idf.py flash`
3. **Monitor**: `idf.py monitor`

## Notas Importantes

- **No se necesita codec I2C**: INMP441 y MAX98357 son I2S directo
- **Audio mono**: Configurado para compatibilidad con LiveKit
- **PSRAM**: Necesario para el buffer de audio y LiveKit
- **WiFi**: Requerido para conectividad LiveKit

## Troubleshooting

### Error de PSRAM
- Verificar que el ESP32S3 tenga PSRAM habilitado
- Revisar configuración de pines PSRAM

### Error de I2S
- Verificar conexiones de pines
- Confirmar voltajes de alimentación
- Revisar configuración de sample rate

### Error de LiveKit
- Verificar configuración de WiFi
- Confirmar token y URL del servidor
- Revisar logs de conexión
