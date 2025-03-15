#include "./Code/AppData/devicedate.h"

const char* operationCommandTypeToString(OperationCommandType type) {
    switch (type) {
    case OperationCommandType::TransmitAppAlias:
        return "TransmitAppAlias";
    case OperationCommandType::TransmitAppIconData:
        return "TransmitAppIconData";
    case OperationCommandType::TransmitAppCommand:
        return "TransmitAppCommand";
    case OperationCommandType::TransmitConnectTest:
        return "TransmitConnectTest";
    case OperationCommandType::TransmitEnd:
        return "TransmitEnd";
    default:
        return "Unknown";
    }
}
