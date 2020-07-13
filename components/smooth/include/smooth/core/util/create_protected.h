#pragma once

#include <memory>
#include <type_traits>

namespace smooth::core::util
{

    /// \brief El propósito de esta función de plantilla es permitir forzar la creación de instancias de clases
    /// solo por un std::shared_ptr, haciendo que el constructor de T esté protegido. Esto nos es útil cuando quieres
    /// usar std::shared_from_this y evitar que una instancia se cree directamente con new() que
    /// resultaría en el uso de std::shared_from_this en una excepción.
    template<typename T, typename... Args>
    auto create_protected_shared(Args&& ... args)
    {
        class CreationWrapper
            : public T
        {
            public:
                explicit CreationWrapper(Args&& ... args)
                        : T(std::forward<Args>(args)...)
                {
                }
        };

        return std::make_shared<CreationWrapper>(std::forward<Args>(args)...);
    }

    /// \brief El propósito de esta función de plantilla es permitir forzar la creación de instancias de clases
    /// solo por std::unique_ptr, haciendo que el constructor de T esté protegido. Como no hay std::unique_from_this,
    /// este método tiene un uso limitado en comparación con create_protected_shared.
    template<typename T, typename... Args>
    auto create_protected_unique(Args&& ... args)
    {
        class CreationWrapper
            : public T
        {
            public:
                explicit CreationWrapper(Args&& ... args)
                        : T(std::forward<Args>(args)...)
                {
                }
        };

        return std::make_unique<CreationWrapper>(std::forward<Args>(args)...);
    }
    
}
