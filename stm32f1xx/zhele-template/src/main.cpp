#include <zhele/iopins.h>

using namespace Zhele::IO;

int main()
{
    // Включаем тактирование порта.
    Pa4::Port::Enable();

    // Настраиваем на вывод.
    Pa4::SetConfiguration( Pa4::Configuration::Out );

    // Схема управления.
    Pa4::SetDriverType( Pa4::DriverType::PushPull );

    // Установка начального значения на выводе.
    Pa4::Clear();

    for ( ;; )
    {
    }
}
