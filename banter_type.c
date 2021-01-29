#include "includes.h"
#include "astnodes.h"
#include "banter_type.h"
#include "macros.h"
#include "debugging.h"
MessageTemplate *MessageTemplate__new(MessageType type, BanterType *returns) {
	MessageTemplate *msgt = new(MessageTemplate);
	msgt->type = type;
	msgt->list = list_alloc(KeyNodeValue);
	msgt->returns = returns;
	return msgt;
}
