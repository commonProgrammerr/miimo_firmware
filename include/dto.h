#ifndef __dto_h
#define __dto_h

#include "stdint.h"

#define SIG_CONFIG 0x0
#define SIG_AWAKE 0x1
#define SIG_SLEEP 0x2
#define SIG_TIME 0x3
#define SIG_DEBOUNCE 0x4
#define SIG_READY 0x6

#define data_size(data, t) sizeof(t) > 1 ? sizeof(t) : sizeof(data)

typedef struct
{
  uint8_t signal;
  uint8_t data[4];
  uint8_t size;
} Data;

class DTO
{
private:
  Data dto;
  Stream *serial;

public:
  DTO(Stream &_serial) { serial = &_serial; }
  ~DTO() { free(this); }

  template <typename T>
  bool get_data(T *data)
  {
    size_t _size = data_size(data, T);
    if (_size > 4 || this->dto.size != _size)
      return false;

    else
      for (uint8_t i = 0; i < this->dto.size; i++)
        *(((uint8_t)data)[i]) = this->dto.data[i];

    return true;
  }

  template <typename T>
  bool set_data(T *data)
  {
    size_t _size = data_size(data, T);
    if (_size > 4)
      return false;

    else
    {
      this->dto.size = _size;
      for (uint8_t i = 0; i < this->dto.size; i++)
        this->dto.data[i] = *(((uint8_t)data)[i]);
    }

    return true;
  }

  void set_signal(uint8_t _signal) { this->dto.signal = _signal; }
  uint8_t get_signal() { return this->dto.signal; }

  size_t send() { return serial->write((byte *)&dto, 6); }
  size_t read() { return serial->readBytes((byte *)&dto, 6); }
  size_t read(Data &obj) { return serial->readBytes((byte *)&obj, 6); }

  template <typename T>
  size_t send(T *data) { return set_data(data) ? send() : -1; }

  template <typename T>
  size_t send(uint8_t sig, T *data)
  {
    set_signal(sig);
    return send(data);
  }

  template <typename T>
  size_t read(uint8_t &sig, T *data)
  {
    read(data);
    sig = get_signal();
  }

  template <typename T>
  size_t read(T *data)
  {
    size_t bytes = read();
    return bytes > 0 ? get_data(data) ? bytes : -1 : bytes;
  }
};

#endif