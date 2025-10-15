# Endian Utils Library
MIT License | C++20 | Cross-Platform
## Overview
Endian Utils is a modern C++ library for endian-aware serialization and binary data manipulation with automatic byte-order handling. It provides a comprehensive toolkit for dealing with binary data across different platforms regardless of the native byte order.
## Key Features
•	Automatic endianness detection and conversion
•	Platform-optimized byte-swapping implementations
•	Type-safe serialization and deserialization
•	Optimized buffer handling for both read and write operations
•	String and container serialization support
•	Modern C++20 design with concepts
•	Cross-platform compatibility (Windows, macOS, Linux, BSD)
## Requirements
•	C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 19.29+)
•	CMake 3.16+ (for building)
## Installation
### Using CMake
``` git clone https://github.com/meysam-zm/endian_utils.git cd endian_utils mkdir build && cd build cmake .. cmake --build . cmake --install . ```
### As a Subproject
Add the repository as a git submodule:
``` git submodule add https://github.com/meysam-zm/endian_utils.git extern/endian_utils ```
Then in your CMakeLists.txt:
``` add_subdirectory(extern/endian_utils) target_link_libraries(your_target_name PRIVATE mz::endian_utils) ```
## Basic Usage
### Reading and Writing Individual Values
```
#include <mz/endian/EndianVector.h>
#include <iostream>
int main()
{
  mz::endianVector buffer;
  uint32_t writeValue = 0x12345678;
  buffer.pushBack(writeValue);
  uint32_t readValue = 0;
  mz::endian::ReadBuffer reader(buffer.data(), buffer.size());
  reader.popFront(readValue);

  std::cout << "Read value: 0x" << std::hex << readValue << std::endl;
  // Will output "Read value: 0x12345678" regardless of platform endianness
  return 0;
}
```

### Serializing Complex Data

```
#include <mz/endian/EndianVector.h> 
#include <string>
#include <vector>
void serializeMessage(uint32_t id, const stdstring& text, const stdvector<float>& values)
{ 
  mz::endianVector buffer; 
  buffer.pushBack(id); 
  buffer.pushBack(text); 
  buffer.pushBack(static_cast<uint32_t>(values.size())); 
  for (float value : values)
  { 
    buffer.pushBack(value); 
  } 
  // ... send buffer.data() with buffer.size() bytes ... 
}

void deserializeMessage(const uint8_t* data, size_t size)
{ 
  mzendianReadBuffer reader(data, size); 
  uint32_t id; 
  stdstring text; 
  reader.popFront(id); 
  reader.popFront(text); 
  uint32_t count; 
  reader.popFront(count); 
  std::vector<float> values(count); 
  for (uint32_t i = 0; i < count; ++i)
  { 
    reader.popFront(values[i]); 
  } 
  // Process the deserialized data... } 
```

### Working with Fixed Buffers

```
#include <mz/endian/EndianWriteBuffer.h>
#include <mz/endian/EndianReadBuffer.h>
#include <array>
void example()
{
  std::array<uint8_t, 64> headerBuffer;
  mz::endian::WriteBuffer writer(headerBuffer.data(), headerBuffer.size());
  writer.pushBack(uint16_t(0x0102));
  writer.pushBack(uint32_t(0x12345678));
  writer.pushBack(uint64_t(0x0102030405060708));
  mz::endian::ReadBuffer reader(headerBuffer.data(), headerBuffer.size());
  uint16_t version;
  uint32_t messageId;
  uint64_t timestamp;
  reader.popFront(version);
  reader.popFront(messageId);
  reader.popFront(timestamp);
}
```
## Advanced Features
### Byte-Swapping Operations
```
#include <mz/endian/EndianConversions.h>
void byteSwapExample()
{
  uint32_t value = 0x12345678;
  uint32_t swapped = mzendianbyteSwap(value);  // Will be 0x78563412
  uint32_t littleEndian = mzendiantoLittleEndian(value);
  uint32_t bigEndian = mzendiantoBigEndian(value);
  uint32_t streamEndian = mzendiantoStreamEndian(value);
}
```
### Working with Enum Types
```
#include <mz/endian/EndianConcepts.h>
enum class MessageType : uint16_t
{
  none = 0,
  heartbeat = 1,
  data = 2,
  control = 3,
  invalid = 1000
};

void enumExample()
{
  mz::endianVector buffer;
  buffer.pushBack(MessageType::data);
  MessageType type;
  mz::endian::ReadBuffer reader(buffer.data(), buffer.size());
  reader.popFront(type);

  if (mz::endian::isValid(type)) {
    // Process valid message type
  }
}
```
### Stream Position Manipulation
```
#include <mz/endian/EndianReadBuffer.h>
void positionExample()
{
  std::vector<uint8_t> data = /* some binary data */;
  mz::endian::ReadBuffer reader(data.data(), data.size());
  reader.skipBytes(8);
  uint32_t value;
  reader.popFront(value);
  reader.reset();
  size_t position = reader.position();
  size_t remaining = reader.available();
}
```
## Library Components
•	EndianConversions.h: Platform-optimized byte-swapping functions
•	EndianConcepts.h: Type constraints and core utilities
•	EndianBasicBuffers.h: Templated base buffer classes
•	EndianReadBuffer.h: Buffer for reading endian-aware data
•	EndianWriteBuffer.h: Buffer for writing endian-aware data
•	EndianBasicVector.h: Templated base vector class
•	EndianVector.h: Dynamically growing buffer for serialization
•	EndianByteArray.h: Fixed-size array with endian-aware access
## Performance Considerations
•	Uses platform-specific intrinsics for optimal byte-swapping performance
•	Skips endianness conversion for single-byte values
•	No-op optimizations when native endianness matches stream endianness
•	Minimizes memory allocations with efficient buffer management
•	Uses std::span for zero-copy operations where possible
## License
This library is distributed under the MIT License. See the LICENSE file for details.
## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
1.	Fork the repository
2.	Create your feature branch (git checkout -b feature/amazing-feature)
3.	Commit your changes (git commit -m 'Add some amazing feature')
4.	Push to the branch (git push origin feature/amazing-feature)
5.	Open a Pull Request
## Credits
Developed by Meysam Zare (c) 2021-2025
