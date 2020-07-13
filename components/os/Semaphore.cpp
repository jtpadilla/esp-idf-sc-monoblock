#include <sstream>
#include <iomanip>
#include "esp_log.h"
#include <Semaphore.h>

namespace genielink::os
{

	Semaphore::Semaphore() 
	{
		semaphore = ::xSemaphoreCreateBinary();
		::xSemaphoreGive(semaphore);
	}

	Semaphore::~Semaphore() 
	{
		::vSemaphoreDelete(semaphore);
	}

	void Semaphore::wait() {
		::xSemaphoreTake(semaphore, portMAX_DELAY);
		::xSemaphoreGive(semaphore);
	}

	void Semaphore::give() {
		::xSemaphoreGive(semaphore);
	}

	void Semaphore::giveFromISR() {
		BaseType_t higherPriorityTaskWoken;
		::xSemaphoreGiveFromISR(semaphore, &higherPriorityTaskWoken);
	}

	bool Semaphore::take() {
		return ::xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE;
	}

	bool Semaphore::take(uint32_t timeoutMs) {
		return ::xSemaphoreTake(semaphore, timeoutMs / portTICK_PERIOD_MS) == pdTRUE;
	} 

}