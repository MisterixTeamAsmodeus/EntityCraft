#pragma once

// Базовый класс и общие реализации
#include "converter/typeconverter_base.hpp"

// Строковые типы (bool, string, wstring, u16string, u32string)
#include "converter/typeconverter_strings.hpp"

// Числовые типы (int, long, float, double и т.д.)
#include "converter/typeconverter_numeric.hpp"

// Умные указатели (shared_ptr, unique_ptr, T*)
#include "converter/typeconverter_pointers.hpp"

// Контейнеры (pair, vector, array, deque, list, set, map и т.д.)
#include "converter/typeconverter_containers.hpp"

// Qt типы (QDateTime, QDate, QString, QByteArray, QJsonObject)
#include "converter/typeconverter_qt.hpp"
