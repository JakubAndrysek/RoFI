#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <esp_console.h>

#include <string_view>
#include <iostream>
#include <sstream>
#include <optional>

#include <atoms/util.hpp>
#include <atoms/units.hpp>

#include <nvs/nvs.hpp>
#include <rofi_hal.hpp>

using namespace std::literals;

// This is a simple utility, so let's make things simple and use global objects
// so we can reference it from plain C functions required by CLI interface
std::optional< rofi::hal::RoFI > localRoFI;


// Simple wrapper that catches any exceptions, reports them an returns with a
// non-zero code if any error ocurred.
template < void (*Proc)( int argc, char** argv ) >
int handled( int argc, char** argv ) {
    try {
        Proc( argc, argv );
    }
    catch ( const std::exception& e ) {
        std::cerr << "Failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}


void ensureArguments( int argc, int expected ) {
    if ( argc != expected + 1 )
        throw std::runtime_error( "Invalid number of arguments " + std::to_string( argc ) );
}

void setId( int argc, char **argv ) {
    ensureArguments( argc, 1 );

    int id;
    std::istringstream s( argv[ 1 ] );
    if ( !( s >> id ) )
        throw std::runtime_error( "Invalid ID "s + argv[ 1 ] );

    localRoFI->setId( id );

    std::cout << "ID set to " << id << std::endl;
}

void registerSetId() {
    const esp_console_cmd_t cmd = {
        .command = "setId",
        .help = "Set RoFI ID",
        .hint = "Arguments: id",
        .func = &handled< setId >,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK( esp_console_cmd_register( &cmd ) );
}

void getId( int argc, char **argv ) {
    ensureArguments( argc, 0 );
    std::cout << "ID: " << localRoFI->getId() << std::endl;
}

void registerGetId() {
    const esp_console_cmd_t cmd = {
        .command = "getId",
        .help = "Get RoFI ID",
        .hint = "",
        .func = &handled< getId >,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK( esp_console_cmd_register( &cmd ) );
}


void registerCommands() {
    registerSetId();
    registerGetId();
}

extern "C" void app_main() {
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "RoFI>";
    esp::EspNvs::initNvs();

    localRoFI = rofi::hal::RoFI::getLocalRoFI();

    registerCommands();

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_console_new_repl_uart( &uart_config, &repl_config, &repl ) );
    ESP_ERROR_CHECK( esp_console_start_repl( repl ) );
}
