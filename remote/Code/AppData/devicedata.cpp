#include "./Code/AppData/devicedate.h"

const char* operationCommandTypeToString(OperationCommandType type) {
    switch (type) {
    case OperationCommandType::TransmitAppAlias:
        return "TransmitAppAlias";
    case OperationCommandType::TransmitAppIconData:
        return "TransmitAppIconData";
    case OperationCommandType::TransmitAppCommand:
        return "TransmitAppCommand";
    case OperationCommandType::TransmitOnceEnd:
        return "TransmitOnceEnd";
    case OperationCommandType::TransmitEnd:
        return "TransmitEnd";
    default:
        return "Unknown";
    }
}

