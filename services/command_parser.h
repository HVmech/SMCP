#pragma once

#ifndef SMCP_COMMAND_PARSER_H
#define SMCP_COMMAND_PARSER_H

#include <core/event_bus.h>
#include <common/types.h>

bool command_parser_parse_and_post(event_bus_t *bus, const char *cmd);

#endif
