using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using System.Runtime.InteropServices.WindowsRuntime;

namespace AdapterLib
{
    class ModbusGateway
    {
        private StreamSocket clientSocket;
        private bool m_isConnected;
        public bool isConnected
        {
            get { return m_isConnected; }
        }

        private string m_errorMsg;
        public string ErrorMsg
        {
            get { return m_errorMsg; }
        }

        private string m_deviceIP;
        public string DeviceIP
        {
            get { return m_deviceIP; }
        }

        public ModbusGateway(string gatewayIP)
        {
            m_deviceIP = gatewayIP;
            clientSocket = new Windows.Networking.Sockets.StreamSocket();
            m_isConnected = false;
            m_errorMsg = "";
        }

        public async Task<uint> InitializeAsync()
        {

            return await ConnectAsync();
        }

        public async Task<uint> ConnectAsync()
        {
            try
            {
                if (clientSocket == null)
                {
                    clientSocket = new Windows.Networking.Sockets.StreamSocket();
                }
                HostName serverHost = new HostName(m_deviceIP);
                if (serverHost != null)
                {
                    await clientSocket.ConnectAsync(serverHost, "502");
                    m_isConnected = true;
                }
            }
            catch (Exception e)
            {
                SocketErrorStatus errStatus = SocketError.GetStatus(e.HResult);
                m_errorMsg = string.Format("Attempt to connect to device {0} failed: SocketError {1}", m_deviceIP, errStatus);
                Disconnect();
                return (Convert.ToUInt32(errStatus) | 0xFF00);
            }

            return 0;
        }// end connect

        public void Disconnect()
        {
            if (null != clientSocket)
            {
                clientSocket.Dispose();
                clientSocket = null;
            }

            m_isConnected = false;
        }// end disconnect


        public async Task<ModbusResponse> sendRequestAsync(ModbusRequest request)
        {
            ModbusResponse response = null;

            //Send Read Request
            uint status = await clientSocket.OutputStream.WriteAsync(request.getByteArray().AsBuffer());
            bool done = await clientSocket.OutputStream.FlushAsync();

            //Get Read Response
            response = new ModbusResponse();
            uint bufferLen = getResponseBufferLength(request);
            byte[] readBuffer = new byte[bufferLen];
            var readResponse = await clientSocket.InputStream.ReadAsync(readBuffer.AsBuffer(), bufferLen, InputStreamOptions.Partial);

            response.FunctionCode = readBuffer[7];

            if ((readBuffer[7] & 0x80) == 0x80)     //check exception: readBuffer[7] & 0x80
            {
                m_errorMsg = String.Format("[Error] Read operation failed. Exception Code: {0}.", readBuffer[8]);
                response.Status = readBuffer[8];
            }
            else
            {
                //Get Read Response
                switch (response.FunctionCode)
                {
                    case 1: //Read Coils
                    case 2: //Read Discrete Input
                    case 3: //Read Holding Registers
                    case 4: //Read Input Registers
                        response.ByteCount = readBuffer[8];
                        response.Data = parseReadResponse(readBuffer);
                        response.Status = 0;
                        break;
                    case 5: //Write Single Coil 
                    case 6: //Write Single Register
                        response.Address = (UInt16)((readBuffer[8] << 8) + readBuffer[9]);
                        response.Data = new ushort[1] { (UInt16)((readBuffer[10] << 8) + readBuffer[11]) };
                        response.Status = 0;
                        break;
                    case 15: //Write Multiple Coils
                    case 16: //Write Multiple Registers
                        response.Address = (UInt16)((readBuffer[8] << 8) + readBuffer[9]);
                        response.RegCount = (UInt16)((readBuffer[10] << 8) + readBuffer[11]);
                        response.Status = 0;
                        break;
                    default:
                        break;
                }
            }// end if-else check function code

            return response;
        }//END readHelper

