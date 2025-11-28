/**
 * ПРИМЕР 4: Работа с reference_property (вложенные структуры)
 * 
 * Этот пример демонстрирует работу с вложенными объектами:
 * 
 * 1. REFERENCE_PROPERTY
 *    - Представляет свойство, которое само является объектом
 *    - Связывает свойство с entity вложенного объекта
 *    - Позволяет работать с вложенными структурами рекурсивно
 * 
 * 2. ВЛОЖЕННЫЕ СТРУКТУРЫ
 *    - Создание entity для вложенных типов
 *    - Использование reference_property для связи
 *    - Доступ к свойствам вложенных объектов
 * 
 * 3. МНОГОУРОВНЕВАЯ ВЛОЖЕННОСТЬ
 *    - Работа с объектами, вложенными на несколько уровней
 *    - Рекурсивная обработка вложенных структур
 * 
 * 4. ПРАКТИЧЕСКИЕ ПРИМЕРЫ
 *    - Адреса в объектах Person
 *    - Компании с адресами
 *    - Многоуровневая иерархия
 */

#include <ReflectionApi/reflectionapi.hpp>
#include <iostream>
#include <string>

// ============================================================
// ВЛОЖЕННАЯ СТРУКТУРА - АДРЕС
// ============================================================
//
// Эта структура будет использоваться как вложенный объект
// в других структурах

struct Address
{
    std::string street;      // Улица
    std::string city;        // Город
    int zipCode = 0;         // Почтовый индекс
};

// ============================================================
// ОСНОВНАЯ СТРУКТУРА - ЧЕЛОВЕК С АДРЕСОМ
// ============================================================
//
// Эта структура содержит вложенный объект Address
// Для работы с ним используется reference_property

struct PersonWithAddress
{
    int id = 0;              // Идентификатор
    std::string name;        // Имя
    Address address;         // Вложенный объект - адрес
};

// ============================================================
// КЛАСС С ВЛОЖЕННЫМ ОБЪЕКТОМ ЧЕРЕЗ ГЕТТЕРЫ/СЕТТЕРЫ
// ============================================================
//
// Демонстрирует использование reference_property
// с классами, где вложенный объект доступен через методы

class Company
{
public:
    // Методы доступа к адресу
    void setAddress(const Address& addr) { 
        _address = addr; 
    }
    
    Address getAddress() const { 
        return _address; 
    }

    // Методы доступа к имени
    void setName(const std::string& name) { 
        _name = name; 
    }
    
    const std::string& getName() const { 
        return _name; 
    }

private:
    Address _address;        // Приватное поле - адрес
    std::string _name;       // Приватное поле - имя
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 4: Работа с reference_property\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: СОЗДАНИЕ ENTITY ДЛЯ ВЛОЖЕННОЙ СТРУКТУРЫ
    // ============================================================
    //
    // Перед созданием reference_property необходимо создать
    // entity для вложенного типа
    // Это entity описывает структуру вложенного объекта
    
    std::cout << "ШАГ 1: Создание entity для вложенной структуры Address\n";
    std::cout << "-----------------------------------------------------\n";
    
    // Создаем entity для Address
    // Эта entity описывает свойства адреса
    auto addressEntity = reflection_api::make_entity<Address>(
        reflection_api::make_property("street", &Address::street),
        reflection_api::make_property("city", &Address::city),
        reflection_api::make_property("zipCode", &Address::zipCode)
    );

    std::cout << "Создана entity для Address:\n";
    std::cout << "  Количество свойств: " << addressEntity.property_count() << "\n";
    std::cout << "  Свойства: street, city, zipCode\n\n";

    // ============================================================
    // ШАГ 2: СОЗДАНИЕ REFERENCE_PROPERTY
    // ============================================================
    //
    // reference_property связывает поле родительского объекта
    // с entity вложенного объекта
    //
    // make_reference_property(name, variable, entity) создает
    // reference_property для поля-объекта
    
    std::cout << "ШАГ 2: Создание reference_property для вложенного объекта\n";
    std::cout << "----------------------------------------------------------\n";
    
    // Создаем reference_property для поля address
    // Первый параметр - имя свойства в родительском объекте
    // Второй параметр - указатель на поле-объект
    // Третий параметр - entity для вложенного типа
    auto addressRefProperty = make_reference_property(
        "address",                      // Имя свойства в PersonWithAddress
        &PersonWithAddress::address,    // Указатель на поле-объект
        addressEntity                   // Entity для типа Address
    );

    std::cout << "Создан reference_property для address:\n";
    std::cout << "  Имя свойства: '" << addressRefProperty.name() << "'\n";
    std::cout << "  Количество свойств в reference_entity: " 
              << addressRefProperty.reference_entity().property_count() << "\n\n";

    // ============================================================
    // ШАГ 3: РАБОТА С REFERENCE_PROPERTY НАПРЯМУЮ
    // ============================================================
    //
    // reference_property работает как обычное property,
    // но значение имеет тип вложенного объекта
    
