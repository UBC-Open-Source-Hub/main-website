#include "inc/securityModel.h"

SecurityModel::SecurityModel(int socket) : 
   socket(socket), isActive(false) {}

SecurityModel::~SecurityModel() {}
