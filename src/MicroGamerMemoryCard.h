#ifndef MICROGAMER_MEMORYCARD_H
#define MICROGAMER_MEMORYCARD_H

/** \brief
 * Provide non volatile memory for Micro:Gamer platform.
 *
 * \details
 * This class provides functions to save and load data to that is preserved when
 * the Micro:Gamer is powered off.
 *
 * Example:
 *
 * \code
 * #include <MicroGamerMemoryCard.h>
 *
 * MicroGamerMemoryCard mem(1); // A memory card of 1 word (4 bytes);
 * 
 * ...
 * mem.load()
 * *mem.data() += 42;
 * mem.save();
 * \endcode
 *
 */
class MicroGamerMemoryCard
{
    
 public:

  /** \brief
   * The MicroGamerMemoryCard class constructor.
   *
   * \param data_length_in_word The size in words (32bit) of data that can be
   * saved. This value has to be lower or equal to 256. The size limit
   * requirement comes from the implementation that only supports writing one
   * page (1024 bytes) of memory.
   */
   MicroGamerMemoryCard(size_t data_length_in_word);

  /** \brief
   * Load the non-volatile data in a writable temporary RAM buffer.
   *
   * \see load()
   */
  void load();

  /** \brief
   * Save the writable RAM buffer into the non-volatile memory.
   *
   * \see load()
   */
  void save();

  /** \brief
   * Return a pointer to the temporary RAM buffer.
   *
   * \details
   * This address can be used to read from and write to the temporary RAM buffer
   * before saving it to the non-volatile memory.
   *
   * \see load() save()
   */
  uint8_t * data();

  /** \brief
   * Write a byte in temporary RAM buffer.
   *
   * \param offset Offset in bytes from the data() address where the byte will
   * be written.
   *
   * \see load() save()
   */
  void update(int offset, uint8_t b);

  /** \brief
   * Write a byte in temporary RAM buffer.
   *
   * \param offset Offset in bytes from the data() address where the byte will
   * be written.
   *
   * \see load() save()
   */
  void write(int offset, uint8_t b);

  /** \brief
   * Write a byte in temporary RAM buffer.
   *
   * \param offset Offset in bytes from the data() address where the byte will
   * be written.
   *
   * \see load() save()
   */
  uint8_t read(int offset);

  /** \brief
   * Read an object from the temporary RAM buffer.
   *
   * \param offset Offset in bytes from the data() address where the object will
   * be read.
   *
   * \see load() save()
   */
  template<typename T>
  T &get(int offset, T &t)
  {
      uint8_t *e = data() + offset;
      uint8_t *ptr = (uint8_t*) &t;
      for (int count = sizeof(T); count; --count, ++e) {
          *ptr++ = *e;
      }
      return t;
  }

  /** \brief
   * Write an object tothe temporary RAM buffer.
   *
   * \param offset Offset in bytes from the data() address where the object will
   * be written.
   *
   * \see load() save()
   */
  template<typename T>
  const T &put(int offset, const T &t)
  {
      uint8_t *e = data() + offset;
      const uint8_t *ptr = (const uint8_t*) &t;
      for (int count = sizeof(T); count; --count, ++e) {
          *e = *ptr++;
      }
      return t;
  }

 protected:
  size_t   _data_length;
  uint32_t *_data;
};

#endif
