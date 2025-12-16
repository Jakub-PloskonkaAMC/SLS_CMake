#include <stdbool.h>

struct CyclicBuffer {
	uint16_t writePtr;
	uint16_t readPtr;
	uint16_t count;
	const uint16_t size;
	uint8_t * const data;

};
//*********************************************************************//
static inline void CyclicBuffer_init(struct CyclicBuffer * buff) {
	buff->writePtr = 0;
	buff->readPtr = 0;
	buff->count = 0;
}
//*********************************************************************//
static inline void CyclicBuffer_flush(struct CyclicBuffer * buff)
{
	CyclicBuffer_init(buff);
}
//*********************************************************************//
static inline bool CyclicBuffer_isEmpty(struct CyclicBuffer * buff) {
	if(buff->writePtr == buff->readPtr){
		return true;
	}else{
		return false;
	}
}
//*********************************************************************//
static inline int8_t CyclicBuffer_isNotEmpty(struct CyclicBuffer * buff) {
	return (buff->writePtr != buff->readPtr);
}
//*********************************************************************//
static inline int8_t CyclicBuffer_isFull(struct CyclicBuffer * buff) {
	uint16_t tmpWrite = buff->writePtr;
	tmpWrite = (tmpWrite + 1) % buff->size;
	return (tmpWrite == buff->readPtr);
}
//*********************************************************************//
static inline int8_t CyclicBuffer_isNotFull(struct CyclicBuffer * buff) {
	uint16_t tmpWrite = buff->writePtr;
	tmpWrite = (tmpWrite + 1) % buff->size;
	return (tmpWrite != buff->readPtr);
}
//*********************************************************************//
static inline uint8_t CyclicBuffer_get(struct CyclicBuffer * buff) {
	//__disable_irq();
	char c = buff->data[(++buff->readPtr) % buff->size];
	buff->count--;
	//__enable_irq();
	
	return c;
}
//*********************************************************************//
static inline int8_t CyclicBuffer_append(struct CyclicBuffer * buff, uint8_t c) {
	///__disable_irq();
	if (CyclicBuffer_isFull(buff)){
		//__enable_irq();
		return -1;
	}
	buff->data[(++buff->writePtr) % buff->size] = c;
	buff->count++;
	//__enable_irq();
	return 1;
}

//*********************************************************************//
static inline uint16_t CyclicBuffer_avaiableSize(struct CyclicBuffer *buff)
{
	//__disable_irq();
	uint16_t s = buff->size - buff->count;
	//__enable_irq();
	return s;
}


