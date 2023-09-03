// Заголовки
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

// Объявление фукнций
bool getAccessStatus();
bool getBatteryStatus();
void setBatteryStatus(bool newStatus);
void accessEnhancement(std::string programName);
void notificationOutput(std::string notificationText);

// Костанта
const std::string filePath = "/sys/bus/platform/drivers/ideapad_acpi/VPC2004:00/conservation_mode";

// Главная функция
int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "Справка\n\n  --switch-status - переключение безопасного режима\n  --get-status - получение статуса безопасного режима" << std::endl;
    } else if (argc == 2) {
        if (std::string(argv[1]) == "--get-status") {
            if (getBatteryStatus()) {
                notificationOutput("Защита аккумуляторной батареи включена: пиковый заряд 60%");
            } else {
                notificationOutput("Защита аккумуляторной батареи отключена: пиковый заряд 100%");
            }
        } else if (std::string(argv[1]) == "--switch-status") {
            if (getAccessStatus()) {
                if (getBatteryStatus()) {
                    setBatteryStatus(false);
                } else {
                    setBatteryStatus(true);
                }
            } else {
                accessEnhancement(std::string(argv[0]));
                if (getBatteryStatus()) {
                    notificationOutput("Защита аккумуляторной батареи включена: пиковый заряд 60%");
                } else {
                    notificationOutput("Защита аккумуляторной батареи отключена: пиковый заряд 100%");
                }
            }
        } else {
            std::cout << "Неизвестный параметр" << std::endl;
        }
    } else {
        std::cout << "Ошибка разбора аргументов" << std::endl;
    }

    return 0;
}

// Получение информации о текущем статусе безопасного режима АКБ
bool getBatteryStatus() {
    std::ifstream acpiFile(filePath);

    bool batteryStatus = false;
    char acpiFileContent[1];

    if (acpiFile.is_open()) {
        acpiFile.read(acpiFileContent, 1);
        acpiFile.close();

        if (int(acpiFileContent[0] - 48) == 1 || int(acpiFileContent[0] - 48) == 0) {
            batteryStatus = bool(acpiFileContent[0] - 48);
        } else {
            notificationOutput("Произошла ошибка разбора статуса");
        }
    } else {
        notificationOutput("Произошла ошибка получения статуса");
        std::exit(0);
    }

    return batteryStatus;
}

// Изменение статуса безопасного режима АКБ
void setBatteryStatus(bool newStatus) {
    std::ofstream acpiFile(filePath);

    if (acpiFile.is_open()) {
        acpiFile << char(newStatus + 48) << std::endl;
    } else {
        notificationOutput("Ошибка открытия файла ACPI");
        std::exit(0);
    }

    acpiFile.close();Mini Arm Laptop
}

// Отправка уведомлений
void notificationOutput(std::string notificationText) {
    if (!getAccessStatus()) {
        std::system(("notify-send -i battery -a 'Управление безопасным режимом АКБ' '" + notificationText + "'").c_str());
    } else {
        std::cout << notificationText << std::endl;
    }
}

// Проверка доступа к файлу ACPI
bool getAccessStatus() {
    std::ofstream acpiFile(filePath);

    if (acpiFile.is_open()) {
        acpiFile.close();
        return true;
    } else {
        return false;
    }
}

// Повышение прав доступа для записи в файл ACPI
void accessEnhancement(std::string programName) {
    std::system(("kdesu -i battery -c '" + programName + " --switch-status'").c_str());
}