    std::cout << "ШАГ 3: Работа с reference_property напрямую\n";
    std::cout << "-------------------------------------------\n";
    
    // Создаем объект PersonWithAddress
    PersonWithAddress person;
    
    // Создаем объект Address
    Address addr;
    addr.street = "Ленина, 10";
    addr.city = "Москва";
    addr.zipCode = 123456;
    
    // Устанавливаем весь объект адреса через reference_property
    // set_value принимает весь объект Address
    addressRefProperty.set_value(person, addr);

    std::cout << "Установлен адрес через reference_property:\n";
    
    // Получаем весь объект адреса
    Address retrievedAddr = addressRefProperty.value(person);
    std::cout << "  street = '" << retrievedAddr.street << "'\n";
    std::cout << "  city = '" << retrievedAddr.city << "'\n";
    std::cout << "  zipCode = " << retrievedAddr.zipCode << "\n\n";

    // ============================================================
    // ШАГ 4: ДОСТУП К СВОЙСТВАМ ВЛОЖЕННОГО ОБЪЕКТА
    // ============================================================
    //
    // reference_entity() возвращает entity для вложенного типа
    // Через эту entity можно работать с отдельными свойствами
    // вложенного объекта
    
    std::cout << "ШАГ 4: Доступ к свойствам вложенного объекта через reference_entity\n";
    std::cout << "-------------------------------------------------------------------\n";
    
    // Получаем entity для вложенного объекта
    auto refEntity = addressRefProperty.reference_entity();
    
    // Получаем значения отдельных свойств адреса
    std::string street;
    std::string city;
    int zipCode = 0;
    
    // Используем get_property_value для получения отдельных свойств
    refEntity.get_property_value(retrievedAddr, street, "street");
    refEntity.get_property_value(retrievedAddr, city, "city");
    refEntity.get_property_value(retrievedAddr, zipCode, "zipCode");
    
    std::cout << "Доступ к отдельным свойствам адреса:\n";
    std::cout << "  street = '" << street << "'\n";
    std::cout << "  city = '" << city << "'\n";
    std::cout << "  zipCode = " << zipCode << "\n\n";

    // ============================================================
    // ШАГ 5: УСТАНОВКА ЗНАЧЕНИЙ ОТДЕЛЬНЫХ СВОЙСТВ ВЛОЖЕННОГО ОБЪЕКТА
    // ============================================================
    //
    // Для изменения отдельных свойств вложенного объекта:
    // 1. Получить текущий объект через value()
    // 2. Изменить нужные свойства через reference_entity
    // 3. Установить обновленный объект обратно через set_value()
    
    std::cout << "ШАГ 5: Установка значений отдельных свойств вложенного объекта\n";
    std::cout << "---------------------------------------------------------------\n";
    
    // Получаем текущий адрес
    Address currentAddr = addressRefProperty.value(person);
    
    // Изменяем отдельные свойства через reference_entity
    refEntity.set_property_value(currentAddr, std::string("Пушкина, 5"), "street");
    refEntity.set_property_value(currentAddr, std::string("Санкт-Петербург"), "city");
    refEntity.set_property_value(currentAddr, 654321, "zipCode");
    
    // Устанавливаем обновленный адрес обратно
    addressRefProperty.set_value(person, currentAddr);
    
    std::cout << "Обновленный адрес:\n";
    Address updatedAddr = addressRefProperty.value(person);
    std::cout << "  street = '" << updatedAddr.street << "'\n";
    std::cout << "  city = '" << updatedAddr.city << "'\n";
    std::cout << "  zipCode = " << updatedAddr.zipCode << "\n\n";

    // ============================================================
    // ШАГ 6: СОЗДАНИЕ ENTITY С КОМБИНАЦИЕЙ ОБЫЧНЫХ И ССЫЛОЧНЫХ СВОЙСТВ
    // ============================================================
    //
    // entity может содержать как обычные property, так и
    // reference_property в любом сочетании
    
    std::cout << "ШАГ 6: Создание entity с комбинацией обычных и ссылочных свойств\n";
    std::cout << "----------------------------------------------------------------\n";
    
    // Создаем entity для PersonWithAddress
    // Включаем обычные свойства и reference_property
    auto personEntity = make_entity<PersonWithAddress>(
        reflection_api::make_property("id", &PersonWithAddress::id),           // Обычное свойство
        reflection_api::make_property("name", &PersonWithAddress::name),      // Обычное свойство
        make_reference_property("address", &PersonWithAddress::address, addressEntity)  // Ссылочное свойство
    );

    std::cout << "Создана entity для PersonWithAddress:\n";
    std::cout << "  Количество свойств: " << personEntity.property_count() << "\n";
    std::cout << "  Свойства: id (обычное), name (обычное), address (reference)\n\n";

    // ============================================================
    // ШАГ 7: РАБОТА С ОБЪЕКТОМ ЧЕРЕЗ ENTITY С REFERENCE_PROPERTY
    // ============================================================
    //
    // Работа через entity с reference_property аналогична
    // работе с обычными свойствами, но значение - это объект
    
