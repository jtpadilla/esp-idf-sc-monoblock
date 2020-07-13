#pragma once

#include <chrono>

namespace smooth::core::timer
{

    /// Utilizada para medir el tiempo entre dos puntos en el tiempo.
    class ElapsedTime
    {
        public:

            /// Inicia el temporizador
            void start()
            {
                active = true;
                zero();
            }

            /// Detiene el temporizador
            void stop()
            {
                end_time = std::chrono::steady_clock::now();
                active = false;
                elapsed = end_time - start_time;
            }

            /// Igual que start(), pero se proporciona por cuestiones sintacticas
            void reset()
            {
                // Simplemente reinicia el timer
                start();
            }

            /// Reseta el contador a zero, pero lo mentiene funcionando
            void zero()
            {
                start_time = std::chrono::steady_clock::now();
                end_time = start_time;
            }

            /// Detiene el contador y lo resetea a cero
            void stop_and_zero()
            {
                stop();
                zero();
            }

            /// Obtiene la cantidad de tiempo trascurrida desde el inicio.
            /// \return La cantidad de tiempo.
            std::chrono::microseconds get_running_time();

            [[nodiscard]] std::chrono::microseconds get_running_time() const;

            /// \returns true si el timer esta funcionando, false si no.
            [[nodiscard]] bool is_running() const
            {
                return active;
            }

        private:
            bool active = false;
            std::chrono::steady_clock::time_point start_time{};
            std::chrono::steady_clock::time_point end_time{};
            std::chrono::steady_clock::duration elapsed{};

    };
    
}
