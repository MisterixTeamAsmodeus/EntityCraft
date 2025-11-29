#include "PostgreAdapter/postgreerrorcode.hpp"

namespace database_adapter {
namespace postgre {

postgre_exec_status to_postgre_exec_status(ExecStatusType status)
{
    switch(status) {
        case PGRES_EMPTY_QUERY:
            return postgre_exec_status::EMPTY_QUERY;
        case PGRES_COMMAND_OK:
            return postgre_exec_status::COMMAND_OK;
        case PGRES_TUPLES_OK:
            return postgre_exec_status::TUPLES_OK;
        case PGRES_COPY_OUT:
            return postgre_exec_status::COPY_OUT;
        case PGRES_COPY_IN:
            return postgre_exec_status::COPY_IN;
        case PGRES_BAD_RESPONSE:
            return postgre_exec_status::BAD_RESPONSE;
        case PGRES_NONFATAL_ERROR:
            return postgre_exec_status::NONFATAL_ERROR;
        case PGRES_FATAL_ERROR:
            return postgre_exec_status::FATAL_ERROR;
        case PGRES_COPY_BOTH:
            return postgre_exec_status::COPY_BOTH;
        case PGRES_SINGLE_TUPLE:
            return postgre_exec_status::SINGLE_TUPLE;
        default:
            // Для неизвестных кодов возвращаем BAD_RESPONSE как общий код ошибки
            return postgre_exec_status::BAD_RESPONSE;
    }
}

ExecStatusType to_exec_status_type(postgre_exec_status status)
{
    return static_cast<ExecStatusType>(status);
}

postgre_conn_status to_postgre_conn_status(ConnStatusType status)
{
    switch(status) {
        case CONNECTION_OK:
            return postgre_conn_status::OK;
        case CONNECTION_BAD:
            return postgre_conn_status::BAD;
        case CONNECTION_STARTED:
            return postgre_conn_status::STARTED;
        case CONNECTION_MADE:
            return postgre_conn_status::MADE;
        case CONNECTION_AWAITING_RESPONSE:
            return postgre_conn_status::AWAITING_RESPONSE;
        case CONNECTION_AUTH_OK:
            return postgre_conn_status::AUTH_OK;
        case CONNECTION_SETENV:
            return postgre_conn_status::SETENV;
        case CONNECTION_SSL_STARTUP:
            return postgre_conn_status::SSL_STARTUP;
        case CONNECTION_NEEDED:
            return postgre_conn_status::NEEDED;
        default:
            // Для неизвестных кодов возвращаем BAD как общий код ошибки
            return postgre_conn_status::BAD;
    }
}

ConnStatusType to_conn_status_type(postgre_conn_status status)
{
    return static_cast<ConnStatusType>(status);
}

bool is_success(postgre_exec_status status)
{
    return status == postgre_exec_status::COMMAND_OK ||
           status == postgre_exec_status::TUPLES_OK ||
           status == postgre_exec_status::COPY_OUT ||
           status == postgre_exec_status::COPY_IN ||
           status == postgre_exec_status::COPY_BOTH ||
           status == postgre_exec_status::SINGLE_TUPLE;
}

bool is_critical_error(postgre_exec_status status)
{
    return status == postgre_exec_status::FATAL_ERROR ||
           status == postgre_exec_status::BAD_RESPONSE;
}

bool is_connection_ok(postgre_conn_status status)
{
    return status == postgre_conn_status::OK;
}

} // namespace postgre
} // namespace database_adapter

