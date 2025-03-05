#ifdef WIN32
#include "tcpservertest.h"
/*服务器端测试需求如下：
 * 1、测试连网速度
 * 2、等待客户端接入并返回连接确认信息
 * 其中确认信息中需包含该机IP、应用数
 */
tcpservertest::tcpservertest() {
    context_ = zmq_ctx_new();
    responder_ = zmq_socket(context_, ZMQ_REP);
    int rc = zmq_bind(responder_, "tcp://*:5555");
    logger.print("RDP_Server","StartListening》》》》》》");

    assert(rc == 0);

    while (1) {
        zmq_recv(responder_, &recvPacket_, sizeof(recvPacket_), 0);
        logger.print("RDP_Server","recvPacket数据包大小："+sizeof(recvPacket_));
        if(recvPacket_.RD_Type == OperationCommandType::TransmitAppAlias)
        {

        }
        zmq_send(responder_, "World", 5, 0);
    }
}
void tcpservertest::appNamesend()
{
    logger.print("RDP_Server","传输应用别名");
    // send
    InstalledSoftware *pc_software = new InstalledSoftware();
    pc_software->refreshSoftwareList();
    zmq_send(responder_,&pc_software->m_softwareList,sizeof(pc_software),0);
}
#endif
