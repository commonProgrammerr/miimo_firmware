#ifndef __dto_h
#define __dto_h

#include "stdint.h"

#define SIG_AWAKE 0x1
#define SIG_SLEEP 0x2
#define SIG_TIME 0x3
#define SIG_CONFIG 0x0
#define SIG_DEBOUNCE 0x4

#define data_size(data, t) sizeof(t) > 1 ? sizeof(t) : sizeof(data)

template <typename T>
bool get_dto_data(DTO dto, T *data);
template <typename T>
bool set_dto_data(DTO *dto, T *data);
size_t read_dto(Stream &serial, DTO *data);
size_t send_dto(Stream &serial, DTO data);

typedef struct
{
  uint8_t signal;
  uint8_t data[4];
  uint8_t size;
} DTO;

template <typename T>
bool get_dto_data(DTO dto, T *data)
{
  size_t _size = data_size(data, T);
  if (_size > 4 || dto.size != _size)
    return false;

  else
    for (uint8_t i = 0; i < dto.size; i++)
      *(((uint8_t)data)[i]) = dto.data[i];

  return true;
}

template <typename T>
bool set_dto_data(DTO *dto, T *data)
{
  size_t _size = data_size(data, T);

  if (_size > 4)
    return false;

  else
  {
    dto->size = _size;
    for (uint8_t i = 0; i < dto->size; i++)
      dto->data[i] = *(((uint8_t)data)[i]);
  }

  return true;
}

size_t send_dto(Stream &serial, DTO data) { return serial.write((byte *)&data, 6); }

size_t read_dto(Stream &serial, DTO *data) { return serial.readBytes((byte *)data, 6); }

// DTO dto(uint8 sig, const uint8 *data, size_t size)
// {
//   DTO dto;
//   dto.signal = sig;

//   for (uint8 i = 0; i < size; i++)
//     dto.data[i] = data[i];

//   dto.size = size;

//   return dto;
// }

// DTO dto(uint8 sig, const uint8 *data) { return dto(sig, data, sizeof(data)); }

#endif