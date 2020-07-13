
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

#include "include/Task.h"
#include "sdkconfig.h"

namespace genielink::os
{

	static const char* LOG_TAG = "Task";

	Task::Task(std::string taskName, uint16_t stackSize, uint8_t priority) {
		m_taskName  = taskName;
		m_stackSize = stackSize;
		m_priority  = priority;
		m_taskData  = nullptr;
		m_handle    = nullptr;
		m_coreId	= tskNO_AFFINITY;
	} 

	Task::~Task() {
	}

	void Task::delay(int ms) {
		::vTaskDelay(ms / portTICK_PERIOD_MS);
	}

	void Task::runTask(void* pTaskInstance) {
		Task* pTask = (Task*) pTaskInstance;
		ESP_LOGD(LOG_TAG, ">> runTask: taskName=%s", pTask->m_taskName.c_str());
		pTask->run(pTask->m_taskData);
		ESP_LOGD(LOG_TAG, "<< runTask: taskName=%s", pTask->m_taskName.c_str());
		pTask->stop();
	}

	void Task::start(void* taskData) {
		if (m_handle != nullptr) {
			ESP_LOGW(LOG_TAG, "Task::start - There might be a task already running!");
		}
		m_taskData = taskData;
		::xTaskCreatePinnedToCore(&runTask, m_taskName.c_str(), m_stackSize, this, m_priority, &m_handle, m_coreId);
	}


	uint32_t Task::getTimeSinceStart() {
		return (uint32_t) (xTaskGetTickCount() * portTICK_PERIOD_MS);
	}

	void Task::stop() {
		if (m_handle == nullptr) return;
		xTaskHandle temp = m_handle;
		m_handle = nullptr;
		::vTaskDelete(temp);
	}

	void Task::setStackSize(uint16_t stackSize) {
		m_stackSize = stackSize;
	}

	void Task::setPriority(uint8_t priority) {
		m_priority = priority;
	}

	void Task::setName(std::string name) {
		m_taskName = name;
	}

	void Task::setCore(BaseType_t coreId) {
		m_coreId = coreId;
	}

}