        private uint getResponseBufferLength(ModbusRequest request)
        {
            switch (request.FunctionCode)
            {
                case 1: //Read Coils
                case 2: //Read Discrete Input
                    int N = request.Length / 8;
                    if (request.Length % 8 == 0)
                    {
                        return Convert.ToUInt32(N + 9);
                    }
                    return Convert.ToUInt32(N + 10);
                case 3: //Read Holding Registers
                case 4: //Read Input Registers
                    return Convert.ToUInt32(request.Length * 2 + 9);
                case 5: //Write Single Coil                
                case 6: //Write Single Register
                case 15: //Write Single Registers
                case 16: //Write Multiple Registers
                    return Convert.ToUInt32(12);
                default:
                    return 9;
            }
        }

        private UInt16[] parseReadResponse(byte[] readResponseData)
        {
            uint length;
            UInt16[] data;

            switch (readResponseData[7])
            {
                case 1: //Read Coils
                case 2: //Read Discrete Input
                    length = Convert.ToUInt32(readResponseData[8]);
                    data = new UInt16[length];
                    for (uint i = 0; i < length; i++)
                    {
                        data[i] = (UInt16)(readResponseData[i + 9]);
                    }
                    break;
                case 3: //Read Holding Registers
                case 4: //Read Input Registers
                    length = Convert.ToUInt32(readResponseData[8] / 2);
                    data = new UInt16[length];
                    for (uint i = 0; i < length; i++)
                    {
                        data[i] = (UInt16)((readResponseData[i * 2 + 9] << 8) + readResponseData[i * 2 + 10]);
                    }
                    break;
                case 5: //Write Single Coil
                case 6: //Write Single Register
                case 15: //Write Single Registers
                case 16: //Write Multiple Registers
                default:
                    data = null;
                    break;
            }


            return data;
        }

    }//END class ModbusGateway

    public sealed class ModbusRequest
    {
        private ushort m_id = 0;
        private byte m_unit = 0;
        private byte m_functionCode = 0;
        public byte FunctionCode
        {
            get { return m_functionCode; }
        }
        private ushort m_addr = 0;
        public ushort Address
        {
            get { return m_addr; }
        }

        private ushort m_length = 0;
        public ushort Length
        {
            get { return m_length; }
        }

        private byte m_byteCount = 0;
        public byte ByteCount
        {
            get { return m_byteCount; }
        }

        private ushort[] m_data = null;
        public ushort[] Data
        {
            get { return m_data; }
        }

        public ModbusRequest() { }

        public ModbusRequest(ushort id, byte unit, byte functionCode, ushort address, ushort length)
        {
            m_id = id;
            m_unit = unit;
            m_functionCode = functionCode;
            m_addr = address;
            m_length = length;
            m_data = new ushort[1] { 0 };
        }

        public ModbusRequest(ushort id, byte unit, byte functionCode, ushort address, ushort length, byte byteCount, [System.Runtime.InteropServices.WindowsRuntime.ReadOnlyArray] ushort[] dataArray)
        {
            m_id = id;
            m_unit = unit;
            m_functionCode = functionCode;
            m_addr = address;
            m_length = length;
            m_byteCount = byteCount;
            m_data = dataArray;
        }

