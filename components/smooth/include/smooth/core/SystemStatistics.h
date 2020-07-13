#include <unordered_map>
#include <mutex>

namespace smooth::core
{
    class TaskStats
    {
        public:
            TaskStats() = default;

            explicit TaskStats(uint32_t stack_size);

            TaskStats(const TaskStats&) = default;

            TaskStats(TaskStats&&) = default;

            TaskStats& operator=(const TaskStats&) = default;

            TaskStats& operator=(TaskStats&&) = default;

            [[nodiscard]] uint32_t get_stack_size() const noexcept
            {
                return stack_size;
            }

            [[nodiscard]] uint32_t get_high_water_mark() const noexcept
            {
                return high_water_mark;
            }

        private:
            uint32_t stack_size{};
            uint32_t high_water_mark{};

    };

    /// Muestra las estadisticas del sistema: uso de memoria y de stack.
    class SystemStatistics
    {
        public:

            static SystemStatistics& instance()
            {
                // La primera vez que se ejecuta este metodo se crea la instancia estatica
                static SystemStatistics instance{};

                // Retorna la instancia estatica
                return instance;

            }

            // Cada tarea, despues de su inicializacion y despues de la llamada a su tick()
            // (transcurridos x segundos) invocan a este metodo para actualiza 
            // la entrada en el mapa con el uso de memoria de la tarea.
            void report(const std::string& task_name, TaskStats&& stats) noexcept
            {
                // Se obtiene acceso esclusivo al mapa con las estadisticas de todas las tareas
                std::lock_guard<std::mutex> guard{ lock };

                // Se actualizan las estadisticas de la tarea 'task_name'
                task_info[task_name] = stats;

            }

            // Lanza el volcado de las estadisticas
            void dump() const noexcept;

        private:
            
            // Volcado parcial de cada uno de lso tipos de memoria general
            void dump_mem_stats(const char* header, uint32_t caps) const noexcept;

            // Mapa que almacena las estadisticas de uso de memoria de cada una de las tares
            std::unordered_map<std::string, TaskStats> task_info{};

            // Controla el acceso esclusivo al mapa de memoria de las tareas.
            mutable std::mutex lock{};

    };

}
