#pragma once

#include <string>

namespace database_adapter {

/// @brief Класс описывающий настройки для подключения к базе данных
struct database_connection_settings
{
    /// @brief Название базы данных для подключения
    std::string database_name {};
    /// @brief Адрес/путь для подключения к базе данных
    std::string url {};
    /// @brief Порт для подключения к базе данных
    std::string port {};
    /// @brief Имя пользователя для авторизации
    std::string login {};
    /// @brief Пароль пользователя для авторизации
    std::string password {};
};

} // namespace DatabaseAdapter
