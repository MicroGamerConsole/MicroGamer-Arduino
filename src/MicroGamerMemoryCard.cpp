#include <Arduino.h>
#include "MicroGamerMemoryCard.h"


#define FLASH_PAGE_SIZE (1024)

uint32_t flash_data[FLASH_PAGE_SIZE]
  __attribute__((aligned(FLASH_PAGE_SIZE), section (".rodata")))
    = { 0 };

static void memcpy_by_word(uint32_t *dest, const uint32_t *src, size_t n)
{
    int i = 0;
    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

MicroGamerMemoryCard::MicroGamerMemoryCard(size_t data_length_in_word)
      : _data_length(data_length_in_word)  
      , _data(new uint32_t[data_length_in_word])  
{
}

void MicroGamerMemoryCard::load()
{
    // Load data from flash to the RAM buffer
    memcpy_by_word(_data, flash_data, _data_length);
}

void MicroGamerMemoryCard::save()
{
    // Wait for the end of a current operation, if any
    while (NRF_NVMC->READY == 0) {
        continue;
    }       

    // Enable erase
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos;

    // Erase the page in flash
    NRF_NVMC->ERASEPCR1 = (uint32_t)flash_data;

    // Wait for the end of the erase operation
    while (NRF_NVMC->READY == 0) {
        continue;
    }       

    // Disable erase, Enable write
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;

    memcpy_by_word(flash_data, _data, _data_length);

    // Wait for the end of write operation
    while (NRF_NVMC->READY == 0) {
        continue;
    }       

    // Disable write
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;

}

uint8_t *MicroGamerMemoryCard::data()
{
    return (uint8_t *)_data;
}

void MicroGamerMemoryCard::update(int offset, uint8_t b)
{
    write(offset, b);
}

void MicroGamerMemoryCard::write(int offset, uint8_t b)
{
    data()[offset] = b;
}

uint8_t MicroGamerMemoryCard::read(int offset)
{
    return data()[offset];
}
