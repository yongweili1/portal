#include "dispatcher.hpp"
#include "id_def.h"

#include "Common/McsfSystemEnvironmentConfig/mcsf_systemenvironment_factory.h"
#include "Common/McsfSystemEnvironmentConfig/mcsf_isystemenvironment_config.h"

#include "McsfErrorHandler/mcsf_error_handler.h"


int main(int argc, char* argv[])
{
    printf("usage:McsfSystemDispatcher | <port>\n");
    std::string log_path("log_config.xml");
    LOG_LOAD_CONFIG(log_path);
    try
    {
        netbasecore::connection_manager connection_manager_;
        dispatcher::subs_event_mngment subs_managment;
        dispatcher::address_managment addr_mng_(connection_manager_);

        dispatcher::proxy_leave_clean_wrapper wrapper_(subs_managment, addr_mng_);
        connection_manager_.set_disconnect_handler(boost::bind(&dispatcher::proxy_leave_clean_wrapper::clean, boost::ref(wrapper_), _1));
        netbasecore::io_service_pool pool(3, 3);
        netbasecore::callback_handle callbacks_(dispatcher_name);

        callbacks_.register_cmd_ex(register_id, McsfICommandHandlerExPtr(
            new dispatcher::register_command_handler(addr_mng_)));
        callbacks_.register_cmd_ex(find_id, McsfICommandHandlerExPtr(
            new dispatcher::find_command_handler(addr_mng_)));
        callbacks_.register_cmd_ex(subs_event_id, McsfICommandHandlerExPtr(
            new dispatcher::subs_command_handler(subs_managment)));
        callbacks_.register_event(broad_cast_id,
            new dispatcher::broadcast_event_handler(connection_manager_));
        callbacks_.register_event(send_event_id,
            new dispatcher::send_event_handler(connection_manager_, subs_managment));
        callbacks_.register_cmd_ex(echo_cmd_id, McsfICommandHandlerExPtr(
            new dispatcher::echoCommandHandler()));

        std::string port = argc > 1 ? argv[1] : "10000";
        // Initialize the server.
        netbasecore::server s(port, pool, connection_manager_, callbacks_);

        // Run the server until stopped.
        pool.join_all();

        //shutdown
      //  s.handle_stop();

    }
    catch (std::exception& e)
    {
        DSP_LOG_ERROR("exit:" << e.what());
    }
    ERASE_LOGGER;
    return 0;
}