        public byte[] getByteArray()
        {
            byte[] MBAPLenByte = null;
            byte[] request = null;
            byte[] idByte = null;
            byte[] lenByte = null;
            byte[] addrByte = null;
            byte[] dataByte = null;

            switch (m_functionCode)
            {
                case 15: //WriteMultipleCoils
                case 16: //WriteMultipleRegisters
                    if (m_byteCount % 2 == 0)
                    {
                        request = new byte[13 + m_byteCount];
                    }
                    else
                    {
                        request = new byte[14 + m_byteCount];
                    }

                    //MBAP Header for TCP/IP protocol
                    idByte = BitConverter.GetBytes(m_id);
                    request[0] = idByte[1]; //Transaction ID (2 bytes)
                    request[1] = idByte[0];
                    request[2] = 0;         //Protocol ID (2 bytes): 0 = MODBUS
                    request[3] = 0;
                    MBAPLenByte = BitConverter.GetBytes(7 + m_byteCount); //TODO:Check
                    request[4] = MBAPLenByte[1];         //Length (2 bytes)
                    request[5] = MBAPLenByte[0];
                    request[6] = m_unit;      //Unit ID (1 byte)

                    //PDU
                    request[7] = m_functionCode;
                    addrByte = BitConverter.GetBytes(m_addr);
                    request[8] = addrByte[1];
                    request[9] = addrByte[0];
                    dataByte = BitConverter.GetBytes(m_length);
                    request[10] = dataByte[1];
                    request[11] = dataByte[0];
                    request[12] = m_byteCount;
                    for (int i = 0; i < m_data.Length; i++)
                    {
                        dataByte = BitConverter.GetBytes(m_data[i]);
                        request[13 + (i * 2)] = dataByte[1];
                        request[14 + (i * 2)] = dataByte[0];
                    }
                    break;
                case 5: //WriteSingleCoil
                case 6: //WriteSingleRegister
                    request = new byte[12];
                    //MBAP Header for TCP/IP protocol
                    idByte = BitConverter.GetBytes(m_id);
                    request[0] = idByte[1]; //Transaction ID (2 bytes)
                    request[1] = idByte[0];
                    request[2] = 0;         //Protocol ID (2 bytes): 0 = MODBUS
                    request[3] = 0;
                    request[4] = 0;         //Length (2 bytes)
                    request[5] = 6;
                    request[6] = m_unit;      //Unit ID (1 byte)

                    //PDU
                    request[7] = m_functionCode;
                    addrByte = BitConverter.GetBytes(m_addr);
                    request[8] = addrByte[1];
                    request[9] = addrByte[0];
                    dataByte = BitConverter.GetBytes(m_data[0]);
                    request[10] = dataByte[1];
                    request[11] = dataByte[0];
                    break;
                case 1: //readCoil
                case 2: //readDiscrete
                case 3: //readHoldingReg
                case 4: //readInputReg
                default:
                    request = new byte[12];
                    //MBAP Header for TCP/IP protocol
                    idByte = BitConverter.GetBytes(m_id);
                    request[0] = idByte[1]; //Transaction ID (2 bytes)
                    request[1] = idByte[0];
                    request[2] = 0;         //Protocol ID (2 bytes): 0 = MODBUS
                    request[3] = 0;
                    request[4] = 0;         //Length (2 bytes)
                    request[5] = 6;
                    request[6] = m_unit;      //Unit ID (1 byte)

                    //PDU
                    request[7] = m_functionCode;
                    addrByte = BitConverter.GetBytes(m_addr);
                    request[8] = addrByte[1];
                    request[9] = addrByte[0];
                    lenByte = BitConverter.GetBytes(m_length);
                    request[10] = lenByte[1];
                    request[11] = lenByte[0];
                    break;
            }

            return request;
        }
    }//end class ModbusRequest

    public sealed class ModbusResponse
    {
        private byte m_functionCode = 0;
        public byte FunctionCode
        {
            get { return m_functionCode; }
            set { m_functionCode = value; }
        }

        private ushort m_address = 0;
        public ushort Address
        {
            get { return m_address; }
            set { m_address = value; }
        }

        private ushort m_regCount = 0;
        public ushort RegCount
        {
            get { return m_regCount; }
            set { m_regCount = value; }
        }

        private byte m_byteCount = 0;
        public byte ByteCount
        {
            get { return m_byteCount; }
            set { m_byteCount = value; }
        }

        private UInt16[] m_data = null;
        public UInt16[] Data
        {
            get { return m_data; }
            set { m_data = value; }
        }

        private byte m_status;
        public byte Status
        {
            get { return m_status; }
            set { m_status = value; }
        }

        public ModbusResponse() { }

    }//end class ModbusResponse
}