    std::cout << "ШАГ 7: Работа с объектом через entity с reference_property\n";
    std::cout << "----------------------------------------------------------\n";
    
    PersonWithAddress person2;
    
    // Устанавливаем обычные свойства
    personEntity.set_property_value(person2, 1, "id");
    personEntity.set_property_value(person2, std::string("Иван Иванов"), "name");
    
    // Для reference_property нужно установить весь объект
    Address addr2;
    addr2.street = "Тверская, 1";
    addr2.city = "Москва";
    addr2.zipCode = 101000;
    personEntity.set_property_value(person2, addr2, "address");

    std::cout << "Установлены значения через entity:\n";
    std::cout << "  person2.id = " << person2.id << "\n";
    std::cout << "  person2.name = '" << person2.name << "'\n";
    std::cout << "  person2.address.street = '" << person2.address.street << "'\n";
    std::cout << "  person2.address.city = '" << person2.address.city << "'\n";
    std::cout << "  person2.address.zipCode = " << person2.address.zipCode << "\n\n";

    // ============================================================
    // ШАГ 8: РАБОТА С REFERENCE_PROPERTY ЧЕРЕЗ ГЕТТЕРЫ/СЕТТЕРЫ
    // ============================================================
    //
    // reference_property можно создать и для классов,
    // где вложенный объект доступен через методы доступа
    
    std::cout << "ШАГ 8: Работа с reference_property через геттеры/сеттеры\n";
    std::cout << "--------------------------------------------------------\n";
    
    // Создаем entity для Address (можно переиспользовать существующую)
    auto companyAddressEntity = make_entity<Address>(
        reflection_api::make_property("street", &Address::street),
        reflection_api::make_property("city", &Address::city),
        reflection_api::make_property("zipCode", &Address::zipCode)
    );

    // Создаем reference_property через геттеры и сеттеры
    auto companyAddressRefProperty = make_reference_property(
        "address",
        &Company::setAddress,        // Сеттер для адреса
        &Company::getAddress,         // Геттер для адреса
        companyAddressEntity          // Entity для Address
    );

    Company company;
    Address companyAddr;
    companyAddr.street = "Невский проспект, 28";
    companyAddr.city = "Санкт-Петербург";
    companyAddr.zipCode = 191186;
    
    // Устанавливаем адрес через reference_property
    companyAddressRefProperty.set_value(company, companyAddr);

    std::cout << "Работа с reference_property через геттеры/сеттеры:\n";
    Address companyRetrievedAddr = companyAddressRefProperty.value(company);
    std::cout << "  company.address.street = '" << companyRetrievedAddr.street << "'\n";
    std::cout << "  company.address.city = '" << companyRetrievedAddr.city << "'\n";
    std::cout << "  company.address.zipCode = " << companyRetrievedAddr.zipCode << "\n\n";

    // ============================================================
    // ШАГ 9: МНОГОУРОВНЕВАЯ ВЛОЖЕННОСТЬ
    // ============================================================
    //
    // reference_property можно использовать для создания
    // многоуровневой вложенности объектов
    
    std::cout << "ШАГ 9: Многоуровневая вложенность\n";
    std::cout << "---------------------------------\n";
    
    // Определяем структуру Building с вложенным Address
    struct Building
    {
        Address location;    // Вложенный объект - адрес здания
        int floors = 0;      // Обычное свойство - количество этажей
    };

    // Создаем entity для Building
    // Используем reference_property для location и обычное property для floors
    auto buildingEntity = make_entity<Building>(
        make_reference_property("location", &Building::location, addressEntity),
        reflection_api::make_property("floors", &Building::floors)
    );

    Building building;
    building.location.street = "Красная площадь, 1";
    building.location.city = "Москва";
    building.location.zipCode = 109012;
    building.floors = 5;

    std::cout << "Многоуровневая вложенность:\n";
    std::cout << "  building.floors = " << building.floors << "\n";
    std::cout << "  building.location.street = '" << building.location.street << "'\n";
    std::cout << "  building.location.city = '" << building.location.city << "'\n";
    std::cout << "  building.location.zipCode = " << building.location.zipCode << "\n\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    //
    // В этом примере мы изучили:
    // 1. Как создавать entity для вложенных структур
    // 2. Как создавать reference_property для связи объектов
    // 3. Как работать с вложенными объектами напрямую
    // 4. Как получать доступ к свойствам вложенных объектов
    // 5. Как изменять отдельные свойства вложенных объектов
    // 6. Как комбинировать обычные и ссылочные свойства
    // 7. Как работать с reference_property через геттеры/сеттеры
    // 8. Как создавать многоуровневую вложенность
    //
    // reference_property позволяет:
    // - Работать с вложенными структурами рекурсивно
    // - Создавать сложные иерархии объектов
    // - Применять рефлексию к вложенным объектам
    // - Реализовывать ORM с поддержкой связей между таблицами

    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";

    return 0;
}
